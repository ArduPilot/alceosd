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
#include "widgets.h"


#define CONFIG_MAX_WIDGETS      (100)


struct alceosd_config {
    /* telemetry baudrate */
    unsigned int baudrate;

    /* video config */
    struct video_config video;

    /* tab change channel */
    unsigned int tab_change_ch_min;
    unsigned int tab_change_ch_max;
    unsigned char tab_change_ch;

    /* vehicle type */
    unsigned char vehicle;

    /* widgets config */
    struct widget_config widgets[CONFIG_MAX_WIDGETS];
};


void load_config(void);
int config_osd(void);

#endif
