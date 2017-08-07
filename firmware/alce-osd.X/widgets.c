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
extern const struct widget_ops gimbal_widget_ops;
extern const struct widget_ops videolvl_widget_ops;
extern const struct widget_ops alarms_widget_ops;
extern const struct widget_ops sonar_widget_ops;
extern const struct widget_ops vidprf_widget_ops;
extern const struct widget_ops temperature_widget_ops;
extern const struct widget_ops ils_widget_ops;
extern const struct widget_ops clock_widget_ops;

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
    &gimbal_widget_ops,
    &videolvl_widget_ops,
    &alarms_widget_ops,
    &sonar_widget_ops,
    &vidprf_widget_ops,
    &temperature_widget_ops,
    &ils_widget_ops,
    &clock_widget_ops,
    NULL,
};

#define WIDGET_FIFO_MASK        (0x1f)
#define MAX_WIDGET_ALLOC_MEM    (0x400)
#define MAX_ACTIVE_WIDGETS      (CONFIG_MAX_WIDGETS)

struct widgets_mem_s {
    unsigned char mem[MAX_WIDGET_ALLOC_MEM];
    unsigned int alloc_size;
} widgets_mem __attribute__((aligned(2))) = {
    .alloc_size = 0,
};

struct widget_fifo {
    unsigned char rd;
    unsigned char wr;
    unsigned char peak;
    struct widget *fifo[WIDGET_FIFO_MASK + 1];
} wfifo = {
    .rd = 0,
    .wr = 0,
};

extern struct alceosd_config config;
static struct widget *active_widgets[MAX_ACTIVE_WIDGETS];
static unsigned char total_active_widgets = 0;
static struct widget *selected_widget = NULL;

