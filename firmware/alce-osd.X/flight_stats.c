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

#define NO_HEADING 0xfff

struct flight_stats stats;
static struct flight_data_priv {
    u16 *rssi_adc;
} priv;

const struct flight_alarm_info fl_info[] = {
    { .name = "BATTERY", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "BATTERY1", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "BATTERY2", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "RSSI", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "DISTANCE", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "AIRSPEED", .type = MAV_PARAM_TYPE_REAL32 },
    { .name = "ALTITUDE", .type = MAV_PARAM_TYPE_UINT16 },
    { .name = "GPS", .type = MAV_PARAM_TYPE_UINT8 },
};

struct flight_stats* get_flight_stats(void)
{
    return &stats;
}

static void find_launch_heading(struct timer *t, void *d);

static void calc_stats(struct timer *t, void *d)
{
    struct home_data *home = get_home_data();
    mavlink_vfr_hud_t *hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    mavlink_sys_status_t *s = mavdata_get(MAVLINK_MSG_ID_SYS_STATUS);
    
    /* accumulate distance */
    stats.total_distance += (hud->groundspeed / 10);
    stats.total_flight_mah += ((float) s->current_battery) / 3600;

    stats.max_air_speed = max(hud->airspeed, stats.max_air_speed);
    stats.max_gnd_speed = max(hud->groundspeed, stats.max_gnd_speed);
    stats.max_altitude  = max(hud->alt, stats.max_altitude);
    stats.max_home_distance = max((unsigned int) home->distance, stats.max_home_distance);
    stats.max_home_altitude = max(home->altitude, stats.max_home_altitude);
    stats.max_bat_current = max(s->current_battery, stats.max_bat_current);

    stats.flight_end = get_millis();

    /* try to guess a landing */
    if ((hud->throttle < 5) &&
        (hud->airspeed < 5) &&
        (home->distance < 50) &&
        (abs(home->altitude) < 10)) {

        remove_timer(t);
        stats.launch_heading = NO_HEADING;

        /* determine launch heading */
        add_timer(TIMER_ALWAYS, 500, find_launch_heading, NULL);
    }

}

static void start_calc_stats(void)
{
    /* flight start time */
    stats.flight_start = get_millis();
    stats.max_air_speed = 0;
    stats.max_gnd_speed = 0;
    stats.max_altitude = 0;
    stats.max_home_distance = 0;
    stats.max_home_altitude = 0;

    stats.max_bat_current = 0;
    stats.total_flight_mah = 0;

    /* start calcs in a 100ms interval */
    add_timer(TIMER_ALWAYS, 100, calc_stats, NULL);
}

static void find_launch_heading(struct timer *t, void *d)
{
    struct home_data *home = get_home_data();
    mavlink_vfr_hud_t *hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);

    if (home->lock != HOME_LOCKED)
        return;

    if ((hud->throttle > 10) &&
        (hud->airspeed > 5) &&
        (home->altitude > 5))
        stats.launch_heading = hud->heading;

    if (stats.launch_heading != NO_HEADING) {
        /* found */
        //printf("found launch heading\n");
        remove_timer(t);
        /* start calcs */
        start_calc_stats();
    }
}

static void calc_mah(struct timer *t, void *d)
{
    mavlink_sys_status_t *s = mavdata_get(MAVLINK_MSG_ID_SYS_STATUS);
    stats.total_mah += ((float) s->current_battery) / 3600;
}

