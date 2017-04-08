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
#pragma config ICS = PGD1
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


#define TRAP_ISR __attribute__((no_auto_psv,__interrupt__(__preprologue__( \
 "mov #_tos,w1\n\tmov w15,[w1]\n\tmov #_StkAddrHi,w1\n\tpop [w1--]\n\tpop [w1++]\n\tpush [w1--]\n\tpush [w1++]"))))
 unsigned int StkAddrLo;
 unsigned int StkAddrHi;
 unsigned int *tos;
 
#define BLINK { \
    LED = 0; for (i = 0; i < 1000000; i++); \
    LED = 1; for (i = 0; i < 1000000; i++); \
}

void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void)
{
    volatile unsigned long i;
    INTCON1bits.OSCFAIL = 0;
    U1TXREG = '!';
    while (1) {
        LED = 1; for (i = 0; i < 3000000; i++);
        BLINK;
    };
}

#if 0
void __attribute__((interrupt,no_auto_psv)) _AddressError(void)
{
    INTCON1bits.ADDRERR = 0;
    while(1);
}
#else


void debug_putc(char c)
{
    while (!U1STAbits.TRMT);
    U1TXREG = c;
    while (!U2STAbits.TRMT);
    U2TXREG = c;
}
void debug_puts(char *c)
{
    while (*c != 0) {
        if (*c == '\n')
            debug_putc('\r');
        debug_putc(*(c++));
        
    }
}

void stack_dump(unsigned int *stack_pos)
{
    unsigned char i;
    //unsigned int *stack_pos;
    unsigned int *p, *t;
    unsigned char *c;
    char buf[100];
    
    asm volatile("mov.w #__SP_init,%0" : "=r"(p));
    asm volatile("mov.w #__SPLIM_init,%0" : "=r"(t));

    //asm volatile("mov.w W15,%0" : "=r"(stack_pos));
    
    c = (unsigned char*) p;
    
    snprintf(buf, 100, "\n\nStack dump :: [0x%4p--->0x%4p   0x%4p]\n\n", p, stack_pos, t);
    debug_puts(buf);
    i = 0;
    snprintf(buf, 100, "0x%4p | ", p);
    debug_puts(buf);
    while (p < stack_pos) {
        snprintf(buf, 100, "%04x ", *p++);
        debug_puts(buf);
        if (i++ == 7) {
            for (i = 0; i < 16; i++) {
                if ((*c) > 20)
                    debug_putc(*c);
                else
                    debug_putc('.');
                c++;
            }
            snprintf(buf, 100, "\n0x%4p : ", p);
            debug_puts(buf);
            i = 0;
        }
    }
}


void TRAP_ISR _AddressError(void)
{
    volatile unsigned long i;
    unsigned char c, j;
    char buf[100];
    INTCON1bits.ADDRERR = 0;
    
    debug_putc('"');

    for (j = 0; j < 4; j++) {
        c = (StkAddrHi >> ((3-j)*4)) & 0xf;
        if (c > 9)
            c += 'A' - 10;
        else
            c += '0';
        debug_putc(c);
    }
    for (j = 0; j < 4; j++) {
        c = (StkAddrLo >> ((3-j)*4)) & 0xf;
        if (c > 9)
            c += 'A' - 10;
        else
            c += '0';
        debug_putc(c);
    }
    snprintf(buf, 100, "\r\n\nAddressError @ 0x%04x%04x\n", StkAddrHi, StkAddrLo);
    debug_puts(buf);
    
    stack_dump(tos);
            
    while (1) {
        LED = 1; for (i = 0; i < 3000000; i++);
        BLINK; BLINK;
    };
}
#endif
void __attribute__((interrupt,no_auto_psv)) _StackError(void)
{
    volatile unsigned long i;
    INTCON1bits.STKERR = 0;
    U1TXREG = '#';
    while (1) {
        LED = 1; for (i = 0; i < 3000000; i++);
        BLINK; BLINK; BLINK;
    };
}

void __attribute__((interrupt,no_auto_psv)) _MathError(void)
{
    volatile unsigned long i;
    INTCON1bits.MATHERR = 0;
    U1TXREG = '$';
    while (1) {
        LED = 1; for (i = 0; i < 3000000; i++);
        BLINK; BLINK; BLINK; BLINK;
    };
}


//#define DEBUG_INIT

unsigned char hw_rev;

void hw_init(void)
{
    volatile unsigned long i, j;
    /* LED */
    LED_DIR = 0;
    LED = 0;

    /* make all ports digital */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

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

    /* enable all interrupts */
    _IPL = 0;
    _IPL3 = 1;
}

void clear_wdt(struct timer *t, void *d)
{
    ClrWdt();
}

int main(void)
{
    /* generic hw init */
    hw_init();

    /* init uart */
    uart_init();

    /* init shell */
    shell_init();

#ifdef DEBUG_INIT
    shell_printf("AlceOSD hw%dv%d fw%d.%d.%d\r\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
    if (RCONbits.WDTO)
        shell_printf("watchdog reset\r\n");
    if (RCONbits.EXTR)
        shell_printf("external reset\r\n");
    if (RCONbits.SWR)
        shell_printf("software reset\r\n");
    if (RCONbits.IOPUWR)
        shell_printf("ill opcode / uninit W reset\r\n");
    if (RCONbits.WDTO)
        shell_printf("trap conflict reset\r\n");
#endif

    /* real time clock init */
    clock_init();

    /* adc init */
    adc_init();

    /* init video driver */
    init_video();

    /* try to load config from flash */
    config_init();

    /* init mavlink module */
    mavlink_init();

    /* init mavdata */
    mavdata_init();

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

    /* init uavtalk module */
    uavtalk_init();

    /* init frsky module */
    frsky_init();

    /* link serial ports to processes */
    uart_set_config_clients();

    /* add watchdog timer */
    add_timer(TIMER_ALWAYS, 60000, clear_wdt, NULL);

    /* main loop */
    console_printf("Processes running...\n");
    process_run();

    return 0;
}
