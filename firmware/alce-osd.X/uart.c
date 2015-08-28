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

#define UART_FIFO_MASK 0x7f

#define DMA_UART1
#define DMA_UART2

#define DMA_BUF_SIZE    (128)

extern struct alceosd_config config;


const struct param_def params_uart[] = {
    PARAM("SERIAL1_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[0].mode, uart_set_config_clients),
    PARAM("SERIAL1_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[0].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL1_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[0].pins, uart_set_config_pins),
    PARAM("SERIAL2_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[1].mode, uart_set_config_clients),
    PARAM("SERIAL2_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[1].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL2_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[1].pins, uart_set_config_pins),
    PARAM_END,
};



struct baudrate_tbl {
    unsigned long baudrate;
    unsigned int brg;
};

static const struct baudrate_tbl baudrates[] = {
    { .baudrate = 19200,  .brg = 227 },
    { .baudrate = 57600,  .brg = 76 },
    { .baudrate = 115200, .brg = 37 },
};


#define UART_CLIENT_MAP_MAX 10
struct uart_client_map {
    unsigned char id;
    unsigned char port;
    struct uart_client *c;
} client_map [UART_CLIENT_MAP_MAX] = { { .id = 0xff } };


struct uart_fifo {
  unsigned char buf[UART_FIFO_MASK+1];
  unsigned char rd, wr;
};

static struct uart_fifo rx2_fifo;
static struct uart_fifo rx1_fifo;


/* tx dma buffers */
__eds__ unsigned char uart1TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-DMA_BUF_SIZE)));
__eds__ unsigned char uart2TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-(DMA_BUF_SIZE*2))));


inline unsigned long uart_get_baudrate(unsigned char b)
{
    if (b < UART_BAUDRATES)
        return baudrates[b].baudrate;
    else
        return 0;
}


void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    unsigned char n_wr = (rx1_fifo.wr + 1) & UART_FIFO_MASK;

    if(U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }

    if (n_wr == rx1_fifo.rd) {
        n_wr = U1RXREG;
        //U2TXREG = 'O';
    } else {
        rx1_fifo.buf[rx1_fifo.wr] = U1RXREG;
        rx1_fifo.wr = n_wr;
    }

    IFS0bits.U1RXIF = 0;
}

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



static void uart1_init(void)
{
    U1BRG = baudrates[UART_BAUD_115200].brg;
    U1MODE = 0;              //clear mode register
    //U2MODEbits.BRGH = 1;     //use high precision baud generator
    U1STA = 0;               //clear status register

    U1MODEbits.UARTEN = 1;   //enable the UART RX

    /* priority */
    IPC2bits.U1RXIP = 1;

    IFS0bits.U1RXIF = 0;     //clear the receive flag
    IEC0bits.U1RXIE = 1;

#ifdef DMA_UART1
    // Configure the UART TX DMA
    DMA0CONbits.MODE = 0b01; // One-shot with ping-pong disabled
    DMA0CONbits.AMODE = 0b00; // Register indirect with post increment
    DMA0CONbits.NULLW = 0; // Normal mode
    DMA0CONbits.HALF = 0; // Interrupt when transfer is complete
    DMA0CONbits.DIR = 1; // From RAM to peripheral
    DMA0CONbits.SIZE = 1; // BYTE mode.
    DMA0REQ = 0x0c; // U1TX interrupt requests transfer
    DMA0PAD = (volatile unsigned int) &U1TXREG; // Transfer to U1TXREG
    DMA0STAL = __builtin_dmaoffset(&uart1TxDataBuf);
    DMA0STAH = __builtin_dmapage(&uart1TxDataBuf);

    // Enable UART TX DMA interrupt (but not the DMA peripheral itself, yet).
    IFS0bits.DMA0IF = 0;
    IEC0bits.DMA0IE = 1;
#endif

    U1STAbits.UTXEN = 1;
}

