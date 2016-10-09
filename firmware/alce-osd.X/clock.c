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
volatile unsigned long jiffies = 0;


struct timer {
    unsigned long period;
    unsigned long last_tick;
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
    struct shell_argval argval[2], *p;
    unsigned int a;

    shell_arg_parser(args, argval, 1);

    p = shell_get_argval(argval, 'd');
    if (p != NULL) {
        a = atoi(p->val);
        t = (struct timer *) a;

        for (i = 0; i < nr_timers; i++) {
            if ((void *) &timers[i] == (void *) a) {
                shell_printf("\n\nRemoved timer: %p\n", (void *) a);
                timers[i].cbk = NULL;
            }
        }
    }

    p = shell_get_argval(argval, 'p');
    if (p != NULL) {
        a = atoi(p->val);
        t = (struct timer *) a;

        for (i = 0; i < nr_timers; i++) {
            if ((void *) &timers[i] == (void *) a) {
                t = &timers[i];
                t->last_tick = get_millis();
                printf("\n\ntimer=%p type=%u period=%5lums last_tick=%5lums cbk=%p data=%p\n",
                    t, t->type, t->period, t->last_tick, t->cbk, t->data);
            }
        }
    }

    t = timers;
    shell_printf("\n\nWidget timers:\n");
    for (i = 0; i < nr_timers; i++) {
        if (t->cbk != NULL) {
            printf(" timer=%p type=%u period=%5lums last_tick=%5lums cbk=%p data=%p\n",
                    t, t->type, t->period, t->last_tick, t->cbk, t->data);
            total++;
        }
        t++;
    }
    shell_printf("\n\ntotal=%d peak=%d\n", total, nr_timers);
}

static void shell_cmd_stats(char *args, void *data)
{
    shell_printf("\nElapsed time since boot: %lums\n", get_millis());
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


unsigned long get_micros(void)
{
    return (get_jiffies() * 625) / 10;
}

inline void set_timer_period(struct timer *t, unsigned long period)
{
    t->period = period;
}

struct timer* add_timer(unsigned char type, unsigned long period, void *cbk, void *data)
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
    t->period = period;
    t->type = type;
    t->last_tick = get_millis();
    if (i == nr_timers)
        nr_timers++;
    
    shell_printf("\nT: cbk=%p type=%u T=%lu LT=%lu", t->cbk, t->type, t->period, t->last_tick);
    
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
    static unsigned char i = 0;
    struct timer *t = timers;

#if 0
    
    do {
        if (i == nr_timers) {
            i = 0;
            return;
        }
        t = &timers[i++];
    } while (t->cbk == NULL);
    
    if ((get_millis() - t->last_tick) > t->period) {
        t->last_tick += t->period;
        t->cbk(t, t->data);
        if (t->type == TIMER_ONCE)
            t->cbk = NULL;
    }
    
#else
    for (i = 0; i < nr_timers; i++) {
        t = &timers[i];
        if (t->cbk == NULL)
            continue;
        if ((get_millis() - t->last_tick) > t->period) {
            t->last_tick += t->period;
            t->cbk(t, t->data);
            if (t->type == TIMER_ONCE)
                t->cbk = NULL;
        }
    }
#endif
}

void clock_init(void)
{
    T1CON = 0x8000;
    IPC0bits.T1IP = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    /* period = 1 / (70000000 / 1) * 4375 = 62.5us */
    PR1 = 4375;

    process_add(clock_process, "CLOCK");
}
