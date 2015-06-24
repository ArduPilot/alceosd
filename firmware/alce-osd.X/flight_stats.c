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

#define NO_HEADING 0xfff


struct stats_priv {
    unsigned int throttle;
    int airspeed, groundspeed;
    int alt;
    int heading;
} priv;


struct flight_stats* get_flight_stats(void)
{
    return &stats;
}


static void store_mavdata(mavlink_message_t *msg, mavlink_status_t *status)
{
    switch (msg->msgid) {
        case MAVLINK_MSG_ID_VFR_HUD:
            priv.throttle = mavlink_msg_vfr_hud_get_throttle(msg);
            priv.airspeed = (int) mavlink_msg_vfr_hud_get_airspeed(msg);
            priv.groundspeed = (int) mavlink_msg_vfr_hud_get_groundspeed(msg);
            priv.alt = (int) mavlink_msg_vfr_hud_get_alt(msg);
            priv.heading = mavlink_msg_vfr_hud_get_heading(msg);
            break;
        default:
            break;
    }
}


static void calc_stats(struct timer *t, void *d)
{
    /* accumulate distance */
    stats.total_distance += (priv.groundspeed * 10) / 1000;

}


static void start_calc_stats(void)
{
    /* flight start time */
    stats.flight_start = get_millis();

    /* start calcs in a 100ms interval */
    add_timer(TIMER_ALWAYS, 1, calc_stats, NULL);
}


static void find_launch_heading(struct timer *t, void *d)
{
    struct home_data *home;

    home = get_home_data();
    if (home->lock != LOCKED)
        return;

    if ((priv.throttle > 10) &&
        (priv.airspeed > 5) &&
        (home->altitude > 5))
        stats.launch_heading = priv.heading;

    if (stats.launch_heading != NO_HEADING) {
        /* found */
        printf("found launch heading\n");
        remove_timer(t);
        /* start calcs */
        start_calc_stats();
    }
}


void init_flight_stats_process(void)
{
    stats.launch_heading = NO_HEADING;

    /* determine launch heading */
    add_timer(TIMER_ALWAYS, 10, find_launch_heading, NULL);

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, store_mavdata, CALLBACK_PERSISTENT);
}
