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
#define MAX_ACTIVE_WIDGETS  (50)

#define TAB_TIMER_IDLE   (0xff)

static struct widget *active_widgets[MAX_ACTIVE_WIDGETS] = {NULL};
unsigned char tab_list[MAX_TABS];
static unsigned char active_tab, val, prev_val, tmr = TAB_TIMER_IDLE;
static unsigned char active_tab_idx = 0, source_mode = 0xff;

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


const struct param_def params_tabs[] = {
    PARAM("TABS_MODE", MAV_PARAM_TYPE_UINT8, &config.tab_change.mode, NULL),
    PARAM("TABS_TIME", MAV_PARAM_TYPE_UINT8, &config.tab_change.time_window, NULL),
    PARAM("TABS_CH", MAV_PARAM_TYPE_UINT8, &config.tab_change.ch, NULL),
    PARAM("TABS_CH_MAX", MAV_PARAM_TYPE_UINT16, &config.tab_change.tab_change_ch_max, NULL),
    PARAM("TABS_CH_MIN", MAV_PARAM_TYPE_UINT16, &config.tab_change.tab_change_ch_min, NULL),
    PARAM_END,
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


static struct widget* load_widget(struct widget_config *w_cfg)
{
    const struct widget_ops *w_ops;
    struct widget *w;
    
    w_ops = get_widget_ops(w_cfg->widget_id);
    if (w_ops == NULL)
        return NULL;

    w = (struct widget*) widget_malloc(sizeof(struct widget));
    if (w == NULL) {
        DTABS("load_tab: no mem for widget %d exiting load tabs\n", w_cfg->widget_id);
        return NULL;
    }
    w->ops = w_ops;
    w->cfg = w_cfg;
    w->status = 0;
    if (w_ops->open(w))
        return NULL;

    alloc_canvas(&w->ca, w->cfg);
    schedule_widget(w);

    return w;
}


void load_tab(unsigned char tab)
{
    struct widget_config *w_cfg = &config.widgets[0];
    struct widget *w;
    struct widget_config tmp_wcfg;
    struct widget **aw = active_widgets;

    DTABS("Loading tab %d\n", tab);

    /* stop rendering */
    video_pause();

    while ((*aw) != NULL) {
        if ((*aw)->ops->close != NULL) {
            DTABS("closing widget %p: %s\n", *aw, (*aw)->ops->name);
            (*aw)->ops->close(*aw);
        }
        aw++;
    }
    aw = active_widgets;

    /* reset widgets module */
    widgets_reset();


    if (tab == 0) {
        /* zero tab */
        
        /* console */
        tmp_wcfg.widget_id = WIDGET_CONSOLE_ID;
        tmp_wcfg.x = 0;
        tmp_wcfg.y = 0;
        tmp_wcfg.props.vjust = VJUST_TOP;
        tmp_wcfg.props.hjust = HJUST_LEFT;
        w = load_widget(&tmp_wcfg);

        tmp_wcfg.widget_id = WIDGET_VIDEOLVL_ID;
        tmp_wcfg.props.hjust = HJUST_RIGHT;
        w = load_widget(&tmp_wcfg);


    } else {
        /* load tab widgets */
        while (w_cfg->tab != TABS_END) {
            if (w_cfg->tab == tab) {
                w = load_widget(w_cfg);
                if (w == NULL)
                    break;
                *(aw++) = w;
            }
            w_cfg++;
        }
    }
    *aw = NULL;
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


    if (get_millis() < 5000) {
        if (active_tab != 0)
            load_tab(0);
        return;
    } else if (active_tab == 0) {
        load_tab(1);
    }


    switch (cfg->mode) {
        case TAB_CHANGE_CHANNEL:
        default:
            idx = ((val * tab_list[0]) / 100) + 1;
            new_tab = tab_list[idx];
            if (new_tab != active_tab) {
                DTABS("tab_change_channel: change to tab %d\n", new_tab);
                load_tab((unsigned char) new_tab);
            }
            break;
        case TAB_CHANGE_FLIGHTMODE:
            /* idle */
            if (tmr == TAB_TIMER_IDLE) {
                break;
            } else if (tmr < cfg->time_window) {
                tmr++;
            } else {
                tmr = TAB_TIMER_IDLE;
            }
            break;
        case TAB_CHANGE_TOGGLE:
            /* idle val not acquired */
            if (val == 0)
                break;

            /* idle */
            if (tmr == TAB_TIMER_IDLE) {
                break;
            } else if (tmr < cfg->time_window) {
                tmr++;
                /* switch returned to idle position */
                if (prev_val == val) {
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
                /* wait until switch  returns to idle state */
                if (val == prev_val)
                    tmr = TAB_TIMER_IDLE;
            }

            break;
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


static void tab_switch_channel_cbk(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct tab_change_config *cfg = (struct tab_change_config*) d;
    unsigned int ch_raw;
    long percent;

    ch_raw = mavlink_msg_rc_channels_raw_get_chan(msg, cfg->ch);
    DTABS("cbk: ch_raw = %u\n", ch_raw);

    percent = (( ((long) ch_raw - cfg->tab_change_ch_min) * 100) /
               (cfg->tab_change_ch_max - cfg->tab_change_ch_min));
    DTABS("cbk: percent = %ld\n", percent);

    switch (cfg->mode) {
        case TAB_CHANGE_CHANNEL:
        default:
            if (percent < 0)
                percent = 0;
            else if (percent > 100)
                percent = 100;
            val = (unsigned char) percent;
            break;
        case TAB_CHANGE_TOGGLE:
            if (percent < 50)
                percent = 1;
            else
                percent = 2;
            if (val == 0)
                prev_val = percent;
            
            val = percent;

            /* start timer */
            if ((val != prev_val) && (tmr == TAB_TIMER_IDLE))
                tmr = 0;

            break;
    }

    DTABS("cbk: val=%u prev_val=%d\n", val, prev_val);
}

static void tab_switch_flightmode_cbk(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    unsigned char mav_type;

    val = mavlink_msg_heartbeat_get_custom_mode(msg);
    mav_type = mavlink_msg_heartbeat_get_type(msg);
    if (mav_type !=  MAV_TYPE_FIXED_WING)
        val += 100;

    /* don't switch tab in case failsafe triggers */
    switch (val) {
        case PLANE_MODE_CIRCLE:
        case PLANE_MODE_AUTO:
        case PLANE_MODE_RTL:
        case PLANE_MODE_LOITER:
        case COPTER_MODE_AUTO:
        case COPTER_MODE_LOITER:
        case COPTER_MODE_RTL:
        case COPTER_MODE_CIRCLE:
            return;
        default:
            break;
    }

    if (val != prev_val) {
        /* mode changed */
        DTABS("flightmode_cbk: mode has changed: tmr=%d source_mode=%d mode=%d\n",
                tmr, source_mode, val);
        if (tmr != TAB_TIMER_IDLE) {
            if (source_mode == val) {
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
        prev_val = val;
    }

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
    static struct mavlink_callback *mav_cbk = NULL;
    static struct timer *tab_timer = NULL;
    unsigned char msgid;
    void *cbk;

    params_add(params_tabs);

    /* build tab list from config */
    build_tab_list();

    switch (config.tab_change.mode) {
        case TAB_CHANGE_CHANNEL:
        case TAB_CHANGE_TOGGLE:
        default:
            msgid = MAVLINK_MSG_ID_RC_CHANNELS_RAW;
            cbk = tab_switch_channel_cbk;
            val = 0;
            break;
        case TAB_CHANGE_FLIGHTMODE:
            msgid = MAVLINK_MSG_ID_HEARTBEAT;
            cbk = tab_switch_flightmode_cbk;
            tmr = TAB_TIMER_IDLE;
        case TAB_CHANGE_DEMO:
            cbk = NULL;
            tmr = 0;
            break;
    }

    /* track required mavlink data */
    if (cbk != NULL) {
        if (mav_cbk == NULL) {
            mav_cbk = add_mavlink_callback(msgid, cbk,
                        CALLBACK_PERSISTENT, &config.tab_change);
        } else {
            mav_cbk->msgid = msgid;
            mav_cbk->cbk = cbk;
        }
    }
    
    /* tab switching task (100ms) */
    if (tab_timer == NULL)
        tab_timer = add_timer(TIMER_ALWAYS, 1, tab_switch_task,
                                &config.tab_change);
}



