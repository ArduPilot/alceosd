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

extern struct alceosd_config config;

const char *mavdata_type_name[] = {
    "CHAR", "UINT8_T", "INT8_T",
    "UINT16_T", "INT16_T",
    "UINT32_T", "INT32_T",
    "UINT64_T", "INT64_T",
    "FLOAT", "DOUBLE"
};

#define MAVDATA_INFO(x, y) const mavlink_message_info_t mi_##x = MAVLINK_MESSAGE_INFO_##x ; static mavlink_##y##_t y
#define MAVDATA_STATE(x,y) [MAVDATA_##x] = { .data = &y, .id = MAVLINK_MSG_ID_##x }
#define MAVDATA_OPS(x,y) [MAVLINK_MSG_ID_##x] = { \
        .decode = ((void*) mavlink_msg_##y##_decode), \
        .offset = MAVDATA_##x, \
        .info = &mi_##x }

MAVDATA_INFO(HEARTBEAT, heartbeat);
MAVDATA_INFO(SYS_STATUS, sys_status);
MAVDATA_INFO(ATTITUDE, attitude);
MAVDATA_INFO(GLOBAL_POSITION_INT, global_position_int);
MAVDATA_INFO(MISSION_ITEM, mission_item);
MAVDATA_INFO(VFR_HUD, vfr_hud);
MAVDATA_INFO(RC_CHANNELS_RAW, rc_channels_raw);
MAVDATA_INFO(RC_CHANNELS, rc_channels);
MAVDATA_INFO(HOME_POSITION, home_position);

struct mavdata_state m[MAVDATA_TOTAL] = {
    MAVDATA_STATE(HEARTBEAT, heartbeat),
    MAVDATA_STATE(SYS_STATUS, sys_status),
    MAVDATA_STATE(ATTITUDE, attitude),
    MAVDATA_STATE(GLOBAL_POSITION_INT, global_position_int),
    MAVDATA_STATE(MISSION_ITEM, mission_item),
    MAVDATA_STATE(VFR_HUD, vfr_hud),
    MAVDATA_STATE(RC_CHANNELS_RAW, rc_channels_raw),
    MAVDATA_STATE(RC_CHANNELS, rc_channels),
    MAVDATA_STATE(HOME_POSITION, home_position),
};

static const struct mavdata_decode_ops decode_ops[256] = {
    MAVDATA_OPS(HEARTBEAT, heartbeat),
    MAVDATA_OPS(SYS_STATUS, sys_status),
    MAVDATA_OPS(ATTITUDE, attitude),
    MAVDATA_OPS(GLOBAL_POSITION_INT, global_position_int),
    MAVDATA_OPS(MISSION_ITEM, mission_item),
    MAVDATA_OPS(VFR_HUD, vfr_hud),
    MAVDATA_OPS(RC_CHANNELS_RAW, rc_channels_raw),
    MAVDATA_OPS(RC_CHANNELS, rc_channels),
    MAVDATA_OPS(HOME_POSITION, home_position),
};

void mavdata_store(mavlink_message_t *msg)
{
    const struct mavdata_decode_ops *ops = &decode_ops[msg->msgid];
    struct mavdata_state *s;
    unsigned long t;
    int target_sys, target_comp;
    
    mavlink_get_targets(msg, &target_sys, &target_comp);

    if (! ((target_sys == -1) || (target_sys == config.mav.osd_sysid)))
        return;
   
    if (ops->decode != NULL) {
        s = &m[ops->offset];
        ops->decode(msg, s->data);
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
    return get_millis() - m[id].time;
}

unsigned long mavdata_period(unsigned int id)
{
    return m[id].period;
}


static void shell_cmd_stats(char *args, void *data)
{
    unsigned char i;
    unsigned long now = get_millis(), age;

    shell_printf("\n id | age(ms) | rate(Hz) | name\n");
    shell_printf("----+---------+----------+---------------\n");
    for (i = 0; i < MAVDATA_TOTAL; i++) {
        shell_printf("%3d | ", m[i].id);
        age = (now - mavdata_time(i));
        if (mavdata_time(i) == 0) {
            shell_printf(" unseen |          | ");
        } else {
            shell_printf("%7lu | ", age);
            if (age < 60000)
                shell_printf("%8.1f | ", 1000.0/mavdata_period(i));
            else
                shell_printf("         | ");
        }
        shell_printf("%s\n", decode_ops[m[i].id].info->name);
    }
}

#define SHELL_CMD_DISPLAY_ARGS 1
static void shell_cmd_display(char *args, void *data)
{
    const struct mavdata_decode_ops *ops;
    const mavlink_message_info_t *info;
    struct shell_argval argval[SHELL_CMD_DISPLAY_ARGS+1], *p;
    unsigned char t, i, j;
    unsigned int data_offset;
    
    t = shell_arg_parser(args, argval, SHELL_CMD_DISPLAY_ARGS);
    
    p = shell_get_argval(argval, 'i');
    if ((t < 1) || (p == NULL)) {
        shell_printf("\ndisplay mavlink data: [-i <msgid>]\n");
        shell_printf(" -i <msgid>  mavlink msgid (0=ALL)\n");
    } else {
        i = atoi(p->val);
        ops = &decode_ops[i];
        if (ops->decode == NULL) {
            shell_printf("\nnot found\n");
            return;
        }
        info = ops->info;
        shell_printf("\n%s\n", info->name);
        for (j = 0; j < info->num_fields; j++) {
            shell_printf("[%8s] ", mavdata_type_name[info->fields[j].type]);
            shell_printf("%34s = ", info->fields[j].name);
            data_offset = (unsigned int) (m[ops->offset].data + info->fields[j].structure_offset);
            switch (info->fields[j].type) {
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
