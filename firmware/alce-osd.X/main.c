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
#pragma config ICS = PGD3 //NONE               // ICD Communication Channel Select bits (Reserved, do not use)
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


extern struct alceosd_config config;

//#define DEBUG_INIT

void hw_init(void)
{
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
    //OSCTUN = 0;
    /* wait for PLL to lock */
    //while(OSCCONbits.LOCK != 1) {};
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
}



int main(void) {
    extern int __C30_UART; __C30_UART = 2;
    unsigned char i;
    unsigned char in_config = 0;
    char c;

    /* generic hw init */
    hw_init();

    /* init uart2 */
    uart_init2(UART_PORT_TELEMETRY);

    /* init video driver */
    init_video();

    /* try to load config from flash */
    load_config();

    /* video driver config */
    video_apply_config(&config.video);

    /* load widget tab 1 */
    load_tab(1);

    /* enable all interrupts */
    SRbits.IPL = 0;
    CORCONbits.IPL3 = 1;

    /* TODO: rework config entry */
    /* check for config entry */
    for (i = 0; i < 8; i++) {
        while (uart_getc2(&c) == 0) {
            widgets_process();
            render_process();
            ClrWdt();
        }
        if (c != '!')
            break;
    }

    if (i == 8) {
        in_config = 1;
        while (in_config) {
            in_config = config_osd();
            widgets_process();
            render_process();
            ClrWdt();
        }
    }

    uart_set_baudrate2(config.baudrate);

    for (;;) {
        mavlink_process();
        widgets_process();
        render_process();
        ClrWdt();
    }

    return 0;
}

