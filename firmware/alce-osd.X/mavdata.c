/*
    AlceOSD - Graphical OSD
    Copyright (C) 2016  Luis Alves

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


#define MAVDATA_INFO(x, y) \
        mavlink_##y##_t alce_##y; \
        const mavlink_message_info_t __attribute__((space(psv))) mi_##x = MAVLINK_MESSAGE_INFO_##x
#define MAVDATA_DEF(x,y) \
        m[MAVLINK_MSG_ID_##x].data = &alce_##y; \
        m[MAVLINK_MSG_ID_##x].decode = ((void*) mavlink_msg_##y##_decode); \
        m[MAVLINK_MSG_ID_##x].info = &mi_##x; \
        m[MAVLINK_MSG_ID_##x].info_pag = __builtin_psvpage(&mi_##x)

extern struct alceosd_config config;

struct mavdata_state m[256];

MAVDATA_INFO(HEARTBEAT, heartbeat);
MAVDATA_INFO(SYS_STATUS, sys_status);
MAVDATA_INFO(ATTITUDE, attitude);                           /* 30 */
MAVDATA_INFO(GLOBAL_POSITION_INT, global_position_int);     /* 33 */
MAVDATA_INFO(MISSION_ITEM, mission_item);
MAVDATA_INFO(VFR_HUD, vfr_hud);
MAVDATA_INFO(RC_CHANNELS_RAW, rc_channels_raw);
MAVDATA_INFO(RC_CHANNELS, rc_channels);
MAVDATA_INFO(HOME_POSITION, home_position);
MAVDATA_INFO(GPS_RAW_INT, gps_raw_int);                     /* 24 */
MAVDATA_INFO(GPS2_RAW, gps2_raw);
MAVDATA_INFO(TERRAIN_REPORT, terrain_report);
MAVDATA_INFO(NAV_CONTROLLER_OUTPUT, nav_controller_output);
MAVDATA_INFO(MISSION_CURRENT, mission_current);
MAVDATA_INFO(RANGEFINDER, rangefinder);
MAVDATA_INFO(WIND, wind);
MAVDATA_INFO(SCALED_PRESSURE, scaled_pressure);
MAVDATA_INFO(SYSTEM_TIME, system_time);

void mavdata_init(void)
{
    /* mavlink info section is > 32k
       due to pic33ep architecture it needs to be inited on runtine 
       because the __builtin_psvpage isn't able to generate the page
       address at compile time */
    
    memset(m, 0, sizeof(struct mavdata_state) * 256);
    MAVDATA_DEF(HEARTBEAT, heartbeat);
    MAVDATA_DEF(SYS_STATUS, sys_status);
    MAVDATA_DEF(ATTITUDE, attitude);
    MAVDATA_DEF(GLOBAL_POSITION_INT, global_position_int);
    MAVDATA_DEF(MISSION_ITEM, mission_item);
    MAVDATA_DEF(VFR_HUD, vfr_hud);
    MAVDATA_DEF(RC_CHANNELS_RAW, rc_channels_raw);
    MAVDATA_DEF(RC_CHANNELS, rc_channels);
    MAVDATA_DEF(HOME_POSITION, home_position);
    MAVDATA_DEF(GPS_RAW_INT, gps_raw_int);
    MAVDATA_DEF(GPS2_RAW, gps2_raw);
    MAVDATA_DEF(TERRAIN_REPORT, terrain_report);
    MAVDATA_DEF(NAV_CONTROLLER_OUTPUT, nav_controller_output);
    MAVDATA_DEF(MISSION_CURRENT, mission_current);
    MAVDATA_DEF(RANGEFINDER, rangefinder);
    MAVDATA_DEF(WIND, wind);
    MAVDATA_DEF(SCALED_PRESSURE, scaled_pressure);
    MAVDATA_DEF(SYSTEM_TIME, system_time);
}

void mavdata_store(mavlink_message_t *msg)
{
    struct mavdata_state *s;
    unsigned long t;
    int target_sys, target_comp;
    
    mavlink_get_targets(msg, &target_sys, &target_comp);

    if (! ((target_sys == -1) || (target_sys == config.mav.osd_sysid)))
        return;
   
    s = &m[msg->msgid];
    if (s->decode != NULL) {
        s->decode(msg, s->data);
        t = get_millis();
        s->period = t - s->time;
        s->time = t;
    }
}

void* mavdata_get(unsigned int id)
{
    return m[id].data;
}

unsigned long mavdata_time(unsigned int id)
{
    return m[id].time;
}

unsigned long mavdata_age(unsigned int id)
{
    if (m[id].time != 0)
        return get_millis() - m[id].time;
    else
        return 99999999;
}

unsigned long mavdata_period(unsigned int id)
{
    return m[id].period;
}

static void mavdata_info_name(u16 id, char *name)
{
    int pag, new_pag = m[id].info_pag;
    char *src;

    pag = DSRPAG;
    DSRPAG = new_pag;
    src = (s8*) m[id].info->name;
    do {
        *(name++) = *(src);
    } while (*(src++) != '\0');
    DSRPAG = pag;
}

