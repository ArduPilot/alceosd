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


// FICD
#pragma config ICS = NONE
#pragma config JTAGEN = OFF

// FPOR
#if defined(__dsPIC33EP512GM604__)
#pragma config BOREN = OFF
#endif
#pragma config ALTI2C1 = ON
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
#if (defined(__dsPIC33EP512MC504__) || defined(__dsPIC33EP512GM604__))
#pragma config PWMLOCK = OFF
#endif
#pragma config IESO = OFF

// FGS
#pragma config GWRP = OFF
#pragma config GCP = OFF


/* bootloader config */
#define BOOT_DELAY                      2

#define IHEX_MODE                       0x01
#define BIN_MODE                        0x02


const char magic_word_ihex[] = "alceOSD";
const char magic_word_bin[] = "alceosd";
const char msg[] = "\r\nAlceOSD bootloader v0.3\r\n";
const char msg_ihex[] = "IHEX\r\n";
const char msg_bin[] = "BIN\r\n";


int main(void)
 {
    char c;
    unsigned char i;
    int ret, mode;
    u32union delay;

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
    ANSELC = 0;

    TRIS_LED = 0;
    LED = 0;

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

    uart_init();

    mode = IHEX_MODE | BIN_MODE;

    for (i = 0; i < 7; i++) {
        ret = get_char(&c);
        if (ret)
            break;

        if (c != magic_word_ihex[i])
            mode &= ~IHEX_MODE;

        if (c != magic_word_bin[i])
            mode &= ~BIN_MODE;

        if (mode == 0) {
            ret = 5;
            break;
        }
    }

    if (ret) {
        put_char('*');
        goto_usercode();
    }
    
    put_str((char*) msg);

    if (mode == IHEX_MODE) {
        put_str((char*) msg_ihex);
        load_ihex();
    } else {
        put_str((char*) msg_bin);
        load_bin();
    }
    return 0;
}
