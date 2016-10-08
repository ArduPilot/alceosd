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

enum {
    TIMER_ONCE = 0,
    TIMER_ALWAYS,
    TIMER_WIDGET,
};


void clock_init(void);
struct timer* add_timer(unsigned char type, unsigned int period, void *cbk, void *data);
inline void remove_timer(struct timer *t);
void remove_timers(unsigned char type);
inline void set_timer_period(struct timer *t, unsigned int period);
unsigned long get_micros(void);
void shell_cmd_clock(char *args, void *data);

/* clock_fast.s */
extern unsigned long get_jiffies(void);
extern unsigned long get_millis(void);
extern unsigned int get_millis16(void);


#endif	/* CLOCK_H */