static void uart2_init(void)
{
    U2BRG = baudrates[UART_BAUD_115200].brg;
    U2MODE = 0;              //clear mode register
    //U2MODEbits.BRGH = 1;     //use high precision baud generator
    U2STA = 0;               //clear status register

    U2MODEbits.UARTEN = 1;   //enable the UART RX

    /* priority */
    IPC7bits.U2RXIP = 1;

    IFS1bits.U2RXIF = 0;     //clear the receive flag
    IEC1bits.U2RXIE = 1;


#ifdef DMA_UART2
    // Configure the UART TX DMA
    DMA1CONbits.MODE = 0b01; // One-shot with ping-pong disabled
    DMA1CONbits.AMODE = 0b00; // Register indirect with post increment
    DMA1CONbits.NULLW = 0; // Normal mode
    DMA1CONbits.HALF = 0; // Interrupt when transfer is complete
    DMA1CONbits.DIR = 1; // From RAM to peripheral
    DMA1CONbits.SIZE = 1; // BYTE mode.
    DMA1REQ = 0x1f; // U2TX interrupt requests transfer
    DMA1PAD = (volatile unsigned int) &U2TXREG; // Transfer to U1TXREG
    DMA1STAL = __builtin_dmaoffset(&uart2TxDataBuf);
    DMA1STAH = __builtin_dmapage(&uart2TxDataBuf);

    // Enable UART TX DMA interrupt (but not the DMA peripheral itself, yet).
    IFS0bits.DMA1IF = 0;
    IEC0bits.DMA1IE = 1;
#endif

    U2STAbits.UTXEN = 1;
}

extern unsigned char hw_rev;

static void uart1_set_pins(unsigned char pins)
{
    // set pins
    switch (pins) {
        default:
        case UART_PINS_TELEMETRY:
            // TX
            _RP37R = 1;
            // RX
            _U1RXR = 38;
            break;
        case UART_PINS_CON2:
            // TX
            if (hw_rev == 0x01)
                _RP41R = 1;
            else
                _RP36R = 1;
            // RX
            _U1RXR = 20;
            break;
        case UART_PINS_ICSP:
            if (hw_rev == 0x01)
                break;
            // TX
            _RP35R = 1;
            // RX
            _U1RXR = 34;
            break;
    }
}

static void uart2_set_pins(unsigned char pins)
{
    switch (pins) {
        case UART_PINS_TELEMETRY:
            // TX
            _RP37R = 3;
            // RX
            _U2RXR = 38;
            break;
        default:
        case UART_PINS_CON2:
            // TX
            if (hw_rev == 0x01)
                _RP41R = 3;
            else
                _RP36R = 3;
            // RX
            _U2RXR = 20;
            break;
        case UART_PINS_ICSP:
            if (hw_rev == 0x01)
                break;
            // TX
            _RP35R = 3;
            // RX
            _U2RXR = 34;
            break;
    }
}


static inline void uart1_set_baudrate(unsigned char b)
{
    if (b < UART_BAUDRATES)
        U1BRG = baudrates[b].brg;
}

static inline void uart2_set_baudrate(unsigned char b)
{
    if (b < UART_BAUDRATES)
        U2BRG = baudrates[b].brg;
}


static unsigned int uart1_read(unsigned char **buf)
{
    unsigned int wr = rx1_fifo.wr;
    unsigned int ret = (wr - rx1_fifo.rd) & UART_FIFO_MASK;
    if (ret) {
        *buf = &rx1_fifo.buf[rx1_fifo.rd];
        if (rx1_fifo.rd > wr) {
            ret = UART_FIFO_MASK + 1 - rx1_fifo.rd;
        }
    }
    return ret;
}

static unsigned int uart2_read(unsigned char **buf)
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

static inline unsigned int uart1_count(void)
{
    return (rx1_fifo.wr - rx1_fifo.rd) & UART_FIFO_MASK;
}

static inline unsigned int uart2_count(void)
{
    return (rx2_fifo.wr - rx2_fifo.rd) & UART_FIFO_MASK;
}


static inline void uart1_discard(unsigned int count)
{
    rx1_fifo.rd += count;
    rx1_fifo.rd &= UART_FIFO_MASK;
}

static inline void uart2_discard(unsigned int count)
{
    rx2_fifo.rd += count;
    rx2_fifo.rd &= UART_FIFO_MASK;
}


void uart1_write(unsigned char *buf, unsigned int len)
{
#ifndef DMA_UART1
    while (len) {
        while (!U1STAbits.TRMT);
        U1TXREG = *buf++;
        len--;
    }
#else
    unsigned int count;
    if (len == 0)
        return;

    /* TODO: split in 2 buffers and send data later */
    while ((DMA0CONbits.CHEN == 1) || (U1STAbits.TRMT == 0));

    count = 0;
    while ((len-- > 0) && (count < sizeof(uart1TxDataBuf))) {
        uart1TxDataBuf[count++] = *buf++;
    }
    DMA0CNT = count - 1; // DMAxCNT is N-1
    DMA0CONbits.CHEN = 1; // DMA one-shot mode requires the CHEN bit be set every time
    DMA0REQbits.FORCE = 1; // Force the DMA to start transferring data.
#endif
}

