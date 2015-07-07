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

#define UART_FIFO_MASK 0xff


const struct baudrate_tbl baudrates[] = {
    { .baudrate = 19200,  .brg = 227 },
    { .baudrate = 57600,  .brg = 76 },
    { .baudrate = 115200, .brg = 37 },
};


struct uart_fifo {
  char buf[UART_FIFO_MASK+1];
  unsigned char rd, wr;
};

static struct uart_fifo rx2_fifo;

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    unsigned char n_wr = (rx2_fifo.wr + 1) & UART_FIFO_MASK;
    
    if(U2STAbits.OERR == 1) {
        U2STAbits.OERR = 0;
    }

    if (n_wr == rx2_fifo.rd) {
        n_wr = U2RXREG;
        //U2TXREG = 'O';
    } else {
        rx2_fifo.buf[rx2_fifo.wr] = U2RXREG;
        rx2_fifo.wr = n_wr;
    }
    
    IFS1bits.U2RXIF = 0;
}


void uart_init2(unsigned char pins)
{

    // set pins
    switch (pins) {
        case UART_PORT_TELEMETRY:
        default:
            // U2TX
            _RP37R = 3;
            // U2RX
            _U2RXR = 38;
            break;
        case UART_PORT_ICSP:
        case UART_PORT_CON2:
            break;
    }

    U2BRG = baudrates[UART_115200].brg;
    U2MODE = 0;              //clear mode register
    //U2MODEbits.BRGH = 1;     //use high precision baud generator
    U2STA = 0;               //clear status register

    U2MODEbits.UARTEN = 1;   //enable the UART RX
    U2STAbits.UTXEN = 1;

    /* priority */
    IPC7bits.U2RXIP = 1;

    IFS1bits.U2RXIF = 0;     //clear the receive flag
    IEC1bits.U2RXIE = 1;
}


unsigned long uart_get_baudrate(unsigned char b)
{
    if (b < UART_BAUDRATES)
        return baudrates[b].baudrate;
    else
        return 0;
}

void uart_set_baudrate2(unsigned char b)
{
    if (b < UART_BAUDRATES)
        U2BRG = baudrates[b].brg;
}

unsigned char uart_getc2(char *c)
{
  unsigned char ret = (rx2_fifo.rd != rx2_fifo.wr);
  if (ret) {
    *c = rx2_fifo.buf[rx2_fifo.rd++];
    rx2_fifo.rd &= UART_FIFO_MASK;
  }
  return ret;
}

unsigned int uart_read2(char **buf)
{
    unsigned int wr = rx2_fifo.wr;
    unsigned int ret = (wr - rx2_fifo.rd) & UART_FIFO_MASK;
    if (ret) {
        *buf = &rx2_fifo.buf[rx2_fifo.rd];
        if (rx2_fifo.rd > wr) {
            ret = UART_FIFO_MASK + 1 - rx2_fifo.rd;
        }
    }
    return ret;
}

void uart_discard2(unsigned int count)
{
    rx2_fifo.rd += count;
    rx2_fifo.rd &= UART_FIFO_MASK;
}
