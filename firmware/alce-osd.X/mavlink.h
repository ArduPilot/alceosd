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

#ifndef _MAVLINK_H
#define	_MAVLINK_H

#define MAVLINK_COMM_NUM_BUFFERS 4
#include "mavlink/v1.0/ardupilotmega/mavlink.h"

#define MAV_SYS_ID_ANY          (0)
#define MAV_TYPE_ALCEOSD        (27)

struct mavlink_config {
    unsigned char streams[8];
    unsigned char uav_sysid, osd_sysid;
};

struct mavlink_callback {
    unsigned char msgid;
    unsigned char sysid;
    unsigned char type;
    void *data;
    void (*cbk) (mavlink_message_t *msg, void *data);
};


void mavlink_init(void);
struct mavlink_callback* add_mavlink_callback(unsigned char msgid,
        void *cbk, unsigned char ctype, void *data);
struct mavlink_callback* add_mavlink_callback_sysid(unsigned char sysid,
        unsigned char msgid,
        void *cbk, unsigned char ctype, void *data);
void del_mavlink_callbacks(unsigned char ctype);
void mavlink_handle_msg(unsigned char ch, mavlink_message_t *msg);
void mavlink_send_msg(mavlink_message_t *msg);

void shell_cmd_mavlink(char *args, void *data);

/* aditional helper functions */
unsigned int mavlink_msg_rc_channels_raw_get_chan(mavlink_message_t *msg, unsigned char ch);


enum {
    CALLBACK_WIDGET = 0,
    CALLBACK_PERSISTENT,
    CALLBACK_HOME_INIT,
    CALLBACK_WATCH,
};

enum {
    PLANE_MODE_MANUAL     = 0,
    PLANE_MODE_CIRCLE     = 1,
    PLANE_MODE_STABILIZE  = 2,
    PLANE_MODE_TRAINING   = 3,
    PLANE_MODE_ACRO       = 4,
    PLANE_MODE_FBWA       = 5,
    PLANE_MODE_FBWB       = 6,
    PLANE_MODE_CRUISE     = 7,
    PLANE_MODE_AUTOTUNE   = 8,
    PLANE_MODE_AUTO       = 10,
    PLANE_MODE_RTL        = 11,
    PLANE_MODE_LOITER     = 12,
    PLANE_MODE_GUIDED     = 15,
    PLANE_MODE_INIT       = 16,
    COPTER_MODE_STABILIZE = 100,
    COPTER_MODE_ACRO      = 101,
    COPTER_MODE_ALTHOLD   = 102,
    COPTER_MODE_AUTO      = 103,
    COPTER_MODE_GUIDED    = 104,
    COPTER_MODE_LOITER    = 105,
    COPTER_MODE_RTL       = 106,
    COPTER_MODE_CIRCLE    = 107,
    COPTER_MODE_LAND      = 109,
    COPTER_MODE_OF_LOITER = 110,
    COPTER_MODE_DRIFT     = 111,
    COPTER_MODE_SPORT     = 113,
    COPTER_MODE_FLIP      = 114,
    COPTER_MODE_AUTOTUNE  = 115,
    COPTER_MODE_POSHOLD   = 116,
};

#endif	/* _MAVLINK_H */

