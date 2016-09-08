/*
    AlceOSD - Graphical OSD
    Copyright (C) 2016  Luis Alves

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

#ifndef _UAV_H
#define	_UAV_H

enum {
    MAVDATA_HEARTBEAT,
    MAVDATA_SYS_STATUS,
    MAVDATA_ATTITUDE,
    MAVDATA_GLOBAL_POSITION_INT,
    MAVDATA_MISSION_ITEM,
    MAVDATA_VFR_HUD,
    MAVDATA_RC_CHANNELS_RAW,
    MAVDATA_RC_CHANNELS,
    MAVDATA_TOTAL,
};

struct mavdata_state {
    void *data;
    unsigned long time;
    unsigned long period;
    unsigned char id;
};

struct mavdata_decode_ops {
    void (*decode)(mavlink_message_t *msg, void *d);
    unsigned int offset;
    const mavlink_message_info_t *info;
};

void mavdata_store(mavlink_message_t *msg);
void* mavdata_get(unsigned int id);
unsigned long mavdata_time(unsigned int id);
unsigned long mavdata_age(unsigned int id);
unsigned long mavdata_period(unsigned int id);

void shell_cmd_mavdata(char *args, void *data);

#endif