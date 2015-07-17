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


#define X_SIZE  40
#define Y_SIZE  100
#define X_CENTER    (X_SIZE/2) + 12
#define Y_CENTER    (Y_SIZE/2) - 1

struct widget_priv {
    int range;
    float speed;
    int speed_i;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    priv->speed = mavlink_msg_vfr_hud_get_airspeed(msg) * 3600 / 1000.0;
    priv->speed_i = (int) priv->speed;

    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->range = 20*5;

    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
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
        j = priv->speed_i + i - priv->range/2;
        if(j < 0)
            continue;
        if (j % major_tick == 0) {
            sprintf(buf, "%3d", j);
            draw_str(buf, 2, y - 2, ca, 0);
            draw_ohline(X_CENTER - 2, X_CENTER + 4, y, 1, 3, ca);
            d = 1;
        } else if (j % minor_tick == 0) {
            draw_ohline(X_CENTER - 2, X_CENTER + 2, y, 1, 3, ca);
            d = 1;
        } else {
            d = 0;
        }
    }

    draw_frect(1, Y_CENTER-4, X_CENTER - 10, Y_CENTER + 4, 0, ca);
    sprintf(buf, "%3d", (int) priv->speed_i);
    draw_str(buf, 2, Y_CENTER - 3, ca, 0);

    draw_hline(0, X_CENTER - 10, Y_CENTER - 5, 1, ca);
    draw_hline(0, X_CENTER - 10, Y_CENTER + 5, 1, ca);
    draw_vline(0, Y_CENTER - 3 , Y_CENTER + 3, 1, ca);

    draw_line(X_CENTER-10, Y_CENTER-5, X_CENTER-10+5, Y_CENTER, 1, ca);
    draw_line(X_CENTER-10, Y_CENTER+5, X_CENTER-10+5, Y_CENTER, 1, ca);
}


const struct widget_ops speed_widget_ops = {
    .name = "Air speed",
    .id = WIDGET_SPEED_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