static void rssi_monitor(void)
{
    mavlink_rc_channels_raw_t *rcr;
    mavlink_rc_channels_t *rc;
    u16 *val_ptr, val;
    
    
    /* rssi monitor */
    switch (config.rssi.mode.source) {
        default:
            /* RC Channels */
            if (mavdata_age(MAVLINK_MSG_ID_RC_CHANNELS) < 5000) {
                rc = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS);
                val_ptr = &rc->chan1_raw;
            } else {
                rcr = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
                val_ptr = &rcr->chan1_raw;
            }
            val = *(val_ptr + (u16)config.rssi.mode.source);
            break;
        case RSSI_SOURCE_MAVLINK:
            if (mavdata_age(MAVLINK_MSG_ID_RC_CHANNELS) < 5000) {
                rc = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS);
                val = rc->rssi;
            } else {
                rcr = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
                val = rcr->rssi;
            }
            break;
        case RSSI_SOURCE_ADC0:
        case RSSI_SOURCE_ADC1:
            val = *(priv.rssi_adc);
            break;
    }
    
    switch (config.rssi.mode.units) {
        case RSSI_UNITS_PERCENT:
            val = (( ((long) val - config.rssi.min) * 100) /
                    (config.rssi.max - config.rssi.min));
            val = TRIM(val, 0, 100);
            break;
        case RSSI_UNITS_RAW:
        default:
            break;
    }

    stats.rssi = val;
}

static inline int is_alarm_enabled(struct flight_alarm *fa)
{
    return (fa->cfg != NULL);
}

static inline void test_and_set_alarm(struct flight_alarm *fa)
{
    float value = cast2float(fa->value, fa->info->type);
    u16 timer = fa->ctrl & ~FL_ALARM_ACTIVE;
    u16 active = 0;
    
    switch (fa->cfg->props.mode) {
        default:
        case FL_ALARM_MODE_LOW:
            if (value < fa->cfg->value) {
                if (++timer >= fa->cfg->timer)
                    active = 1;
            } else {
                timer = 0;
            }
            break;
        case FL_ALARM_MODE_HIGH:
            if (value > fa->cfg->value) {
                if (++timer >= fa->cfg->timer)
                    active = 1;
            } else {
                timer = 0;
            }
            break;
    }
    
    if (active && !(fa->ctrl & FL_ALARM_ACTIVE)) {
        if (fa->cfg->props.mode == 1)
            console_print("ALARM: LOW ");
        else
            console_print("ALARM: HIGH ");
        console_printf("%s = %f\n", fa->info->name, value);
    }
    
    fa->ctrl = timer | (active << 15);
}

