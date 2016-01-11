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

#include "alce-osd.h"

static unsigned int buf[2];
static struct timer *adc_sampler_timer = NULL;

void adc_init(void)
{
    /* disable dma (for now) */
    AD1CON4bits.ADDMAEN = 0;
    
    /* 12 bit, 1 channel */
    AD1CON1bits.AD12B = 0;

    /* voltage reference = 0V ~ 3.3V */
    AD1CON2bits.VCFG = 0;
    
    /* clk from Tcy */
    AD1CON3bits.ADRC = 0;

    /* clk multiplier */
    AD1CON3bits.ADCS = 16;

    /* output data format - unsigned integer */
    AD1CON1bits.FORM = 0;

    /* ch1 and ch2 */
    AD1CON2bits.CHPS = 2;

    /* ch0 connected to temp diode */
    AD1CHS0bits.CH0SA = 0x1e;
    AD1CHS0bits.CH0NA = 0;

    /* ch1,2,3 = an0,1,2*/
#if defined (__dsPIC33EP512GM604__)
    AD1CHS123bits.CH123SA0 = 0;
    AD1CHS123bits.CH123SA1 = 0;
    AD1CHS123bits.CH123SA2 = 0;
    AD1CHS123bits.CH123NA = 0;
#else
    AD1CHS123bits.CH123SA = 0;
    AD1CHS123bits.CH123NA = 0;
#endif
    
    AD1CON1bits.ADON = 1;

    AD1CON1bits.ASAM = 1;
}


static void adc_sampler(void *data)
{
    if (_AD1IF) {
        _AD1IF = 0;
        buf[0] = ADC1BUF1;
        buf[1] = ADC1BUF2;
    } else {
        AD1CON1bits.SAMP = 0;
    }
}

void adc_start(unsigned int t)
{
    if (adc_sampler_timer == NULL)
        adc_sampler_timer = add_timer(TIMER_ALWAYS, t, adc_sampler, NULL);
}

void adc_link_ch(unsigned char ch, unsigned int **v)
{
    if (ch > 1)
        return;

    (*v) = &buf[ch];

    if (ch == 0) {
        ANSELAbits.ANSA0 = 1;
    } else {
        ANSELAbits.ANSA1 = 1;
    }
}

void adc_unlink_ch(unsigned char ch)
{
    if (ch > 1)
        return;
    if (ch == 0) {
        ANSELAbits.ANSA0 = 0;
    } else {
        ANSELAbits.ANSA1 = 0;
    }
}

void adc_stop(void)
{
    remove_timer(adc_sampler_timer);
    adc_sampler_timer = NULL;
    buf[0] = NULL;
    buf[1] = NULL;
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
}