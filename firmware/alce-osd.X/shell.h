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

#ifndef _SHELL_H
#define	_SHELL_H

#include "alce-types.h"
#define MAX_SHELL_ARGVAL_LEN    15

#define SHELL_GET_EXIT  0x8000

enum {
    SHELL_CMD_SIMPLE = 0,
    SHELL_CMD_SUBCMD,
};

struct shell_cmdmap_s {
    const char *cmd;
    void (*handler)(char *args, void *data);
    const char *usage;
    unsigned char type;
};

struct shell_argval {
    char key;
    char val[MAX_SHELL_ARGVAL_LEN];
};

void shell_init(void);
void shell_exec(char *cmd, const struct shell_cmdmap_s *c, void *data);
unsigned char shell_arg_parser(char *args, struct shell_argval *v, unsigned char max);
struct shell_argval* shell_get_argval(struct shell_argval *v, char k);

int shell_printf(const char *fmt, ...);
void shell_get(void *f, u32 data);
void shell_write(u8 *buf, u16 len);
void shell_write_eds(__eds__ u8 *buf, u16 len);

#endif
