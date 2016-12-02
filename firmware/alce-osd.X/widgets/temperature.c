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
    add_timer(TIMER_WIDGET, 1000, render_timer, w);
    return 0;
}

static void render(struct widget *w)
{
    struct canvas *ca = &w->ca;
    char buf[10];
    mavlink_scaled_pressure_t *sp = mavdata_get(MAVLINK_MSG_ID_SCALED_PRESSURE);
    float temperature = sp->temperature / 100.0;
    char unit_char = 'C';

    if (get_units(w->cfg) == UNITS_IMPERIAL) {
        temperature = temperature * 1.8 + 32;
        unit_char = 'F';
    }
    
    sprintf(buf, "%.2f%c", (double) temperature, unit_char);
    draw_jstr(buf, X_SIZE, Y_SIZE/2, JUST_RIGHT | JUST_VCENTER, ca, 2);
}


const struct widget_ops temperature_widget_ops = {
    .name = "Temperature",
    .mavname = "TEMPER",
    .id = WIDGET_TEMPERATURE_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
