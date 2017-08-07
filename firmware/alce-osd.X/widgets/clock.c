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
    char buf[50];
    u8 h, m;
};

static void pre_render(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    struct tm ts;
    u8 fmt = get_units(w->cfg);

    mavlink_system_time_t *utime = mavdata_get(MAVLINK_MSG_ID_SYSTEM_TIME);
    time_t now = utime->time_unix_usec / 1000000;

    ts = *localtime(&now);
    
    switch (w->cfg->props.mode) {
        default:
        case 0:
            /* date & time */
            if (fmt == UNITS_METRIC)
                strftime(priv->buf, sizeof(priv->buf), "%a %Y-%m-%d %H:%M:%S", &ts);
            else
                strftime(priv->buf, sizeof(priv->buf), "%a %Y-%m-%d %I:%M:%S %p", &ts);
            break;
        case 1:
            /* date */
            strftime(priv->buf, sizeof(priv->buf), "%a %d %b %Y", &ts);
            break;
        case 2:
            /* time */
            if (fmt == UNITS_METRIC)
                strftime(priv->buf, sizeof(priv->buf), "%H:%M:%S", &ts);
            else
                strftime(priv->buf, sizeof(priv->buf), "%I:%M:%S %p", &ts);
            break;
        case 15:
            /* analog clock */
            priv->h = ts.tm_hour;
            priv->m = ts.tm_min;
            break;
    }
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    if (w->cfg->props.mode < 15) {
        w->ca.height = 20;
        w->ca.width = 200;
    } else {
        w->ca.height = 50;
        w->ca.width = 50;
    }
    
    add_timer(TIMER_WIDGET, 1000, pre_render, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    int xc = ca->width / 2;
    int yc = ca->height / 2;

    
    if (w->cfg->props.mode == 15) {
        float ah, am;
        ah = (float)priv->h / 12.0 * 2 * PI;
        am = (float)priv->m / 60.0 * 2 * PI;
        
        draw_circle(xc, yc, 22, 1, ca);
        draw_circle(xc, yc, 23, 3, ca);

        draw_line(xc-1, yc-1, xc + 15 * sin(ah) - 1, yc - 15*cos(ah), 3, ca);
        draw_line(xc, yc, xc + 15 * sin(ah), yc - 15*cos(ah) - 1, 1, ca);

        draw_line(xc-1, yc-1, xc + 21 * sin(am) - 1, yc - 15*cos(am) -1, 3, ca);
        draw_line(xc, yc, xc + 21 * sin(am), yc - 15*cos(am), 1, ca);
    } else {
        draw_jstr(priv->buf, xc, yc,
                JUST_HCENTER | JUST_VCENTER, ca, w->cfg->params[0]);
    }
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
