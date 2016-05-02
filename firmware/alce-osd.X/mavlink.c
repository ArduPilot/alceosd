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

#define MAX_MAVLINK_CALLBACKS 40
#define MAX_MAVLINK_ROUTES 10


static struct mavlink_callback callbacks[MAX_MAVLINK_CALLBACKS];
static unsigned char nr_callbacks = 0;

static unsigned char uav_sysid = 1, osd_sysid = 200;
static unsigned char active_channel_mask = 0, total_routes = 0;

static unsigned int pidx = 0, total_params = 0;

struct uart_client mavlink_uart_clients[MAVLINK_COMM_NUM_BUFFERS];

static struct mavlink_route_entry {
    unsigned char ch;
    unsigned char sysid;
    unsigned char compid;
} routes[MAX_MAVLINK_ROUTES];

const struct param_def params_mavlink[] = {
    PARAM("MAV_UAVSYSID", MAV_PARAM_TYPE_UINT8, &uav_sysid, NULL),
    PARAM("MAV_OSDSYSID", MAV_PARAM_TYPE_UINT8, &osd_sysid, NULL),
    PARAM_END,
};


/* additional helper functions */
unsigned int mavlink_msg_rc_channels_raw_get_chan(mavlink_message_t *msg, unsigned char ch)
{
    return _MAV_RETURN_uint16_t(msg,  4 + ch*2);
}

