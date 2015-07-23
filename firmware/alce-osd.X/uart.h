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
    UART_19200,
    UART_57600,
    UART_115200,
    UART_BAUDRATES,
};

enum {
    UART_PORT_TELEMETRY = 0,
    UART_PORT_ICSP,
    UART_PORT_CON2,
};

struct baudrate_tbl {
    unsigned long baudrate;
    unsigned int brg;
};

void uart_init2(unsigned char pins);

unsigned long uart_get_baudrate(unsigned char b);
void uart_set_baudrate2(unsigned char b);

unsigned char uart_getc2(char *c);
unsigned int uart_read2(char **buf);
void uart_discard2(unsigned int count);
void uart_write2(unsigned char *buf, unsigned int len);

#endif
