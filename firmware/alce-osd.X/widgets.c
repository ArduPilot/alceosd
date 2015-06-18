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

#define MAX_TABS 10
#define WIDGET_FIFO_MASK    (0x1f)

unsigned char tab_list[MAX_TABS];
static unsigned char tidx, new_tidx;

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
    
    /* tab switching callback */
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, tab_switch_cbk);
    set_mavlink_callback_type(CALLBACK_PERSISTENT, tab_switch_cbk);
}

extern volatile unsigned char sram_busy;

void load_tab(unsigned char tab)
{
    struct widget_config *wcfg = &config.widgets[0];
    const struct widget *w;
    int ipl;

    while (sram_busy);
    SET_AND_SAVE_CPU_IPL(ipl, 7);

    del_mavlink_callbacks(CALLBACK_WIDGET);
    wfifo.rd = wfifo.wr = 0;

    free_mem();
    clear_sram();

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
    RESTORE_CPU_IPL(ipl);
}


void widgets_process(void)
{
    const struct widget *w;

    /* check for tab change */
    if (new_tidx != tidx) {
        tidx = new_tidx;
        load_tab(tidx);
    }

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