void uart2_write(unsigned char *buf, unsigned int len)
{
#ifndef DMA_UART2
    while (len) {
        while (!U2STAbits.TRMT);
        U2TXREG = *buf++;
        len--;
    }
#else
    unsigned int count;
    if (len == 0)
        return;

    /* still busy */
    while ((DMA1CONbits.CHEN == 1) || (U2STAbits.TRMT == 0));

    count = 0;
    while ((len-- > 0) && (count < sizeof(uart2TxDataBuf))) {
        uart2TxDataBuf[count++] = *buf++;
    }
    DMA1CNT = count - 1; // DMAxCNT is N-1
    DMA1CONbits.CHEN = 1; // DMA one-shot mode requires the CHEN bit be set every time
    DMA1REQbits.FORCE = 1; // Force the DMA to start transferring data.
#endif
}



unsigned char uart_getc1(char *c)
{
    unsigned char ret = (rx1_fifo.rd != rx1_fifo.wr);
    if (ret) {
        *c = rx1_fifo.buf[rx1_fifo.rd++];
        rx1_fifo.rd &= UART_FIFO_MASK;
    }
    return ret;
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


#ifdef REDIRECT_LIBC_WRITE
int __attribute__((__weak__, __section__(".libc"))) write(int handle, void *buf, unsigned int len)
{
    switch (handle) {
        case 0:
        case 1:
        case 2:
            console_printn((char *) buf, len);
            break;
    }
    return len;
}
#endif


const struct uart_ops uart[2] = {
    {
        .init = uart1_init,
        .count = uart1_count,
        .read = uart1_read,
        .discard = uart1_discard,
        .write = uart1_write,
        .set_baudrate = uart1_set_baudrate,
        .set_pins = uart1_set_pins,
    },
    {
        .init = uart2_init,
        .count = uart2_count,
        .read = uart2_read,
        .discard = uart2_discard,
        .write = uart2_write,
        .set_baudrate = uart2_set_baudrate,
        .set_pins = uart2_set_pins,
    },
};


inline const struct uart_ops* uart_get(unsigned char port)
{
    return &uart[port];
}

static struct uart_client *clients[2] = {NULL, NULL};


static void uart_process(void)
{
    int i;
    unsigned char *b;
    unsigned int len;

    for (i = 0; i < 2; i++) {
        if ((clients[i] != NULL) && (uart[i].count() > 0)) {
            len = uart[i].read(&b);
            len = clients[i]->read(b, len);
            uart[i].discard(len);
        }
    }
}


void uart_set_client(unsigned char port, unsigned char client_id)
{
    struct uart_client_map *cmap = client_map;
    struct uart_client **c = &clients[port];

    if (port > 1)
        return;

    /* disable current client write op */
    if ((*c) != NULL)
        (*c)->write = NULL;

    while (cmap->id != 0xff) {
        if ((cmap->id == client_id) && (cmap->port == port)) {
            (*c) = cmap->c;
            cmap->c->write = uart[port].write;
            break;
        }
        cmap++;
    }

    /* client not found or none */
    if (cmap->id == 0xff) {
        (*c) = NULL;
    }

}

void uart_set_config_clients(void)
{
    uart_set_client(UART_PORT1, config.uart[0].mode);
    uart_set_client(UART_PORT2, config.uart[1].mode);
}

void uart_set_config_baudrates(void)
{
    uart[0].set_baudrate(config.uart[0].baudrate);
    uart[1].set_baudrate(config.uart[1].baudrate);
}

void uart_set_config_pins(void)
{
    uart[0].set_pins(config.uart[0].pins);
    uart[1].set_pins(config.uart[1].pins);
}

void uart_add_client_map(unsigned char id, unsigned char port, struct uart_client *c)
{
    struct uart_client_map *cmap = client_map;
    while (cmap->id != 0xff)
        cmap++;

    cmap->id = id;
    cmap->port = port;
    (cmap++)->c = c;
    cmap->id = 0xff;
}

void uart_init(void)
{
    extern int __C30_UART;
    __C30_UART = 1;

    uart_set_config_pins();

    uart[0].init();
    uart[1].init();

    process_add(uart_process);
}