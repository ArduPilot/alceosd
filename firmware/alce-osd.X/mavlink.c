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

struct mavlink_callback {
    unsigned char msgid;
    void (*cbk) (mavlink_message_t *msg, mavlink_status_t *status);
};

static struct mavlink_callback callbacks[MAX_MAVLINK_CALLBACKS];
static unsigned char nr_callbacks = 0;

static void mavlink_parse_msg(mavlink_message_t *msg, mavlink_status_t *status)
{
    struct mavlink_callback *c;
    unsigned char i;
    for (i = 0; i < nr_callbacks; i++) {
        c = &callbacks[i];
        if (msg->msgid == c->msgid)
            c->cbk(msg, status);
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

void add_mavlink_callback(unsigned char msgid, void *cbk)
{
    struct mavlink_callback *c;
    if (nr_callbacks == MAX_MAVLINK_CALLBACKS)
        return;
    c = &callbacks[nr_callbacks++];
    c->msgid = msgid;
    c->cbk = cbk;
}

void del_mavlink_callbacks(void)
{
    nr_callbacks = 0;
}
