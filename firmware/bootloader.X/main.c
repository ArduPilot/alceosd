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

#include <xc.h>
#include <string.h>
#include <p33Exxxx.h>

// FICD
#pragma config ICS = NONE
#pragma config JTAGEN = OFF

// FPOR
#pragma config ALTI2C1 = OFF
#pragma config ALTI2C2 = OFF
#pragma config WDTWIN = WIN25

// FWDT
#pragma config WDTPOST = PS32768
#pragma config WDTPRE = PR128
#pragma config PLLKEN = OFF
#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF

// FOSC
#pragma config POSCMD = NONE
#pragma config OSCIOFNC = ON
#pragma config IOL1WAY = OFF
#pragma config FCKSM = CSECMD

// FOSCSEL
#pragma config FNOSC = FRC
#pragma config IESO = OFF

// FGS
#pragma config GWRP = OFF
#pragma config GCP = OFF


/* bootloader config */
#define FCY                             70000000
#define BOOT_DELAY                      2
#define BAUDRATE                        115200
#define BRGVAL                          37 /* bgrval = ((FCY/BAUDRATE)/16)-1 */

#define BOOT_FLASH_START_ADDR           (0x000800)
#define BOOT_FLASH_END_ADDR             (0x000fff)
#define DEV_CONFIG_REG_BASE_ADDRESS 	(0x02AFEC)
#define DEV_CONFIG_REG_END_ADDRESS   	(0x02AFFE)
#define USER_APP_RESET_ADDRESS          (0x001000)

#define PAGE_SIZE                       (1024)
#define TOTAL_PAGES                     (85)

/* intel hex record types */
#define DATA_RECORD                     0x00
#define END_OF_FILE_RECORD              0x01
#define EXT_SEG_ADRS_RECORD             0x02
#define EXT_LIN_ADRS_RECORD             0x04

/* board config */
#define TRIS_LED1                       TRISAbits.TRISA10
#define LED1                            LATAbits.LATA10

#define MAX_IHEX_LINE_LEN               50


typedef union {
    unsigned long l;
    unsigned int  w[2];
    unsigned char b[4];
} u32union;

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

static unsigned long erased_page[TOTAL_PAGES];

const char magic_word[] = "alceosd";
const char msg[] = "\r\nAlceOSD bootloader v0.1\r\nWaiting for intel hex...";


int get_char(char *c)
{
    int ret = 0;

    while (1) {
        if (IFS0bits.T3IF == 1) {
            /* boot timer expired */
            ret = 1;
            T2CONbits.TON = 0;
            *c = 0xff;
        }
        if (U2STAbits.FERR == 1) {
            /* uart rx err */
            ret = 2;
        }
        if (U2STAbits.OERR == 1) {
            /* overrun error */
            ret = 3;
            /* clear to continue rx */
            U2STAbits.OERR = 0;
        }
        if (ret)
            break;

        if (U2STAbits.URXDA == 1) {
            T2CONbits.TON = 0;
            *c = U2RXREG;
            break;
        }
    }
    return ret;
}

void put_char(char c)
{
    while (!U2STAbits.TRMT);
    U2TXREG = c;
}

void put_str(char *c)
{
    while (*c != '\0')
        put_char(*c++);
}

int erase_page(unsigned long erase_address)
{
    u32union addr;
    addr.l = erase_address & 0xfff800;

    NVMADRU = addr.w[1];
    NVMADR = addr.w[0];
    NVMCON = 0x4003;

    INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while (NVMCONbits.WR == 1) {}
    INTCON2bits.GIE = 1;

    return NVMCONbits.WRERR;
}

int write_dword(unsigned long addr, unsigned long data0, unsigned long data1)
{
    u32union wr_addr;
    u32union wr_data0, wr_data1;

    wr_addr.l = addr;
    wr_data0.l = data0;
    wr_data1.l = data1;

    NVMCON = 0x4001;
    NVMADRU = wr_addr.w[1];
    NVMADR = wr_addr.w[0];

    TBLPAG = 0xFA;
    __builtin_tblwtl(0, wr_data0.w[0]);
    __builtin_tblwth(1, wr_data0.w[1]);
    __builtin_tblwtl(2, wr_data1.w[0]);
    __builtin_tblwth(3, wr_data1.w[1]);

    INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);
    INTCON2bits.GIE = 1;

    return NVMCONbits.WRERR;
}

