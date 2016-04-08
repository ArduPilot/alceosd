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
    unsigned int custom_mode, prev_custom_mode;
    unsigned char font_size;
    unsigned char mav_type;
};

static void mav_callback(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    priv->custom_mode = mavlink_msg_heartbeat_get_custom_mode(msg);
    if (priv->custom_mode == priv->prev_custom_mode)
        return;

    priv->mav_type = mavlink_msg_heartbeat_get_type(msg);
    priv->prev_custom_mode = priv->custom_mode;
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;
    unsigned char m = w->cfg->props.mode;
    const struct font *f;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    if (m > 2)
        m = 2;

    priv->font_size = m;

    f = get_font(m);
    w->ca.height = f->size + 2;
    w->ca.width = f->size * 12;

    priv->prev_custom_mode = 0xff;
    add_mavlink_callback(MAVLINK_MSG_ID_HEARTBEAT, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char mode[17];
    unsigned int cust_mode;

    cust_mode = priv->custom_mode;
    if (priv->mav_type !=  MAV_TYPE_FIXED_WING)
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

    draw_str(mode, 0, 0, ca, priv->font_size);
}


const struct widget_ops flightmode_widget_ops = {
    .name = "Flight mode",
    .mavname = "FLTMODE",
    .id = WIDGET_FLIGHT_MODE_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
