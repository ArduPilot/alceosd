/*
    AlceOSD - Graphical OSD
    Copyright (C) 2016  Luis Alves

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


#define X_SIZE  128
#define Y_SIZE  30

struct widget_priv {
    mavlink_statustext_t msg;
    int x, i;
};

static void mav_callback(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    mavlink_msg_statustext_decode(msg, &priv->msg);
}

static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = (struct widget*) d;
    struct widget_priv *priv = (struct widget_priv*) w->priv;

    priv->x += priv->i;
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    /* init private variables */
    priv->x = 0;
    priv->i = -1;
    
    /* set the widget size */
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;

    add_mavlink_callback(MAVLINK_MSG_ID_STATUSTEXT, mav_callback, CALLBACK_WIDGET, w);
    add_timer(TIMER_WIDGET, 1, timer_callback, w);

    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[100];
    int z;
    
    sprintf(buf, "[%d] %s", priv->msg.severity, priv->msg.text);
    
    z = (int) get_str_width(buf, get_font(1));
    if (z > X_SIZE) {
        z = X_SIZE - z;
        if (priv->i == 0)
            priv->i = -2;
    } else {
        priv->i = 0;
        z = 0;
    }
    draw_str(buf, priv->x, 0, ca, 1);
    if ((priv->x < z) || (priv->x > 0))
        priv->i *= -1;

}

const struct widget_ops messages_widget_ops = {
    .name = "Messages",
    .mavname = "MESSAGE",
    .id = WIDGET_MESSAGES_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
