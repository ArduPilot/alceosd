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

#define FRSKY_SPORT_PACKET_SIZE     8
#define FRSKY_SPORT_BAUDRATE        57600
#define FRSKY_D_BAUDRATE            9600

#define FRSKY_DATA_FRAME            0x10
#define FRSKY_START_STOP            0x7e
#define FRSKY_BYTESTUFF             0x7d
#define FRSKY_STUFF_MASK            0x20


/* sensor id's */
#define GPS_LAT_LON_DATA_ID         0x0800
#define GPS_ALT_DATA_ID             0x0820


struct frsky_sport_data {
    unsigned int id;
    unsigned long data;
} __attribute__ ((packed, aligned(2)));

#define FRSKY_FIFO_MASK   0xf
static struct frsky_sport_data fifo[FRSKY_FIFO_MASK+1];
static unsigned char fifo_rd = 0, fifo_wr = 0;

struct uart_client frsky_uart_client;


static int frsky_sport_queue_data(unsigned int id, unsigned long data)
{
    unsigned char n_wr, i = fifo_rd;
    
    while (i != fifo_wr) {
        if (fifo[i].id == id) {
            /* special case of same id for different data */
            if (id == GPS_LAT_LON_DATA_ID) {
                if (((fifo[i].data ^ data) & (1L << 31)) == 0) {
                    fifo[i].data = data;
                    return 0;
                }
            } else {
                fifo[i].data = data;
                return 0;
            }
        }
        i = (i + 1) & FRSKY_FIFO_MASK;
    }

    n_wr = (fifo_wr+1) & FRSKY_FIFO_MASK;
    if (n_wr == fifo_rd)
        return -1;
    
    fifo[fifo_wr].id = id;
    fifo[fifo_wr].data = data;
    fifo_wr = n_wr;
    return 1;
}

static unsigned int frsky_sport_msg_pack(unsigned char *buf, unsigned char type,
                        struct frsky_sport_data *d)
{
    unsigned char *c = (unsigned char*) d;
    unsigned int crc = type;
    unsigned int len = 0;
    unsigned char i;

    buf[len++] = type;
    for (i = 0; i < sizeof(struct frsky_sport_data); i++) {
        if ((c[i] == FRSKY_START_STOP) || (c[i] == FRSKY_BYTESTUFF)) {
            buf[len++] = FRSKY_BYTESTUFF;
            buf[len++] = c[i] ^ FRSKY_STUFF_MASK;
        } else {
            buf[len++] = c[i];
        }
        crc += c[i];
        crc += crc >> 8;
        crc &= 0xff;
    }
    buf[len++] = (unsigned char) crc;
    return len;
}

static void frsky_sport_send(struct frsky_sport_data *d)
{
    unsigned char buf[FRSKY_SPORT_PACKET_SIZE * 2];
    unsigned int len = frsky_sport_msg_pack(buf, FRSKY_DATA_FRAME, d);
    frsky_uart_client.write(buf, len);
}

static unsigned int frsky_receive(struct uart_client *cli, unsigned char *buf, unsigned int len)
{
    static unsigned char last_ch;
    unsigned char ch = buf[len-1];

    if (last_ch == FRSKY_START_STOP) {
        /* send packet */
        if (fifo_rd != fifo_wr) {
            uart_set_direction(cli->port, UART_DIR_TX);
            frsky_sport_send(&fifo[fifo_rd++]);
            fifo_rd &= FRSKY_FIFO_MASK;
            uart_set_direction(cli->port, UART_DIR_RX);
        } else {
            /* do we need to send a dummy packet? */
        }
    }
    last_ch = ch;
    return len;
}


/* GPS */
static void gps_mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    unsigned long data;
    float tmp;
    
    /* send latitude */
    tmp = ((float) mavlink_msg_gps_raw_int_get_lat(msg)) * 0.06;
    data = ((unsigned long) abs(tmp)) & 0x3fffffff;
    if (tmp < 0)
        data |= 1L << 30;
    frsky_sport_queue_data(GPS_LAT_LON_DATA_ID, data);
    
    /* send longitude */
    tmp = ((float) mavlink_msg_gps_raw_int_get_lon(msg)) * 0.06;
    data = ((unsigned long) abs(tmp)) & 0x3fffffff;
    data |= 1L << 31;
    if (tmp < 0)
        data |= 1L << 30;
    frsky_sport_queue_data(GPS_LAT_LON_DATA_ID, data);

    /* send altitude */
    frsky_sport_queue_data(GPS_ALT_DATA_ID,
            mavlink_msg_gps_raw_int_get_alt(msg) / 10);
}


static void frsky_init_client(struct uart_client *cli)
{
    uart_set_props(cli->port,
            //UART_PROP_TX_INVERTED | 
            //UART_PROP_RX_INVERTED | 
            UART_PROP_HALF_DUPLEX);

    /* send GPS data */
    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, gps_mav_callback, CALLBACK_PERSISTENT, NULL);
}


void frsky_init(void)
{
    frsky_uart_client.id = UART_CLIENT_FRSKY;
    frsky_uart_client.init = frsky_init_client;
    frsky_uart_client.read = frsky_receive;
    
    uart_add_client(&frsky_uart_client);
}