void *get_flight_alarm_source(u8 id)
{
    mavlink_sys_status_t *s = mavdata_get(MAVLINK_MSG_ID_SYS_STATUS);
    mavlink_gps_raw_int_t *gps = mavdata_get(MAVLINK_MSG_ID_GPS_RAW_INT);
    mavlink_vfr_hud_t *vfr = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    mavlink_global_position_int_t *gp = mavdata_get(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
    struct home_data *home = get_home_data();
    void *ret = NULL;

    switch (id) {
        default:
            break;
        case FL_ALARM_ID_MAVBAT:
            ret = &s->voltage_battery;
            break;
        case FL_ALARM_ID_RSSI:
            ret = &stats.rssi;
            break;
        case FL_ALARM_ID_GPS:
            ret = &gps->satellites_visible;
            break;
        case FL_ALARM_ID_SPEED:
            ret = &vfr->airspeed;
            break;
        case FL_ALARM_ID_ALT:
            ret = &gp->relative_alt;
            break;
        case FL_ALARM_ID_DIST:
            ret = &home->distance;
            break;
    }
    return ret;
}

static void init_monitor(void)
{
    struct flight_alarm_config *fac = config.flight_alarm;
    u8 i;
        
    memset(stats.fl_alarms, 0, sizeof(struct flight_alarm) * MAX_FLIGHT_ALARMS);
    for (i = 0; i < MAX_FLIGHT_ALARMS; i++) {
        if (fac->props.id == FL_ALARM_ID_END)
            break;
        if (fac->props.id == 0xff)
            continue;
        stats.fl_alarms[i].cfg = fac;
        stats.fl_alarms[i].ctrl = 0;
        stats.fl_alarms[i].info = &fl_info[fac->props.id];
        stats.fl_alarms[i].value = get_flight_alarm_source(fac->props.id);
        fac++;
        /*shell_printf("fl_alarms: enabled alarm %s l=%u h=%u\n",
                stats.fl_alarms[fac->id].name,
                stats.fl_alarms[fac->id].cfg->low,
                stats.fl_alarms[fac->id].cfg->high);*/
    }
    
    switch (config.rssi.mode.source) {
        case RSSI_SOURCE_ADC0:
            adc_start(100);
            adc_link_ch(0, &priv.rssi_adc);
            break;
        case RSSI_SOURCE_ADC1:
            adc_start(100);
            adc_link_ch(1, &priv.rssi_adc);
            break;
        default:
            break;
    }
}

static void monitor(struct timer *t, void *d)
{
    struct flight_alarm *fa;
    u8 i;

    rssi_monitor();
    
    for (i = 0; i < MAX_FLIGHT_ALARMS; i++) {
        fa = &stats.fl_alarms[i];
        if (fa->cfg == NULL)
            break;
        test_and_set_alarm(fa);
        /*shell_printf("fl_alarms: monitor %s v=%u l=%u h=%u f=%2x\n",
                fa->name, (u16) *(fa->value), fa->cfg->low, fa->cfg->high, fa->flags);*/
    }
}

void init_flight_stats(void)
{
    stats.launch_heading = NO_HEADING;
    stats.total_mah = 0;

    init_monitor();
    
    /* determine launch heading */
    add_timer(TIMER_ALWAYS, 500, find_launch_heading, NULL);
    add_timer(TIMER_ALWAYS, 100, calc_mah, NULL);
    add_timer(TIMER_ALWAYS, 100, monitor, NULL);
}


static void shell_cmd_stats(char *args, void *data)
{
    shell_printf("Flight stats:\n");
}

#define SHELL_CMD_WARNINGS_ARGS 5
static void shell_cmd_alarms(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_WARNINGS_ARGS+1], *p;
    u8 t, n = 0xff, id, action = 0;
    struct flight_alarm *fa;
    struct flight_alarm_config *fac;

    t = shell_arg_parser(args, argval, SHELL_CMD_WARNINGS_ARGS);

    p = shell_get_argval(argval, 'a');
    if ((t < 1) || (p == NULL)) {
        shell_printf("Available alarms:\n");
        for (n = 0; n < FL_ALARM_ID_END; n++) {
            shell_printf("id=%u %-8s value_type=%s\n", n, fl_info[n].name,
                    mavdata_type_name[fl_info[n].type]);
        }
        
        shell_printf("\nActive alarms:\n");
        for (n = 0; n < MAX_FLIGHT_ALARMS; n++) {
            fa = &stats.fl_alarms[n];
            if (fa->cfg == NULL)
                break;
            shell_printf("nr%u (%u)%-8s %8s=%.2f active=%u cfg=", n,
                    fa->cfg->props.id, fa->info->name, mavdata_type_name[fa->info->type],
                    cast2float(fa->value, fa->info->type), fa->ctrl >> 15);

            if (fa->cfg)
                shell_printf("(mode=%u val=%.2f timer=%u)\n",
                    fa->cfg->props.mode, fa->cfg->value, fa->cfg->timer);
            else
                shell_printf("(none)\n");
        }
        shell_printf("\nsyntax: flight alarm -a <action> [-n <nr>|-i <id>] -v <value> -t <timer>\n");
        shell_printf("      <action> 0=remove, 1=add low limit, 2=add high limit, 3=remove all\n");
    } else {
        action = atoi(p->val);
        switch (action) {
            default:
            case 0:
                /* remove alarm */
                p = shell_get_argval(argval, 'n');
                if (p != NULL) {
                    n = atoi(p->val);
                }
    
                fa = &stats.fl_alarms[n];
                fac = fa->cfg;
                /* mark empty config */
                fac->props.id = 0x1f;

                /* remove alarm */
                shell_printf("Removed %s alarm nr %u\n",
                        fa->info->name, n);
                while (fa->cfg != NULL) {
                    memcpy(fa, fa+1, sizeof(struct flight_alarm));
                    fa++;
                }
                break;
            case 1:
            case 2:
                /* add/modify alarm */
                p = shell_get_argval(argval, 'n');
                if (p != NULL) {
                    /* modify */
                    n = atoi(p->val);
                    fa = &stats.fl_alarms[n];
                    fac = fa->cfg;
                }
                if (n == 0xff) {
                    /* add */
                    p = shell_get_argval(argval, 'i');
                    if (p != NULL) {
                        /* modify */
                        id = atoi(p->val);
                    } else {
                        shell_printf("Invalid id=%s\n", p->val);
                        return;
                    }
                    
                    /* search for same alarm id+mode */
                    fac = config.flight_alarm;
                    while ((fac->props.id != FL_ALARM_ID_END) && (fac->props.id != 0x1f)) {
                        if ((fac->props.mode == action) &&
                            (fac->props.id == id))
                            break;
                        fac++;
                    }
                    fa = stats.fl_alarms;
                    while (fa->cfg != NULL) {
                        if (fa->cfg == fac)
                            break;
                        fa++;
                    }
                    
                    if ((fac->props.id == FL_ALARM_ID_END) || (fac->props.id == 0x1f)) {
                        shell_printf("Adding new alarm id=%u\n", id);
                        /* not found, add new config */
                        (fac+1)->props.id = FL_ALARM_ID_END;
                        fac->props.id = id;
                        fac->timer = 0;
                        fac->value = 0;
                        fa->cfg = fac;
                        fa->info = &fl_info[id];
                        fa->value = get_flight_alarm_source(id);
                        fa->ctrl = 0;
                    } else {
                        shell_printf("Modifying alarm id=%u\n", id);
                    }
                }
                
                fac->props.mode = action;
                
                p = shell_get_argval(argval, 'v');
                if (p != NULL) {
                    fac->value = atof(p->val);
                }
                p = shell_get_argval(argval, 't');
                if (p != NULL) {
                    fac->timer = atoi(p->val);
                }
                
                break;
            case 3:
                config.flight_alarm[0].props.id = FL_ALARM_ID_END;
                stats.fl_alarms[0].cfg = NULL;
                break;
        }
    }
}

