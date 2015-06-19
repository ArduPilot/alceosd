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

volatile unsigned long millis = 0;

unsigned long get_millis(void)
{
    unsigned long m;
    IEC0bits.T1IE = 0;
    m = millis;
    IEC0bits.T1IE = 1;
    return m;
}

void clock_init(void)
{
    T1CON = 0x8010;
    IPC0bits.T1IP = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    /* period = 1 / (70000000 / 8) * 8750 = 1ms */
    PR1 = 8750;
}

void __attribute__((__interrupt__, no_auto_psv )) _T1Interrupt()
{
    jiffies++;
    IFS0bits.T1IF = 0;
}
