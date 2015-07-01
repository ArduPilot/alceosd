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


#define X_SIZE  48
#define Y_SIZE  48


static struct widget_priv {
    int direction, heading;
    float speed;
    float speed_z;
    struct canvas ca;
    struct widget_config *cfg;
} priv;


const struct widget wind_widget;

static void mav_callback_vfr_hud(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.heading = mavlink_msg_vfr_hud_get_heading(msg);
}

static void mav_callback_wind(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.direction = mavlink_msg_wind_get_direction(msg);
    priv.speed = mavlink_msg_wind_get_speed(msg);
    priv.speed_z = mavlink_msg_wind_get_speed_z(msg);
    schedule_widget(&wind_widget);
}

static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.direction = 0;
    priv.speed = 0;
    priv.speed_z = 0;
    priv.cfg = wcfg;
    
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);

    add_mavlink_callback(MAVLINK_MSG_ID_WIND, mav_callback_wind, CALLBACK_WIDGET);
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback_vfr_hud, CALLBACK_WIDGET);
}

static int render(void)
{
    struct canvas *ca = &priv.ca;
    char buf[10];
    struct point uav_points[4] = { {0, 0}, {6, 8}, {0, -8}, {-6, 8} };
    struct polygon uav = {
        .len = 4,
        .points = uav_points,
    };
    struct point arrow_points[7] = { {1, -10}, {1, -3}, {4, -4}, {0, 0},
                                     {-4, -4}, {-1, -3}, {-1, -10} };
    struct polygon arrow = {
        .len = 7,
        .points = arrow_points,
    };

    if (init_canvas(ca, 0))
        return 1;


    move_polygon(&uav, ca->width >> 1, ca->height >> 1);
    draw_polygon(&uav, 3, ca);
    move_polygon(&uav, -1, -1);
    draw_polygon(&uav, 1, ca);


    move_polygon(&arrow, 0, -11);
    transform_polygon(&arrow, ca->width >> 1, ca->height >> 1, priv.direction - priv.heading);
    draw_polygon(&arrow, 3, ca);
    move_polygon(&arrow, -1, -1);
    draw_polygon(&arrow, 1, ca);

    switch (get_units(priv.cfg)) {
        case UNITS_METRIC:
        default:
            sprintf(buf, "%dkm/h", (int) ((priv.speed * 3600) / 1000));
            break;
        case UNITS_IMPERIAL:
            sprintf(buf, "%dmph", (int) ((priv.speed * 3600) * M2MILE));
            break;
        case UNITS_CUSTOM_1:
            sprintf(buf, "%dm/s", (int) (priv.speed));
            break;
        case UNITS_CUSTOM_2:
            sprintf(buf, "%df/s", (int) (priv.speed * M2FEET));
            break;
        case UNITS_CUSTOM_3:
            sprintf(buf, "%dkn", (int) ((priv.speed * 3600 * 1.852) / 1000));
            break;
    }
    draw_str(buf, 0, 0, ca, 0);

    schedule_canvas(ca);
    return 0;
}


const struct widget wind_widget = {
    .name = "Wind information",
    .id = WIDGET_WIND_ID,
    .init = init,
    .render = render,
};
