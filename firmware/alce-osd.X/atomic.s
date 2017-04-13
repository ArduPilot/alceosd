/*
    AlceOSD - Graphical OSD
    Copyright (C) 2017  Luis Alves

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

.global _atomic_set16
_atomic_set16:
; void atomic_set16(u16 *var, u16 val)
    disi #1
    mov.w   W1,[W0]
    return

.global _atomic_get16
_atomic_get16:
; u16 atomic_get16(u16 *var)
    disi #1
    mov.w   [W0], W0
    return

.global _atomic_clr16
_atomic_clr16:
; void atomic_clr16(u16 *var)
    disi #1
    clr.w   [W0]
    return

.global _atomic_inc16
_atomic_inc16:
; void atomic_inc16(u16 *var)
    disi #1
    inc.w   [W0], [W0]
    return

.global _atomic_set8
_atomic_set8:
; void atomic_set8(u8 *var, u8 val)
    disi #1
    mov.b   W1,[W0]
    return

.global _atomic_get8
_atomic_get8:
; u8 atomic_get8(u8 *var)
    disi #1
    mov.b   [W0], W0
    return

.global _atomic_bset8
_atomic_bset8:
; void atomic_bset8(u8 *var, u8 bit)
    disi #2
    bset    SR, #0
    bsw.c   [W0], W1
    return

.global _atomic_bclr8
_atomic_bclr8:
; void atomic_bclr8(u8 *var, u8 bit)
    disi #1
    bclr    SR, #0
    bsw.c   [W0], W1
    return
