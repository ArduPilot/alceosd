/*
    AlceOSD - Graphical OSD
    Copyright (C) 2016  Luis Alves

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

.global _jiffies
.global _millis

.global _get_jiffies
_get_jiffies:
    BCLR IEC0, #3
    MOV.W _jiffies,W0
    MOV.W _jiffies+2,W1
    BSET IEC0, #3
    RETURN

.global _get_millis
_get_millis:
    BCLR IEC0, #3
    MOV.W _millis,W0
    MOV.W _millis+2,W1
    BSET IEC0, #3
    RETURN

.global _get_millis16
_get_millis16:
    MOV.W _millis,W0
    RETURN

    
.global __T1Interrupt
__T1Interrupt:
    MOV.D W0, [W15++]
    MOV _jiffies, W0
    MOV _jiffies+2, W1
    ADD W0, #0x1, W0
    ADDC W1, #0x0, W1
    MOV W0, _jiffies
    MOV W1, _jiffies+2
    
    AND W0, #0xf, W0
    BRA NZ, skip_inc_millis
    
    MOV _millis, W0
    MOV _millis+2, W1
    ADD W0, #0x1, W0
    ADDC W1, #0x0, W1
    MOV W0, _millis
    MOV W1, _millis+2
    
skip_inc_millis:
    
    BCLR IFS0, #3
    MOV.D [--W15], W0
    RETFIE
