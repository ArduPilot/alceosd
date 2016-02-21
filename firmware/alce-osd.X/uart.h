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

#ifndef UART_H
#define	UART_H

enum {
    UART_PORT1 = 0,
    UART_PORT2,
    UART_PORT3,
    UART_PORT4,
};

enum {
    UART_BAUD_19200,
    UART_BAUD_57600,
    UART_BAUD_115200,
    UART_BAUDRATES,
};

enum {
    UART_PINS_TELEMETRY = 0,
    UART_PINS_CON2,
    UART_PINS_ICSP, /* hw_rev 0v2 only */
    UART_PINS_CON3, /* hw_rev 0v3b only : DF13*/
    UART_PINS_OFF,
    UART_PINS,
};

enum {
    UART_CLIENT_NONE = 0,
    UART_CLIENT_MAVLINK,
    UART_CLIENT_UAVTALK,
    UART_CLIENT_CONFIG,
    UART_CLIENTS,
};


struct uart_config {
    unsigned char mode;
    unsigned char baudrate;
    unsigned char pins;
};

struct uart_client {
    /* received data is feed to this function */
    unsigned int (*read)(unsigned char *buf, unsigned int len);

    /* modules should use this function to send data */
    /* pointer is set by client request function */
    void (*write)(unsigned char *buf, unsigned int len);
};

void uart_init(void);
void uart_set_client(unsigned char port, unsigned char client_id);
void uart_add_client_map(unsigned char id, struct uart_client *c);

inline unsigned long uart_get_baudrate(unsigned char b);
void uart_set_config_clients(unsigned char boot);
void uart_set_config_baudrates(void);
void uart_set_config_pins(void);


//unsigned char uart_getc1(char *c);
//unsigned char uart_getc2(char *c);

#endif