static void get_targets(mavlink_message_t *msg, int *sysid, int *compid)
{
    *sysid = -1;
    *compid = -1;
    switch (msg->msgid) {
        // these messages only have a target system
        case MAVLINK_MSG_ID_CAMERA_FEEDBACK:
            *sysid = mavlink_msg_camera_feedback_get_target_system(msg);
            break;
        case MAVLINK_MSG_ID_CAMERA_STATUS:
            *sysid = mavlink_msg_camera_status_get_target_system(msg);
            break;
        case MAVLINK_MSG_ID_CHANGE_OPERATOR_CONTROL:
            *sysid = mavlink_msg_change_operator_control_get_target_system(msg);
            break;
        case MAVLINK_MSG_ID_SET_MODE:
            *sysid = mavlink_msg_set_mode_get_target_system(msg);
            break;
        case MAVLINK_MSG_ID_SET_GPS_GLOBAL_ORIGIN:
            *sysid = mavlink_msg_set_gps_global_origin_get_target_system(msg);
            break;

        // these support both target system and target component
        case MAVLINK_MSG_ID_DIGICAM_CONFIGURE:
            *sysid  = mavlink_msg_digicam_configure_get_target_system(msg);
            *compid = mavlink_msg_digicam_configure_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_DIGICAM_CONTROL:
            *sysid  = mavlink_msg_digicam_control_get_target_system(msg);
            *compid = mavlink_msg_digicam_control_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_FENCE_FETCH_POINT:
            *sysid  = mavlink_msg_fence_fetch_point_get_target_system(msg);
            *compid = mavlink_msg_fence_fetch_point_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_FENCE_POINT:
            *sysid  = mavlink_msg_fence_point_get_target_system(msg);
            *compid = mavlink_msg_fence_point_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MOUNT_CONFIGURE:
            *sysid  = mavlink_msg_mount_configure_get_target_system(msg);
            *compid = mavlink_msg_mount_configure_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MOUNT_CONTROL:
            *sysid  = mavlink_msg_mount_control_get_target_system(msg);
            *compid = mavlink_msg_mount_control_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MOUNT_STATUS:
            *sysid  = mavlink_msg_mount_status_get_target_system(msg);
            *compid = mavlink_msg_mount_status_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_RALLY_FETCH_POINT:
            *sysid  = mavlink_msg_rally_fetch_point_get_target_system(msg);
            *compid = mavlink_msg_rally_fetch_point_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_RALLY_POINT:
            *sysid  = mavlink_msg_rally_point_get_target_system(msg);
            *compid = mavlink_msg_rally_point_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_SET_MAG_OFFSETS:
            *sysid  = mavlink_msg_set_mag_offsets_get_target_system(msg);
            *compid = mavlink_msg_set_mag_offsets_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_COMMAND_INT:
            *sysid  = mavlink_msg_command_int_get_target_system(msg);
            *compid = mavlink_msg_command_int_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_COMMAND_LONG:
            *sysid  = mavlink_msg_command_long_get_target_system(msg);
            *compid = mavlink_msg_command_long_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL:
            *sysid  = mavlink_msg_file_transfer_protocol_get_target_system(msg);
            *compid = mavlink_msg_file_transfer_protocol_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_GPS_INJECT_DATA:
            *sysid  = mavlink_msg_gps_inject_data_get_target_system(msg);
            *compid = mavlink_msg_gps_inject_data_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_LOG_ERASE:
            *sysid  = mavlink_msg_log_erase_get_target_system(msg);
            *compid = mavlink_msg_log_erase_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_LOG_REQUEST_DATA:
            *sysid  = mavlink_msg_log_request_data_get_target_system(msg);
            *compid = mavlink_msg_log_request_data_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_LOG_REQUEST_END:
            *sysid  = mavlink_msg_log_request_end_get_target_system(msg);
            *compid = mavlink_msg_log_request_end_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_LOG_REQUEST_LIST:
            *sysid  = mavlink_msg_log_request_list_get_target_system(msg);
            *compid = mavlink_msg_log_request_list_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_ACK:
            *sysid  = mavlink_msg_mission_ack_get_target_system(msg);
            *compid = mavlink_msg_mission_ack_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:
            *sysid  = mavlink_msg_mission_clear_all_get_target_system(msg);
            *compid = mavlink_msg_mission_clear_all_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_COUNT:
            *sysid  = mavlink_msg_mission_count_get_target_system(msg);
            *compid = mavlink_msg_mission_count_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_ITEM:
            *sysid  = mavlink_msg_mission_item_get_target_system(msg);
            *compid = mavlink_msg_mission_item_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_ITEM_INT:
            *sysid  = mavlink_msg_mission_item_int_get_target_system(msg);
            *compid = mavlink_msg_mission_item_int_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_REQUEST:
            *sysid  = mavlink_msg_mission_request_get_target_system(msg);
            *compid = mavlink_msg_mission_request_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
            *sysid  = mavlink_msg_mission_request_list_get_target_system(msg);
            *compid = mavlink_msg_mission_request_list_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_REQUEST_PARTIAL_LIST:
            *sysid  = mavlink_msg_mission_request_partial_list_get_target_system(msg);
            *compid = mavlink_msg_mission_request_partial_list_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
            *sysid  = mavlink_msg_mission_set_current_get_target_system(msg);
            *compid = mavlink_msg_mission_set_current_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_MISSION_WRITE_PARTIAL_LIST:
            *sysid  = mavlink_msg_mission_write_partial_list_get_target_system(msg);
            *compid = mavlink_msg_mission_write_partial_list_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
            *sysid  = mavlink_msg_param_request_list_get_target_system(msg);
            *compid = mavlink_msg_param_request_list_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
            *sysid  = mavlink_msg_param_request_read_get_target_system(msg);
            *compid = mavlink_msg_param_request_read_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_PARAM_SET:
            *sysid  = mavlink_msg_param_set_get_target_system(msg);
            *compid = mavlink_msg_param_set_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_PING:
            *sysid  = mavlink_msg_ping_get_target_system(msg);
            *compid = mavlink_msg_ping_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:
            *sysid  = mavlink_msg_rc_channels_override_get_target_system(msg);
            *compid = mavlink_msg_rc_channels_override_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
            *sysid  = mavlink_msg_request_data_stream_get_target_system(msg);
            *compid = mavlink_msg_request_data_stream_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_SAFETY_SET_ALLOWED_AREA:
            *sysid  = mavlink_msg_safety_set_allowed_area_get_target_system(msg);
            *compid = mavlink_msg_safety_set_allowed_area_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_SET_ATTITUDE_TARGET:
            *sysid  = mavlink_msg_set_attitude_target_get_target_system(msg);
            *compid = mavlink_msg_set_attitude_target_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT:
            *sysid  = mavlink_msg_set_position_target_global_int_get_target_system(msg);
            *compid = mavlink_msg_set_position_target_global_int_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED:
            *sysid  = mavlink_msg_set_position_target_local_ned_get_target_system(msg);
            *compid = mavlink_msg_set_position_target_local_ned_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_V2_EXTENSION:
            *sysid  = mavlink_msg_v2_extension_get_target_system(msg);
            *compid = mavlink_msg_v2_extension_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_GIMBAL_REPORT:
            *sysid  = mavlink_msg_gimbal_report_get_target_system(msg);
            *compid = mavlink_msg_gimbal_report_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_GIMBAL_CONTROL:
            *sysid  = mavlink_msg_gimbal_control_get_target_system(msg);
            *compid = mavlink_msg_gimbal_control_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_GIMBAL_TORQUE_CMD_REPORT:
            *sysid  = mavlink_msg_gimbal_torque_cmd_report_get_target_system(msg);
            *compid = mavlink_msg_gimbal_torque_cmd_report_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK:
            *sysid  = mavlink_msg_remote_log_data_block_get_target_system(msg);
            *compid = mavlink_msg_remote_log_data_block_get_target_component(msg);
            break;
        case MAVLINK_MSG_ID_REMOTE_LOG_BLOCK_STATUS:
            *sysid  = mavlink_msg_remote_log_block_status_get_target_system(msg);
            *compid = mavlink_msg_remote_log_block_status_get_target_component(msg);
            break;
    }
}
/* *************** */

