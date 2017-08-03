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

extern unsigned char hw_rev;

void uart_init(void)
{
    /* setup all ports */
    /* and detect used port */
    
    if (hw_rev >= 0x05) {
        /* u1 */
        _RP43R = 1;
        _U1RXR = 44;
        /* u2 */
        _RP42R = 3;
        _U2RXR = 47;
        /* u3 */
        _RP54R = 0x1b;
        _U3RXR = 55;
        /* u4 */
        _RP40R = 0x1d;
        _U4RXR = 39;
    } else if (hw_rev >= 0x03) {
        /* u1 */
        _RP42R = 1;
        _U1RXR = 43;
        /* u2 */
        _RP37R = 3;
        _U2RXR = 38;
        /* u3 */
        _RP35R = 0x1b;
        _U3RXR = 34;
        /* u4 */
        _RP39R = 0x1d;
        _U4RXR = 45;
    } else if (hw_rev == 0x02) {
        /* u1 */
        _RP37R = 1;
        _U1RXR = 38;
        /* u2 */
        _RP36R = 3;
        _U2RXR = 20;
    } else {
        /* u1 */
        _RP37R = 1;
        _U1RXR = 38;
        /* u2 */
        _RP41R = 3;
        _U2RXR = 20;
    }

    U1BRG = BRGVAL;
    U1MODE = 0x8000;
    U1STA = 0x0400;

    U2BRG = BRGVAL;
    U2MODE = 0x8000;
    U2STA = 0x0400;

    if (hw_rev > 0x03) {
        U3BRG = BRGVAL;
        U3MODE = 0x8000;
        U3STA = 0x0400;

        if (hw_rev >= 0x05) {
            /* usb */
            U4BRG = 18;
            U4MODE = 0x8008;
            U4STA = 0x0400;
        } else {
            U4BRG = BRGVAL;
            U4MODE = 0x8000;
            U4STA = 0x0400;
        }
    }        
}

extern unsigned char used_port;

int get_char(char *c)
{
    int ret = 0;
    switch (used_port) {
        case 0:
        default:
            if (U1STAbits.OERR == 1)
                U1STAbits.OERR = 0;
            if (U1STAbits.URXDA == 1) {
                *c = U1RXREG;
                ret = 1;
            }
            break;
        case 1:
            if (U2STAbits.OERR == 1)
                U2STAbits.OERR = 0;
            if (U2STAbits.URXDA == 1) {
                *c = U2RXREG;
                ret = 1;
            }
            break;
        case 2:
            if (U3STAbits.OERR == 1)
                U3STAbits.OERR = 0;
            if (U3STAbits.URXDA == 1) {
                *c = U3RXREG;
                ret = 1;
            }
            break;
        case 3:
            if (U4STAbits.OERR == 1)
                U4STAbits.OERR = 0;
            if (U4STAbits.URXDA == 1) {
                *c = U4RXREG;
                ret = 1;
            }
            break;
    }
    return ret;
}

void put_char(char c)
{
    switch (used_port) {
        case 0:
        default:
            while (!U1STAbits.TRMT);
            U1TXREG = c;
            break;
        case 1:
            while (!U2STAbits.TRMT);
            U2TXREG = c;
            break;
        case 2:
            while (!U3STAbits.TRMT);
            U3TXREG = c;
            break;
        case 3:
            while (!U4STAbits.TRMT);
            U4TXREG = c;
            break;
    }
}

void put_str(char *c)
{
    while (*c != '\0')
        put_char(*c++);
}