#define SHELL_CMD_CONFIG_ARGS 4
static void shell_cmd_rssi(char *args, void *data)
{
    struct rssi_config *cfg = &config.rssi;
    struct shell_argval argval[SHELL_CMD_CONFIG_ARGS+1], *p;
    unsigned char t, i;
    unsigned int w;

    t = shell_arg_parser(args, argval, SHELL_CMD_CONFIG_ARGS);
    if (t < 1) {
        shell_printf("RSSI config:\n");
        shell_printf(" Source: %u (0-17:RC1-18; 29:ADC0; 30:ADC1; 31:Mavlink RSSI)\n", cfg->mode.source);
        shell_printf(" Units:  %u (0:percent; 1:raw)\n", cfg->mode.units);
        shell_printf(" Min:    %u\n", cfg->min);
        shell_printf(" Max:    %u\n", cfg->max);
        shell_printf("\nconfig options: -s <source> -u <units> -l <min> -h <max>\n");
    } else {
        p = shell_get_argval(argval, 's');
        if (p != NULL) {
            cfg->mode.source = atoi(p->val) & 0x1f;
        }
        p = shell_get_argval(argval, 'u');
        if (p != NULL) {
            cfg->mode.units = atoi(p->val) & 1;
        }
        p = shell_get_argval(argval, 'l');
        if (p != NULL) {
            cfg->min = atoi(p->val);
        }
        p = shell_get_argval(argval, 'h');
        if (p != NULL) {
            cfg->max = atoi(p->val);
        }
    }
}

static const struct shell_cmdmap_s flight_cmdmap[] = {
    {"rssi", shell_cmd_rssi, "RSSI", SHELL_CMD_SIMPLE},
    {"stats", shell_cmd_stats, "Flight stats", SHELL_CMD_SIMPLE},
    {"alarms", shell_cmd_alarms, "Alarms", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_flight(char *args, void *data)
{
    shell_exec(args, flight_cmdmap, data);
}
