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

#define FRSKY_SPORT_PACKET_SIZE     9
#define FRSKY_SPORT_BAUDRATE        57600
#define FRSKY_D_BAUDRATE            9600

#define FRSKY_DATA_FRAME            0x10
#define FRSKY_START_STOP            0x7e
#define FRSKY_BYTESTUFF             0x7d
#define FRSKY_STUFF_MASK            0x20

struct frsky_sport_data {
    unsigned int id;
    unsigned long data;
} __attribute__ ((packed, aligned(2)));


struct uart_client frsky_uart_client;


static unsigned int frsky_sport_msg_pack(char *buf, unsigned char type,
                        struct frsky_sport_data *d)
{
    unsigned char *c = (unsigned char*) d;
    unsigned int crc = type;
    unsigned int len = 0;
    unsigned char i;

    buf[len++] = FRSKY_DATA_FRAME;
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


void frsky_sport_send(struct frsky_sport_data *d)
{
    char buf[FRSKY_SPORT_PACKET_SIZE * 2];
    unsigned int len = frsky_sport_msg_pack(buf, FRSKY_DATA_FRAME, d);
    frsky_uart_client.write(buf, len);
}

static void frsky_init_port(struct uart_client *cli)
{
    /* invert TX logic */
    uart_set_props(cli->port, UART_PROP_TX_INVERTED);
}

static unsigned int frsky_receive(struct uart_client *cli, unsigned char *buf, unsigned int len)
{
    /* no RX for now */
    return len;
}

void frsky_init(void)
{
    frsky_uart_client.id = UART_CLIENT_FRSKY;
    frsky_uart_client.init = frsky_init_port;
    frsky_uart_client.read = frsky_receive;
    
    uart_add_client(&frsky_uart_client);
}

