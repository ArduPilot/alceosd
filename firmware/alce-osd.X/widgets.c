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

/* all availabe widgets */
extern const struct widget_ops altitude_widget_ops;
extern const struct widget_ops bat_info_widget_ops;
extern const struct widget_ops compass_widget_ops;
extern const struct widget_ops flightmode_widget_ops;
extern const struct widget_ops gps_info_widget_ops;
extern const struct widget_ops home_info_widget_ops;
extern const struct widget_ops horizon_widget_ops;
extern const struct widget_ops radar_widget_ops;
extern const struct widget_ops rc_channels_widget_ops;
extern const struct widget_ops rssi_widget_ops;
extern const struct widget_ops speed_widget_ops;
extern const struct widget_ops throttle_widget_ops;
extern const struct widget_ops vario_graph_widget_ops;
extern const struct widget_ops wind_widget_ops;
extern const struct widget_ops flight_info_widget_ops;
extern const struct widget_ops console_widget_ops;


const struct widget_ops *all_widget_ops[] = {
    &altitude_widget_ops,
    &bat_info_widget_ops,
    &compass_widget_ops,
    &flightmode_widget_ops,
    &gps_info_widget_ops,
    &home_info_widget_ops,
    &horizon_widget_ops,
    &radar_widget_ops,
    &rc_channels_widget_ops,
    &rssi_widget_ops,
    &speed_widget_ops,
    &throttle_widget_ops,
    &vario_graph_widget_ops,
    &wind_widget_ops,
    &flight_info_widget_ops,
    &console_widget_ops,
    NULL,
};

#define WIDGET_FIFO_MASK    (0x3f)
#define MAX_WIDGET_ALLOC_MEM (0x1000)

struct widgets_mem_s {
    unsigned int mem[MAX_WIDGET_ALLOC_MEM/2];
    unsigned int alloc_size;
} widgets_mem = {
    .alloc_size = 0,
};

struct widget_fifo {
    struct widget *fifo[WIDGET_FIFO_MASK + 1];
    unsigned int rd;
    unsigned int wr;
} wfifo = {
    .rd = 0,
    .wr = 0,
};


/* custom memory allocator for widgets */
void* widget_malloc(unsigned int size)
{
    unsigned int *ptr;
    size = (size + 1) & 0xfffe;
    if ((widgets_mem.alloc_size + size) >= MAX_WIDGET_ALLOC_MEM)
        return NULL;

    ptr = &widgets_mem.mem[widgets_mem.alloc_size];
    widgets_mem.alloc_size += size;

    memset(ptr, 0, size);

    return ptr;
}


const struct widget_ops *get_widget_ops(unsigned int id)
{
    const struct widget_ops **w = all_widget_ops;

    while ((*w) != NULL) {
        if ((*w)->id == id)
            break;
        else
            w++;
    }
    return (*w);
}


static inline void render_widget(struct widget *w)
{
    if (init_canvas(&w->ca, 0) == 0) {
        w->ops->render(w);
        schedule_canvas(&w->ca);
    }
    w->status = 0;
}

extern volatile unsigned char sram_busy;

void widgets_process(void)
{
    struct widget *w;

    while (wfifo.rd != wfifo.wr) {
        w = wfifo.fifo[wfifo.rd++];
        wfifo.rd &= WIDGET_FIFO_MASK;
        render_widget(w);
        if (!sram_busy)
            break;
    }
}


void schedule_widget(struct widget *w)
{
    if (w->status == WIDGET_SCHEDULED)
        return;

    w->status = WIDGET_SCHEDULED;
    wfifo.fifo[wfifo.wr++] = w;
    wfifo.wr &= WIDGET_FIFO_MASK;
}


void widgets_reset(void)
{
    /* remove widget related timers/tasks */
    remove_timers(TIMER_WIDGET);
    /* remove widget related mavlink callbacks */
    del_mavlink_callbacks(CALLBACK_WIDGET);
    /* reset widget fifo */
    wfifo.rd = wfifo.wr = 0;
    /* reset widgets mem allocator */
    widgets_mem.alloc_size = 0;
    /* release all canvas memory */
    free_mem();
    /* clear display */
    clear_sram();
}


void widgets_init(void)
{
    const struct widget_ops **w = all_widget_ops;
    while ((*w) != NULL) {
        if ((*w)->init != NULL)
            (*w)->init();
        w++;
    }
}
