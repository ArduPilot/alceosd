 /*
    AlceOSD - UART bootloader
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

#include "bootloader.h"


#define BAUDRATE                        115200
#define BRGVAL                          37 /* bgrval = ((FCY/BAUDRATE)/16)-1 */

void uart_init(void)
{
    volatile unsigned int i;
    
    /* uart setup */
    /* detect hw_rev */
    
    /* set RB9 internal pull down */
    _TRISB9 = 1;
    _CNPUB9 = 0;
    _CNPDB9 = 1;

    for (i = 0; i < 10000; i++);
    
    if (_RB9 == 1) {
        /* hv_rev 0v3 */
        _RP42R = 3;
        _U2RXR = 43;
    } else {
        /* hw_rev 0v1 and 0v2 */
        _RP37R = 3;
        _U2RXR = 38;
    }

    /* set baudrate  */
    U2BRG = BRGVAL;
    U2MODE = 0x8000;
    U2MODEbits.BRGH = 0;
    U2STA = 0x0400;
}

int get_char(char *c)
{
    int ret = 0;

    while (1) {
        if (_T3IF == 1) {
            /* boot timer expired */
            ret = 1;
            T2CONbits.TON = 0;
            *c = 0xff;
        }
        if (U2STAbits.FERR == 1) {
            /* uart rx err */
            ret = 2;
        }
        if (U2STAbits.OERR == 1) {
            /* overrun error */
            ret = 3;
            /* clear to continue rx */
            U2STAbits.OERR = 0;
        }
        if (ret)
            break;

        if (U2STAbits.URXDA == 1) {
            T2CONbits.TON = 0;
            *c = U2RXREG;
            break;
        }
    }
    return ret;
}

void put_char(char c)
{
    while (!U2STAbits.TRMT);
    U2TXREG = c;
}

void put_str(char *c)
{
    while (*c != '\0')
        put_char(*c++);
}
