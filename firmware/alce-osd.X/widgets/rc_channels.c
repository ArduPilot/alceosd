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
    unsigned int ch_raw[8];
    unsigned char bar_size;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    priv->ch_raw[0] = mavlink_msg_rc_channels_raw_get_chan1_raw(msg);
    priv->ch_raw[1] = mavlink_msg_rc_channels_raw_get_chan2_raw(msg);
    priv->ch_raw[2] = mavlink_msg_rc_channels_raw_get_chan3_raw(msg);
    priv->ch_raw[3] = mavlink_msg_rc_channels_raw_get_chan4_raw(msg);
    priv->ch_raw[4] = mavlink_msg_rc_channels_raw_get_chan5_raw(msg);
    priv->ch_raw[5] = mavlink_msg_rc_channels_raw_get_chan6_raw(msg);
    priv->ch_raw[6] = mavlink_msg_rc_channels_raw_get_chan7_raw(msg);
    priv->ch_raw[7] = mavlink_msg_rc_channels_raw_get_chan8_raw(msg);

    schedule_widget(w);
}


static int init(struct widget *w)
{
    struct widget_priv *priv;
    unsigned int i;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    /* set initial values */
    for (i = 0; i < 8; i++)
        priv->ch_raw[i] = 1500;

    /* setup canvas according to widget mode */
    switch (w->cfg->props.mode) {
        case 0:
        default:
            /* raw numbers and bars */
            priv->bar_size = 25;
            w->cfg->w = 84;
            w->cfg->props.mode = 0;
            break;
        case 1:
            /* only numbers */
            w->cfg->w = 58;
            break;
        case 2:
            /* only bars */
            priv->bar_size = 50;
            w->cfg->w = 80;
            break;
    }
    w->cfg->h = 64;
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned char i;
    unsigned int width = ca->width;
    int x;
    char buf[10];

    for (i = 0; i < 8; i++) {
        if ((w->cfg->props.mode == 0) || (w->cfg->props.mode == 1))
            sprintf(buf, "CH%u %4d", i+1, priv->ch_raw[i]);
        else
            sprintf(buf, "CH%u", i+1);
        draw_str(buf, 0, i*8+1, ca, 0);

        if ((w->cfg->props.mode == 0) || (w->cfg->props.mode == 2)) {
            x = priv->ch_raw[i] - 1000;
            if (x < 0)
                x = 0;
            else if (x > 1000)
                x = 1000;

            x = (x * (unsigned int) priv->bar_size) / 1000;

            draw_rect(width-priv->bar_size-1,      i*8, width-1, i*8+6, 3, ca);
            draw_rect(width-priv->bar_size,      i*8+1, width-2, i*8+5, 1, ca);

            draw_vline(width-priv->bar_size-1+x,   i*8+1, i*8+5, 1, ca);
            draw_vline(width-priv->bar_size-1+x-1, i*8+1, i*8+5, 3, ca);
            draw_vline(width-priv->bar_size-1+x+1, i*8+1, i*8+5, 3, ca);
        }
    }
}


const struct widget_ops rc_channels_widget_ops = {
    .name = "RC Channels",
    .id = WIDGET_RC_CHANNELS_ID,
    .init = init,
    .render = render,
    .close = NULL,
};