int erase_addr(unsigned long addr)
{
    unsigned long page = (addr >> 11);
    int ret = 0;

    if (erased_page[page])
        return ret;

    ret = erase_page(addr);
    if (ret)
        return ret;
    erased_page[page] = 1;
    put_char('e');
    if (page == 0)
        ret = write_dword(0, 0x040800, 0x000000);
    return ret;
}

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

void goto_usercode(void)
{	
    void (*fptr)(void);
    LED1 = 1;
    fptr = (void (*)(void))USER_APP_RESET_ADDRESS;
    fptr();
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
                if ((ihex_rec.len > 7) && ((prog_addr % 8) == 0)) {
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
                   && (((prog_addr + (i>>1) - 1) < DEV_CONFIG_REG_BASE_ADDRESS) || (prog_addr > DEV_CONFIG_REG_END_ADDRESS))
                   && (prog_addr > 3)) {

                    ret = erase_addr(prog_addr);
                    ret += write_dword(prog_addr, wr_data0, wr_data1);
                    if (ret)
                        put_char('+');
                    else
                        put_char('.');
                } else {
                    put_char('p');                        
                }

                ihex_rec.addr.l += 4;
                ihex_rec.data += 4;

                if(ihex_rec.len > 3)
                    ihex_rec.len -= 4;
                else
                    ihex_rec.len = 0;
            }
            break;

        case EXT_SEG_ADRS_RECORD:
            ihex_rec.e_addr.b[3] = 0;
            ihex_rec.e_addr.b[2] = ihex_rec.data[0];
            ihex_rec.e_addr.b[1] = ihex_rec.data[1];
            ihex_rec.e_addr.b[0] = 0;
            put_char('S');
            break;

        case EXT_LIN_ADRS_RECORD:
            ihex_rec.e_addr.b[3] = ihex_rec.data[0];
            ihex_rec.e_addr.b[2] = ihex_rec.data[1];
            ihex_rec.e_addr.w[0] = 0;
            put_char('L');
            break;

        case END_OF_FILE_RECORD:
        default:
            put_char('E');
            ihex_rec.e_addr.l = 0;
            break;
    }

    return ihex_rec.type;
}	


int main(void)
 {
    char c;
    unsigned char i;
    int ret;
    u32union delay;
    char buf[MAX_IHEX_LINE_LEN];

    /* 70 MIPS; 140MHz */
    CLKDIVbits.PLLPRE = 0;
    PLLFBDbits.PLLDIV = 74;
    CLKDIVbits.PLLPOST = 0;

    /* switch clock to FRC oscillator with PLL */
    __builtin_write_OSCCONH(1);
    __builtin_write_OSCCONL(OSCCON | 1);

    /* wait for clock switch to complete */
    while (OSCCONbits.OSWEN == 1);
    /* wait for PLL lock */
    while (OSCCONbits.LOCK != 1);

    ANSELA = 0;
    ANSELB = 0;

    TRIS_LED1 = 0;
    LED1 = 0;

    delay.l = BOOT_DELAY;

    if (delay.b[0] == 0)
        goto_usercode();

    /* Timer setup */
    /* increments every instruction cycle */
    T2CONbits.T32 = 1;
    /* clear timer3 IF */
    IFS0bits.T3IF = 0;
    /* disable timer3 isr */
    IEC0bits.T3IE = 0; 
    /* Convert seconds into timer count value */
    delay.l = ((unsigned long) (FCY)) * ((unsigned long) (delay.b[0]));
    PR3 = delay.w[1];
    PR2 = delay.w[0];
    TMR3HLD = 0x0000;
    TMR2 = 0x0000;
    T2CONbits.TON = 1;

    /* uart setup */
    TRISBbits.TRISB5 = 0;
    _RP37R = 3;
    _U2RXR = 38;
    /* set baudrate  */
    U2BRG = BRGVAL;
    U2MODE = 0x8000;
    U2MODEbits.BRGH = 0;
    U2STA = 0x0400;

    for (i = 0; i < TOTAL_PAGES; i++)
        erased_page[i] = 0;

    for (i = 0; i < 7; i++) {
        ret = get_char(&c);
        if (ret || (c != magic_word[i])) {
            put_char('*');
            goto_usercode();
        }
    }
    put_str((char*) msg);

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
            LED1 = 1;
            ascii2hex((unsigned char *) buf);
            c = write_ihex2flash(buf);
            LED1 = 0;

            if (c == END_OF_FILE_RECORD)
                goto_usercode();
        }
    }
}
