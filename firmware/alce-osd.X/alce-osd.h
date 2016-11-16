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

#ifndef ALCE_OSD_H
#define	ALCE_OSD_H

#include <p33Exxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#include "graphics.h"
#include "config.h"
#include "widgets.h"
#include "videocore.h"
#include "mavlink.h"
#include "uart.h"
#include "clock.h"
#include "alce-math.h"
#include "home.h"
#include "flight_stats.h"
#include "fonts.h"
#include "adc.h"
#include "tabs.h"
#include "flash.h"
#include "params.h"
#include "uavtalk.h"
#include "frsky.h"
#include "process.h"
#include "shell.h"
#include "mavdata.h"


#define VERSION_MAJOR   0
#define VERSION_MINOR   11
#define VERSION_DEV     0

#define WITH_BOOTLOADER

/* maximum line length for print function stack buffers */
#define MAX_LINE_LENGTH 128

/* led on the board */
#define LED_DIR     TRISAbits.TRISA10
#define LED         LATAbits.LATA10


typedef union {
    unsigned long l;
    unsigned int  w[2];
    unsigned char b[4];
} u32union __attribute__((aligned(2)));

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef char s8;
typedef int s16;
typedef long s32;
typedef long long s64;

extern void console_print(char *str);
extern int console_printf(const char *fmt, ...);

#endif	/* ALCE_OSD_H */

