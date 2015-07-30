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

#define MAX_MAVLINK_CALLBACKS 50

#ifdef DEBUG_MAVLINK
#define DMAV(x...) \
    do { \
        console_printf("MAV: "); \
        console_printf(x); \
    } while(0)
#else
#define DMAV(x...)
#endif


static struct mavlink_callback callbacks[MAX_MAVLINK_CALLBACKS];
static unsigned char nr_callbacks = 0;

static unsigned char uav_sysid = 1, osd_sysid = 200;

static unsigned int pidx = 0, total_params = 0;


const struct param_def mavparams_mavlink[] = {
    PARAM("OSD_MAV_UAVSYSID", MAV_PARAM_TYPE_UINT8, &uav_sysid, NULL),
    PARAM("OSD_MAV_OSDSYSID", MAV_PARAM_TYPE_UINT8, &osd_sysid, NULL),
    PARAM_END,
};


/* aditional helper functions */
unsigned int mavlink_msg_rc_channels_raw_get_chan(mavlink_message_t *msg, unsigned char ch)
{
    switch (ch) {
        default:
        case 0:
            return mavlink_msg_rc_channels_raw_get_chan1_raw(msg);
        case 1:
            return mavlink_msg_rc_channels_raw_get_chan2_raw(msg);
        case 2:
            return mavlink_msg_rc_channels_raw_get_chan3_raw(msg);
        case 3:
            return mavlink_msg_rc_channels_raw_get_chan4_raw(msg);
        case 4:
            return mavlink_msg_rc_channels_raw_get_chan5_raw(msg);
        case 5:
            return mavlink_msg_rc_channels_raw_get_chan6_raw(msg);
        case 6:
            return mavlink_msg_rc_channels_raw_get_chan7_raw(msg);
        case 7:
            return mavlink_msg_rc_channels_raw_get_chan8_raw(msg);
    }
}
/* *************** */


static void mavlink_parse_msg(mavlink_message_t *msg, mavlink_status_t *status)
{
    struct mavlink_callback *c;
    unsigned char i;

    LED = 0;
    //console_printf("rcv:sys=%d cmp=%d msg=%d\n", msg->sysid, msg->compid, msg->msgid);
    //if (msg->msgid == 0) {
    //    console_printf("heartbeat:type=%d\n", mavlink_msg_heartbeat_get_type(msg));
    //}


    for (i = 0; i < nr_callbacks; i++) {
        c = &callbacks[i];
        if ((msg->msgid == c->msgid) && ((msg->sysid == c->sysid) || (c->sysid == MAV_SYS_ID_ANY)))
            c->cbk(msg, status, c->data);
    }
    LED = 1;
}


static void mavlink_send_msg(mavlink_message_t *msg)
{
    unsigned int len;
    unsigned char buf[MAVLINK_MAX_PACKET_LEN];

    len = mavlink_msg_to_send_buffer(buf, msg);

    /* TODO: implement routing tables */
    uart_write2(buf, len);
    uart_write1(buf, len);
}


void mavlink_process(void)
{
    mavlink_message_t msg __attribute__ ((aligned(2)));
    mavlink_status_t status;
    char *buf;
    int count, i, len;

    unsigned char msg_buf[MAVLINK_MAX_PACKET_LEN];

    i = count = uart_read2(&buf);
    while (i--) {    
        if (mavlink_parse_char(MAVLINK_COMM_0, *(buf++), &msg, &status)) {

            /* forward to uart1 */
            len = mavlink_msg_to_send_buffer(msg_buf, &msg);
            uart_write1(msg_buf, len);

            mavlink_parse_msg(&msg, &status);
        }
    }
    uart_discard2(count);


    i = count = uart_read1(&buf);
    while (i--) {
        if (mavlink_parse_char(MAVLINK_COMM_1, *(buf++), &msg, &status)) {
            /* forward to uart2 */
            len = mavlink_msg_to_send_buffer(msg_buf, &msg);
            uart_write2(msg_buf, len);

            mavlink_parse_msg(&msg, &status);
        }
    }
    uart_discard1(count);

}

struct mavlink_callback* add_mavlink_callback(unsigned char msgid,
            void *cbk, unsigned char ctype, void *data)
{
    struct mavlink_callback *c;
    if (nr_callbacks == MAX_MAVLINK_CALLBACKS)
        return NULL;
    c = &callbacks[nr_callbacks++];
    c->sysid = uav_sysid;
    c->msgid = msgid;
    c->cbk = cbk;
    c->type = ctype;
    c->data = data;
    return c;
}


