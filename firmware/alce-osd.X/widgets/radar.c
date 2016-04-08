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

struct widget_priv {
    struct home_data *home;
    struct flight_stats *stats;
    int heading;

    int wp_target_bearing;
    unsigned int wp_distance, wp_seq;
};

static void mav_callback(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    priv->heading = mavlink_msg_vfr_hud_get_heading(msg);
}
static void mav_callback_nav(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    priv->wp_target_bearing = mavlink_msg_nav_controller_output_get_target_bearing(msg);
    priv->wp_distance = mavlink_msg_nav_controller_output_get_wp_dist(msg);
}
static void mav_callback_wp_seq(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    priv->wp_seq = mavlink_msg_mission_current_get_seq(msg);
}

static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = d;
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->home = get_home_data();
    priv->stats = get_flight_stats();

    if (w->cfg->props.mode & 1) {
        w->ca.width = 84*2;
        w->ca.height = 84*2;
    } else {
        w->ca.width = 84;
        w->ca.height = 84;
    }

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET, w);
    add_mavlink_callback(MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT, mav_callback_nav, CALLBACK_WIDGET, w);
    add_mavlink_callback(MAVLINK_MSG_ID_MISSION_CURRENT, mav_callback_wp_seq, CALLBACK_WIDGET, w);

    /* refresh rate of 0.2 sec */
    add_timer(TIMER_WIDGET, 2, timer_callback, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[10];
    unsigned long d = (unsigned long) priv->home->distance;
    unsigned int r = (w->ca.width/2)-2;
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

    x = (w->ca.width/2)-1;
    y = (w->ca.height/2)-1;


    draw_vline(x, 0, r*2, 2, ca);
    draw_hline(0, r*2, y, 2, ca);

    //draw_circle(x, y, r+1, 3, ca);
    draw_circle(x, y, r  , 2, ca);


    /* auto scale */
    switch (get_units(w->cfg)) {
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

    switch (w->cfg->props.mode >> 1) {
        case 0:
        default:
            /* radar fixed at uav heading, home moves */
            x += sin(DEG2RAD(priv->home->direction)) * i;
            y -= cos(DEG2RAD(priv->home->direction)) * i;
            transform_polygon(&ils, x, y, priv->stats->launch_heading - priv->heading - 180);
            p = &ils;
            break;
        case 1:
            /* radar always facing north, uav moves */
            x += sin(DEG2RAD(priv->home->uav_bearing)) * i;
            y -= cos(DEG2RAD(priv->home->uav_bearing)) * i;
            transform_polygon(&uav, x, y, priv->heading);
            p = &uav;
            break;
        case 2:
            /* radar always facing launch direction, uav moves */
            x += sin(DEG2RAD(priv->home->uav_bearing - priv->stats->launch_heading)) * i;
            y -= cos(DEG2RAD(priv->home->uav_bearing - priv->stats->launch_heading)) * i;
            transform_polygon(&uav, x, y, priv->heading - priv->stats->launch_heading);
            p = &uav;
            break;
        case 3:
            /* testing waypoints */
            /* radar always facing north, uav moves with waypoints */
            if (priv->wp_seq > 0) {
                long i_wp = (long) priv->wp_distance * r;
                i_wp /= scale;
                int x_wp = x, y_wp = y;
                x_wp += sin(DEG2RAD(priv->wp_target_bearing - priv->heading)) * i_wp;
                y_wp -= cos(DEG2RAD(priv->wp_target_bearing - priv->heading)) * i_wp;
                sprintf(buf, "%d", priv->wp_seq);
                draw_str(buf, x_wp, y_wp, ca, 0);
            }
            x += sin(DEG2RAD(priv->home->uav_bearing)) * i;
            y -= cos(DEG2RAD(priv->home->uav_bearing)) * i;
            transform_polygon(&uav, x, y, priv->heading);
            p = &uav;
            break;
    }

    draw_polygon(p, 3, ca);
    move_polygon(p, -1, -1);
    draw_polygon(p, 1, ca);
}


const struct widget_ops radar_widget_ops = {
    .name = "Radar",
    .mavname = "RADAR",
    .id = WIDGET_RADAR_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