static void mavlink_send_msg_to_channels(unsigned char ch_mask, mavlink_message_t *msg)
{
    unsigned char i;
    unsigned int len;
    unsigned char buf[MAVLINK_MAX_PACKET_LEN];

    len = mavlink_msg_to_send_buffer(buf, msg);
    
    for (i = 0; i < MAVLINK_COMM_NUM_BUFFERS; i++) {
        if (ch_mask & 1)
            mavlink_uart_clients[i].write(buf, len);
        ch_mask = ch_mask >> 1;
        if (ch_mask == 0)
            break;
    }
}

static void mavlink_set_active_channels(struct uart_client *cli)
{
    active_channel_mask |= 1 << (cli->ch);
}
static void mavlink_unset_active_channels(struct uart_client *cli)
{
    active_channel_mask &= ~(1 << (cli->ch));
}


static void mavlink_learn_route(unsigned char ch, mavlink_message_t *msg)
{
    unsigned char i;
    
    if (msg->sysid == 0 || (msg->sysid == osd_sysid && msg->compid == MAV_COMP_ID_OSD))
        return;

    for(i = 0; i < total_routes; i++) {
        if (routes[i].sysid == msg->sysid && 
            routes[i].compid == msg->compid &&
            routes[i].ch == ch) {
            //if (routes[i].mavtype == 0 && msg->msgid == MAVLINK_MSG_ID_HEARTBEAT) {
            //    routes[i].mavtype = mavlink_msg_heartbeat_get_type(msg);
            //}
            break;
        }
    }
    if (i == total_routes && i < MAX_MAVLINK_ROUTES) {
        routes[i].sysid = msg->sysid;
        routes[i].compid = msg->compid;
        routes[i].ch = ch;
        //if (msg->msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        //    routes[i].mavtype = mavlink_msg_heartbeat_get_type(msg);
        //}
        total_routes++;
#ifdef ROUTING_DEBUG
        printf("learned route %u %u via %u\n",
                 (unsigned)msg->sysid, 
                 (unsigned)msg->compid,
                 (unsigned)ch);
#endif
    }
}


static unsigned char mavlink_get_route(unsigned char ch, mavlink_message_t *msg)
{
    int target_sys, target_comp;
    unsigned char i, route = active_channel_mask;

    /* if the message wasn't generated by us
     * mask out source channel 
     * and learn the route */
    if (ch != 255) {
        route &= ~(1 << ch);
        mavlink_learn_route(ch, msg);
    }

    /* heartbeats goes to all channels except origin */
    if (msg->msgid == MAVLINK_MSG_ID_HEARTBEAT)
        return route;

    get_targets(msg, &target_sys, &target_comp);

    /* its for us - don't route */
    if ((target_sys == osd_sysid) && (target_comp == MAV_COMP_ID_OSD))
        return 0;

    /* broadcast message - route to all active ports*/
    if (target_sys <= 0)
        return route;

    /* selective routing - try match sysid and compid */
    route = 0;
    for (i = 0; i < total_routes; i++) {
        if (target_sys == routes[i].sysid &&
                    (target_comp <= 0 || target_comp == routes[i].compid) &&
                    routes[i].ch != ch) {
            route |= 1 << routes[i].ch;
        }
    }
    if (route)
        return route;

    /* try forwarding to any route that match the sysid */
    route = 0;
    for (i = 0; i < total_routes; i++) {
        if (target_sys == routes[i].sysid && routes[i].ch != ch) {
            route |= 1 << routes[i].ch;
        }
    }
    return route;
}

