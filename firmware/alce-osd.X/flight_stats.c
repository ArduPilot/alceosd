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

struct flight_stats* get_flight_stats(void)
{
    return &stats;
}

static void find_launch_heading(struct timer *t, void *d);

static void calc_stats(struct timer *t, void *d)
{
    struct home_data *home = get_home_data();
    mavlink_vfr_hud_t *hud = mavdata_get(MAVDATA_VFR_HUD);
    mavlink_sys_status_t *s = mavdata_get(MAVDATA_SYS_STATUS);
    
    /* accumulate distance */
    stats.total_distance += (hud->groundspeed / 10);
    stats.total_flight_mah += ((float) s->current_battery) / 3600;

    stats.max_air_speed = MAX(hud->airspeed, stats.max_air_speed);
    stats.max_gnd_speed = MAX(hud->groundspeed, stats.max_gnd_speed);
    stats.max_altitude  = MAX(hud->alt, stats.max_altitude);
    stats.max_home_distance = MAX((unsigned int) home->distance, stats.max_home_distance);
    stats.max_home_altitude = MAX(home->altitude, stats.max_home_altitude);
    stats.max_bat_current = MAX(s->current_battery, stats.max_bat_current);

    stats.flight_end = get_millis();

    /* try to guess a landing */
    if ((hud->throttle < 5) &&
        (hud->airspeed < 5) &&
        (home->distance < 50) &&
        (abs(home->altitude) < 10)) {

        remove_timer(t);
        stats.launch_heading = NO_HEADING;

        /* determine launch heading */
        add_timer(TIMER_ALWAYS, 500, find_launch_heading, NULL);
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
    stats.total_flight_mah = 0;

    /* start calcs in a 100ms interval */
    add_timer(TIMER_ALWAYS, 100, calc_stats, NULL);
}

static void find_launch_heading(struct timer *t, void *d)
{
    struct home_data *home = get_home_data();
    mavlink_vfr_hud_t *hud = mavdata_get(MAVDATA_VFR_HUD);

    if (home->lock != HOME_LOCKED)
        return;

    if ((hud->throttle > 10) &&
        (hud->airspeed > 5) &&
        (home->altitude > 5))
        stats.launch_heading = hud->heading;

    if (stats.launch_heading != NO_HEADING) {
        /* found */
        //printf("found launch heading\n");
        remove_timer(t);
        /* start calcs */
        start_calc_stats();
    }
}

static void calc_mah(struct timer *t, void *d)
{
    mavlink_sys_status_t *s = mavdata_get(MAVDATA_SYS_STATUS);
    stats.total_mah += ((float) s->current_battery) / 3600;
}

void init_flight_stats(void)
{
    stats.launch_heading = NO_HEADING;
    stats.total_mah = 0;

    /* determine launch heading */
    add_timer(TIMER_ALWAYS, 500, find_launch_heading, NULL);
    add_timer(TIMER_ALWAYS, 100, calc_mah, NULL);
}
