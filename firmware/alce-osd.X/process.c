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


#define MAX_PROCESSES   10

static void *process_list[MAX_PROCESSES] = {NULL};
static unsigned char nr_processes = 0;


void process_add(void *f)
{
    if (nr_processes < MAX_PROCESSES) {
        process_list[nr_processes++] = f;
        process_list[nr_processes] = NULL;
    }
}

void process_remove(void *f)
{
    void **p = process_list;
    unsigned char i;

    while (*p != NULL) {
        if (*p == f)
            break;
        p++;
    }
    i = p - process_list;
    if (i < (nr_processes - 1))
        memcpy(&process_list[i], &process_list[i + 1], sizeof(void *) * (nr_processes - i - 1));
    nr_processes--;
}

void process_run(void)
{
    void (*fptr)(void);
    void **p = process_list;
    for (;;) {
        fptr = *p++;
        fptr();
        if (*p == NULL) {
            p = process_list;
            ClrWdt();
        }
    }
}