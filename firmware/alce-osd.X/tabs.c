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
#define TAB_TIMER_IDLE   (0xff)

#ifdef DEBUG_TABS
#define DTABS(x...) \
    do { \
        shell_printf("TABS: "); \
        shell_printf(x); \
    } while(0)
#else
#define DTABS(x...)
#endif

extern struct alceosd_config config;

unsigned char tab_list[MAX_TABS];

static const struct param_def params_tabs[] = {
    PARAM("TABS_MODE", MAV_PARAM_TYPE_UINT8, &config.tab_change.mode, NULL),
    PARAM("TABS_TIME", MAV_PARAM_TYPE_UINT8, &config.tab_change.time_window, NULL),
    PARAM("TABS_CH", MAV_PARAM_TYPE_UINT8, &config.tab_change.ch, NULL),
    PARAM("TABS_CH_MAX", MAV_PARAM_TYPE_UINT16, &config.tab_change.tab_change_ch_max, NULL),
    PARAM("TABS_CH_MIN", MAV_PARAM_TYPE_UINT16, &config.tab_change.tab_change_ch_min, NULL),
    PARAM_END,
};

static const struct widget_config tab0_widgets[] = {
    { 0, 0, WIDGET_CONSOLE_ID,  0, 0, {JUST_TOP | JUST_LEFT}},
    { 0, 0, WIDGET_VIDEOLVL_ID, 0, 0, {JUST_TOP | JUST_RIGHT}},
    { TABS_END, 0, 0, 0, 0, {0}},
};


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
    struct widget_config *w_cfg;
    static unsigned char active_tab = 0xff;

    DTABS("Loading tab %d\n", tab);
    
    if (active_tab == tab)
        return;

    /* stop rendering */
    video_pause();

    /* reset widgets module */
    widgets_reset();

    /* default video profile */
    video_apply_config(0);

    if (tab == 0) {
        w_cfg = (struct widget_config*) tab0_widgets;
    } else {
        w_cfg = config.widgets;
    }

    /* load widgets config */
    while (w_cfg->tab != TABS_END) {
        if (w_cfg->tab == tab)
            load_widget_config(w_cfg);
        w_cfg++;
    }
    
    load_widgets();

    /* resume video rendering */
    video_resume();
    active_tab = tab;
    DTABS("Loaded\n");
}

static void build_tab_list(void)
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
    /* add an empty tab (tab 255) at the end */
    *p++ = 255;
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

/* return rc_channel in percentage according to config values */
static unsigned char get_rc_channel(struct tab_change_config *cfg)
{
    unsigned int *val;
    void *rc;
    long x;    
    
    if (mavdata_age(MAVDATA_RC_CHANNELS) < 5000)
        rc = mavdata_get(MAVDATA_RC_CHANNELS);
    else
        rc = mavdata_get(MAVDATA_RC_CHANNELS_RAW);
    val = (unsigned int*) (rc + 4 + 2 * cfg->ch);
    x = (long) *(val);
    x = ( ((x - cfg->tab_change_ch_min) * 100) /
          (cfg->tab_change_ch_max - cfg->tab_change_ch_min));
    if (x < 0)
        x = 0;
    else if (x > 100)
        x = 100;
    DTABS("cbk: percent = %ld\n", x);
    return (unsigned char) x;
}

