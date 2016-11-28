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


static void render_timer(struct timer *t, void *d)
{
    struct widget *w = d;
    schedule_widget(w);
}

static int open(struct widget *w)
{
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;
    add_timer(TIMER_WIDGET, 500, render_timer, w);
    return 0;
}

static void render(struct widget *w)
{
    struct canvas *ca = &w->ca;
    char buf[10];
    mavlink_rangefinder_t *rfinder = mavdata_get(MAVLINK_MSG_ID_RANGEFINDER);
    float distance = rfinder->distance;

    sprintf(buf, "%.2fm", (double) distance);
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
