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


static u32union page[PAGE_SIZE];



void load_bin(void)
{
    unsigned char c;
    int ret;
    u32union page_addr;
    unsigned int crc, rcrc, i;

    for (i = 0; i < PAGE_SIZE; i++)
        page[i].b[3] = 0xff;


    while (1) {

        /* request page */

        /* 3 bytes - msb first */
        page_addr.b[3] = 0;
        while( get_char((char*) &page_addr.b[2]) == 0);
        while( get_char((char*) &page_addr.b[1]) == 0);
        while( get_char((char*) &page_addr.b[0]) == 0);

        if (page_addr.l == 0x00ffffff) {
            put_char('e');
            break;
        } else {
            put_char('p');
        }

        page_addr.l &= ~((PAGE_SIZE << 1)-1);

        if ( ((page_addr.l >= BOOT_FLASH_START_ADDR) && (page_addr.l < BOOT_FLASH_END_ADDR)) 
           || (page_addr.l == DEV_CONFIG_PAGE_ADDRESS) ) {
            put_char('s');
            continue;
        } else {
            put_char('d');
        }

        /* load page to sram */
        ret = 0;
        crc = 0;
        for (i = 0; i < PAGE_SIZE; i++) {
            while(get_char((char*) &c) == 0);
            crc += c;
            page[i].b[2] = c;
                    
            while(get_char((char*) &c) == 0);
            crc += c;
            page[i].b[1] = c;

            while(get_char((char*) &c) == 0);
            crc += c;
            page[i].b[0] = c;
        }

        /* get crc */
        put_char('c');
        rcrc = 0;
        while(get_char((char*) &c) == 0);
        rcrc = c << 8;
        while(get_char((char*) &c) == 0);
        rcrc |= c;

        if (rcrc != crc) {
            ret = 100;
            put_char('x');
        }

        if (ret)
            break;

        erase_page(page_addr.l);
        put_char('e');

        /* protect bootloader start addr */
        if (page_addr.l == 0) {
            page[0].l = 0x040800;
            page[1].l = 0x000000;
        }

        for (i = 0; i < PAGE_SIZE/2; i++) {
            write_dword(page_addr.l + i*4, page[i*2].l, page[i*2 + 1].l);
        }

        put_char('w');

    }

    if (ret) {
        put_char('X');
        while (1);
    } else {
        put_char('K');
        goto_usercode();
    }
}
