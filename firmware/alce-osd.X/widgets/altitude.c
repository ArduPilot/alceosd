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
#define Y_SIZE  100
#define X_CENTER    (X_SIZE/2) - 15
#define Y_CENTER    (Y_SIZE/2) - 1


struct widget_priv {
    long altitude;
    int range;
    struct home_data *home;
};


static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *data)
{
    struct widget *w = (struct widget*) data;
    struct widget_priv *priv = (struct widget_priv*) w->priv;
    float altitude;
    
    switch (w->cfg->props.mode) {
        case 0:
        default:
            altitude = mavlink_msg_gps_raw_int_get_alt(msg) / 1000.0;
            break;
        case 1:
            if (priv->home->lock == HOME_LOCKED)
                altitude = (long) priv->home->altitude;
            else
                altitude = 0;
            break;
    }

    if (get_units(w->cfg) == UNITS_IMPERIAL)
        altitude *= M2FEET;
    
    priv->altitude = (long) altitude;
    schedule_widget(w);
}


int init(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;

    w->priv = priv;

    priv->home = get_home_data();

    switch (get_units(w->cfg)) {
        case UNITS_METRIC:
        default:
            priv->range = 20*5;
            break;
        case UNITS_IMPERIAL:
            priv->range = 100*5;
            break;
    }

    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, mav_callback, CALLBACK_WIDGET, w);
    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = (struct widget_priv*) w->priv;
    struct canvas *ca = &w->ca;
    int i, j, y = -1;
    long yy;
    char buf[10], d = 0;
    int major_tick = priv->range / 5;
    int minor_tick = major_tick / 4;

    for (i = 0; i < priv->range; i++) {
        yy = ((long) i * Y_SIZE) / priv->range;
        if ((yy == y) && (d == 1))
            continue;
        y = Y_SIZE - (int) yy;
        j = priv->altitude + i - priv->range/2;
        if (j % major_tick == 0) {
            draw_ohline(X_CENTER + 2, X_CENTER - 4, y, 1, 3, ca);
            sprintf(buf, "%4d", j);
            draw_str(buf, X_CENTER + 13, y-3, ca, 0);
            d = 1;
        } else if (j % minor_tick == 0) {
            draw_ohline(X_CENTER + 2, X_CENTER - 2, y, 1, 3, ca);
            d = 1;
        } else {
            d = 0;
        }
    }

    draw_frect(X_CENTER + 10, Y_CENTER-4, X_SIZE-2, Y_CENTER + 4, 0, ca);
    sprintf(buf, "%4ld", priv->altitude);
    draw_str(buf, X_CENTER + 13, Y_CENTER - 3, ca, 0);

    draw_hline(X_CENTER + 10, X_SIZE - 1, Y_CENTER-5, 1, ca);
    draw_hline(X_CENTER + 10, X_SIZE - 1, Y_CENTER+5, 1, ca);
    draw_vline(X_SIZE - 1, Y_CENTER-4, Y_CENTER+4, 1, ca);

    /* draw arrow */
    draw_line(X_CENTER+10, Y_CENTER-5, X_CENTER+10-5, Y_CENTER, 1, ca);
    draw_line(X_CENTER+10, Y_CENTER+5, X_CENTER+10-5, Y_CENTER, 1, ca);
}


const struct widget_ops altitude_widget_ops = {
    .name = "Altitude",
    .id = WIDGET_ALTITUDE_ID,
    .init = init,
    .render = render,
};
