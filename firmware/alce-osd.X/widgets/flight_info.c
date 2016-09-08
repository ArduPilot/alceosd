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


static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = (struct widget*) d;
    schedule_widget(w);
}


static int open(struct widget *w)
{
    const struct font *f = get_font(1);
    w->priv = (struct flight_stats*) get_flight_stats();

    w->ca.width = 4*60;
    w->ca.height = f->size*9+4;

    /* refresh rate of 0.2 sec */
    add_timer(TIMER_WIDGET, 200, timer_callback, w);
    return 0;
}


static void render(struct widget *w)
{
    struct flight_stats *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[50];
    unsigned long duration = (priv->flight_end - priv->flight_start) / 1000;

    sprintf(buf, "Distance traveled:     %ld m", (unsigned long) priv->total_distance);
    draw_str(buf, 0, 0, ca, 1);

    sprintf(buf, "Maximum altitude:      %d m", priv->max_altitude);
    draw_str(buf, 0, 12*1, ca, 1);
    
    sprintf(buf, "Maximum home altitude: %d m", priv->max_home_altitude);
    draw_str(buf, 0, 12*2, ca, 1);

    sprintf(buf, "Maximum ground speed:  %d m/s", priv->max_gnd_speed);
    draw_str(buf, 0, 12*3, ca, 1);

    sprintf(buf, "Maximum air speed:     %d m/s", priv->max_air_speed);
    draw_str(buf, 0, 12*4, ca, 1);

    sprintf(buf, "Maximum home distance: %d m", priv->max_home_distance);
    draw_str(buf, 0, 12*5, ca, 1);

    sprintf(buf, "Peak current:          %.2fA", ((double) priv->max_bat_current) / 100);
    draw_str(buf, 0, 12*6, ca, 1);

    sprintf(buf, "Total used current:    %dmAh", (unsigned int) priv->total_flight_mah);
    draw_str(buf, 0, 12*7, ca, 1);

    sprintf(buf, "Flight duration:       %02dm%02ds", (unsigned int) duration / 60, (unsigned int) duration % 60);
    draw_str(buf, 0, 12*8, ca, 1);
}


const struct widget_ops flight_info_widget_ops = {
    .name = "Flight information",
    .mavname = "FLTINFO",
    .id = WIDGET_FLIGHT_INFO_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};

