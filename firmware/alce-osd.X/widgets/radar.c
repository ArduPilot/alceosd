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
#include "flight_stats.h"

static struct widget_priv {
    struct home_data *home;
    struct flight_stats *stats;
    unsigned char bar_size;
    struct canvas ca;
    struct widget_config *cfg;
    int heading;
} priv;

const struct widget radar_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.heading = mavlink_msg_vfr_hud_get_heading(msg);
}

static void timer_callback(struct timer *t, void *d)
{
    schedule_widget(&radar_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.cfg = wcfg;
    priv.home = get_home_data();
    priv.stats = get_flight_stats();

    switch (wcfg->props.mode) {
        default:
        case 0:
            alloc_canvas(ca, wcfg, 84, 84);
            break;
        case 1:
            alloc_canvas(ca, wcfg, 84*2, 84*2);
            break;

    }

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);

    /* refresh rate of 0.2 sec */
    add_timer(TIMER_WIDGET, 2, timer_callback, NULL);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    char buf[10];
    unsigned int d = (unsigned int) priv.home->distance;
    unsigned int r = (priv.ca.width/2)-2;
    int x, y;
    long i, scale;
    struct point ils_points[5] = { {-4, -6}, {-4, 6}, {0, 10}, {4, 6}, {4, -6} };
    struct polygon ils = {
        .len = 5,
        .points = ils_points,
    };

    
    if (init_canvas(ca, 0))
        return 1;

    x = (priv.ca.width/2)-1;
    y = (priv.ca.height/2)-1;


    draw_vline(x, 0, r*2, 1, ca);
    draw_hline(0, r*2, y, 1, ca);

    draw_circle(x, y, r+1, 3, ca);
    draw_circle(x, y, r  , 1, ca);
    draw_circle(x, y, r-1, 3, ca);



    scale = 500;
    if (d > 1000)
        scale = 10000;
    else if (d > 500)
        scale = 1000;

    draw_circle(x, y, r/2, 3, ca);
    sprintf(buf, "%u", (unsigned int) scale/2);
    draw_str(buf, x, y+r/2, ca);

    i = (long) d * r;
    i /= scale;
    
    x += sin(DEG2RAD(priv.home->direction)) * i;
    y -= cos(DEG2RAD(priv.home->direction)) * i;


    transform_polygon(&ils, x, y, priv.stats->launch_heading - priv.heading - 180);
    draw_polygon(&ils, 3, ca);
    move_polygon(&ils, -1, -1);
    draw_polygon(&ils, 1, ca);


    schedule_canvas(ca);
    return 0;
}


const struct widget radar_widget = {
    .name = "Radar",
    .id = WIDGET_RADAR_ID,
    .init = init,
    .render = render,
};