static void tab_switch_task(struct timer *t, void *d)
{
    static unsigned char tmr = TAB_TIMER_IDLE;
    static unsigned char prev_val = 255, active_tab_idx = 0, source_mode = 0xff;
    struct tab_change_config *cfg = d;
    unsigned int val;
    
    switch (cfg->mode) {
        case TAB_CHANGE_CHANNEL:
        default:
            val = get_rc_channel(cfg);
            val = ((val * tab_list[0]) / 101) + 1;
            if ((unsigned char) val != prev_val) {
                DTABS("tab_change_channel: change to tab %d\n", tab_list[val]);
                load_tab((unsigned char) tab_list[val]);
                prev_val = (unsigned char) val;
            }
            break;
        case TAB_CHANGE_TOGGLE:
            val = get_rc_channel(cfg);
            if (val < 50)
                val = 1;
            else
                val = 2;            

            /* store idle position */
            if (prev_val == 255)
                prev_val = (unsigned char) val;
            
            /* idle */
            if (tmr == TAB_TIMER_IDLE) {
                if ((unsigned char) val != prev_val)
                    tmr = 0;
            } else if (tmr < cfg->time_window) {
                tmr++;
                /* switch returned to idle position */
                if (prev_val == (unsigned char) val) {
                    /* next tab */
                    active_tab_idx++;
                    if (active_tab_idx >= tab_list[0])
                        active_tab_idx = 0;
                    load_tab(tab_list[active_tab_idx+1]);
                    tmr = TAB_TIMER_IDLE;
                }
            } else if (tmr == cfg->time_window) {
                tmr++;
                /* previous tab */
                if (active_tab_idx == 0)
                    active_tab_idx = tab_list[0]-1;
                else
                    active_tab_idx--;
                load_tab(tab_list[active_tab_idx+1]);
            } else {
                /* wait until switch returns to idle state */
                if ((unsigned char) val == prev_val)
                    tmr = TAB_TIMER_IDLE;
            }            
            break;
        case TAB_CHANGE_FLIGHTMODE:
        {
            mavlink_heartbeat_t *hb = mavdata_get(MAVDATA_HEARTBEAT);
            unsigned char i;
            const unsigned char mode_ignore_list[] = {
                PLANE_MODE_CIRCLE, PLANE_MODE_AUTO,
                PLANE_MODE_RTL, PLANE_MODE_LOITER,
                COPTER_MODE_AUTO, COPTER_MODE_LOITER,
                COPTER_MODE_RTL, COPTER_MODE_CIRCLE,
            };

            if (tmr < cfg->time_window)
                tmr++;
            else
                tmr = TAB_TIMER_IDLE;
            
            val = (unsigned char) hb->custom_mode;
            if (hb->type != MAV_TYPE_FIXED_WING)
                val += 100;

            /* don't switch tab in case failsafe triggers */
            for (i = 0; i < sizeof(mode_ignore_list); i++)
                if (mode_ignore_list[i] == (unsigned char) val)
                    return;

            if ((unsigned char) val != prev_val) {
                /* mode changed */
                DTABS("flightmode_cbk: mode has changed: tmr=%d source_mode=%d mode=%d\n",
                        tmr, source_mode, val);
                if (tmr != TAB_TIMER_IDLE) {
                    if (source_mode == (unsigned char) val) {
                        /* change to next tab */
                        DTABS("flightmode_cbk: back to the same mode in less than 2 sec\n");
                        active_tab_idx++;
                        if (active_tab_idx >= tab_list[0])
                            active_tab_idx = 0;
                        load_tab(tab_list[active_tab_idx+1]);
                    }
                    tmr = TAB_TIMER_IDLE;
                } else {
                    DTABS("flightmode_cbk: starting timer\n");
                    tmr = 0;
                    source_mode = prev_val;
                }
                prev_val = (unsigned char) val;
            }            
            break;
        }
        case TAB_CHANGE_DEMO:
            tmr++;
            if (tmr > cfg->time_window) {
                /* next tab */
                active_tab_idx++;
                if (active_tab_idx >= tab_list[0])
                    active_tab_idx = 0;
                load_tab(tab_list[active_tab_idx+1]);
                tmr = 0;
            }
            break;
    }
}

void start_tab_switch_task(struct timer *t, void *d)
{
    load_tab(1);
    /* tab switching task (100ms) */
    add_timer(TIMER_ALWAYS, 100, tab_switch_task, d);
}

void tabs_init(void)
{
    /* add mavlink parameters */
    params_add(params_tabs);
    /* build tab list from config */
    build_tab_list();
    /* 5 seconds on tab 0 */
    load_tab(0);
    add_timer(TIMER_ONCE, 5000, start_tab_switch_task, &config.tab_change);
}
