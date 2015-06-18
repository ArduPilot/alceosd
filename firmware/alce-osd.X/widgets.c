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


extern struct widget altitude_widget;
extern struct widget horizon_widget;
extern struct widget vario_graph_widget;
extern struct widget compass_widget;
extern struct widget speed_widget;
extern struct widget bat_info_widget;
extern struct widget gps_info_widget;
extern struct widget flightmode_widget;
extern struct widget throttle_widget;
extern struct widget rssi_widget;

const struct widget *all_widgets[] = {
    &altitude_widget,
    &bat_info_widget,
    &compass_widget,
    &flightmode_widget,
    &gps_info_widget,
    &horizon_widget,
    &rssi_widget,
    &speed_widget,
    &throttle_widget,
    &vario_graph_widget,
    NULL,
};


#define WIDGET_FIFO_MASK    (0x1f)

struct widget_fifo {
    const struct widget *fifo[WIDGET_FIFO_MASK + 1];
    unsigned int rd;
    unsigned int wr;
} wfifo = {
    .rd = 0,
    .wr = 0,
};


const struct widget *get_widget(unsigned int id)
{
    const struct widget **w = all_widgets;

    while ((*w) != NULL) {
        if ((*w)->id == id)
            break;
        else
            w++;
    }
    return (*w);
}


void load_tab(unsigned char tab)
{
    struct widget_config *wcfg = &config.widgets[0];
    const struct widget *w;

    del_mavlink_callbacks();
    free_mem();
    clear_video();

    while (wcfg->tab != TABS_END) {
        if (wcfg->tab == tab) {
            w = get_widget(wcfg->widget_id);
            if (w != NULL) {
                w->init(wcfg);
                schedule_widget(w);
            }
        }
        wcfg++;
    }    
}


void widgets_process(void)
{
    const struct widget *w;

    while (wfifo.rd != wfifo.wr) {
        w = wfifo.fifo[wfifo.rd++];
        wfifo.rd &= WIDGET_FIFO_MASK;
        w->render();
        //if (w->render())
        //    schedule_widget(w);
    }
}


void schedule_widget(const struct widget *w)
{
    unsigned int r = wfifo.rd;

    while (r != wfifo.wr) {
        if (wfifo.fifo[r++] == w)
            return;
        r &= WIDGET_FIFO_MASK;
    }
    wfifo.fifo[wfifo.wr++] = w;
    wfifo.wr &= WIDGET_FIFO_MASK;
}
