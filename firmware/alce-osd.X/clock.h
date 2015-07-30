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

#ifndef CLOCK_H
#define	CLOCK_H

#define TIMER_10MS  0x10

enum {
    TIMER_ONCE = 0,
    TIMER_ALWAYS,
    TIMER_WIDGET,
};


void clock_init(void);
struct timer* add_timer(unsigned char type, unsigned int time, void *cbk, void *data);
void remove_timer(struct timer *t);
void remove_timers(unsigned char type);
unsigned long get_millis(void);

#endif	/* CLOCK_H */

