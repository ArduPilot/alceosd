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

static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = d;
    schedule_widget(w);
}


static int open(struct widget *w)
{
    w->priv = (struct home_data*) get_home_data();

    w->cfg->w = 92;
    w->cfg->h = 12*3;

    /* refresh rate of 0.2 sec */
    add_timer(TIMER_WIDGET, 2, timer_callback, w);
    return 0;
}

static void render(struct widget *w)
{
    struct home_data *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[50];
    float d, a;
    struct point arrow_points[7] = { {-3, 0}, {-3, -6}, {3, -6}, {3, 0},
                                     {6, 0}, {0, 6}, {-6, 0} };
    struct polygon arrow = {
        .len = 7,
        .points = arrow_points,
    };

    if (priv->lock != HOME_LOCKED) {
        sprintf(buf, "No Home");
        draw_str(buf, 0, 9, ca, 1);
        if (priv->lock & HOME_LOCK_FIX)
            buf[0] = 'F';
        else
            buf[0] = ' ';
        if (priv->lock & HOME_LOCK_POS)
            buf[1] = 'P';
        else
            buf[1] = ' ';
        if (priv->lock & HOME_LOCK_ALT)
            buf[2] = 'A';
        else
            buf[2] = ' ';
        buf[3] = 0;
        draw_str(buf, 0, 9*2, ca, 1);
        sprintf(buf, "Lock in %d", config.home_lock_sec - priv->lock_sec);
        draw_str(buf, 0, 9*3, ca, 1);
    } else {
        sprintf(buf, "Home");
        draw_str(buf, 0, 0, ca, 2);

        switch (get_units(w->cfg)) {
            case UNITS_METRIC:
            default:
                sprintf(buf, "Alt %dm\nDis %dm",
                        priv->altitude,
                        (unsigned int) priv->distance);
                break;
            case UNITS_IMPERIAL:
                d = (float) priv->altitude * M2FEET;
                a = (float) priv->distance * M2FEET;
                sprintf(buf, "Alt %df\nDis %df",
                        (unsigned int) a,
                        (unsigned int) d);
                break;
        }

        draw_str(buf, 0, 15, ca, 1);

        transform_polygon(&arrow, 4 * 12 + 6, 7, priv->direction + 180);
        draw_polygon(&arrow, 3, ca);
        move_polygon(&arrow, -1, -1);
        draw_polygon(&arrow, 1, ca);
    }
}


const struct widget_ops home_info_widget_ops = {
    .name = "Home info",
    .mavname = "HOMEINF",
    .id = WIDGET_HOME_INFO_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};

