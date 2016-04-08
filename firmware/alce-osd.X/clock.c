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

#define MAX_TIMERS  (30)

volatile unsigned long millis = 0;
volatile unsigned int ms10 = 0;


struct timer {
    unsigned int time;
    unsigned int last_time;
    unsigned char type;
    void (*cbk)(struct timer *t, void *data);
    void *data;
};

static struct timer timers[MAX_TIMERS];
static unsigned char nr_timers = 0;


static void shell_cmd_timers(char *args, void *data)
{
    unsigned char i, total = 0;
    struct timer *t = timers;

    shell_printf("\n\nWidget timers:\n");
    for (i = 0; i < nr_timers; i++) {
        if ((t->cbk != NULL) && (t->type == TIMER_WIDGET)) {
            printf(" period=%5dms last_tick=%5d cbk=%p data=%p\n", t->time * 10, t->last_time, t->cbk, t->data);
            total++;
        }
        t++;
    }
    shell_printf("\n\nGeneric timers:\n");
    t = timers;
    for (i = 0; i < nr_timers; i++) {
        if ((t->cbk != NULL) && (t->type != TIMER_WIDGET)) {
            printf(" period=%5dms last_tick=%5d cbk=%p data=%p\n", t->time * 10, t->last_time, t->cbk, t->data);
            total++;
        }
        t++;
    }
    shell_printf("\n\ntotal=%d peak=%d\n", total, nr_timers);
}

static void shell_cmd_stats(char *args, void *data)
{
    shell_printf("\nElapsed time since boot: %lums\n", millis);
}

static const struct shell_cmdmap_s clock_cmdmap[] = {
    {"timers", shell_cmd_timers, "callbacks", SHELL_CMD_SIMPLE},
    {"stats", shell_cmd_stats, "stats", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_clock(char *args, void *data)
{
    shell_exec(args, clock_cmdmap, data);
}


struct timer* add_timer(unsigned char type, unsigned int time, void *cbk, void *data)
{
    struct timer *t = timers;
    unsigned char i;

    for (i = 0; i < nr_timers; i++) {
        if (timers[i].cbk == NULL)
            break;
    }

    if (i == MAX_TIMERS)
        return NULL;

    t = &timers[i];
    t->cbk = cbk;
    t->data = data;
    if (type & TIMER_10MS)
        t->time = time;
    else
        t->time = time * 10;
    t->type = type;
    t->last_time = ms10;
    if (i == nr_timers)
        nr_timers++;
    return t;
}

inline void remove_timer(struct timer *t)
{
    t->cbk = NULL;
}

void remove_timers(unsigned char ctype)
{
    struct timer *t = timers;
    unsigned char i;

    for (i = 0; i < nr_timers; i++) {
        if (t->type == ctype)
            t->cbk = NULL;
        t++;
    }
}

static void clock_process(void)
{
    unsigned char i;
    struct timer *t;

    for (i = 0; i < nr_timers; i++) {
        t = &timers[i];
        if (t->cbk == NULL)
            continue;
        if ((ms10 - t->last_time) > t->time) {
            t->last_time += t->time;
            t->cbk(t, t->data);
            if (t->type == TIMER_ONCE)
                t->cbk = NULL;
        }
    }
}

unsigned long get_millis(void)
{
    unsigned long m;
    IEC0bits.T1IE = 0;
    m = millis;
    IEC0bits.T1IE = 1;
    return m;
}

void clock_init(void)
{
    T1CON = 0x8010;
    IPC0bits.T1IP = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    /* period = 1 / (70000000 / 8) * 8750 = 1ms */
    PR1 = 8750;

    process_add(clock_process);
}

void __attribute__((__interrupt__, no_auto_psv )) _T1Interrupt()
{
    static unsigned int j = 0;
    millis++;

    if (++j == 10) {
        j = 0;
        ms10++;
    }
    IFS0bits.T1IF = 0;
}
