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
static unsigned char active_tab = 0xff;

static const struct param_def params_tabs[] = {
    PARAM("TABS_MODE", MAV_PARAM_TYPE_UINT8, &config.tab_sw.mode, NULL),
    PARAM("TABS_TIME", MAV_PARAM_TYPE_UINT8, &config.tab_sw.time, NULL),
    PARAM("TABS_CH", MAV_PARAM_TYPE_UINT8, &config.tab_sw.ch, NULL),
    PARAM("TABS_CH_MAX", MAV_PARAM_TYPE_UINT16, &config.tab_sw.ch_max, NULL),
    PARAM("TABS_CH_MIN", MAV_PARAM_TYPE_UINT16, &config.tab_sw.ch_min, NULL),
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

unsigned char get_active_tab(void)
{
    return active_tab;
}

void load_tab(unsigned char tab)
{
    struct widget_config *w_cfg;

    DTABS("Loading tab %d\n", tab);
    
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

unsigned char *get_tab_list(void)
{
    return tab_list;
}

static void tab_switch_task(struct timer *t, void *d)
{
    static unsigned char tmr = TAB_TIMER_IDLE;
    static unsigned char prev_val = 255, active_tab_idx = 0, source_mode = 0xff;
    struct ch_switch *cfg = d;
    unsigned int val;
    
    switch (cfg->mode) {
        case SW_MODE_CHANNEL:
        default:
            val = get_sw_state(cfg);
            val = ((val * tab_list[0]) / 101) + 1;
            if ((unsigned char) val != prev_val) {
                DTABS("tab_change_channel: change to tab %d\n", tab_list[val]);
                load_tab((unsigned char) tab_list[val]);
                prev_val = (unsigned char) val;
            }
            break;
        case SW_MODE_TOGGLE:
            val = get_sw_state(cfg);
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
            } else if (tmr < cfg->time) {
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
            } else if (tmr == cfg->time) {
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
        case SW_MODE_FLIGHTMODE:
        {
            mavlink_heartbeat_t *hb = mavdata_get(MAVDATA_HEARTBEAT);
            unsigned char i;
            const unsigned char mode_ignore_list[] = {
                PLANE_MODE_CIRCLE, PLANE_MODE_AUTO,
                PLANE_MODE_RTL, PLANE_MODE_LOITER,
                COPTER_MODE_AUTO, COPTER_MODE_LOITER,
                COPTER_MODE_RTL, COPTER_MODE_CIRCLE,
            };

            if (tmr < cfg->time)
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
        case SW_MODE_DEMO:
            tmr++;
            if (tmr > cfg->time) {
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
    add_timer(TIMER_ONCE, 5000, start_tab_switch_task, &config.tab_sw);
}


#define SHELL_CMD_CONFIG_ARGS 4
static void shell_cmd_config(char *args, void *data)
{
    struct ch_switch *cfg = &config.tab_sw;
    struct shell_argval argval[SHELL_CMD_CONFIG_ARGS+1], *p;
    unsigned char t, i;
    unsigned int w;

    t = shell_arg_parser(args, argval, SHELL_CMD_CONFIG_ARGS);
    if (t < 1) {
        shell_printf("\nTab switch config:\n");
        shell_printf(" Mode: %d (0:ch percent; 1:flight mode; 2:ch toggle)\n", cfg->mode);
        shell_printf(" Ch:   CH%d\n", cfg->ch + 1);
        shell_printf(" Min:  %d\n", cfg->ch_min);
        shell_printf(" Max:  %d\n", cfg->ch_max);
        shell_printf(" Time: %d00ms\n", cfg->time);
        shell_printf("\noptions: -m <mode> -c <ch> -l <min> -h <max> -t <time>\n");
    } else {
        p = shell_get_argval(argval, 'm');
        if (p != NULL) {
            i = atoi(p->val);
            if (i < SW_MODE_END)
                cfg->mode = i;
        }
        p = shell_get_argval(argval, 'c');
        if (p != NULL) {
            i = atoi(p->val) - 1;
            i = min(i, 18);
            cfg->ch = i;
        }
        p = shell_get_argval(argval, 'l');
        if (p != NULL) {
            w = atoi(p->val);
            w = TRIM(w, 900, 2100);
            cfg->ch_min = w;
        }
        p = shell_get_argval(argval, 'h');
        if (p != NULL) {
            w = atoi(p->val);
            w = TRIM(w, 900, 2100);
            cfg->ch_max = w;
        }
        p = shell_get_argval(argval, 't');
        if (p != NULL) {
            w = atoi(p->val);
            w = w / 100;
            cfg->time = w;
        }
    }
}

#define SHELL_CMD_LOAD_ARGS  1
static void shell_cmd_load(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_LOAD_ARGS+1], *idx, *tab;
    unsigned char t, j;
    
    t = shell_arg_parser(args, argval, SHELL_CMD_LOAD_ARGS);
    
    idx = shell_get_argval(argval, 'i');
    tab = shell_get_argval(argval, 't');
    if ((t != 1) && ((idx == NULL) && (tab == NULL))) {
        shell_printf("\nload tab: [-i <tab index> | -t <tab number>]\n");
        shell_printf(" -i <tab index>   tab index (0 to %d)\n", tab_list[0]-1);
        shell_printf(" -t <tab id>      tab id\n");
    } else {
        if (idx != NULL) {
            j = atoi(idx->val);
            if (j < tab_list[0]) {
                shell_printf("\nLoading tab index %d of %d\n", j, tab_list[0]-1);
                load_tab(tab_list[j+1]);
            } else {
                shell_printf("\nOut of range [0 ... %d]\n", tab_list[0]-1);
            }
        } else {
            j = atoi(tab->val);
            shell_printf("\nLoading tab id %d\n", j);
            load_tab(j);
        }
    }
}

static const struct shell_cmdmap_s tabs_cmdmap[] = {
    {"config", shell_cmd_config, "Tab config", SHELL_CMD_SIMPLE},
    {"load", shell_cmd_load, "Load tab", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_tabs(char *args, void *data)
{
    shell_exec(args, tabs_cmdmap, data);
}
