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

#define X_SIZE  64
#define Y_SIZE  17


struct widget_priv {
    u16 rssi, last_rssi;
};

static void pre_render(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    struct flight_stats *f = get_flight_stats();

    priv->rssi = f->rssi;
    if (priv->rssi ==  priv->last_rssi)
        return;
    priv->last_rssi = priv->rssi;
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;
    priv->last_rssi = 0xff;
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;
    add_timer(TIMER_WIDGET, 500, pre_render, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned int i, x;
    char buf[5];

    x = 0;
    for (i = 0; i < (5 * priv->rssi)/(100-100/5); i++) {
        draw_vline(x, Y_SIZE-1 - i*3, Y_SIZE-1, 3, ca);
        draw_vline(x+1, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        draw_vline(x+2, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        x += 4;
    }

    sprintf(buf, "%3d", priv->rssi);
    draw_jstr(buf, X_SIZE, Y_SIZE/2, JUST_RIGHT | JUST_VCENTER, ca, 2);
}


const struct widget_ops rssi_widget_ops = {
    .name = "RSSI",
    .mavname = "RSSI",
    .id = WIDGET_RSSI_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
