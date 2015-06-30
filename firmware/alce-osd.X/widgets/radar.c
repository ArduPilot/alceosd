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

static struct widget_priv {
    struct home_data *home;
    struct flight_stats *stats;
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
        case 2:
        case 4:
            alloc_canvas(ca, wcfg, 84, 84);
            break;
        case 1:
        case 3:
        case 5:
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
    unsigned long d = (unsigned long) priv.home->distance;
    unsigned int r = (priv.ca.width/2)-2;
    int x, y;
    int min_increment;
    long i, scale;
    struct point ils_points[5] = { {-4, -6}, {-4, 6}, {0, 10}, {4, 6}, {4, -6} };
    struct polygon ils = {
        .len = 5,
        .points = ils_points,
    };
    struct point uav_points[4] = { {0, 0}, {6, 8}, {0, -8}, {-6, 8} };
    struct polygon uav = {
        .len = 4,
        .points = uav_points,
    };

    struct polygon *p;

    if (init_canvas(ca, 0))
        return 1;

    x = (priv.ca.width/2)-1;
    y = (priv.ca.height/2)-1;


    draw_vline(x, 0, r*2, 2, ca);
    draw_hline(0, r*2, y, 2, ca);

    draw_circle(x, y, r+1, 3, ca);
    draw_circle(x, y, r  , 1, ca);


    /* auto scale */
    switch (get_units(priv.cfg)) {
        default:
        case UNITS_METRIC:
            min_increment = 500;
            scale = ((d / min_increment) + 1) * min_increment;
            sprintf(buf, "%um", (unsigned int) scale);
            break;
        case UNITS_IMPERIAL:
            d *= M2FEET;
            min_increment = 1000;
            scale = ((d / min_increment) + 1) * min_increment;
            if (d > 5000) {
                d = (d * 1000) / MILE2FEET;
                scale /= 1000;
                sprintf(buf, "%umi", (unsigned int) scale);
            } else {
                sprintf(buf, "%uf", (unsigned int) scale);
            }
            break;
    }
    
    draw_str(buf, 0, 0, ca, 0);

    i = (long) d * r;
    i /= scale;

    switch (priv.cfg->props.mode) {
        case 0:
        case 1:
        default:
            /* radar fixed at uav heading, home moves */
            x += sin(DEG2RAD(priv.home->direction)) * i;
            y -= cos(DEG2RAD(priv.home->direction)) * i;
            transform_polygon(&ils, x, y, priv.stats->launch_heading - priv.heading - 180);
            p = &ils;
            break;
        case 2:
        case 3:
            /* radar always facing north, uav moves */
            x += sin(DEG2RAD(priv.home->uav_bearing)) * i;
            y -= cos(DEG2RAD(priv.home->uav_bearing)) * i;
            transform_polygon(&uav, x, y, priv.heading);
            p = &uav;
            break;
        case 4:
        case 5:
            /* radar always facing launch direction, uav moves */
            x += sin(DEG2RAD(priv.home->uav_bearing - priv.stats->launch_heading)) * i;
            y -= cos(DEG2RAD(priv.home->uav_bearing - priv.stats->launch_heading)) * i;
            transform_polygon(&uav, x, y, priv.heading - priv.stats->launch_heading);
            p = &uav;
            break;
    }

    draw_polygon(p, 3, ca);
    move_polygon(p, -1, -1);
    draw_polygon(p, 1, ca);

    schedule_canvas(ca);
    return 0;
}


const struct widget radar_widget = {
    .name = "Radar",
    .id = WIDGET_RADAR_ID,
    .init = init,
    .render = render,
};
