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

#define MAX_MAVLINK_CALLBACKS 100

extern struct alceosd_config config;


static struct mavlink_callback callbacks[MAX_MAVLINK_CALLBACKS];
static unsigned char nr_callbacks = 0;

static unsigned char uav_sysid = 1;


#ifdef DEBUG_MAVLINK
#define DMAV(x...) \
    do { \
        console_printf("MAV: "); \
        console_printf(x); \
    } while(0)
#else
#define DMAV(x...)
#endif


static void mavlink_parse_msg(mavlink_message_t *msg, mavlink_status_t *status)
{
    struct mavlink_callback *c;
    unsigned char i;

    console_printf("rcv:sys=%d cmp=%d msg=%d\n", msg->sysid, msg->compid, msg->msgid);
    if (msg->msgid == 0) {
        console_printf("heartbeat:type=%d\n", mavlink_msg_heartbeat_get_type(msg));
    }


    for (i = 0; i < nr_callbacks; i++) {
        c = &callbacks[i];
        if ((msg->msgid == c->msgid) && ((msg->sysid == c->sysid) || (c->sysid == MAV_SYS_ID_ANY)))
            c->cbk(msg, status, c->data);
    }
}

void mavlink_process()
{
    static mavlink_message_t msg;
    static mavlink_status_t status;
    char *buf;
    int count, i;

    i = count = uart_read2(&buf);
    while (i--) {    
        if (mavlink_parse_char(MAVLINK_COMM_0, *(buf++), &msg, &status)) {
            LED = 0;
            mavlink_parse_msg(&msg, &status);
            LED = 1;
        }
    }
    uart_discard2(count);
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
    unsigned int len;
    unsigned char buf[30], *c = buf;

    mavlink_msg_heartbeat_pack(uav_sysid,
            MAV_COMP_ID_ALCEOSD, &msg, MAV_TYPE_ALCEOSD,
            MAV_AUTOPILOT_INVALID,
            MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, // base_mode
            0, //custom_mode
            MAV_STATE_ACTIVE);

    len = mavlink_msg_to_send_buffer(buf, &msg);
    while (len) {
        while (!U2STAbits.TRMT);
        U2TXREG = *c++;
        len--;
    }
}


void mavlink_init(void)
{
    /* heartbeat sender task */
    add_timer(TIMER_ALWAYS, 10, mav_heartbeat, NULL);
}
