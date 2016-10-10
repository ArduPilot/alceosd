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

.global _sram_bytei_sqi
_sram_bytei_sqi:
;unsigned char sram_bytei_sqi(void)
    MOV #0x000F, W1
;   CLK_HIGH;
    BSET LATC, #8
    MOV PORTC, W2
    AND W1, W2, W2
;   CLK_LOW;
    BCLR LATC, #8
    MOV.B W2, W0
    SL W0, #4, W0
;   CLK_HIGH;
    BSET LATC, #8
    MOV PORTC, W2
    AND W1, W2, W2
;   CLK_LOW;
    BCLR LATC, #8
    IOR.B W2, W0, W0
    RETURN

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
    MOV DSRPAG, W6
    MOV W1, DSRPAG

;   store LATC and clear clk+data bits
    MOV LATC, W5
    MOV #0xFEF0, W3
    AND W5, W3, W5 ; base LATC

;   don't call this routine with W2(count) = 0
    DEC W2,W2
    DO  W2, copy_line_loop

    MOV.B [W0++], W4

;   upper nibble
    LSR W4, #4, W2
    AND W2, #0xF, W2
    IOR W5, W2, W2
    MOV W2, LATC
;   CLK_HIGH;
    BSET LATC, #8

    AND W4, #0xF, W2
    IOR W5, W2, W2
    MOV W2, LATC
    DISI #1
copy_line_loop:
;   CLK_HIGH;
    BSET LATC, #8
;   exit with clk low
;   CLK_LOW;
    BCLR LATC, #8

    MOV W6, DSRPAG
    RETURN


.global _clear_canvas
_clear_canvas:
    MOV DSWPAG, W6
    MOV W1, DSWPAG

    DEC W2,W2

    REPEAT W2
    MOV.B W3, [W0++]

    MOV W6, DSWPAG
    RETURN

