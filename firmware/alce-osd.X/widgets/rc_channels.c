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
    unsigned int ch_raw[8];
    unsigned char bar_size;
    struct canvas ca;
    struct widget_config *cfg;
} priv;

const struct widget rc_channels_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.ch_raw[0] = mavlink_msg_rc_channels_raw_get_chan1_raw(msg);
    priv.ch_raw[1] = mavlink_msg_rc_channels_raw_get_chan2_raw(msg);
    priv.ch_raw[2] = mavlink_msg_rc_channels_raw_get_chan3_raw(msg);
    priv.ch_raw[3] = mavlink_msg_rc_channels_raw_get_chan4_raw(msg);
    priv.ch_raw[4] = mavlink_msg_rc_channels_raw_get_chan5_raw(msg);
    priv.ch_raw[5] = mavlink_msg_rc_channels_raw_get_chan6_raw(msg);
    priv.ch_raw[6] = mavlink_msg_rc_channels_raw_get_chan7_raw(msg);
    priv.ch_raw[7] = mavlink_msg_rc_channels_raw_get_chan8_raw(msg);

    schedule_widget(&rc_channels_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;
    unsigned int i;

    priv.cfg = wcfg;

    /* set initial values */
    for (i = 0; i < 8; i++)
        priv.ch_raw[i] = 1500;

    /* setup canvas according to widget mode */
    switch (wcfg->props.mode) {
        case 0:
        default:
            /* raw numbers and bars */
            priv.bar_size = 25;
            alloc_canvas(ca, wcfg, 84, 64);
            wcfg->props.mode = 0;
            break;
        case 1:
            /* only numbers */
            alloc_canvas(ca, wcfg, 58, 64);
            break;
        case 2:
            /* only bars */
            priv.bar_size = 50;
            alloc_canvas(ca, wcfg, 80, 64);
            break;
    }

    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned char i;
    unsigned int w = priv.ca.width;
    int x;
    char buf[10];


    if (init_canvas(ca, 0))
        return 1;

    for (i = 0; i < 8; i++) {
        if ((priv.cfg->props.mode == 0) || (priv.cfg->props.mode == 1))
            sprintf(buf, "CH%u %4d", i+1, priv.ch_raw[i]);
        else
            sprintf(buf, "CH%u", i+1);
        draw_str(buf, 0, i*8+1, ca, 0);

        if ((priv.cfg->props.mode == 0) || (priv.cfg->props.mode == 2)) {
            x = priv.ch_raw[i] - 1000;
            if (x < 0)
                x = 0;
            else if (x > 1000)
                x = 1000;

            x = (x * (unsigned int) priv.bar_size) / 1000;

            draw_rect(w-priv.bar_size-1,      i*8, w-1, i*8+6, 3, ca);
            draw_rect(w-priv.bar_size,      i*8+1, w-2, i*8+5, 1, ca);

            draw_vline(w-priv.bar_size-1+x,   i*8+1, i*8+5, 1, ca);
            draw_vline(w-priv.bar_size-1+x-1, i*8+1, i*8+5, 3, ca);
            draw_vline(w-priv.bar_size-1+x+1, i*8+1, i*8+5, 3, ca);
        }
    }

    schedule_canvas(ca);
    return 0;
}


const struct widget rc_channels_widget = {
    .name = "RC Channels",
    .id = WIDGET_RC_CHANNELS_ID,
    .init = init,
    .render = render,
};