struct mavlink_callback* add_mavlink_callback_sysid(unsigned char sysid, unsigned char msgid,
            void *cbk, unsigned char ctype, void *data)
{
    struct mavlink_callback *c;
    if (nr_callbacks == MAX_MAVLINK_CALLBACKS)
        return NULL;
    c = &callbacks[nr_callbacks++];
    c->sysid = sysid;
    c->msgid = msgid;
    c->cbk = cbk;
    c->type = ctype;
    c->data = data;
    return c;
}


void del_mavlink_callbacks(unsigned char ctype)
{
    struct mavlink_callback *c = callbacks;
    unsigned char i = 0;

    while (i < nr_callbacks) {
        if (c->type == ctype) {
            memcpy(c, c + 1, sizeof(struct mavlink_callback) * (nr_callbacks - i - 1));
            nr_callbacks--;
        } else {
            c++;
            i++;
        }
    }
}


static void mav_heartbeat(struct timer *t, void *d)
{
    mavlink_message_t msg;

    mavlink_msg_heartbeat_pack(osd_sysid,
            MAV_COMP_ID_ALCEOSD, &msg, MAV_TYPE_ALCEOSD,
            MAV_AUTOPILOT_INVALID,
            MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, // base_mode
            0, //custom_mode
            MAV_STATE_ACTIVE);

    mavlink_send_msg(&msg);
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
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_ALCEOSD, &msg,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, pidx++);
    mavlink_send_msg(&msg);
}


void mav_param_request_list(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    unsigned char sys, comp;

    sys = mavlink_msg_param_request_list_get_target_system(msg);
    comp = mavlink_msg_param_request_list_get_target_component(msg);
    
    //if ((comp != MAV_COMP_ID_ALCEOSD) || (sys != osd_sysid))
    if (sys != osd_sysid)
        return;
    
    pidx = 0;
    total_params = params_get_total();
    add_timer(TIMER_ALWAYS | TIMER_10MS, 1, send_param_list_cbk, d);

    console_printf("plist:sysid=%d compid=%d\n", sys, comp);
}


void mav_param_request_read(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    unsigned char sys, comp;
    mavlink_message_t msg2;
    char param_name[17];
    float param_value;
    int idx;

    sys = mavlink_msg_param_request_read_get_target_system(msg);
    comp = mavlink_msg_param_request_read_get_target_component(msg);
    //console_printf("get_param_start %d,%d\n", sys, comp);
    if ((comp != MAV_COMP_ID_ALCEOSD) || (sys != osd_sysid))
        return;

    idx = mavlink_msg_param_request_read_get_param_index(msg);
    if (idx == -1) {
        mavlink_msg_param_request_read_get_param_id(msg, param_name);
        param_name[16]= '\0';
    }

    param_value = params_get_value(idx, param_name);
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_ALCEOSD, &msg2,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, idx);
    mavlink_send_msg(&msg2);

    console_printf("param_req_read %d %s=%f\n", idx, param_name, param_value);
}


void mav_param_set(mavlink_message_t *msg, mavlink_status_t *status, void *d)
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
    if ((comp != MAV_COMP_ID_ALCEOSD) || (sys != osd_sysid))
        return;

    len = mavlink_msg_param_set_get_param_id(msg, param_name);
    param_name[16] = '\0';

    param_value = mavlink_msg_param_set_get_param_value(msg);

    console_printf("set_param: %s\n", param_name);

    idx = params_set_value(param_name, param_value, 1);

    /* broadcast new parameter value */
    mavlink_msg_param_value_pack(osd_sysid, MAV_COMP_ID_ALCEOSD, &msg2,
                                    param_name, param_value, MAVLINK_TYPE_FLOAT,
                                    total_params, idx);
    mavlink_send_msg(&msg2);
}


void mavlink_init(void)
{
    /* register module parameters */
    params_add(mavparams_mavlink);

    /* heartbeat sender task */
    add_timer(TIMER_ALWAYS, 10, mav_heartbeat, NULL);

    /* parameter request handlers */
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_REQUEST_LIST, mav_param_request_list, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_REQUEST_READ, mav_param_request_read, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_PARAM_SET, mav_param_set, CALLBACK_PERSISTENT, NULL);
}
