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

#define UART_FIFO_MASK 0x3f

#define DMA_UART1
#define DMA_UART2
#define DMA_UART3
#define DMA_UART4

#define DMA_BUF_SIZE    (64)

extern struct alceosd_config config;
extern unsigned char hw_rev;

const struct uart_regs {
    volatile unsigned int *BRG;
    unsigned int TXRP;
    volatile unsigned int *RXRP;
    volatile unsigned int *RX;
    volatile unsigned int *STA;
    volatile unsigned int *MODE;
} UARTS[] = {
    {
        .BRG = &U1BRG,
        .TXRP = 1,
        .RXRP = &RPINR18,
        .RX = &U1RXREG,
        .STA = &U1STA,
        .MODE = &U1MODE,
    },
    {
        .BRG = &U2BRG,
        .TXRP = 3,
        .RXRP = &RPINR19,
        .RX = &U2RXREG,
        .STA = &U2STA,
        .MODE = &U2MODE,
    },
    {
        .BRG = &U3BRG,
        .TXRP = 0x1b,
        .RXRP = &RPINR27,
        .RX = &U3RXREG,
        .STA = &U3STA,
        .MODE = &U3MODE,
    },
    {
        .BRG = &U4BRG,
        .TXRP = 0x1d,
        .RXRP = &RPINR28,
        .RX = &U4RXREG,
        .STA = &U4STA,
        .MODE = &U4MODE,
    },
};



const struct param_def params_uart12[] = {
    PARAM("SERIAL1_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[0].mode, uart_set_config_clients),
    PARAM("SERIAL1_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[0].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL1_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[0].pins, uart_set_config_pins),
    PARAM("SERIAL2_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[1].mode, uart_set_config_clients),
    PARAM("SERIAL2_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[1].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL2_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[1].pins, uart_set_config_pins),
    PARAM_END,
};

const struct param_def params_uart34[] = {
    PARAM("SERIAL3_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[2].mode, uart_set_config_clients),
    PARAM("SERIAL3_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[2].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL3_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[2].pins, uart_set_config_pins),
    PARAM("SERIAL4_MODE", MAV_PARAM_TYPE_UINT8, &config.uart[3].mode, uart_set_config_clients),
    PARAM("SERIAL4_BAUD", MAV_PARAM_TYPE_UINT8, &config.uart[3].baudrate, uart_set_config_baudrates),
    PARAM("SERIAL4_PINS", MAV_PARAM_TYPE_UINT8, &config.uart[3].pins, uart_set_config_pins),
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

static const struct hw_pin_map_table {
    unsigned int rx;
    unsigned int tx;
} hw_pin_map[3][4] = {
    /* telemetry, con2, icsp, con3 */
    /* hw0v1 */
    { { .rx = 38, .tx = 37 }, { .rx = 20, .tx = 41 }, { .rx = 0,  .tx = 0 },  { .rx = 0,  .tx = 0 } },
    /* hw0v2 */
    { { .rx = 38, .tx = 37 }, { .rx = 20, .tx = 36 }, { .rx = 34, .tx = 35 }, { .rx = 0,  .tx = 0 } },
    /* hw0v3 */
    { { .rx = 43, .tx = 42 }, { .rx = 38, .tx = 37 }, { .rx = 34, .tx = 35 }, { .rx = 45, .tx = 39 } },
};

#define UART_CLIENTS_MAX 10
static struct uart_client *uart_client_list[UART_CLIENTS_MAX] = { NULL };


struct uart_fifo {
  unsigned char buf[UART_FIFO_MASK+1];
  unsigned char rd, wr;
};

static struct uart_fifo rx_fifo[4];


/* tx dma buffers */
__eds__ unsigned char uart1TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-DMA_BUF_SIZE)));
__eds__ unsigned char uart2TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-(DMA_BUF_SIZE*2))));
__eds__ unsigned char uart3TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-(DMA_BUF_SIZE*3))));
__eds__ unsigned char uart4TxDataBuf[DMA_BUF_SIZE] __attribute__((eds,space(dma),address(0x4000-(DMA_BUF_SIZE*4))));


