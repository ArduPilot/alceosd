/*
    AlceOSD - Graphical OSD
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
#include <stdio.h>
#include <string.h>
#include "alce-osd.h"


#ifndef WITH_BOOTLOADER

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

#endif



void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        //Clear the trap flag
        U1TXREG = '!';
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _AddressError(void)
{
        INTCON1bits.ADDRERR = 0;        //Clear the trap flag
        U1TXREG = '"';
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _StackError(void)
{
        INTCON1bits.STKERR = 0;         //Clear the trap flag
        U1TXREG = '#';
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _MathError(void)
{
        INTCON1bits.MATHERR = 0;        //Clear the trap flag
        U1TXREG = '$';
        while (1);
}


//#define DEBUG_INIT

unsigned char hw_rev;

void hw_init(void)
{
    volatile unsigned int i;
    /* LED */
    LED_DIR = 0;
    LED = 0;

    /* make all ports digital */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

    /* for debug */
    TRISBbits.TRISB4 = 0;
    LATBbits.LATB4 = 0;


#ifndef WITH_BOOTLOADER
    /* oscillator config */
    /* Fosc = 140MHz (70MIPS) */
    /* Fsys = 280MHz */
    CLKDIVbits.PLLPRE = 0;
    CLKDIVbits.PLLPOST = 0;
    PLLFBDbits.PLLDIV = 74;

    /* switch clock to FRC oscillator with PLL */
    __builtin_write_OSCCONH(1);
    __builtin_write_OSCCONL(OSCCON | 1);

    /* wait for clock switch to complete */
    while (OSCCONbits.OSWEN == 1);
    /* wait for PLL lock */
    while (OSCCONbits.LOCK != 1);
#endif

    LED = 1;

    /* detect hw revision */
    /* set RB9 internal pull down */
    _TRISB9 = 1;
    _CNPUB9 = 0;
    _CNPDB9 = 1;

    /* set RA9 internal pull down */
    _TRISA9 = 1;
    _CNPUA9 = 0;
    _CNPDA9 = 1;
    for (i = 0; i < 10000; i++);
    
    if (_RB9 == 1)
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

    /* weak pull up on serial port rx pins */
    if (hw_rev == 0x03) {
        _CNPUB11 = 1;
        _CNPUB6 = 1;
        _CNPUB2 = 1;
        _CNPUB13 = 1;
    } else {
        _CNPUB6 = 1;
        _CNPUA4 = 1;
    }
}



int main(void) {
    /* generic hw init */
    hw_init();
    
    /* init uart */
    uart_init();

#ifdef DEBUG_INIT
    printf("AlceOSD hw%dv%d fw%d.%d.%d\r\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
    if (RCONbits.WDTO)
        printf("watchdog reset\r\n");
    if (RCONbits.EXTR)
        printf("external reset\r\n");
    if (RCONbits.SWR)
        printf("software reset\r\n");
    if (RCONbits.IOPUWR)
        printf("ill opcode / uninit W reset\r\n");
    if (RCONbits.WDTO)
        printf("trap conflict reset\r\n");
#endif

    /* real time clock init */
    clock_init();

    /* adc init */
    adc_init();

    /* init video driver */
    init_video();

    /* try to load config from flash */
    config_init();

    /* init widget modules */
    widgets_init();
    
    /* setup tabs */
    tabs_init();

    /* welcome message */
    console_printf("AlceOSD hw%dv%d fw%d.%d.%d\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);

    /* init home tracking */
    init_home();
    
    /* init flight status tracking */
    init_flight_stats();

    /* init mavlink module */
    mavlink_init();

    /* init uavtalk module */
    uavtalk_init();
    
    /* init frsky module */
    frsky_init();

    /* link serial ports to processes */
    uart_set_config_clients();

    /* enable all interrupts */
    _IPL = 0;
    _IPL3 = 1;

    console_printf("Processes running...\n");
    /* main loop */
    process_run();

    return 0;
}