static void mavlink_send_msg(mavlink_message_t *msg)
{
    unsigned char route = mavlink_get_route(255, msg);
    mavlink_send_msg_to_channels(route, msg);
}

void mavlink_handle_msg(unsigned char ch, mavlink_message_t *msg)
{
    struct mavlink_callback *c;
    unsigned char i, route;

    LED = ~LED;
    
    route = mavlink_get_route(ch, msg);
    if (route)
        mavlink_send_msg_to_channels(route, msg);
    
    for (i = 0; i < nr_callbacks; i++) {
        c = &callbacks[i];
        if (c->cbk == NULL)
            continue;
        if ((msg->msgid == c->msgid) && ((msg->sysid == c->sysid) || (c->sysid == MAV_SYS_ID_ANY)))
            c->cbk(msg, c->data);
    }
    
    LED = ~LED;
}

static unsigned int mavlink_receive(struct uart_client *cli, unsigned char *buf, unsigned int len)
{
    mavlink_message_t msg __attribute__ ((aligned(2)));
    mavlink_status_t status;
    unsigned int i = len;
    while (i--) {
        if (mavlink_parse_char(cli->ch, *(buf++), &msg, &status)) {
            mavlink_handle_msg(cli->ch, &msg);
        }
    }
    return len;
}

struct mavlink_callback* add_mavlink_callback_sysid(unsigned char sysid, unsigned char msgid,
            void *cbk, unsigned char ctype, void *data)
{
    struct mavlink_callback *c = callbacks;
    unsigned char i;
    
    for (i = 0; i < nr_callbacks; i++) {
        if (callbacks[i].cbk == NULL)
            break;
    }
    
    if (i == MAX_MAVLINK_CALLBACKS)
        return NULL;
    
    c = &callbacks[i];
    c->sysid = sysid;
    c->msgid = msgid;
    c->cbk = cbk;
    c->type = ctype;
    c->data = data;
    if (i == nr_callbacks)
        nr_callbacks++;
    return c;
}

inline struct mavlink_callback* add_mavlink_callback(unsigned char msgid,
            void *cbk, unsigned char ctype, void *data)
{
    return add_mavlink_callback_sysid(uav_sysid, msgid, cbk, ctype, data);
}

void del_mavlink_callbacks(unsigned char ctype)
{
    struct mavlink_callback *c = callbacks;
    unsigned char i;

    for (i = 0; i < nr_callbacks; i++) {
        if (c->type == ctype)
            c->cbk = NULL;
        c++;
    }
}

void inline del_mavlink_callback(struct mavlink_callback *c)
{
    c->cbk = NULL;
}

static void mav_heartbeat(struct timer *t, void *d)
{
    mavlink_message_t msg;

    mavlink_msg_heartbeat_pack(osd_sysid,
            MAV_COMP_ID_OSD, &msg, MAV_TYPE_ALCEOSD,
            MAV_AUTOPILOT_INVALID,
            MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, // base_mode
            0, //custom_mode
            MAV_STATE_ACTIVE);

    mavlink_send_msg(&msg);
    LED = ~LED;
}


static void send_param_list_cbk(struct timer *t, void *d)
{
    mavlink_message_t msg;
    float param_value;
    char param_name[17];

    if (pidx == total_params) {
        console_printf("send param end\n", pidx);
        remove_timer(t);
        return;
    }

    param_value = params_get_value(pidx, param_name);
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_OSD, &msg,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, pidx++);
    mavlink_send_msg(&msg);
}


void mav_param_request_list(mavlink_message_t *msg, void *d)
{
    unsigned char sys, comp;

    sys = mavlink_msg_param_request_list_get_target_system(msg);
    comp = mavlink_msg_param_request_list_get_target_component(msg);
    
    //if ((comp != MAV_COMP_ID_OSD) || (sys != osd_sysid))
    if (sys != osd_sysid)
        return;
    
    pidx = 0;
    total_params = params_get_total();
    add_timer(TIMER_ALWAYS | TIMER_10MS, 1, send_param_list_cbk, d);

    console_printf("plist:sysid=%d compid=%d\n", sys, comp);
}


