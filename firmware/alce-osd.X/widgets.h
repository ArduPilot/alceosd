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

#ifndef WIDGETS_H
#define	WIDGETS_H

#include "videocore.h"


enum {
    WIDGET_ALTITUDE_ID = 0,
    WIDGET_HORIZON_ID,
    WIDGET_BATTERY_INFO_ID,
    WIDGET_COMPASS_ID,
    WIDGET_FLIGHT_MODE_ID,
    WIDGET_GPS_INFO_ID,
    WIDGET_RSSI_ID,
    WIDGET_SPEED_ID,
    WIDGET_THROTTLE_ID,
    WIDGET_VARIOMETER_ID,
    WIDGET_RC_CHANNELS_ID,
    WIDGET_HOME_INFO_ID,
    WIDGET_RADAR_ID,
    WIDGET_WIND_ID,
    WIDGET_FLIGHT_INFO_ID,
    WIDGET_CONSOLE_ID,
};

#define WIDGET_SCHEDULED    (0x1)

typedef union {
    unsigned int raw;
    struct {
        unsigned vjust:2;
        unsigned hjust:2;
        unsigned mode:4;
        unsigned units:3;
        unsigned source:3;
        unsigned :2;
    };
} widget_props;


struct widget_config {
    unsigned char tab;
    unsigned char uid;
    unsigned char widget_id;
    int x, y;
    widget_props props;
    unsigned int w, h;
    unsigned int params[4];
};

struct widget;

struct widget_ops {
    unsigned int id;
    char name[20];
    char mavname[8];

    void (*init)(void);
    int (*open)(struct widget *w);
    void (*render)(struct widget *w);
    void (*close)(struct widget *w);
};

struct widget {
    const struct widget_ops *ops;
    struct widget_config *cfg;
    void *priv;
    struct canvas ca;
    unsigned int status;
};

void widgets_init(void);
void widgets_reset(void);
void widgets_process(void);
void schedule_widget(struct widget *w);
const struct widget_ops *get_widget_ops(unsigned int id);
void* widget_malloc(unsigned int size);

#endif
