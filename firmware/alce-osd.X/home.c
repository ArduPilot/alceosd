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


#define LOCK_FIX    0x01
#define LOCK_POS    0x02
#define LOCK_ALT    0x04
#define LOCKED      0x07


/* data that is passed to widgets */
struct home_data_s {
    unsigned int altitude;
    int direction;
    float distance;

    unsigned char lock;
    
} home_data;


static struct home_priv {
    struct gps_coord home_coord, uav_coord;
    unsigned char gps_fix_type;
    unsigned int altitude, home_altitude;
} priv;


static void store_mavdata(mavlink_message_t *msg, mavlink_status_t *status)
{

    switch (msg->msgid) {
        case MAVLINK_MSG_ID_VFR_HUD:
            priv.altitude = (unsigned int) mavlink_msg_vfr_hud_get_alt(msg);
            break;
        case MAVLINK_MSG_ID_GPS_RAW_INT:
            priv.uav_coord.lat = DEG2RAD(mavlink_msg_gps_raw_int_get_lat(msg) / 10000000.0);
            priv.uav_coord.lon = DEG2RAD(mavlink_msg_gps_raw_int_get_lon(msg) / 10000000.0);
            priv.gps_fix_type = mavlink_msg_gps_raw_int_get_fix_type(msg);
            break;
        default:
            break;
    }
}

static void calc_home(void *d)
{
    home_data.direction = (int) get_heading(&priv.home_coord, &priv.uav_coord);
    home_data.distance = earth_distance(&priv.home_coord, &priv.uav_coord);
    home_data.altitude = priv.altitude - priv.home_altitude;
}


void find_home(struct timer *t, void *d)
{
    static unsigned char sec = 0;
    
    /* stable timer = 15 sec */
    if (sec < 15) {
        if (sec == 0){
            memcpy(&priv.home_coord, &priv.uav_coord, sizeof(struct gps_coord));
            priv.home_altitude = priv.altitude;
        }

        sec++;
        
        /* GPD 2D fix for 5 sec */
        if (priv.gps_fix_type > 1) {
            home_data.lock |= LOCK_FIX;
        } else {
            home_data.lock &= ~LOCK_FIX;
            sec = 0;
        }

        /* GPS position */
        if (home_data.distance <= 1) {
            home_data.lock |= LOCK_POS;
        } else {
            home_data.lock &= ~LOCK_POS;
            sec = 0;
        }

        /* Altitude */
        if (home_data.altitude <= 1) {
            home_data.lock |= LOCK_ALT;
        } else {
            home_data.lock &= ~LOCK_ALT;
            sec = 0;
        }
    } else {
        /* locked */
        remove_timer(t);
    }
}


void init_home_process(void)
{
    home_data.lock = 0;

    /* do home calculations in a 500ms interval */
    add_timer(TIMER_ALWAYS, 5, calc_home, NULL);

    /* find home position */
    add_timer(TIMER_ALWAYS, 10, find_home, NULL);

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, store_mavdata, CALLBACK_PERSISTENT);
    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, store_mavdata, CALLBACK_PERSISTENT);
}
