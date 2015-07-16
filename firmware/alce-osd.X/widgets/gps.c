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


#define X_SIZE  60 + 32 + 20 + 32
#define Y_SIZE  18


struct widget_priv {
    float gps_lat, gps_lon, gps_eph;
    unsigned char gps_nrsats, gps_fix_type;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    priv->gps_lat = mavlink_msg_gps_raw_int_get_lat(msg) / 10000000.0;
    priv->gps_lon = mavlink_msg_gps_raw_int_get_lon(msg) / 10000000.0;
    priv->gps_fix_type = mavlink_msg_gps_raw_int_get_fix_type(msg);
    priv->gps_nrsats = mavlink_msg_gps_raw_int_get_satellites_visible(msg);
    priv->gps_eph = (float) mavlink_msg_gps_raw_int_get_eph(msg) / 100.0;

    schedule_widget(w);
}

static int init(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;
    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[20];

    sprintf(buf, "%10.6f", priv->gps_lat);
    draw_str(buf, 0, 0, ca, 0);

    sprintf(buf, "%10.6f", priv->gps_lon);
    draw_str(buf, 0, 9, ca, 0);

    sprintf(buf, "%2d", priv->gps_nrsats);
    draw_str(buf, 60 + 20 + 1, 0, ca, 0);

    buf[1] = 'D';
    switch (priv->gps_fix_type) {
    default:
    case 0:
    case 1:
        buf[0] = ' ';
        buf[1] = ' ';
    break;
    case 2:
        buf[0] = '2';
        break;
    case 3:
        buf[0] = '3';
        break;
    }
    draw_chr(buf[0], 60 + 20 + 1, 9, ca, 0);
    draw_chr(buf[1], 60 + 20 + 1 + 6, 9, ca, 0);

    strcpy(buf, "HDP");
    draw_str(buf, 60 + 20 + 1 + 18, 0, ca, 0);
    sprintf(buf, "%2.1f", priv->gps_eph);
    draw_str(buf, 60 + 20 + 1 + 18, 9, ca, 0);
}


const struct widget_ops gps_info_widget_ops = {
    .name = "GPS stats",
    .id = WIDGET_GPS_INFO_ID,
    .init = init,
    .render = render,
    .close = NULL,
};
