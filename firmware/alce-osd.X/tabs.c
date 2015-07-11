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

//#define DEBUG_TABS
#define MAX_TABS 10

unsigned char tab_list[MAX_TABS];
static unsigned char active_tab, ch_val;

extern struct alceosd_config config;


#ifdef DEBUG_TABS
#define DTABS(x...) \
    do { \
        printf("TABS: "); \
        printf(x); \
    } while(0)
#else
#define DTABS(x...)
#endif


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


void load_tab(unsigned char tab)
{
    struct widget_config *w_cfg = &config.widgets[0];
    const struct widget_ops *w_ops;
    struct widget *w;

    DTABS("Loading tab %d\n", tab);

    /* stop rendering */
    video_pause();
    /* reset widgets module */
    widgets_init();

    /* load tab widgets */
    while (w_cfg->tab != TABS_END) {
        if (w_cfg->tab == tab) {
            w_ops = get_widget_ops(w_cfg->widget_id);
            if (w_ops != NULL) {
                w = (struct widget*) widget_malloc(sizeof(struct widget));
                if (w == NULL) {
                    DTABS("load_tab: no mem for widget %d exiting load tabs\n", w_cfg->widget_id);
                    break;
                }
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
    /* resume video rendering */
    video_resume();
    active_tab = tab;
    DTABS("Loaded\n");
}


static void tab_switch_task(struct timer *t, void *d)
{
    struct tab_change_config *cfg = (struct tab_change_config*) d;
    unsigned int idx;
    unsigned char new_tab;

    switch (cfg->mode) {
        case TAB_CHANGE_CHANNEL:
        default:
            idx = ((ch_val * tab_list[0]) / 100) + 1;
            new_tab = tab_list[idx];
            if (new_tab != active_tab) {
                DTABS("tab_change_channel: change to tab %d\n", new_tab);
                load_tab((unsigned char) new_tab);
            }
            break;
        case TAB_CHANGE_FLIGHTMODE:
            
            break;
        case TAB_CHANGE_TOGGLE:
            break;
    }
}


static void tab_switch_cbk(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct tab_change_config *cfg = (struct tab_change_config*) d;
    unsigned int ch_raw;
    long percent;

    switch (cfg->ch) {
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
    DTABS("cbk: ch_raw = %u\n", ch_raw);

    percent = (( ((long) ch_raw - cfg->tab_change_ch_min) * 100) /
               (cfg->tab_change_ch_max - cfg->tab_change_ch_min));
    DTABS("cbk: percent = %ld\n", percent);

    if (percent < 0)
        percent = 0;
    else if (percent > 100)
        percent = 100;

    ch_val = (unsigned char) percent;
    DTABS("cbk: ch_val = %u\n", ch_val);
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
    DTABS("found %d tabs\n", tab_list[0]);
    int i;
    for (i = 0; i < tab_list[0]; i++)
        DTABS(" tab%d = %d\n", i, tab_list[i+1]);
#endif

    /* load first tab */
    load_tab(tab_list[1]);
}


void tabs_init(void)
{
    /* build tab list from config */
    build_tab_list();

    /* track channel raw values */
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, tab_switch_cbk, CALLBACK_PERSISTENT, &config.tab_change);

    /* tab switching task (100ms) */
    add_timer(TIMER_ALWAYS, 1, tab_switch_task, &config.tab_change);
}