static const char keywords[] = "I want to enter AlceOSD setup";
static const unsigned char answer[] = "AlceOSD setup starting";
static unsigned char key_idx[4] = {0, 0, 0, 0};

inline unsigned long uart_get_baudrate(unsigned char b)
{
    if (b < UART_BAUDRATES)
        return baudrates[b].baudrate;
    else
        return 0;
}

static inline void uart_set_baudrate(unsigned char port, unsigned char b)
{
    if (b < UART_BAUDRATES)
        *(UARTS[port].BRG) = baudrates[b].brg;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA2Interrupt(void)
{
    IFS1bits.DMA2IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
{
    IFS2bits.DMA3IF = 0;
}

inline static void handle_uart_int(unsigned char port)
{
    unsigned char n_wr = rx_fifo[port].wr;
    unsigned char ch;

    /* if set, clear overflow bit */
    if (*(UARTS[port].STA) & 2) {
        *(UARTS[port].STA) &= ~2;
    }

    while (*(UARTS[port].STA) & 1) {
        n_wr = (n_wr + 1) & UART_FIFO_MASK;
        
        ch = *(UARTS[port].RX);
        if (keywords[key_idx[port]] == ch) {
            key_idx[port]++;
            if (key_idx[port] == (sizeof(keywords)-1)) {
                uart_set_client(port, UART_CLIENT_CONFIG);
                uart_get_client(port)->write(answer, sizeof(answer)-1);
                while ( (*(UARTS[port].STA) & 0x0100) == 0);
                uart_set_baudrate(port, UART_BAUD_115200);
                return;
            }
        } else {
            key_idx[port] = 0;
        }
        
        
        if (n_wr != rx_fifo[port].rd) {
            rx_fifo[port].buf[rx_fifo[port].wr] = ch;
            rx_fifo[port].wr = n_wr;
        }
    }
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    handle_uart_int(0);
    IFS0bits.U1RXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    handle_uart_int(1);
    IFS1bits.U2RXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U3RXInterrupt(void)
{
    handle_uart_int(2);
    IFS5bits.U3RXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U4RXInterrupt(void)
{
    handle_uart_int(3);
    IFS5bits.U4RXIF = 0;
}

void uart_set_direction(unsigned char port, unsigned char direction)
{
    unsigned char t;
    
    if ((hw_rev < 0x03) && (port > 1))
        return;
   
    if (direction == UART_DIR_TX) {
        /* disable rx */
        switch (port) {
            case 0:
                _U1RXIE = 0;
                break;
            case 1:
                _U2RXIE = 0;
                break;
            case 2:
                _U3RXIE = 0;
                break;
            case 3:
                _U4RXIE = 0;
                break;
            default:
                break;
        }
        
        /* enable tx */
        *(UARTS[port].STA) |= 0x0400;
    } else {
        /* disable tx */
        while ( (*(UARTS[port].STA) & 0x0100) == 0);
        *(UARTS[port].STA) &= ~0x0400;
        
        /* enable rx */
        while (*(UARTS[port].STA) & 1)
            t = *(UARTS[port].RX);
        if (*(UARTS[port].STA) & 2)
            *(UARTS[port].STA) &= ~2;

        switch (port) {
            case 0:
                _U1RXIF = 0;
                _U1RXIE = 1;
                break;
            case 1:
                _U2RXIF = 0;
                _U2RXIE = 1;
                break;
            case 2:
                _U3RXIF = 0;
                _U3RXIE = 1;
                break;
            case 3:
                _U4RXIF = 0;
                _U4RXIE = 1;
                break;
            default:
                break;
        }
    }
}


static void uart_init_(unsigned char port)
{
    uart_set_baudrate(port, UART_BAUD_115200);
    
    /* clear status reg */
    *(UARTS[port].STA) = 0;
    /* enable */
    *(UARTS[port].MODE) = 0x8000;
    
    switch (port) {
        case UART_PORT1:
            _U1RXIP = 1;
            _U1RXIF = 0;
            _U1RXIE = 1;
#ifdef DMA_UART1
            DMA0CON = 0x6001; /* one-shot p-p disabled, ram to per, byte mode */
            DMA0REQ = 0x0c; // U1TX interrupt requests transfer
            DMA0PAD = (volatile unsigned int) &U1TXREG; // Transfer to U1TXREG
            DMA0STAL = __builtin_dmaoffset(&uart1TxDataBuf);
            DMA0STAH = __builtin_dmapage(&uart1TxDataBuf);
            //_DMA0IF = 0;
            //_DMA0IE = 1;
#endif
            break;
        case UART_PORT2:
            _U2RXIP = 1;
            _U2RXIF = 0;
            _U2RXIE = 1;
#ifdef DMA_UART2
            DMA1CON = 0x6001; /* one-shot p-p disabled, ram to per, byte mode */
            DMA1REQ = 0x1f; // U2TX interrupt requests transfer
            DMA1PAD = (volatile unsigned int) &U2TXREG; // Transfer to U2TXREG
            DMA1STAL = __builtin_dmaoffset(&uart2TxDataBuf);
            DMA1STAH = __builtin_dmapage(&uart2TxDataBuf);
            //_DMA1IF = 0;
            //_DMA1IE = 1;
#endif
            break;
        case UART_PORT3:
            _U3RXIP = 1;
            _U3RXIF = 0;
            _U3RXIE = 1;
#ifdef DMA_UART3
            DMA2CON = 0x6001; /* one-shot p-p disabled, ram to per, byte mode */
            DMA2REQ = 0x53; // U3TX interrupt requests transfer
            DMA2PAD = (volatile unsigned int) &U3TXREG; // Transfer to U3TXREG
            DMA2STAL = __builtin_dmaoffset(&uart3TxDataBuf);
            DMA2STAH = __builtin_dmapage(&uart3TxDataBuf);
            //_DMA2IF = 0;
            //_DMA2IE = 1;
#endif            
            break;
        case UART_PORT4:
            _U4RXIP = 1;
            _U4RXIF = 0;
            _U4RXIE = 1;
#ifdef DMA_UART4
            DMA3CON = 0x6001; /* one-shot p-p disabled, ram to per, byte mode */
            DMA3REQ = 0x59; // U4TX interrupt requests transfer
            DMA3PAD = (volatile unsigned int) &U4TXREG; // Transfer to U4TXREG
            DMA3STAL = __builtin_dmaoffset(&uart4TxDataBuf);
            DMA3STAH = __builtin_dmapage(&uart4TxDataBuf);
            //_DMA3IF = 0;
            //_DMA3IE = 1;
#endif
            break;
        default:
            break;
    }
    
    /* enable TX */
    *(UARTS[port].STA) |= 0x0400;
}


static void uart_set_pins(unsigned char port, unsigned char pins)
{
    if (((hw_rev < 0x03) && (port > 1)) || (pins >= UART_PINS_OFF))
        return;

    *(UARTS[port].RXRP) = hw_pin_map[hw_rev-1][pins].rx;
    
    switch (pins) {
        case UART_PINS_TELEMETRY:
            if (hw_rev == 0x03) {
                _RP42R = UARTS[port].TXRP;
                //*(UARTS[port].RXRP) = 43;
            } else {
                _RP37R = UARTS[port].TXRP;
                //*(UARTS[port].RXRP) = 38;
            }
            break;
        case UART_PINS_CON2:
            if (hw_rev == 0x03) {
                _RP37R = UARTS[port].TXRP;
                //*(UARTS[port].RXRP) = 38;
            } else {
                // TX
                if (hw_rev == 0x01)
                    _RP41R = UARTS[port].TXRP;
                else
                    _RP36R = UARTS[port].TXRP;
                // RX
                //*(UARTS[port].RXRP) = 20;
            }
            break;
        case UART_PINS_ICSP:
            if (hw_rev == 0x01)
                break;
            _RP35R = UARTS[port].TXRP;
            //*(UARTS[port].RXRP) = 34;
            break;
        case UART_PINS_CON3:
            if (hw_rev < 0x03)
                break;
            _RP39R = UARTS[port].TXRP;
            //*(UARTS[port].RXRP) = 45;
            break;
        default:
            //*(UARTS[port].RXRP) = 0;
            break;
    }
}

static unsigned int uart_read(unsigned char port, unsigned char **buf)
{
    unsigned int wr = rx_fifo[port].wr;
    unsigned int ret = (wr - rx_fifo[port].rd) & UART_FIFO_MASK;
    if (ret) {
        *buf = &rx_fifo[port].buf[rx_fifo[port].rd];
        if (rx_fifo[port].rd > wr) {
            ret = UART_FIFO_MASK + 1 - rx_fifo[port].rd;
        }
    }
    return ret;
}

static inline unsigned int uart_count(unsigned char port)
{
    return (rx_fifo[port].wr - rx_fifo[port].rd) & UART_FIFO_MASK;
}

static inline void uart_discard(unsigned char port, unsigned int count)
{
    rx_fifo[port].rd += count;
    rx_fifo[port].rd &= UART_FIFO_MASK;
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

static void uart3_write(unsigned char *buf, unsigned int len)
{
#ifndef DMA_UART3
    while (len) {
        while (!U3STAbits.TRMT);
        U3TXREG = *buf++;
        len--;
    }
#else
    unsigned int count;
    if (len == 0)
        return;

    /* still busy */
    while ((DMA2CONbits.CHEN == 1) || (U3STAbits.TRMT == 0));

    count = 0;
    while ((len-- > 0) && (count < sizeof(uart3TxDataBuf))) {
        uart3TxDataBuf[count++] = *buf++;
    }
    DMA2CNT = count - 1; // DMAxCNT is N-1
    DMA2CONbits.CHEN = 1; // DMA one-shot mode requires the CHEN bit be set every time
    DMA2REQbits.FORCE = 1; // Force the DMA to start transferring data.
#endif
}

void uart4_write(unsigned char *buf, unsigned int len)
{
#ifndef DMA_UART4
    while (len) {
        while (!U4STAbits.TRMT);
        U4TXREG = *buf++;
        len--;
    }
#else
    unsigned int count;
    if (len == 0)
        return;

    /* still busy */
    while ((DMA3CONbits.CHEN == 1) || (U4STAbits.TRMT == 0));

    count = 0;
    while ((len-- > 0) && (count < sizeof(uart4TxDataBuf))) {
        uart4TxDataBuf[count++] = *buf++;
    }
    DMA3CNT = count - 1; // DMAxCNT is N-1
    DMA3CONbits.CHEN = 1; // DMA one-shot mode requires the CHEN bit be set every time
    DMA3REQbits.FORCE = 1; // Force the DMA to start transferring data.
#endif
}

unsigned char uart_getc(unsigned char port, char *c)
{
    unsigned char ret = (rx_fifo[port].rd != rx_fifo[port].wr);
    if (ret) {
        *c = rx_fifo[port].buf[rx_fifo[port].rd++];
        rx_fifo[port].rd &= UART_FIFO_MASK;
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


static struct uart_client *port_clients[4] = {NULL, NULL, NULL, NULL};


static void uart_process(void)
{
    int i;
    unsigned char *b;
    unsigned int len;

    for (i = 0; i < 4; i++) {
        if ((port_clients[i] != NULL) && (uart_count(i) > 0)) {
            len = uart_read(i, &b);
            len = port_clients[i]->read(port_clients[i], b, len);
            uart_discard(i, len);
        }
    }
}


void uart_set_client(unsigned char port, unsigned char client_id)
{
    struct uart_client **clist = uart_client_list;
    struct uart_client **c = &port_clients[port];
    extern int __C30_UART;

    if (port > 3)
        return;
    
    /* detach client */
    if ((*c) != NULL) {
        /* client_id is already on this port */
        if ((*c)->id == client_id)
            return;
        if ((*c)->close != NULL)
            (*c)->close(*c);
        (*c)->write = NULL;
    }

    if (client_id == UART_CLIENT_NONE)
        return;
    
    while (*clist != NULL) {
        if (((*clist)->id == client_id) && ((*clist)->write == NULL)) {
            if (client_id == UART_CLIENT_CONFIG)
                __C30_UART = port+1;
            *c = *clist;
            switch (port) {
                case UART_PORT1:
                    (*c)->write = uart1_write;
                    break;
                case UART_PORT2:
                    (*c)->write = uart2_write;
                    break;
                case UART_PORT3:
                    (*c)->write = uart3_write;
                    break;
                case UART_PORT4:
                    (*c)->write = uart4_write;
                    break;
                default:
                    break;
            }
            (*c)->port = port;
            if ((*c)->init != NULL)
                (*c)->init(*c);
            break;
        }
        clist++;
    }

    /* client not found or no clients */
    if (*clist == NULL)
        *c = NULL;
}

struct uart_client* uart_get_client(unsigned char port)
{
    struct uart_client **c = &port_clients[port];
    if (port > 3)
        return NULL;
    return *c;
}

void uart_set_props(unsigned char port, unsigned int props)
{
    if ((hw_rev < 0x03) && (port > 1))
        return;

    if (props & UART_PROP_TX_INVERTED)
        *(UARTS[port].STA) |= 0x4000;
    else
        *(UARTS[port].STA) &= ~0x4000;

    if (props & UART_PROP_RX_INVERTED)
        *(UARTS[port].MODE) |= 0x0010;
    else
        *(UARTS[port].MODE) &= ~0x0010;
    
    if (props & UART_PROP_HALF_DUPLEX) {
        *(UARTS[port].RXRP) = hw_pin_map[hw_rev-1][config.uart[port].pins].tx;
        uart_set_direction(port, UART_DIR_RX);
    }
}

void uart_set_config_clients(void)
{
    uart_set_client(0, config.uart[0].mode);
    uart_set_client(1, config.uart[1].mode);
    if (hw_rev > 0x02) {
        uart_set_client(2, config.uart[2].mode);
        uart_set_client(3, config.uart[3].mode);
    }
}

void uart_set_config_baudrates(void)
{
    uart_set_baudrate(0, config.uart[0].baudrate);
    uart_set_baudrate(1, config.uart[1].baudrate);
    if (hw_rev > 0x02) {
        uart_set_baudrate(2, config.uart[2].baudrate);
        uart_set_baudrate(3, config.uart[3].baudrate);
    }
}

void uart_set_config_pins(void)
{
    uart_set_pins(0, config.uart[0].pins);
    uart_set_pins(1, config.uart[1].pins);
    if (hw_rev > 0x02) {
        uart_set_pins(2, config.uart[2].pins);
        uart_set_pins(3, config.uart[3].pins);
    }
}

void uart_add_client(struct uart_client *c)
{
    struct uart_client **clist = uart_client_list;
    while (*clist != NULL)
        clist++;

    *(clist++) = c;
    *clist = NULL;
}

void uart_init(void)
{
    extern int __C30_UART;
    __C30_UART = 1;

    uart_set_config_pins();

    uart_init_(0);
    uart_init_(1);
    params_add(params_uart12);
    
    if (hw_rev > 0x02) {
        uart_init_(2);
        uart_init_(3);
        params_add(params_uart34);
    }
    process_add(uart_process);
}
