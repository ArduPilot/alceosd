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

.global _set_pixel

_set_pixel:

; void set_pixel(unsigned int x, unsigned int y, unsigned int v, struct canvas *canv)

    ;   if ((x >= canv->width) || (y >= canv->height))
    MOV [W3+6], W4
    SUB W1, W4, [W15]
    BRA C, set_pixel_exit

    LSR W0, #2, W4  ; rx
    MOV [W3+8], W6 ; rwidth
    SUB W4, W6, [W15]
    BRA C, set_pixel_exit

    SL  W0, #1, W0
    AND W0, #0x6, W0

    MUL.SS W6, W1, W6
    ADD W4, W6, W6

    MOV [W3+12], W1 ; buf
    ADD W1, W6, W1  ; pixel location in W1

    MOV #0xff3f, W3
    LSR W3, W0, W3
    AND.B W3, [W1], [W1]

    SL  W2, #6, W2
    LSR W2, W0, W2
    IOR.B W2, [W1], [W1]

set_pixel_exit:
    RETURN




.global _set_pixel_fast

_set_pixel_fast:
; void set_pixel(unsigned int x, unsigned int y, unsigned int v, char *buf)
    LSR W0, #2, W4  ; rx
    MOV [W3+8], W6 ; rwidth

    SL  W0, #1, W0
    AND W0, #0x6, W0

    MUL.SS W6, W1, W6
    ADD W4, W6, W6

    MOV [W3+12], W1 ; buf
    ADD W1, W6, W1  ; pixel location in W1

    MOV #0xff3f, W3
    LSR W3, W0, W3
    AND.B W3, [W1], [W1]

    SL  W2, #6, W2
    LSR W2, W0, W2
    IOR.B W2, [W1], [W1]

    RETURN




.global _draw_hline
_draw_hline:
;void draw_hline(int x0, int x1, int y, unsigned char p, struct canvas *ca)
    CP W1, W0
    BRA GEU, _no_swap_draw_hline
    EXCH W0, W1
_no_swap_draw_hline:

    MOV [W4+6], W5 ; height
    CP W2, W5
    BRA GEU, _exit_draw_hline ; y is outside canvas

    MOV [W4+4], W5 ; width

    CP W0, W5
    BRA GEU, _exit_draw_hline

    CP W1, W5
    BRA LTU, _no_clip_x_draw_hline
    SUB W5, #1, W1 ; trim x1 with canvas width
_no_clip_x_draw_hline:

    MOV [W4+8], W6 ; rwidth
    MUL.SS W6, W2, W6 ; y addr
    MOV [W4+12], W7 ; buf
    ADD W7, W6, W6  ; pixel location in W1

    ; W7 contains ram start addr with x=0

    SUB W1, W0, W1 ; W1 contains pixel count

    DO  W1, _draw_hline_loop

    LSR W0, #2, W1  ; rx
    ADD W1, W6, W7 ; addr

    SL  W0, #1, W1
    AND W1, #0x6, W1

    MOV #0xff3f, W2
    LSR W2, W1, W2
    AND.B W2, [W7], [W7]

    SL  W3, #6, W2
    LSR W2, W1, W2
    IOR.B W2, [W7], [W7]

_draw_hline_loop:
    INC W0, W0

_exit_draw_hline:
    RETURN





.global _draw_vline
_draw_vline:
; void draw_vline(int x, int y0, int y1, unsigned char p, struct canvas *ca)
    CP W2, W1
    BRA GEU, _no_swap_draw_vline
    EXCH W1, W2
_no_swap_draw_vline:

    MOV [W4+4], W5 ; width
    CP W0, W5
    BRA GEU,_exit_draw_vline
    ;CPSLT W0, W5
    ;RETURN ; outside canvas width

    MOV [W4+6], W5 ; height

    CP W1, W5
    BRA GEU, _exit_draw_vline

    CP W2, W5 ; height
    ;CPSLT W2, W5
    ;SUB W5, #1, W2 ; trim y1 to canvas height
    BRA LTU,_no_clip_y_draw_vline
    SUB W5, #1, W2 ; trim y1 to canvas height
_no_clip_y_draw_vline:


    MOV [W4+12], W7 ; buf
    MOV [W4+8], W6 ; rwidth
    MUL.SS W6, W1, W4 ; y addr
    ADD W4, W7, W7  ; pixel location in W1

    ; W7 contains ram start addr with x=0 and y=y0

    LSR W0, #2, W5  ; rx
    ADD W5, W7, W7 ; addr
    ; W7 contains ram start addr with x=x and y=y0

    SL  W0, #1, W5
    AND W5, #0x6, W5
    ; W5 has the mask shifter

    SUB W2, W1, W2 ; W1 contains pixel count

    DO  W2, _draw_vline_loop

    MOV #0xff3f, W2
    LSR W2, W5, W2
    AND.B W2, [W7], [W7]

    SL  W3, #6, W2
    LSR W2, W5, W2
    IOR.B W2, [W7], [W7]

_draw_vline_loop:
    ADD W7, W6, W7
_exit_draw_vline:
    RETURN


