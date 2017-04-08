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
#define BOOT_DELAY                      1

const char magic_word_bin[] = "alceosd";
const char msg[] = "\r\nAlceOSD bootloader v0.6\r\n";
const char msg_bin[] = "BIN\r\n";

unsigned long devid;
unsigned char hw_rev;
unsigned char used_port = 0;

int main(void)
 {
    char c;
    unsigned char idx[4] = {0, 0, 0, 0};
    unsigned char port, total_ports = 2, valid_ports = 0x3;
    volatile unsigned long i, j;
    int ret = 0;
    u32union delay;

    RCONbits.SWDTEN = 0;

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

    /* detect hw revision */
    /* set RB1 internal pull down */
    _TRISB1 = 1;
    _CNPDB1 = 1;
    
    /* hw 0v5 has RB1 connected to RB4 */
    _TRISB4 = 0;
    _LATB4 = 0;
    for (i = 0; i < 100000; i++);
    j = (_RB1 & 1);
    _LATB4 = 1;
    for (i = 0; i < 100000; i++);
    j |= ((_RB1 & 1) << 1);
    
    if (j == 0b10) {
        hw_rev = 0x05;
    } else {
        /* set RB9 internal pull down */
        _TRISB9 = 1;
        _CNPDB9 = 1;

        /* set RA9 internal pull down */
        _TRISA9 = 1;
        _CNPDA9 = 1;

        ANSELBbits.ANSB1 = 1;
        AD1CON3bits.ADCS = 16;
        AD1CON2bits.CHPS = 2;
        AD1CON1bits.ADON = 1;
        AD1CHS0bits.CH0SA = 3;
        AD1CON1bits.SAMP = 1;
        for (i = 0; i < 1000; i++);
        AD1CON1bits.SAMP = 0;
        for (i = 0; i < 10000; i++);

        if (ADC1BUF0 > 400)
            /* hw 0v4 has 1.25V vref on AN3 */
            hw_rev = 0x04;
        else if (_RB9 == 1)
            /* hw 0v3 has external pull-up on RB9 */
            hw_rev = 0x03;
        else if (_RA9 == 1)
            /* hw 0v2 has RB9 floating and RA9 pull-up */
            hw_rev = 0x02;
        else
            /* hw 0v1 has RA9 and RB9 floating */
            hw_rev = 0x01;
        _CNPDB9 = 0;
        _CNPDA9 = 0;
        AD1CON1bits.ADON = 0;
    }
    _CNPDB1 = 0;
    _TRISB4 = 1;
    hw_rev = 0x05;
    
    /* weak pull up on serial port rx pins */
    if (hw_rev >= 0x05) {
        _CNPUB12 = 1;
        _CNPUB15 = 1;
        _CNPUC7 = 1;
        _CNPUB7 = 1;
    } else if (hw_rev >= 0x03) {
        _CNPUB11 = 1;
        _CNPUB6 = 1;
        _CNPUB2 = 1;
        _CNPUB13 = 1;
    } else {
        _CNPUB6 = 1;
        _CNPUA4 = 1;
    }

    if (hw_rev >= 0x03) {
        total_ports = 4;
        valid_ports = 0xf;
    }
    
    /* get devid */
    read_flash(0xff0000, &devid);
    
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

    while (1) {
        for (port = 0; port < total_ports; port++) {
            if (valid_ports & (1 << port)) {
                used_port = port;
                if (get_char(&c)) {
                    if (c != magic_word_bin[idx[port]])
                        valid_ports &= ~(1 << port);
                    else
                        idx[port]++;

                    if (idx[port] == (sizeof(magic_word_bin)-1)) {
                        ret = 1;
                        break;
                    }
                }
            }
        }

        if (valid_ports == 0)
            ret = 0xff;
        if (_T3IF == 1) {
            ret = 0xff;
        }
        if (ret)
            break;
    }
    T2CONbits.TON = 0;

    LED = 1;
    
    if (ret == 0xff) {
        put_char('*');
        goto_usercode();
    }
    
    put_str((char*) msg);
    put_str((char*) msg_bin);
    load_bin();
    return 0;
}
