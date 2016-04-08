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

struct flight_stats stats;
extern struct home_data home;

#define NO_HEADING 0xfff


struct stats_priv {
    unsigned int throttle;
    int airspeed, groundspeed;
    int alt;
    int heading;

    unsigned int bat_current;
} priv;


struct flight_stats* get_flight_stats(void)
{
    return &stats;
}

static void store_mavdata(mavlink_message_t *msg, void *d)
{
    switch (msg->msgid) {
        case MAVLINK_MSG_ID_VFR_HUD:
            priv.throttle = mavlink_msg_vfr_hud_get_throttle(msg);
            priv.airspeed = (int) mavlink_msg_vfr_hud_get_airspeed(msg);
            priv.groundspeed = (int) mavlink_msg_vfr_hud_get_groundspeed(msg);
            priv.alt = (int) mavlink_msg_vfr_hud_get_alt(msg);
            priv.heading = mavlink_msg_vfr_hud_get_heading(msg);
            break;
        case MAVLINK_MSG_ID_SYS_STATUS:
            priv.bat_current = mavlink_msg_sys_status_get_current_battery(msg);
            break;
        default:
            break;
    }
}

static void find_launch_heading(struct timer *t, void *d);

static void calc_stats(struct timer *t, void *d)
{
    struct home_data *home = get_home_data();
    
    /* accumulate distance */
    stats.total_distance += ((float) priv.groundspeed / 10);
    stats.total_mah += ((float) priv.bat_current) / 3600;

    stats.max_air_speed = MAX(priv.airspeed, stats.max_air_speed);
    stats.max_gnd_speed = MAX(priv.groundspeed, stats.max_gnd_speed);
    stats.max_altitude  = MAX(priv.alt, stats.max_altitude);
    stats.max_home_distance = MAX((unsigned int) home->distance, stats.max_home_distance);
    stats.max_home_altitude = MAX(home->altitude, stats.max_home_altitude);
    stats.max_bat_current = MAX(priv.bat_current, stats.max_bat_current);

    stats.flight_end = get_millis();


    /* try to guess a landing */
    if ((priv.throttle < 5) &&
        (priv.airspeed < 5) &&
        (home->distance < 50) &&
        (abs(home->altitude) < 10)) {

        remove_timer(t);
        stats.launch_heading = NO_HEADING;

        /* determine launch heading */
        add_timer(TIMER_ALWAYS, 5, find_launch_heading, NULL);
    }

}

static void start_calc_stats(void)
{
    /* flight start time */
    stats.flight_start = get_millis();
    stats.max_air_speed = 0;
    stats.max_gnd_speed = 0;
    stats.max_altitude = 0;
    stats.max_home_distance = 0;
    stats.max_home_altitude = 0;

    stats.max_bat_current = 0;
    stats.total_mah = 0;

    /* start calcs in a 100ms interval */
    add_timer(TIMER_ALWAYS, 1, calc_stats, NULL);
}

static void find_launch_heading(struct timer *t, void *d)
{
    struct home_data *home;

    home = get_home_data();
    if (home->lock != HOME_LOCKED)
        return;

    if ((priv.throttle > 10) &&
        (priv.airspeed > 5) &&
        (home->altitude > 5))
        stats.launch_heading = priv.heading;

    if (stats.launch_heading != NO_HEADING) {
        /* found */
        //printf("found launch heading\n");
        remove_timer(t);
        /* start calcs */
        start_calc_stats();
    }
}

void init_flight_stats(void)
{
    stats.launch_heading = NO_HEADING;

    /* determine launch heading */
    add_timer(TIMER_ALWAYS, 5, find_launch_heading, NULL);

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, store_mavdata, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback(MAVLINK_MSG_ID_SYS_STATUS, store_mavdata, CALLBACK_PERSISTENT, NULL);
}
