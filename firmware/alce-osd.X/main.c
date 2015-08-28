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

// DSPIC33EP512GP504 Configuration Bit Settings

// FICD
#pragma config ICS = PGD1 //NONE               // ICD Communication Channel Select bits (Reserved, do not use)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = OFF             // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC) Oscillator)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

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

#ifdef DEBUG_INIT
    printf("AlceOSD\r\n");
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

    LED = 1;

    /* detect hw revision */
    _TRISA9 = 1;
    _CNPUA9 = 0;
    _CNPDA9 = 1;
    for (i = 0; i < 10000; i++);
    if (_RA9 == 1)
        hw_rev = 0x02;
    else
        hw_rev = 0x01;
    _CNPDA9 = 0;
}



int main(void) {
    /* generic hw init */
    hw_init();
    
    /* init uart */
    uart_init();

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

    /* enable all interrupts */
    _IPL = 0;
    _IPL3 = 1;

    /* main loop */
    process_run();

    return 0;
}

