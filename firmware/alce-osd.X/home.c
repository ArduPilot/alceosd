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

extern struct alceosd_config config;

/* data that is passed to widgets */
struct home_data home;

static struct home_priv {
    struct gps_coord home_coord, uav_coord;
    unsigned int altitude, home_altitude;
    int heading;
} priv;

struct home_data* get_home_data(void)
{
    return &home;
}

static void calc_home(struct timer *t, void *d)
{
    mavlink_heartbeat_t *hb = mavdata_get(MAVLINK_MSG_ID_HEARTBEAT);

    mavlink_message_t this_msg;
    
    switch (home.lock) {
        case HOME_NONE:
        default:
            if (mavdata_age(MAVLINK_MSG_ID_HEARTBEAT) > 5000)
                return;

            /* check arming status */
            if (hb->base_mode & MAV_MODE_FLAG_SAFETY_ARMED)
                home.lock = HOME_WAIT;
            
            break;
        case HOME_WAIT:
            if (mavdata_age(MAVLINK_MSG_ID_MISSION_ITEM) > 2000) {
                /* when UAV is armed, home is WP0 */
                mavlink_msg_mission_request_pack(config.mav.osd_sysid, MAV_COMP_ID_OSD, &this_msg,
                                    config.mav.uav_sysid, MAV_COMP_ID_ALL, 0);
                mavlink_send_msg(&this_msg);
            } else {
                mavlink_mission_item_t *mi = mavdata_get(MAVLINK_MSG_ID_MISSION_ITEM);
                priv.home_coord.lat = DEG2RAD(mi->x);
                priv.home_coord.lon = DEG2RAD(mi->y);
                priv.home_altitude = (unsigned int) mi->z;
                home.lock = HOME_GOT;
            }
            break;
        case HOME_GOT:
            home.lock = HOME_LOCKED;
            set_timer_period(t, 200);
            break;
        case HOME_LOCKED:
        {
            mavlink_global_position_int_t *gpi = mavdata_get(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
 
            priv.uav_coord.lat = DEG2RAD(gpi->lat / 10000000.0);
            priv.uav_coord.lon = DEG2RAD(gpi->lon / 10000000.0);
            priv.altitude = (unsigned int) (gpi->alt / 1000);
            priv.heading = (int) (gpi->hdg / 100);

            home.uav_bearing = (int) get_bearing(&priv.home_coord, &priv.uav_coord);

            home.direction = home.uav_bearing + 180;
            home.direction -= priv.heading;
            if (home.direction < 0)
                home.direction += 360;

            home.distance = earth_distance(&priv.home_coord, &priv.uav_coord);
            home.altitude = priv.altitude - priv.home_altitude;

            if ((hb->base_mode & MAV_MODE_FLAG_SAFETY_ARMED) == 0)
                home.lock = HOME_RESET;
            break;
        case HOME_RESET:
            home.lock = HOME_NONE;
            set_timer_period(t, 1000);
            break;
        }
        case HOME_FORCE:
            mavlink_global_position_int_t *gpi = mavdata_get(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
            priv.home_coord.lat = DEG2RAD(gpi->lat / 10000000.0);
            priv.home_coord.lon = DEG2RAD(gpi->lon / 10000000.0);
            priv.home_altitude = (unsigned int) (gpi->alt / 1000);
            home.lock = HOME_GOT;
            break;
    }
}

void init_home(void)
{
    home.lock = HOME_NONE;
    add_timer(TIMER_ALWAYS, 1000, calc_home, NULL);
}

static void shell_cmd_stats(char *args, void *data)
{
    switch (home.lock) {
        default:
        case HOME_NONE:
            shell_printf("No home; UAV not armed\n");
            break;
        case HOME_WAIT:
            shell_printf("Requesting WP0 (home)\n");
            break;
        case HOME_LOCKED:
            shell_printf("Home GPS coords: lat=%.6f lon=%.6f\n", priv.home_coord.lat, priv.home_coord.lon);
            shell_printf("Home M.S.L. altitude: %um\n\n", priv.home_altitude);
            shell_printf("Relative altitude to home: %um\n", home.altitude);
            shell_printf("Home direction: %d\n", home.direction);
            shell_printf("Home distance: %.2fm\n", home.distance);
            shell_printf("Home distance: %.2fm\n", home.distance);
            break;
    }
}

static void shell_cmd_lock(char *args, void *data)
{
    int v;
    
    if (strlen(args) > 0) {
        v = atoi(args);
        switch (v) {
            default:
            case 0:
                home.lock = HOME_RESET;
                break;
            case 1:
                home.lock = HOME_FORCE;
                break;
            case 2:
                home.lock = HOME_WAIT;
                break;
        }
    } else {
        shell_printf("syntax: home lock <value>\n");
        shell_printf("  Valid values:\n");
        shell_printf("     0     restart home locking\n");
        shell_printf("     1     force home to current GPS data\n");
        shell_printf("     2     bypass arming\n");
    }
}

static const struct shell_cmdmap_s home_cmdmap[] = {
    {"stats", shell_cmd_stats, "Home stats", SHELL_CMD_SIMPLE},
    {"lock", shell_cmd_lock, "Force home lock", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_home(char *args, void *data)
{
    shell_exec(args, home_cmdmap, data);
}
