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


#define UAVTALK_MSG_HEADER_LEN              10
#define UAVTALK_MSG_CHECKSUM_LEN            1
#define UAVTALK_MSG_MAX_PAYLOAD_LEN         256
#define UAVTALK_MSG_MAX_PACKET_LEN          (UAVTALK_MSG_HEADER_LEN + UAVTALK_MSG_MAX_PAYLOAD_LEN + UAVTALK_MSG_CHECKSUM_LEN)


#define UAVTALK_MSG_SYNC_TOKEN                              0x3c

#define UAVTALK_MSG_TYPE_MASK                               0xf8
#define UAVTALK_MSG_TYPE_VER                                0x20



/* object IDs */
/* these are highly dependent on OP version */
#define UAVTALK_OBJID_ATTITUDESTATE                         0xD7E0D964
#define UAVTALK_OBJID_ATTITUDESTATE_ROLL                    16
#define UAVTALK_OBJID_ATTITUDESTATE_PITCH                   20
#define UAVTALK_OBJID_ATTITUDESTATE_YAW                     24

#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND                  0x1E82C2D2
#define UAVTALK_OBJID_MANUALCONTROLCOMMAND_001              0xB8C7F78A
#define UAVTALK_OBJID_MANUALCONTROLCOMMAND_002              0x161A2C98
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_THROTTLE         0
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_0        24
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_1        26
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_2        28
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_3        30
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_4        32
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_5        34
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_6        36
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_7        38
#define	UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_8        40

#define	UAVTALK_OBJID_FLIGHTSTATUS                          0x9B6A127E
#define UAVTALK_OBJID_FLIGHTSTATUS_001                      0x0ED79A04
#define UAVTALK_OBJID_FLIGHTSTATUS_002                      0x1B7AEB74
#define UAVTALK_OBJID_FLIGHTSTATUS_003                      0x0B37AA16
#define UAVTALK_OBJID_FLIGHTSTATUS_004                      0xC5FF2D54
#define UAVTALK_OBJID_FLIGHTSTATUS_005                      0x8A80EA52
#define	UAVTALK_OBJID_FLIGHTSTATUS_ARMED                    0
#define	UAVTALK_OBJID_FLIGHTSTATUS_FLIGHTMODE               1


enum {
    UAVTALK_STATE_SYNC = 0,
    UAVTALK_STATE_TYPE,
    UAVTALK_STATE_SIZE,
    UAVTALK_STATE_DATA,
    UAVTALK_STATE_CS,
    UAVTALK_STATE_DONE,
};

struct uavtalk_message {
    unsigned char sync;
    unsigned char type;
    unsigned int len;
    unsigned long objid;
    unsigned int instid;
    unsigned char data[255];
    unsigned char crc;
} __attribute__ ((packed, aligned(2)));


/* CRC lookup table */
static const unsigned char crc_table[256] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};


/* helper functions to extract values from the message payload */
static inline char uavtalk_get_int8(struct uavtalk_message *msg, int pos) {
	return msg->data[pos];
}

static inline int uavtalk_get_int16(struct uavtalk_message *msg, int pos) {
	int i;
	memcpy(&i, msg->data+pos, sizeof(int));
	return i;
}

static inline long uavtalk_get_int32(struct uavtalk_message *msg, int pos) {
	long i;
	memcpy(&i, msg->data+pos, sizeof(long));
	return i;
}

static inline float uavtalk_get_float(struct uavtalk_message *msg, int pos) {
	float f;
	memcpy(&f, msg->data+pos, sizeof(float));
	return f;
}



