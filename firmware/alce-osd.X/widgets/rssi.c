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
#define Y_SIZE  16

#define RSSI_MAX 255

struct widget_priv {
    unsigned char rssi, last_rssi;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    priv->rssi = mavlink_msg_rc_channels_raw_get_rssi(msg);
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

    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned char i, x;
    char buf[5];

    x = 0;
    for (i = 0; i < (5 * priv->rssi)/(RSSI_MAX-RSSI_MAX/5); i++) {
        draw_vline(x, Y_SIZE-1 - i*3, Y_SIZE-1, 3, ca);
        draw_vline(x+1, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        draw_vline(x+2, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        x += 4;
    }

    sprintf(buf, "%3d", (priv->rssi * 100) / RSSI_MAX);
    draw_str(buf, 25, 4, ca, 2);
}


const struct widget_ops rssi_widget_ops = {
    .name = "RSSI",
    .id = WIDGET_RSSI_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
