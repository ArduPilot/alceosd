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
    //float voltage;
    float distance;
};

static void mav_callback(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    //priv->voltage = mavlink_msg_rangefinder_get_voltage(msg);
    priv->distance = mavlink_msg_rangefinder_get_distance(msg);
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;
    add_mavlink_callback(MAVLINK_MSG_ID_RANGEFINDER, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[10];

    sprintf(buf, "%.2fm", priv->distance);
    draw_jstr(buf, X_SIZE, Y_SIZE/2, JUST_RIGHT | JUST_VCENTER, ca, 2);
}


const struct widget_ops sonar_widget_ops = {
    .name = "Range Finder",
    .mavname = "SONAR",
    .id = WIDGET_SONAR_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
