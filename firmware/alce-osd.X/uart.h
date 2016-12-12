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
    UART_PORTS
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
    UART_CLIENT_SHELL,
    UART_CLIENT_FRSKY,
    UART_CLIENTS,
};

enum {
    UART_DIR_RX = 0,
    UART_DIR_TX,
};

/* params */
#define UART_PROP_TX_INVERTED       1
#define UART_PROP_RX_INVERTED       2
#define UART_PROP_HALF_DUPLEX       4

#define UART_TX_BUF_SIZE    (128)

struct uart_config {
    unsigned char mode;
    unsigned char baudrate;
    unsigned char pins;
};

struct uart_client {
    /* client ID */
    unsigned char id;
    
    /* comm channel */
    unsigned char ch;
    
    /* assigned serial port */
    unsigned char port;
    
    /* called when client is set */
    void (*init)(struct uart_client *cli);
    
    /* called when client is removed */
    void (*close)(struct uart_client *cli);

    /* received data is fed to this function */
    unsigned int (*read)(struct uart_client *cli, unsigned char *buf, unsigned int len);

    /* modules should use this function to send data */
    /* pointer is set by client request function */
    int (*write)(unsigned char *buf, unsigned int len);
};

void uart_init(void);
void uart_add_client(struct uart_client *c);
struct uart_client* uart_get_client(unsigned char port);

inline unsigned long uart_get_baudrate(unsigned char b);
void uart_set_config_clients(void);
void uart_set_config_baudrates(void);
void uart_set_config_pins(void);
void uart_set_props(unsigned char port, unsigned int props);
void uart_set_direction(unsigned char port, unsigned char direction);

void shell_cmd_uart(char *args, void *data);

#endif