static unsigned int uavtalk_parse_byte(unsigned char b, struct uavtalk_message *msg)
{
    static unsigned char state = UAVTALK_STATE_SYNC;
    static unsigned int cnt;
    register unsigned char crc = 0;
    unsigned int ret = 0;
    unsigned char *p = (unsigned char *) msg;

    switch (state) {
        case UAVTALK_STATE_SYNC:
            if (b == UAVTALK_MSG_SYNC_TOKEN) {
                cnt = 0;
                p[cnt++] = b;
                state = UAVTALK_STATE_TYPE;
            }
            break;
        case UAVTALK_STATE_TYPE:
            if ((b & UAVTALK_MSG_TYPE_MASK) != UAVTALK_MSG_TYPE_VER) {
                state = UAVTALK_STATE_SYNC;
            } else {
                p[cnt++] = b;
                state = UAVTALK_STATE_SIZE;
            }
            break;
        case UAVTALK_STATE_SIZE:
            p[cnt++] = b;
            if (cnt > 3) {
                if (msg->len < UAVTALK_MSG_HEADER_LEN || msg->len > UAVTALK_MSG_HEADER_LEN + UAVTALK_MSG_MAX_PAYLOAD_LEN) {
                    state = UAVTALK_STATE_SYNC;
                } else {
                    state = UAVTALK_STATE_DATA;
                }
            }
            break;
        case UAVTALK_STATE_DATA:
            p[cnt++] = b;
            if (cnt >= msg->len)
                state = UAVTALK_STATE_CS;
            break;
        case UAVTALK_STATE_CS:
            msg->crc = b;
            state = UAVTALK_STATE_DONE;
            break;
    }

    if (state == UAVTALK_STATE_DONE) {
        for (cnt = 0; cnt < msg->len; cnt++)
            crc = crc_table[crc ^ p[cnt]];
        state = UAVTALK_STATE_SYNC;
        if (crc == msg->crc)
            ret = msg->len;
    }

    return ret;
}


static void uavtalk_handle_msg(struct uavtalk_message *msg)
{
    mavlink_message_t mav_msg;

    switch (msg->objid) {
        case UAVTALK_OBJID_ATTITUDESTATE:
            mavlink_msg_attitude_pack(1, MAV_COMP_ID_ALL, &mav_msg, 0,
                    DEG2RAD(uavtalk_get_float(msg, UAVTALK_OBJID_ATTITUDESTATE_ROLL)),
                    DEG2RAD(uavtalk_get_float(msg, UAVTALK_OBJID_ATTITUDESTATE_PITCH)),
                    DEG2RAD(uavtalk_get_float(msg, UAVTALK_OBJID_ATTITUDESTATE_YAW)),
                    0, 0, 0);
            mavlink_handle_msg(255, &mav_msg);
            break;
        case UAVTALK_OBJID_MANUALCONTROLCOMMAND:
        case UAVTALK_OBJID_MANUALCONTROLCOMMAND_001:
        case UAVTALK_OBJID_MANUALCONTROLCOMMAND_002:
            mavlink_msg_rc_channels_raw_pack(1, MAV_COMP_ID_ALL, &mav_msg, 0, 0,
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_1),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_2),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_3),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_4),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_5),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_6),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_7),
                    (unsigned int) uavtalk_get_int16(msg, UAVTALK_OBJID_MANUALCONTROLCOMMAND_CHANNEL_8),
                    0);
            mavlink_handle_msg(255, &mav_msg);
            break;
        case UAVTALK_OBJID_FLIGHTSTATUS:
        case UAVTALK_OBJID_FLIGHTSTATUS_001:
        case UAVTALK_OBJID_FLIGHTSTATUS_002:
        case UAVTALK_OBJID_FLIGHTSTATUS_003:
        case UAVTALK_OBJID_FLIGHTSTATUS_004:
        case UAVTALK_OBJID_FLIGHTSTATUS_005:
            mavlink_msg_heartbeat_pack(1, MAV_COMP_ID_ALL, &mav_msg,
                    MAV_TYPE_GENERIC, MAV_AUTOPILOT_OPENPILOT,
                    (uavtalk_get_int8(msg, UAVTALK_OBJID_FLIGHTSTATUS_ARMED) != 0) ? MAV_MODE_FLAG_SAFETY_ARMED : 0,
                    uavtalk_get_int8(msg, UAVTALK_OBJID_FLIGHTSTATUS_FLIGHTMODE),
                    MAV_STATE_STANDBY);
            mavlink_handle_msg(255, &mav_msg);
            break;
    }
}

static unsigned int uavtalk_receive(struct uart_client *cli, unsigned char *buf, unsigned int len)
{
    static struct uavtalk_message msg;
    unsigned int i = len;
    while (i--) {
        if (uavtalk_parse_byte(*(buf++), &msg)) {
            uavtalk_handle_msg(&msg);
        }
    }
    return len;
}

struct uart_client uavtalk_uart_client;

void uavtalk_init(void)
{
    memset(&uavtalk_uart_client, 0, sizeof(struct uart_client));
    uavtalk_uart_client.id = UART_CLIENT_UAVTALK;
    uavtalk_uart_client.read = uavtalk_receive;
    uart_add_client(&uavtalk_uart_client);
}