/* custom memory allocator for widgets */
void* widget_malloc(unsigned int size)
{
    unsigned char *ptr;
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

struct widget* load_widget_config(struct widget_config *w_cfg)
{
    const struct widget_ops *w_ops;
    struct widget *w;
    
    w_ops = get_widget_ops(w_cfg->widget_id);
    if (w_ops == NULL)
        return NULL;

    w = (struct widget*) widget_malloc(sizeof(struct widget));
    if (w == NULL)
        return NULL;
    w->ops = w_ops;
    w->cfg = w_cfg;
    w->status = 0;
    if (w_ops->open(w))
        return NULL;

    active_widgets[total_active_widgets++] = w;
    return w;
}

void load_widgets(void)
{
    struct widget *w;
    unsigned char i;
    
    for (i = 0; i < total_active_widgets; i++) {
        w = active_widgets[i];
        if (alloc_canvas(&w->ca, w->cfg))
            continue;
        if (w->ops->render)
            schedule_widget(w);
    }
}

static void close_widgets(void)
{
    struct widget *w;
    unsigned char i;

    for (i = 0; i < total_active_widgets; i++) {
        w = active_widgets[i];
        if (w->ops->close)
            w->ops->close(w);
    }
    total_active_widgets = 0;
}

static inline void render_widget(struct widget *w)
{
    if (init_canvas(&w->ca) == 0) {
        w->ops->render(w);
        if (selected_widget == w)
            draw_rect(0, 0, w->ca.width-1, w->ca.height-1, ((get_millis16()/500) & 1) ? 3 : 1, &w->ca);
        schedule_canvas(&w->ca);
    }
    w->status = 0;
}

extern volatile unsigned char sram_busy;

static void widgets_process(void)
{
    struct widget *w;

    if (wfifo.rd == wfifo.wr)
        return;
    //while (wfifo.rd != wfifo.wr) {
    w = wfifo.fifo[wfifo.rd++];
    wfifo.rd &= WIDGET_FIFO_MASK;
    render_widget(w);
    //    if (!sram_busy)
    //        break;
    //}
}

void schedule_widget(struct widget *w)
{
    if (w->status == WIDGET_SCHEDULED)
        return;

    w->status = WIDGET_SCHEDULED;
    wfifo.fifo[wfifo.wr++] = w;
    wfifo.wr &= WIDGET_FIFO_MASK;
    wfifo.peak = max(wfifo.peak, wfifo.wr - wfifo.rd);
}

void reconfig_widget(struct widget *w)
{
    if (w->ops->render) {
        reconfig_canvas(&w->ca, w->cfg);
        w->ops->render(w);
    }
}

static void blink_selected_widget_timer(struct timer *t, void *d)
{
    if (selected_widget == NULL)
        remove_timer(t);
    else
        schedule_widget(d);
}

void select_widget(struct widget *w)
{
    if (selected_widget != NULL)
        schedule_widget(selected_widget);
    selected_widget = w;
    add_timer(TIMER_ALWAYS, 500, blink_selected_widget_timer, w);
}

void widgets_reset(void)
{
    /* close active widgets */
    close_widgets();
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
    
    selected_widget = NULL;
}

extern struct alceosd_config config;

enum {
    WID_PARAM_TAB = 0,
    WID_PARAM_X,
    WID_PARAM_Y,
    WID_PARAM_HJUST,
    WID_PARAM_VJUST,
    WID_PARAM_MODE,
    WID_PARAM_SOURCE,
    WID_PARAM_UNITS,
    WID_PARAM_PARAM1,
    WID_PARAM_PARAM2,
    WID_PARAM_PARAM3,
    WID_PARAM_PARAM4,
    WID_PARAM_END
};

const char widget_param_lut[WID_PARAM_END][10] = {
    [WID_PARAM_TAB] = "TAB",
    [WID_PARAM_X] = "X",
    [WID_PARAM_Y] = "Y",
    [WID_PARAM_HJUST] = "HJUST",
    [WID_PARAM_VJUST] = "VJUST",
    [WID_PARAM_MODE] = "MODE",
    [WID_PARAM_SOURCE] = "SOURCE",
    [WID_PARAM_UNITS] = "UNITS",
    [WID_PARAM_PARAM1] = "PARAM1",
    [WID_PARAM_PARAM2] = "PARAM2",
    [WID_PARAM_PARAM3] = "PARAM3",
    [WID_PARAM_PARAM4] = "PARAM4",
};


/* appends a param to the name */
static void get_widget_param(unsigned int pidx,
            struct widget_config *wcfg, struct param_def *p)
{
    struct param_value *pv = p->value;
    if (pidx >= WID_PARAM_END)
        return;

    strncat(p->name, widget_param_lut[pidx], 8);
    switch (pidx) {
        case WID_PARAM_TAB:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->tab;
            break;
        case WID_PARAM_X:
            p->type = MAV_PARAM_TYPE_INT16;
            pv->param_int16 = wcfg->x;
            break;
        case WID_PARAM_Y:
            p->type = MAV_PARAM_TYPE_INT16;
            pv->param_int16 = wcfg->y;
            break;
        case WID_PARAM_VJUST:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->props.vjust;
            break;
        case WID_PARAM_HJUST:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->props.hjust;
            break;
        case WID_PARAM_MODE:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->props.mode;
            break;
        case WID_PARAM_UNITS:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->props.units;
            break;
        case WID_PARAM_SOURCE:
            p->type = MAV_PARAM_TYPE_UINT8;
            pv->param_uint8 = wcfg->props.source;
            break;
        case WID_PARAM_PARAM1:
        case WID_PARAM_PARAM2:
        case WID_PARAM_PARAM3:
        case WID_PARAM_PARAM4:
            p->type = MAV_PARAM_TYPE_UINT16;
            pv->param_uint16 = wcfg->params[pidx - WID_PARAM_PARAM1];
            break;
        default:
            break;
    }
}

static int set_widget_param(struct widget_config *wcfg, struct param_def *p, char *pname)
{
    unsigned char i;
    struct param_value *pv = p->value;
    /* mavlink param protocol is broken - messages always have the param as float */
    float v = pv->param_float;
    
    for(i = 0; i < WID_PARAM_END; i++) {
        if (strcmp(pname, widget_param_lut[i]) == 0)
            break;
    }

    if (i == WID_PARAM_END) {
        console_printf("lut failed\n");
        return -1;
    }

    console_printf("pidx=%d\n", i);

    switch (i) {
        case WID_PARAM_TAB:
            wcfg->tab = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_X:
            wcfg->x = (int) v; //pv->param_int16;
            break;
        case WID_PARAM_Y:
            wcfg->y = (int) v; //pv->param_int16;
            break;
        case WID_PARAM_VJUST:
            wcfg->props.vjust = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_HJUST:
            wcfg->props.hjust = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_MODE:
            wcfg->props.mode = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_UNITS:
            wcfg->props.units = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_SOURCE:
            wcfg->props.source = (unsigned char) v; //pv->param_uint8;
            break;
        case WID_PARAM_PARAM1:
        case WID_PARAM_PARAM2:
        case WID_PARAM_PARAM3:
        case WID_PARAM_PARAM4:
            wcfg->params[i - WID_PARAM_PARAM1] = (unsigned int) v; //pv->param_uint16;
            break;
    }
    return (int) i;
}

static unsigned int widgets_total_params(void)
{
    struct widget_config *wcfg = config.widgets;
    unsigned int total = 0;

    while ((wcfg++)->tab != TABS_END) {
        total++;
    }
    return WID_PARAM_END * total;
}

static void widgets_get_params(int idx, struct param_def *p)
{
    struct widget_config *wcfg = config.widgets;
    const struct widget_ops *wops;
    unsigned int table_idx = idx / WID_PARAM_END;
    unsigned int param_idx = idx % WID_PARAM_END;
    unsigned int table_total = 0;

    while ((wcfg++)->tab != TABS_END) {
        table_total++;
    }

    if (table_idx >= table_total)
        return;

    /* get widget at location */
    wcfg = &config.widgets[table_idx];
    wops = get_widget_ops(wcfg->widget_id);

    sprintf(p->name, "%s%1d_", wops->mavname, wcfg->uid);
    get_widget_param(param_idx, wcfg, p);
}

static int widgets_set_params(struct param_def *p)
{
    struct widget_config *wcfg = config.widgets;
    const struct widget_ops *wops;
    const struct widget_ops **w = all_widget_ops;

    unsigned int idx = 0;
    int ret = -1;
    unsigned char uid;
    char *pname;
    char buf[17];

    strcpy(buf, p->name);
    pname = strchr(buf, '_');
    if (pname == NULL)
        return -1;
    
    *pname++ = '\0';
    uid = *(pname-2) - '0';
    *(pname-2) = '\0';

    /* find widget_ops */
    while ((*w) != NULL) {
        if (strcmp(buf, (*w)->mavname) == 0)
            break;
        w++;
    }
    if ((*w) == NULL)
        return -1;

    wops = (*w);
    while (wcfg->tab != TABS_END) {
        if ((uid == wcfg->uid) && (wops->id == wcfg->widget_id)) {
            ret = set_widget_param(wcfg, p, pname);
            break;
        }
        idx++;
        wcfg++;
    }
    if (ret == -1) {
        /* TODO: check space for new widget */
        /* widget+uid not found in config*/
        memset(wcfg, 0, sizeof(struct widget_config));
        wcfg->uid = uid;
        wcfg->widget_id = wops->id;
        ret = set_widget_param(wcfg, p, pname);
        (wcfg+1)->tab = TABS_END;
    }
    return idx * ret;
}

unsigned char widget_get_uid(unsigned char id)
{
    struct widget_config *wcfg;
    unsigned char uid = 0, busy;

    for (uid = 0; uid < 10; uid++) {
        wcfg = config.widgets;
        busy = 0;
        while (wcfg->tab != TABS_END) {
            if ((id == wcfg->widget_id) && (uid == wcfg->uid))
                busy = 1;
            wcfg++;
        }
        if (!busy)
            break;
    }
    return uid;
}


const struct param_dynamic_def widget_dynamic_params = {
    .set = widgets_set_params,
    .get = widgets_get_params,
    .total = widgets_total_params,
};

void widgets_init(void)
{
    const struct widget_ops **w = all_widget_ops;
    params_set_dynamic_params(&widget_dynamic_params);
    while ((*w) != NULL) {
        if ((*w)->init != NULL)
            (*w)->init();
        w++;
    }
    process_add(widgets_process, "WIDGETS", 10);
}


static void shell_cmd_stats(char *args, void *data)
{
    shell_printf("Widgets mem: %u/%u bytes\n",
        widgets_mem.alloc_size, MAX_WIDGET_ALLOC_MEM);

    shell_printf("Widgets fifo: size=%u peak=%u max=%u\n",
                (wfifo.wr - wfifo.rd) & WIDGET_FIFO_MASK, wfifo.peak, WIDGET_FIFO_MASK+1);
    
}

static void shell_cmd_loaded(char *args, void *data)
{
    struct widget *w;
    unsigned char i;
    
    shell_printf("Loaded widgets:\n");
    shell_printf("\n id+uid | name                 |   x |   y | hjust | vjust\n");
    shell_printf(  "--------+----------------------+-----+-----+-------+-------\n");
    for (i = 0; i < total_active_widgets; i++) {
        w = active_widgets[i];
        shell_printf("  %02u+%02u | %20s | %3d | %3d | %5d | %5d\n",
            w->ops->id, w->cfg->uid, w->ops->name, w->cfg->x, w->cfg->y,
            w->cfg->props.hjust, w->cfg->props.vjust);
    }
}

static void shell_cmd_list(char *args, void *data)
{
    const struct widget_ops *w_ops;
    struct widget_config *w_cfg = config.widgets;
    struct shell_argval argval[2], *p;
    unsigned char t, tab, i, nr_tabs;
    unsigned char *tl = get_tab_list();
    
    t = shell_arg_parser(args, argval, 1);
    
    p = shell_get_argval(argval, 't');
    if (p != NULL) {
        tab = atoi(p->val);
    } else {
        tab = get_active_tab();
    }

    shell_printf("syntax: [-t <tab_id>]\n");
    shell_printf("    ommiting tab_id lists active tab\n");
    shell_printf("    using tab_id 0 shows all widgets\n\n");
    shell_printf("Widget configuration for ");
    if (tab == 0)
        shell_printf("all tabs\n");
    else
        shell_printf("tab %u:\n", tab);
    shell_printf("\n id+uid | name                 |   x |   y | hjust | vjust\n");
    //shell_printf(  "-----+----------------------+-----+-----+-------+-------\n");

    if (tab == 0) {
        nr_tabs = (*(tl++)) - 1;
        tab = *(tl++);
    } else {
        nr_tabs = 1;
    }
    
    /* list widgets from all tabs */
    for (i = 0; i < nr_tabs; i++) {
        shell_printf(  "--------+--- TAB %3u ----------+-----+-----+-------+-------\n", tab);

        while (w_cfg->tab != TABS_END) {
            if (w_cfg->tab == tab) {
                w_ops = get_widget_ops(w_cfg->widget_id);

                shell_printf("  %02u+%02u | %20s |%4d |%4d | %5d | %5d\n",
                    w_cfg->widget_id, w_cfg->uid, w_ops->name,
                    w_cfg->x, w_cfg->y, w_cfg->props.hjust, w_cfg->props.vjust);
            }
            w_cfg++;
        }
        if (nr_tabs > 1) {
            tab = *(tl++);
            w_cfg = config.widgets;
        }
    }
}



static void shell_cmd_avail(char *args, void *data)
{
    const struct widget_ops **w_ops = all_widget_ops;

    if ((strlen(args) > 0) && atoi(args) == 1) {
        /* raw mode */
        while ((*w_ops) != NULL) {
            shell_printf("%u,%s,%s\n",
                (*w_ops)->id, (*w_ops)->name, (*w_ops)->mavname);
            w_ops++;
        }
        shell_printf("--\n");
    } else {
        shell_printf("Tab %u widgets\n", get_active_tab());
        shell_printf("\n id | name                 | mavname  | init | open | render | close\n");
        shell_printf(  "----+----------------------+----------+------+------+--------+-------\n");
        while ((*w_ops) != NULL) {
            shell_printf(" %02u | %20s | %8s | %4p | %4p |   %4p |  %4p\n",
                (*w_ops)->id, (*w_ops)->name, (*w_ops)->mavname,
                (*w_ops)->init, (*w_ops)->open, (*w_ops)->render, (*w_ops)->close);
            w_ops++;
        }
    }
}

static void shell_cmd_add(char *args, void *data)
{
    struct shell_argval argval[3], *p;
    struct widget_config *w_cfg = config.widgets;
    const struct widget_ops **w_ops = all_widget_ops;
    unsigned char t, uid;
    char *ptr;
    int tab, id;

    if (strlen(args) == 0) {
        shell_printf("syntax: widgets add <mavname>|-i <id> [-t <tab_id>]\n");
        shell_printf("      <mavname>       mavlink name\n");
        shell_printf("      -i <id>         widget global id\n");
        shell_printf("      -t <tab_id>     tab id number (1-254)\n");
        shell_printf("\n note: use MAVNAME or ID, not both\n\n");
    } else {
        /* tab id */
        tab = get_active_tab();
        t = shell_arg_parser(args, argval, 2);
        p = shell_get_argval(argval, 't');
        if (p != NULL) {
            tab = atoi(p->val);
        }
        tab = max(1, min(254, tab));

        /* widget id  */
        id = -1;
        p = shell_get_argval(argval, 'i');
        if (p != NULL) {
            /* by id */
            id = atoi(p->val);
        } else {
            /* by mavname */
            ptr = strchr(args, ' ');
            if (ptr != NULL) {
                *ptr = '\0';
            }
            while ((*w_ops) != NULL) {
                if (strcmp(args, (*w_ops)->mavname) == 0) {
                    id = (*w_ops)->id;
                    break;
                }
                w_ops++;
            }
        }
        
        if (id != -1) {
            while (w_cfg->tab != TABS_END)
                w_cfg++;
            uid = widget_get_uid(id);
            if (uid > 9) {
                shell_printf("Can't add: widgets (id %d) limit reached\n", id);
                return;
            }
            w_cfg->uid = uid;
            w_cfg->tab = tab;
            w_cfg->widget_id = id;
            w_cfg->x = 0;
            w_cfg->y = 0;
            w_cfg->props.raw = JUST_VCENTER | JUST_HCENTER;

            w_cfg++;
            w_cfg->tab = TABS_END;

            /* rebuild tab list */
            build_tab_list();
            load_tab(tab);

            shell_printf("Added widget: %02u+%02u to tab %d\n", id, uid, tab);
        } else {
            shell_printf("Widget not found: %s / %d\n", args, id);
        }
    }
}

static void shell_cmd_rm(char *args, void *data)
{
    struct widget_config *w_cfg = config.widgets;
    char *ptr;
    unsigned int id, uid;
    int tab;

    if (strlen(args) == 0) {
        shell_printf("syntax: widgets rm [<id>+<uid>|all]\n");
        shell_printf("      <id>    widget global id\n");
        shell_printf("      <uid>   widget unique id (on same widget type)\n");
    } else {
        
        if (strcmp(args, "all") == 0) {
            w_cfg->tab = TABS_END;
            return;
        }
        
        ptr = strchr(args, '+');
        *ptr++ = '\0';

        id = atoi(args);
        uid = atoi(ptr);

        tab = 0;
        while (w_cfg->tab != TABS_END) {
            if ((w_cfg->uid == uid) && (w_cfg->widget_id == id)) {
                tab = w_cfg->tab;
                break;
            }
            w_cfg++;
        }

        if (tab > 0) {
            do {
                memcpy(w_cfg, w_cfg+1, sizeof(struct widget_config));
            } while ((w_cfg++)->tab != TABS_END);
            
            load_tab(tab);
            
            shell_printf("Removed widget: %02u+%02u from tab %d\n", id, uid, tab);
        } else {
            shell_printf("Not found: %02u+%02u\n", id, uid);
        }
    }
}

static void shell_cmd_bitmap(char *args, void *data)
{
    struct widget *w;
    s16 id, uid, i;
    s8 *ptr, found = 0;
    
    if (strlen(args) == 0) {
        shell_printf("syntax: widgets bitmap <id>+<uid>\n");
        shell_printf("      <id>    widget global id\n");
        shell_printf("      <uid>   widget unique id (on same widget type)\n");
    } else {
        ptr = strchr(args, '+');
        *ptr++ = '\0';
        id = atoi(args);
        uid = atoi(ptr);
        for (i = 0; i < total_active_widgets; i++) {
            w = active_widgets[i];
            if ((w->cfg->uid == uid) && (w->cfg->widget_id == id) && (w->ca.size > 0)) {
                found = 1;
                break;
            }
        }
        if (found) {
            shell_printf("w:%u\nh:%u\n", w->ca.rwidth, w->ca.height);
            shell_write_eds(w->ca.buf, w->ca.size);
        } else {
            shell_printf("w:0\nh:0\n");
        }
    }
}

#define SHELL_CMD_CFG_ARGS 13
static void shell_cmd_config(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_CFG_ARGS+1], *p;
    struct widget *w;
    s16 id, uid, i, t, val;
    s8 *ptr, found = 0;
    struct widget_config *w_cfg = config.widgets;

    t = shell_arg_parser(args, argval, SHELL_CMD_CFG_ARGS);
    p = shell_get_argval(argval, 'i');
    if (p == NULL) {
        shell_printf("syntax: widgets cfg -i <id+uid> <options...>\n");
        shell_printf("      -i <id+uid>     widget global id + unique id\n");
        shell_printf("    Options:\n");
        shell_printf("      -w              select widget (blink boundary)\n");
        shell_printf("      -t <tab id>     tab id number (1-254)\n");
        shell_printf("      -x <x pos>      x position\n");
        shell_printf("      -y <y pos>      y position\n");
        shell_printf("      -h <h just>     horizontal justification\n");
        shell_printf("      -v <v just>     vertical justification\n");

        shell_printf("      -m <mode>       drawing mode\n");
        shell_printf("      -s <source>     data source\n");
        shell_printf("      -u <units>      units\n");

        shell_printf("      -a <value>      param1 value\n");
        shell_printf("      -b <value>      param2 value\n");
        shell_printf("      -c <value>      param3 value\n");
        shell_printf("      -d <value>      param4 value\n\n");
    } else {
        /* widget id+uid */
        ptr = strchr(p->val, '+');
        if (ptr != NULL) {
            *ptr++ = '\0';
            uid = atoi(ptr);
        } else {
            uid = -1;
        }
        id = atoi(p->val);
        
        while (w_cfg->tab != TABS_END) {
            if ((w_cfg->uid == uid) && (w_cfg->widget_id == id)) {
                found = 1;
                break;
            }
            w_cfg++;
        }

        if (found && (t == 1)) {
            /* dump widget config */
            shell_printf("t:%u x:%d y:%d h:%u v:%u ",
                    w_cfg->tab, w_cfg->x, w_cfg->y,
                    w_cfg->props.hjust, w_cfg->props.vjust);
            shell_printf("m:%u s:%u u:%u ", w_cfg->props.mode,
                    w_cfg->props.source, w_cfg->props.units);
            shell_printf("a:%u b:%u c:%u d:%u\n",
                    w_cfg->params[0], w_cfg->params[1],
                    w_cfg->params[2], w_cfg->params[3]);
            return;
        }
        
        if (!found) {
            if (t == 13) {
                /* create new if all parameters are provided */
                (w_cfg+1)->tab = TABS_END;
                uid = widget_get_uid(id);
                w_cfg->uid = uid;
                w_cfg->widget_id = id;
                shell_printf("Created widget: %02d+%02d\n", id, uid);
            } else {
                shell_printf("Widget not found: %02d+%02d\n", id, uid);
                return;
            }
        } else {
            shell_printf("Changed widget: %02u+%02u\n", id, uid);
        }

        for (i = 0; i < t; i++) {
            //shell_printf("\nkey=%c val=%s", argval[i].key, argval[i].val);
            val = atoi(argval[i].val);
            switch (argval[i].key) {
                case 't':
                    /* change tab */
                    val = max(1, min(254, val));
                    w_cfg->tab = val;
                    break;
                case 'x':
                    w_cfg->x = val;
                    break;
                case 'y':
                    w_cfg->y = val;
                    break;
                case 'h':
                    w_cfg->props.hjust = (val & 3);
                    break;
                case 'v':
                    w_cfg->props.vjust = (val & 3);
                    break;
                case 'm':
                    w_cfg->props.mode = (val & 0xf);
                    break;
                case 's':
                    w_cfg->props.source = (val & 7);
                    break;
                case 'u':
                    w_cfg->props.units = (val & 3);
                    break;
                case 'a':
                    w_cfg->params[0] = val;
                    break;
                case 'b':
                    w_cfg->params[1] = val;
                    break;
                case 'c':
                    w_cfg->params[2] = val;
                    break;
                case 'd':
                    w_cfg->params[3] = val;
                    break;
                default:
                    break;
            }
        }
        
        /* check if loaded and reconfig */
        for (i = 0; i < total_active_widgets; i++) {
            w = active_widgets[i];
            if ((w->cfg->uid == uid) && (w->cfg->widget_id == id))
                break;
        }
        if (i == total_active_widgets)
            return;

        p = shell_get_argval(argval, 'w');
        if (p != NULL) {
            val = atoi(p->val);
            if (val)
                select_widget(w);
        } else {
            reconfig_widget(w);
        }
    }
}


static const struct shell_cmdmap_s widgets_cmdmap[] = {
    {"stats", shell_cmd_stats, "Widgets module stats", SHELL_CMD_SIMPLE},
    {"loaded", shell_cmd_loaded, "List loaded widgets", SHELL_CMD_SIMPLE},
    {"list", shell_cmd_list, "List widgets from a tab", SHELL_CMD_SIMPLE},
    {"available", shell_cmd_avail, "List all available widgets", SHELL_CMD_SIMPLE},
    {"add", shell_cmd_add, "Add widget", SHELL_CMD_SIMPLE},
    {"rm", shell_cmd_rm, "Remove widget", SHELL_CMD_SIMPLE},
    {"bitmap", shell_cmd_bitmap, "Get widget bitmap", SHELL_CMD_SIMPLE},
    {"cfg", shell_cmd_config, "Config widget", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_widgets(char *args, void *data)
{
    shell_exec(args, widgets_cmdmap, data);
}
