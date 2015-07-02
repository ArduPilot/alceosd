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

static struct widget_priv {
    unsigned int custom_mode, prev_custom_mode;
    unsigned char font_id;
    unsigned char mav_type;
    struct canvas ca;
} priv;

const struct widget flightmode_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.custom_mode = mavlink_msg_heartbeat_get_custom_mode(msg);
    if (priv.custom_mode == priv.prev_custom_mode)
        return;

    priv.mav_type = mavlink_msg_heartbeat_get_type(msg);
    priv.prev_custom_mode = priv.custom_mode;
    schedule_widget(&flightmode_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;
    unsigned int xs = 120, ys;
    unsigned char m = wcfg->props.mode;
    const struct font *f;

    if (m > 2)
        m = 2;

    priv.font_id = m;

    f = get_font(m);
    ys = f->height + 1;

    priv.prev_custom_mode = 0xff;
    alloc_canvas(ca, wcfg, xs, ys);
    add_mavlink_callback(MAVLINK_MSG_ID_HEARTBEAT, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    char mode[17];
    unsigned int cust_mode;

    if (init_canvas(ca, 0))
        return 1;

    cust_mode = priv.custom_mode;
    if (priv.mav_type !=  MAV_TYPE_FIXED_WING)
        cust_mode += 100;

    switch (cust_mode) {
    case PLANE_MODE_MANUAL:
        strcpy(mode, "Manual");
        break;
    case PLANE_MODE_CIRCLE:
    case COPTER_MODE_CIRCLE:
        strcpy(mode, "Circle");
        break;
    case PLANE_MODE_STABILIZE:
    case COPTER_MODE_STABILIZE:
        strcpy(mode, "Stabilize");
        break;
    case PLANE_MODE_TRAINING:
        strcpy(mode, "Training");
        break;
    case PLANE_MODE_ACRO:
    case COPTER_MODE_ACRO:
        strcpy(mode, "Acro");
        break;
    case PLANE_MODE_FBWA:
        strcpy(mode, "Fly-By-Wire A");
        break;
    case PLANE_MODE_FBWB:
        strcpy(mode, "Fly-By-Wire B");
        break;
    case PLANE_MODE_CRUISE:
        strcpy(mode, "Cruise");
        break;
    case PLANE_MODE_AUTOTUNE:
    case COPTER_MODE_AUTOTUNE:
        strcpy(mode, "Auto tune");
        break;
    case PLANE_MODE_AUTO:
    case COPTER_MODE_AUTO:
        strcpy(mode, "Auto");
        break;
    case PLANE_MODE_RTL:
    case COPTER_MODE_RTL:
        strcpy(mode, "Return To Launch");
        break;
    case PLANE_MODE_LOITER:
    case COPTER_MODE_LOITER:
        strcpy(mode, "Loiter");
        break;
    case PLANE_MODE_INIT:
        strcpy(mode, "Initializing");
        break;
    case PLANE_MODE_GUIDED:
    case COPTER_MODE_GUIDED:
        strcpy(mode, "Guided");
        break;
    case COPTER_MODE_ALTHOLD:
        strcpy(mode, "Altitude hold");
        break;
    case COPTER_MODE_LAND:
        strcpy(mode, "Land");
        break;
    case COPTER_MODE_OF_LOITER:
        strcpy(mode, "OF Loiter");
        break;
    case COPTER_MODE_DRIFT:
        strcpy(mode, "Drift");
        break;
    case COPTER_MODE_SPORT:
        strcpy(mode, "Sport");
        break;
    case COPTER_MODE_FLIP:
        strcpy(mode, "Flip");
        break;
    case COPTER_MODE_POSHOLD:
        strcpy(mode, "Position hold");
        break;
    default:
        strcpy(mode, "Unknown Mode");
        break;
    }

    draw_str(mode, 0, 0, ca, priv.font_id);

    schedule_canvas(ca);
    return 0;
}


const struct widget flightmode_widget = {
    .name = "Flight mode",
    .id = WIDGET_FLIGHT_MODE_ID,
    .init = init,
    .render = render,
};
