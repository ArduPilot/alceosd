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


struct widget_priv {
    int direction, heading;
    float speed;
    float speed_z;
};

static void render_timer(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    mavlink_vfr_hud_t *vfr_hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    mavlink_wind_t *wind = mavdata_get(MAVLINK_MSG_ID_WIND);
    
    priv->heading = vfr_hud->heading;
    priv->direction = wind->direction;
    priv->speed = wind->speed;
    priv->speed_z = wind->speed_z;
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->direction = 0;
    priv->speed = 0;
    priv->speed_z = 0;
    
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;

    add_timer(TIMER_WIDGET, 500, render_timer, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
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

    move_polygon(&uav, ca->width >> 1, ca->height >> 1);
    draw_polygon(&uav, 3, ca);
    move_polygon(&uav, -1, -1);
    draw_polygon(&uav, 1, ca);


    move_polygon(&arrow, 0, -11);
    transform_polygon(&arrow, ca->width >> 1, ca->height >> 1, priv->direction - priv->heading);
    draw_polygon(&arrow, 3, ca);
    move_polygon(&arrow, -1, -1);
    draw_polygon(&arrow, 1, ca);

    switch (get_units(w->cfg)) {
        case UNITS_METRIC:
        default:
            sprintf(buf, "%dkm/h", (int) ((priv->speed * 3600) / 1000));
            break;
        case UNITS_IMPERIAL:
            sprintf(buf, "%dmph", (int) ((priv->speed * 3600) * M2MILE));
            break;
        case UNITS_CUSTOM_1:
            sprintf(buf, "%dm/s", (int) (priv->speed));
            break;
        case UNITS_CUSTOM_2:
            sprintf(buf, "%df/s", (int) (priv->speed * M2FEET));
            break;
        case UNITS_CUSTOM_3:
            sprintf(buf, "%dkn", (int) ((priv->speed * 3600 * 1.852) / 1000));
            break;
    }
    draw_str(buf, 0, 0, ca, 0);
}


const struct widget_ops wind_widget_ops = {
    .name = "Wind information",
    .mavname = "WINDINF",
    .id = WIDGET_WIND_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
