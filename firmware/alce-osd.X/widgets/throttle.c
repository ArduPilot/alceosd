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

#define X_SIZE  8
#define Y_SIZE  45

static struct widget_priv {
    unsigned char throttle, last_throttle;
    struct canvas ca;
} priv;

const struct widget throttle_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.throttle = (unsigned char) mavlink_msg_vfr_hud_get_throttle(msg);
    if (priv.throttle ==  priv.last_throttle)
        return;

    schedule_widget(&throttle_widget);
    priv.last_throttle = priv.throttle;
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.last_throttle = 0xff;
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned v = priv.throttle * (Y_SIZE-4) / 100;
    
    if (init_canvas(ca, 0))
        return 1;

    draw_rect(0, 0, X_SIZE-1, Y_SIZE-1, 3, ca);
    draw_rect(1, 1, X_SIZE-2, Y_SIZE-2, 1, ca);
    if (priv.throttle > 0) {
        draw_frect(2, Y_SIZE-3 - v, X_SIZE-3, Y_SIZE-3, 1, ca);
    }

    schedule_canvas(ca);
    return 0;
}


const struct widget throttle_widget = {
    .name = "Throttle bar",
    .id = WIDGET_THROTTLE_ID,
    .init = init,
    .render = render,
};