static u16 mavdata_info_num_fields(u16 id)
{
    int pag, new_pag = m[id].info_pag;
    u16 ret = 0;

    pag = DSRPAG;
    DSRPAG = new_pag;
    ret = m[id].info->num_fields;
    DSRPAG = pag;
    return ret;
}

static u16 mavdata_info_field_type(u16 id, u8 nr)
{
    int pag, new_pag = m[id].info_pag;
    u16 ret = 0;

    pag = DSRPAG;
    DSRPAG = new_pag;
    ret = (u16) m[id].info->fields[nr].type;
    DSRPAG = pag;
    return ret;
}

static u16 mavdata_info_field_structoffset(u16 id, u8 nr)
{
    int pag, new_pag = m[id].info_pag;
    u16 ret = 0;

    pag = DSRPAG;
    DSRPAG = new_pag;
    ret = m[id].info->fields[nr].structure_offset;
    DSRPAG = pag;
    return ret;
}

static void mavdata_info_field_name(u16 id, u8 nr, char *name)
{
    int pag, new_pag = m[id].info_pag;
    char *src;

    pag = DSRPAG;
    DSRPAG = new_pag;
    src = (s8*) m[id].info->fields[nr].name;
    do {
        *(name++) = *(src);
    } while (*(src++) != '\0');
    DSRPAG = pag;
}

static void shell_cmd_stats(char *args, void *data)
{
    char buf[20];
    u32 age;
    u16 i;
    
    shell_printf(" id | age(ms) | rate(Hz) | name\n");
    shell_printf("----+---------+----------+---------------\n");
    for (i = 0; i < 256; i++) {
        if (m[i].decode == NULL)
            continue;
        shell_printf("%3d |", i);
        age = mavdata_age(i);
        if (mavdata_time(i) == 0) {
            shell_printf(" no data |          | ");
        } else {
            shell_printf(" %7lu |", age);
            if (age < 60000)
                shell_printf(" %8.1f | ", 1000.0/mavdata_period(i));
            else
                shell_printf("          | ");
        }
        mavdata_info_name(i, buf);
        shell_printf("%s\n", buf);
    }
}

#define SHELL_CMD_DISPLAY_ARGS 1
static void shell_cmd_display(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_DISPLAY_ARGS+1], *p;
    unsigned char t, j;
    u16 i;
    char buf[40];
    u16 num_fields;
    
    t = shell_arg_parser(args, argval, SHELL_CMD_DISPLAY_ARGS);
    
    p = shell_get_argval(argval, 'i');
    if ((t < 1) || (p == NULL)) {
        shell_printf("display mavlink data: [-i <msgid>]\n");
        shell_printf(" -i <msgid>  mavlink msgid (0=ALL)\n");
    } else {
        i = atoi(p->val);
        if (m[i].decode == NULL) {
            shell_printf("not found\n");
            return;
        }
        mavdata_info_name(i, buf);
        num_fields = mavdata_info_num_fields(i);
        shell_printf("%s\n", buf);
        for (j = 0; j < num_fields; j++) {
            u16 ftype = mavdata_info_field_type(i, j);
            u16 data_offset = (u16) (mavdata_info_field_structoffset(i, j) + m[i].data);
            mavdata_info_field_name(i, j, buf);
            shell_printf("[%8s] ", mavdata_type_name[ftype]);
            shell_printf("%34s = ", buf);
            switch (ftype) {
                case MAVLINK_TYPE_CHAR:
            	case MAVLINK_TYPE_INT8_T:
                {
                    char v = *((char*) data_offset);
                    shell_printf("%d (0x%02x)", v, v);
                    break;
                }
                case MAVLINK_TYPE_UINT8_T:
                {
                    unsigned char v = *((unsigned char*) data_offset);
                    shell_printf("%u (0x%02x)", v, v);
                    break;
                }
                case MAVLINK_TYPE_INT16_T:
                {
                    int v = *((int*) data_offset);
                    shell_printf("%d (0x%04x)", v, v);
                    break;
                }
                case MAVLINK_TYPE_UINT16_T:
                {
                    unsigned int v = *((unsigned int*) data_offset);
                    shell_printf("%u (0x%04x)", v, v);
                    break;
                }
                case MAVLINK_TYPE_INT32_T:
                {
                    long v = *((long*) data_offset);
                    shell_printf("%ld (0x%08lx)", v, v);
                    break;
                }
                case MAVLINK_TYPE_UINT32_T:
                {
                    unsigned long v = *((unsigned long*) data_offset);
                    shell_printf("%lu (0x%08lx)", v, v);
                    break;
                }
                case MAVLINK_TYPE_FLOAT:
                    shell_printf("%f", (float) *((float*) data_offset));
                    break;
                default:
                    shell_printf("?");
                    break;
            }
            shell_printf("\n");
        }
    } 
}


static const struct shell_cmdmap_s mavlink_cmdmap[] = {
    {"stats", shell_cmd_stats, "Brief list of all captured data", SHELL_CMD_SIMPLE},
    {"display", shell_cmd_display, "Display mavlink data", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_mavdata(char *args, void *data)
{
    shell_exec(args, mavlink_cmdmap, data);
}
