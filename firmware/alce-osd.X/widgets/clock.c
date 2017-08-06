/*
    AlceOSD - Graphical OSD
    Copyright (C) 2017  Luis Alves

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
    char buf[80];
};

static void pre_render(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    struct tm ts;

    mavlink_system_time_t *utime = mavdata_get(MAVLINK_MSG_ID_SYSTEM_TIME);
    time_t now = utime->time_unix_usec / 1000000;

    ts = *localtime(&now);
    strftime(priv->buf, sizeof(priv->buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    w->ca.height = 20;
    w->ca.width = 200;
    
    add_timer(TIMER_WIDGET, 1000, pre_render, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;

    draw_jstr(priv->buf, ca->width / 2, 0, JUST_HCENTER, ca, w->cfg->props.mode);
}


const struct widget_ops clock_widget_ops = {
    .name = "Clock",
    .mavname = "CLOCK",
    .id = WIDGET_CLOCK_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
