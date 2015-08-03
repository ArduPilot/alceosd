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

struct uart_ops {
    void (*init)(void);
    void (*set_baudrate)(unsigned char b);
    void (*set_pins)(unsigned char pins);
    unsigned int (*count)(void);
    unsigned int (*read)(unsigned char **buf);
    void (*discard)(unsigned int count);
    void (*write)(unsigned char *buf, unsigned int len);
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
void uart_add_client_map(unsigned char id, unsigned char port, struct uart_client *c);
inline const struct uart_ops* uart_get(unsigned char port);

inline unsigned long uart_get_baudrate(unsigned char b);
void uart_set_config_clients(void);
void uart_set_config_baudrates(void);
void uart_set_config_pins(void);


unsigned char uart_getc1(char *c);
unsigned char uart_getc2(char *c);

#endif