void mav_param_request_read(mavlink_message_t *msg, void *d)
{
    unsigned char sys, comp;
    mavlink_message_t msg2;
    char param_name[17];
    float param_value;
    int idx;

    sys = mavlink_msg_param_request_read_get_target_system(msg);
    comp = mavlink_msg_param_request_read_get_target_component(msg);
    //console_printf("get_param_start %d,%d\n", sys, comp);
    if ((comp != MAV_COMP_ID_OSD) || (sys != osd_sysid))
        return;

    idx = mavlink_msg_param_request_read_get_param_index(msg);
    if (idx == -1) {
        mavlink_msg_param_request_read_get_param_id(msg, param_name);
        param_name[16]= '\0';
    }

    param_value = params_get_value(idx, param_name);
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_OSD, &msg2,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, idx);
    mavlink_send_msg(&msg2);

    console_printf("param_req_read %d %s=%f\n", idx, param_name, param_value);
}


void mav_param_set(mavlink_message_t *msg, void *d)
{
    unsigned char sys, comp;
    mavlink_message_t msg2;
    unsigned int len;
    char param_name[17];
    float param_value;
    int idx;

    sys = mavlink_msg_param_set_get_target_system(msg);
    comp = mavlink_msg_param_set_get_target_component(msg);
    //console_printf("set_param_start %d,%d\n", sys, comp);
    if ((comp != MAV_COMP_ID_OSD) || (sys != osd_sysid))
        return;

    len = mavlink_msg_param_set_get_param_id(msg, param_name);
    param_name[16] = '\0';

    param_value = mavlink_msg_param_set_get_param_value(msg);

    console_printf("set_param: %s\n", param_name);

    idx = params_set_value(param_name, param_value, 1);

    /* broadcast new parameter value */
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_OSD, &msg2,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, idx);
    mavlink_send_msg(&msg2);
}

static void mavlink_request_data_streams(struct timer *t, void *d)
{
    mavlink_message_t msg2;

    /* SCALED_IMU2, SCALED_PRESSURE, SENSOR_OFFSETS */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_RAW_SENSORS, 3, 1);
    mavlink_send_msg(&msg2);

    /* MEMINFO, MISSION_CURRENT, GPS_RAW_INT, NAV_CONTROLLER_OUTPUT, LIMITS_STATUS */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_EXTENDED_STATUS, 1, 1);
    mavlink_send_msg(&msg2);

    /* SERVO_OUTPUT_RAW, RC_CHANNELS_RAW */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_RC_CHANNELS, 4, 1);
    mavlink_send_msg(&msg2);

    /* RC_CHANNELS_SCALED (HIL) */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_RAW_CONTROLLER, 0, 0);
    mavlink_send_msg(&msg2);

    /* GLOBAL_POSITION_INT */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_POSITION, 1, 1);
    mavlink_send_msg(&msg2);

    /* ATTITUDE, SIMSTATE (SITL) */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_EXTRA1, 20, 1);
    mavlink_send_msg(&msg2);

    /* VFR_HUD */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_EXTRA2, 10, 1);
    mavlink_send_msg(&msg2);

    /* AHRS, HWSTATUS, SYSTEM_TIME */
    mavlink_msg_request_data_stream_pack(osd_sysid, MAV_COMP_ID_PERIPHERAL, &msg2,
                        uav_sysid, MAV_COMP_ID_ALL,
                        MAV_DATA_STREAM_EXTRA3, 1, 1);
    mavlink_send_msg(&msg2);
}

static void mav_heartbeat_cbk(mavlink_message_t *msg, void *d)
{
    mavlink_request_data_streams(NULL, NULL);
    add_timer(TIMER_ALWAYS, 600, mavlink_request_data_streams, NULL);

    del_mavlink_callback((struct mavlink_callback *) d);
}

#if 0
void mav_cmd_ack(mavlink_message_t *msg, void *d)
{
    unsigned int c = mavlink_msg_command_ack_get_command(msg);
    unsigned char r = mavlink_msg_command_ack_get_result(msg);
    //printf("cmd %d ack %d\n", c, r);
}
#endif

