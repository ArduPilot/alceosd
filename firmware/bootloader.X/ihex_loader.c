 /*
    AlceOSD - UART bootloader
    Copyright (C) 2015  Luis Alves

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bootloader.h"


/* intel hex record types */
#define DATA_RECORD                     0x00
#define END_OF_FILE_RECORD              0x01
#define EXT_SEG_ADRS_RECORD             0x02
#define EXT_LIN_ADRS_RECORD             0x04

#define MAX_IHEX_LINE_LEN               50


struct ihex_record {
    /* record length */
    unsigned char len;
    /* record type */
    unsigned char type;
    /* data pointer */
    char *data;
    /* extended segment/linear address */
    u32union e_addr;
    /* write address */
    u32union addr;
};


void ascii2hex(unsigned char *buf)
{
    unsigned char hex, i = 0;

    while ((buf[i] >= '0') && (buf[i] <= 'f')) {
        hex = buf[i] & 0xf;
        if(buf[i] > '9')
            hex += 9;

        if (i % 2) {
            buf[i/2] |= hex;
        } else {
            buf[i/2] = (hex << 4) & 0xf0;
        }
        i++;
    }
}

unsigned char write_ihex2flash(char *buf)
{
    static struct ihex_record ihex_rec;
    unsigned char crc = 0;
    unsigned int i;
    unsigned long wr_data0, wr_data1;
    unsigned long prog_addr;
    unsigned int ret;

    ihex_rec.len = buf[0];
    ihex_rec.type = buf[3];
    ihex_rec.data = &buf[4];

    /* validate crc */
    for (i = 0; i < ihex_rec.len + 5; i++)
        crc += buf[i];

    if(crc != 0) {
        put_char('x');
        return 0xff;
    }

    switch(ihex_rec.type) {
        case DATA_RECORD:
            ihex_rec.addr.w[1] = 0;
            ihex_rec.addr.b[1] = buf[1];
            ihex_rec.addr.b[0] = buf[2];
            ihex_rec.addr.l += ihex_rec.e_addr.l;

            while(ihex_rec.len) {
                prog_addr = ihex_rec.addr.l >> 1;

                /* write 2 iwords */
                if ((ihex_rec.len > 7) && ((prog_addr % 4) == 0)) {
                    i = 8;
                    memcpy(&wr_data0, ihex_rec.data, 4);
                    memcpy(&wr_data1, ihex_rec.data+4, 4);
                } else {
                    i = (ihex_rec.len < 4) ? ihex_rec.len : 4;
                    if ((prog_addr % 4) == 0) {
                        memcpy(&wr_data0, ihex_rec.data,  i);
                        wr_data1 = 0xffffff;
                    } else {
                        memcpy(&wr_data1, ihex_rec.data,  i);
                        wr_data0 = 0xffffff;
                    }
                }

                /* protect bootloader, reset vector and config bits */
                if((( (prog_addr + (i>>1) - 1) < BOOT_FLASH_START_ADDR) || (prog_addr > BOOT_FLASH_END_ADDR))
                   && (((prog_addr + (i>>1) - 1) < DEV_CONFIG_REG_BASE_ADDRESS)) && (prog_addr > 3)) {

                    ret = erase_addr(prog_addr);
                    ret += write_dword(prog_addr, wr_data0, wr_data1);
                }

                ihex_rec.addr.l += i;
                ihex_rec.data += i;

                ihex_rec.len -= i;
            }
            break;

        case EXT_SEG_ADRS_RECORD:
            ihex_rec.e_addr.b[3] = 0;
            ihex_rec.e_addr.b[2] = ihex_rec.data[0];
            ihex_rec.e_addr.b[1] = ihex_rec.data[1];
            ihex_rec.e_addr.b[0] = 0;
            //put_char('S');
            break;

        case EXT_LIN_ADRS_RECORD:
            ihex_rec.e_addr.b[3] = ihex_rec.data[0];
            ihex_rec.e_addr.b[2] = ihex_rec.data[1];
            ihex_rec.e_addr.w[0] = 0;
            //put_char('L');
            break;

        case END_OF_FILE_RECORD:
        default:
            put_char('E');
            ihex_rec.e_addr.l = 0;
            break;
    }

    return ihex_rec.type;
}

void load_ihex(void)
{
    char buf[MAX_IHEX_LINE_LEN], c;
    unsigned char i;
    int ret;

    i = 0;
    while (1) {
        ret = get_char(&c);
        if (ret) {
            put_char('0' + ret);
            continue;
        }

        if (c == ':') {
            i = 0;
            continue;
        }

        if (i < MAX_IHEX_LINE_LEN)
            buf[i++] = c;

        if (c == '\n') {
            LED = ~LED;
            ascii2hex((unsigned char *) buf);
            c = write_ihex2flash(buf);

            put_char('.');

            if (c == END_OF_FILE_RECORD)
                goto_usercode();
        }
    }

}

