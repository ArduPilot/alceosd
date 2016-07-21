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

#ifndef CONFIG_H
#define	CONFIG_H

#include "videocore.h"
#include "uart.h"
#include "widgets.h"
#include "tabs.h"
#include "mavlink.h"


#define CONFIG_MAX_WIDGETS      100
#define CONFIG_MAX_VIDEO        2


/* widget config */
#define JUST_TOP        0x0
#define JUST_BOT        0x1
#define JUST_VCENTER    0x2
#define JUST_LEFT       0x0
#define JUST_RIGHT      0x4
#define JUST_HCENTER    0x8

#define TABS_END        (0xff)

enum {
    VJUST_TOP = 0,
    VJUST_BOT = 1,
    VJUST_CENTER = 2,
};

enum {
    HJUST_LEFT = 0,
    HJUST_RIGHT = 1,
    HJUST_CENTER = 2,
};

enum {
    UNITS_DEFAULT = 0,
    UNITS_METRIC,
    UNITS_IMPERIAL,
    UNITS_CUSTOM_1,
    UNITS_CUSTOM_2,
    UNITS_CUSTOM_3,
    UNITS_CUSTOM_4,
    UNITS_CUSTOM_5,
};


struct alceosd_config {
    /* uart config */
    struct uart_config uart[4];

    /* video config */
    struct video_config video[CONFIG_MAX_VIDEO];

    /* tab changing config */
    struct tab_change_config tab_change;

    /* default unit system */
    unsigned char default_units;

    /* home position locking timer */
    unsigned char home_lock_sec;

    /* mavlink config */
    struct mavlink_config mav;
    
    /* widgets config */
    struct widget_config widgets[CONFIG_MAX_WIDGETS];
};


void config_init(void);
void shell_cmd_cfg(char *args, void *data);

unsigned char get_units(struct widget_config *cfg);


#endif