void mavlink_init(void)
{
    struct mavlink_callback *hb_cbk;
    unsigned char i;

    /* register serial port clients */
    for (i = 0; i < MAVLINK_COMM_NUM_BUFFERS; i++) {
        memset(&mavlink_uart_clients[i], 0, sizeof(struct uart_client));
        mavlink_uart_clients[i].id = UART_CLIENT_MAVLINK;
        mavlink_uart_clients[i].ch = i;
        mavlink_uart_clients[i].init = mavlink_set_active_channels;
        mavlink_uart_clients[i].close = mavlink_unset_active_channels;
        mavlink_uart_clients[i].read = mavlink_receive;
        uart_add_client(&mavlink_uart_clients[i]);
    }
    
    /* register module parameters */
    params_add(params_mavlink);

    /* heartbeat timer */
    add_timer(TIMER_ALWAYS, 10, mav_heartbeat, NULL);

    /* parameter request handlers */
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_REQUEST_LIST, mav_param_request_list, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_REQUEST_READ, mav_param_request_read, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_SET, mav_param_set, CALLBACK_PERSISTENT, NULL);

    hb_cbk = add_mavlink_callback(MAVLINK_MSG_ID_HEARTBEAT, mav_heartbeat_cbk, CALLBACK_PERSISTENT, NULL);
    hb_cbk->data = hb_cbk;
    //add_mavlink_callback(MAVLINK_MSG_ID_COMMAND_ACK, mav_cmd_ack, CALLBACK_PERSISTENT, NULL);
}


static void shell_cmd_callbacks(char *args, void *data)
{
    unsigned char i, t = 0;
    struct mavlink_callback *c = callbacks;

    shell_printf("\n\nWidget callbacks:\n");
    for (i = 0; i < nr_callbacks; i++) {
        if ((c->cbk != NULL) && (c->type == CALLBACK_WIDGET)) {
            printf(" sysid=%3d msgid=%3d cbk=%p data=%p\n", c->sysid, c->msgid, c->cbk, c->data);
            t++;
        }
        c++;
    }
    shell_printf("\n\nPersistent callbacks:\n");
    c = callbacks;
    for (i = 0; i < nr_callbacks; i++) {
        if ((c->cbk != NULL) && (c->type == CALLBACK_PERSISTENT)) {
            printf(" sysid=%3d msgid=%3d cbk=%p data=%p\n", c->sysid, c->msgid, c->cbk, c->data);
            t++;
        }
        c++;
    }
    shell_printf("\n\ntotal=%d peak=%d max=%d\n", t, nr_callbacks, MAX_MAVLINK_CALLBACKS);
}

static void shell_cmd_stats(char *args, void *data)
{
    mavlink_status_t *status;
    unsigned char i;

    for (i = 0; i < MAVLINK_COMM_NUM_BUFFERS; i++) {
        status = mavlink_get_channel_status(i);
        shell_printf("\nMavlink channel %d\n", i);
        shell_printf(" msg_received=%d\n", status->msg_received);
        shell_printf(" packet_rx_drop_count=%d\n", status->packet_rx_drop_count);
        shell_printf(" packet_rx_success_count=%d\n", status->packet_rx_success_count);
    }
    shell_printf("\nActive channel mask=%x\n", active_channel_mask);
}

static void shell_cmd_route(char *args, void *data)
{
    unsigned char i;
    
    shell_printf("\nMavlink routing table:\n");
    for(i = 0; i < total_routes; i++) {
        shell_printf(" sysid(%3u) compid(%3u) on channel(%u)\n",
                (unsigned) routes[i].sysid, 
                (unsigned) routes[i].compid,
                (unsigned) routes[i].ch);
    }
    
    shell_printf("\ntotal routes=%d max=%d\n", total_routes, MAX_MAVLINK_ROUTES);
}

void shell_cmd_config(char *args, void *data)
{
    
}

static const struct shell_cmdmap_s mavlink_cmdmap[] = {
    {"callbacks", shell_cmd_callbacks, "Display callback info", SHELL_CMD_SIMPLE},
    {"config", shell_cmd_config, "config", SHELL_CMD_SIMPLE},
    {"route", shell_cmd_route, "Display routing table", SHELL_CMD_SIMPLE},
    {"stats", shell_cmd_stats, "Display statistics", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_mavlink(char *args, void *data)
{
    shell_exec(args, mavlink_cmdmap, data);
}
