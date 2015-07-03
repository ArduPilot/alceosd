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
    NULL,
};

#define MAX_TABS 10
#define WIDGET_FIFO_MASK    (0x3f)
#define MAX_WIDGET_ALLOC_MEM (0x1000)

unsigned char tab_list[MAX_TABS];
static unsigned char tidx, new_tidx;

struct widgets_mem_s {
    unsigned int mem[MAX_WIDGET_ALLOC_MEM/2];
    unsigned int alloc_size;
} widgets_mem;

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
    
    return ptr;
}

static void widget_free_mem(void)
{
    widgets_mem.alloc_size = 0;
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


static void tab_switch_cbk(mavlink_message_t *msg, mavlink_status_t *status)
{
    unsigned int ch_raw;
    int v, ch_delta;
    
    switch (config.tab_change_ch) {
        case 0:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan1_raw(msg);
            break;
        case 1:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan2_raw(msg);
            break;
        case 2:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan3_raw(msg);
            break;
        case 3:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan4_raw(msg);
            break;
        case 4:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan5_raw(msg);
            break;
        case 5:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan6_raw(msg);
            break;
        case 6:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan7_raw(msg);
            break;
        case 7:
        default:
            ch_raw = mavlink_msg_rc_channels_raw_get_chan8_raw(msg);
            break;
    }

    ch_delta = (config.tab_change_ch_max - config.tab_change_ch_min) / ((int) tab_list[0]);
    v = ((((int) ch_raw) - ((int) config.tab_change_ch_min)) / ch_delta);

    /* trim value */
    if (v < 0)
      v = 0;
    if (v >= tab_list[0])
      v = tab_list[0] - 1;

    new_tidx = (unsigned char) v + 1;

}

static unsigned char search_on_list(unsigned char *list, unsigned char tab)
{
  unsigned char *p = &list[0];
  char size = *p++;
  while (size-- > 0) {
    if (*p++ == tab)
      return 1;
  }
  return 0;
}

void build_tab_list(void)
{
    struct widget_config *wcfg = &config.widgets[0];
    unsigned char *p = &tab_list[1];

    /* build tab list */
    tab_list[0] = 0;
    while (wcfg->tab != TABS_END) {
        if (!search_on_list(tab_list, wcfg->tab)) {
            *p++ = wcfg->tab;
            tab_list[0]++;
        }
        wcfg++;
        if (tab_list[0] > (MAX_TABS-2))
            break;
    }
    /* add an empty tab (tab 0) at the end */
    *p++ = 0;
    tab_list[0]++;

#if 0
    printf("found %d tabs\n", tab_list[0]);
    int i;
    for (i = 0; i < tab_list[0]; i++)
        printf(" tab%d = %d\n", i, tab_list[i+1]);
#endif
    
    /* trigger tab switch */
    if (tab_list[0] > 0) {
        new_tidx = tab_list[1];
        tidx = 0xff;
    }
}

void widgets_init(void)
{
    build_tab_list();

    widget_free_mem();
    
    /* tab switching callback */
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, tab_switch_cbk, CALLBACK_PERSISTENT, NULL);
}

extern volatile unsigned char sram_busy;

void load_tab(unsigned char tab)
{
    struct widget_config *w_cfg = &config.widgets[0];
    const struct widget_ops *w_ops;
    struct widget *w;
    int ipl;

    while (sram_busy);
    SET_AND_SAVE_CPU_IPL(ipl, 7);

    adc_stop();
    remove_timers(TIMER_WIDGET);
    del_mavlink_callbacks(CALLBACK_WIDGET);
    wfifo.rd = wfifo.wr = 0;

    widget_free_mem();
    free_mem();
    clear_sram();

    while (w_cfg->tab != TABS_END) {
        if (w_cfg->tab == tab) {
            w_ops = get_widget_ops(w_cfg->widget_id);
            if (w_ops != NULL) {
                w = (struct widget*) widget_malloc(sizeof(struct widget));
                if (w == NULL)
                    break;
                w->ops = w_ops;
                w->cfg = w_cfg;
                w->status = 0;
                if (w_ops->init(w))
                    break;
                alloc_canvas(&w->ca, w->cfg->x, w->cfg->y,
                              w->cfg->props.hjust, w->cfg->props.vjust,
                              w->cfg->w, w->cfg->h);
                schedule_widget(w);
            }
        }
        w_cfg++;
    }
    RESTORE_CPU_IPL(ipl);
}


static void render_widget(struct widget *w)
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

    /* check for tab change */
    if (new_tidx != tidx) {
        tidx = new_tidx;
        load_tab(tidx);
    }

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
