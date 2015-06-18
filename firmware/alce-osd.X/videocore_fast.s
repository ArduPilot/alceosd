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

.text


.global _sram_byteo_sqi
_sram_byteo_sqi:
;void sram_byteo_sqi(unsigned char b)
    MOV LATC, W2
    MOV #0xFEF0, W1
    AND W2, W1, W2

    LSR W0, #4, W1
    AND W1, #0xF, W1
    IOR W2, W1, W1
    MOV W1, LATC
;   CLK_HIGH;
    BSET LATC, #8

    AND W0, #0xF, W0
    IOR W2, W0, W2
    MOV W2, LATC
;   CLK_HIGH;
    BSET LATC, #8
;   CLK_LOW;
    BCLR LATC, #8
    RETURN


.global _copy_line
_copy_line:
;   store LATC and clear clk+data bits
    MOV LATC, W2
    MOV #0xFEF0, W3
    AND W2, W3, W2 ; base LATC

;   don't call this routine with W1(count) = 0
    DEC W1,W1
    DO  W1, copy_line_loop

    MOV.B [W0++], W4

;   upper nibble
    LSR W4, #4, W1
    AND W1, #0xF, W1
    IOR W2, W1, W1
    MOV W1, LATC
;   CLK_HIGH;
    BSET LATC, #8

    AND W4, #0xF, W1
    IOR W2, W1, W1
    MOV W1, LATC
copy_line_loop:
;   CLK_HIGH;
    BSET LATC, #8
;   exit with clk low
;   CLK_LOW;
    BCLR LATC, #8

    RETURN



