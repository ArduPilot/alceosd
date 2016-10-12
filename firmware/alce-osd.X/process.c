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


#define MAX_PROCESSES   20

//#define DEBUG_PROCESS
#ifdef DEBUG_PROCESS
#define DPROC(x...) \
    do { \
        shell_printf("PROCESS: "); \
        shell_printf(x); \
    } while(0)
#else
#define DPROC(x...)
#endif

struct process {
    void (*process)(void);
    unsigned long time, calls;
    unsigned char priority;
    unsigned char weight;
    unsigned int id;
    const char *name;
};

static struct process process_list[MAX_PROCESSES];
static unsigned char nr_processes = 0;


int process_add(void *f, const char *name, unsigned char priority)
{
    struct process *p = &process_list[nr_processes];
    if (nr_processes < MAX_PROCESSES) {
        p->name = name;
        p->time = 0;
        p->calls = 0;
        p->priority = priority;
        p->weight = 100;
        p->process = f;
        if (nr_processes > 0)
            p->id = (p-1)->id+1;
        else
            p->id = 0;
        DPROC("Added process %s, pid %d\n", p->name, nr_processes);
        nr_processes++;
    }
    return p->id;
}

void process_remove(int pid)
{
    unsigned char i;
    
    for (i = 0; i < nr_processes; i++) {
        if (process_list[i].id == pid)
            break;
    }
    if (i == nr_processes)
        return;
    
    DPROC("Removing process %s, pid %d\n", process_list[i].name, pid);

    for (; i < nr_processes-1; i++) {
        memcpy(&process_list[i], &process_list[i+1], sizeof(struct process));
    }
    nr_processes--;
}

static struct process *get_next_process(void)
{
    struct process *p = process_list, *n = process_list;
    unsigned char i, j;
    int c;
    
    for (i = 0; i < nr_processes; i++) {
        if (p->process == NULL)
            continue;
        if (p->weight > n->weight)
            n = p;
        p++;
    }

    j = 100 - n->weight;
    c = (-4 * ((int)n->priority))/50 + 9;
    c = ((int) n->weight) - c;
    n->weight = (unsigned char) c;
    p = process_list;
    for (i = 0; i < nr_processes; i++) {
        if (p->process == NULL)
            continue;
        (p++)->weight += j;
    }
    return n;
}

void process_run(void)
{
    struct process *p;
    unsigned int t;
    
    for (;;) {
        p = get_next_process();
        t = get_micros();
        p->process();
        t = get_micros() - t;
        p->calls++;
        p->time += t;
    }
}

static void shell_cmd_stats(char *args, void *data)
{
    unsigned char i;
    float t;
    unsigned long total = 0, millis = get_millis(), total_calls = 0;
    
    for (i = 0; i < nr_processes; i++)
        total_calls += process_list[i].calls;
    
    shell_printf("\nProcess list\n");
    for (i = 0; i < nr_processes; i++) {
        t = ((float) process_list[i].time / 1000) / (float) millis * 100.0;
        total += process_list[i].time / 1000;
        shell_printf("%2d %-10s time(ms)=%6lu time(%%)=%.2f calls=%lu(%.2f%%) \n",
                process_list[i].id,
                process_list[i].name, process_list[i].time / 1000, t,
                process_list[i].calls,
                (float) process_list[i].calls/total_calls * 100.0);
    }
    total = millis - total;
    t = ((float) total) / (float) millis * 100.0;
    shell_printf("   %-10s time(ms)=%6lu time(%%)=%.2f\n",
            "IRQS", total, t);
}

static void shell_cmd_stack(char *args, void *data)
{
    unsigned char i;
    unsigned int *stack_pos;
    unsigned int *p, *t;
    unsigned char *c;
    
    asm volatile("mov.w #__SP_init,%0" : "=r"(p));
    asm volatile("mov.w #__SPLIM_init,%0" : "=r"(t));
    asm volatile("mov.w W15,%0" : "=r"(stack_pos));
    
    c = (unsigned char*) p;
    
    shell_printf("\nStack dump :: [0x%04p--->0x%04p   0x%04p]\n\n", p, stack_pos, t);
    i = 0;
    shell_printf("0x%4p | ", p);
    while (p < stack_pos) {
        shell_printf("%04x ", *p++);

        if (i++ == 7) {
            for (i = 0; i < 16; i++) {
                if ((*c) > 20)
                    shell_printf("%c", *c);
                c++;
            }
            shell_printf("\n0x%4p : ", p);
            i = 0;
        }
    }
}

static void shell_cmd_kill(char *args, void *data)
{
    int pid;
    
    pid = atoi(args);
    shell_printf("\nRemoving process %s (%d)\n", process_list[pid].name, pid);
    process_remove(pid);
}

static const struct shell_cmdmap_s process_cmdmap[] = {
    {"list", shell_cmd_stats, "stats", SHELL_CMD_SIMPLE},
    {"kill", shell_cmd_kill, "kill process", SHELL_CMD_SIMPLE},
    {"stack", shell_cmd_stack, "dump stack", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_process(char *args, void *data)
{
    shell_exec(args, process_cmdmap, data);
}

