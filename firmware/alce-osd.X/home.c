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

static struct home_priv {
    struct gps_coord home_coord, uav_coord;
    unsigned int altitude, home_altitude;
    int heading;
} priv;

extern struct alceosd_config config;

/* data that is passed to widgets */
struct home_data home;

struct home_data* get_home_data(void)
{
    return &home;
}

static void calc_home(void *d)
{
    home.uav_bearing = (int) get_bearing(&priv.home_coord, &priv.uav_coord);

    home.direction = home.uav_bearing + 180;
    home.direction -= priv.heading;
    if (home.direction < 0)
        home.direction += 360;

    home.distance = earth_distance(&priv.home_coord, &priv.uav_coord);
    home.altitude = priv.altitude - priv.home_altitude;
}

static void store_global_position(mavlink_message_t *msg, void *d)
{
    priv.uav_coord.lat = DEG2RAD(mavlink_msg_global_position_int_get_lat(msg) / 10000000.0);
    priv.uav_coord.lon = DEG2RAD(mavlink_msg_global_position_int_get_lon(msg) / 10000000.0);
    priv.altitude = (unsigned int) (mavlink_msg_global_position_int_get_alt(msg) / 1000);
    priv.heading = (int) (mavlink_msg_global_position_int_get_hdg(msg) / 100);
}

void home_cbk(mavlink_message_t *msg, void *d)
{
    priv.home_coord.lat = DEG2RAD(mavlink_msg_mission_item_get_x(msg));
    priv.home_coord.lon = DEG2RAD(mavlink_msg_mission_item_get_y(msg));
    priv.home_altitude = (unsigned int) mavlink_msg_mission_item_int_get_z(msg);
    
    if (home.lock == HOME_LOCKED)
        del_mavlink_callbacks(CALLBACK_HOME_INIT);
}

void lock_home_cbk(mavlink_message_t *msg, void *d)
{
    mavlink_message_t this_msg;
    unsigned char armed = mavlink_msg_heartbeat_get_base_mode(msg) & 
                            MAV_MODE_FLAG_SAFETY_ARMED;
    
    /* when UAV is armed, home is stored at WP0 */
    mavlink_msg_mission_request_pack(config.mav.osd_sysid, MAV_COMP_ID_OSD, &this_msg,
                        config.mav.uav_sysid, MAV_COMP_ID_ALL, 0);
    mavlink_send_msg(&this_msg);

    if (armed)
        home.lock = HOME_LOCKED;
}

void init_home(void)
{
    home.lock = 0;
    add_mavlink_callback(MAVLINK_MSG_ID_HEARTBEAT, lock_home_cbk, CALLBACK_HOME_INIT, NULL);
    add_mavlink_callback(MAVLINK_MSG_ID_MISSION_ITEM, home_cbk, CALLBACK_HOME_INIT, NULL);
    
    add_mavlink_callback(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, store_global_position, CALLBACK_PERSISTENT, NULL);
    add_timer(TIMER_ALWAYS, 200, calc_home, NULL);
}
