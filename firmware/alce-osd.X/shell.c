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

#include "alce-osd.h"

#define MAX_SHELL_LINE_LEN  50


extern unsigned char hw_rev;
static void shell_cmd_version(char *args, void *data)
{
    shell_printf("\nAlceOSD hw%dv%d fw%d.%d.%d\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
}

static void shell_cmd_reboot(char *args, void *data)
{
    shell_printf("\nRebooting...\n");
    __asm__ volatile ("reset");
}

static const struct shell_cmdmap_s root_cmdmap[] = {
    {"clock", shell_cmd_clock, "Clock module", SHELL_CMD_SUBCMD},
    {"mavlink", shell_cmd_mavlink, "Mavlink module", SHELL_CMD_SUBCMD},
    {"reboot", shell_cmd_reboot, "Reboot AlceOSD", SHELL_CMD_SIMPLE},
    {"uart", shell_cmd_uart, "UART module", SHELL_CMD_SUBCMD},
    {"version", shell_cmd_version, "Display firmware version", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};



void shell_putc(unsigned char c)
{
    printf("%c", c);
}

void shell_puts(char *s)
{
    printf("%s", s);
}

int shell_printf(const char *fmt, ...)
{
    char buf[100];
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vsprintf(buf, fmt, ap);
    va_end(ap);
    if (ret > 0) {
        shell_puts(buf);
    }
    return ret;
}

void shell_exec(char *cmd_line, const struct shell_cmdmap_s *c, void *data)
{
    char *args, *cmd = cmd_line;
    struct shell_cmdmap_s **ac = (struct shell_cmdmap_s **) data;

    args = strchr(cmd_line, ' ');
    if (args != NULL)
        *args++ = '\0';

    //printf("\r\nexec: [%s] [%s] [%p]\r\n", cmd, args, data);
    if (strcmp(cmd, "help") == 0) {
        if (ac == NULL) {
            shell_printf("\r\n");
            while (c->handler != NULL) {
                shell_printf("%-10s : %s\n", c->cmd, c->usage);
                c++;
            }
        } else {
            while (c->handler != NULL) {
                (*ac) = c;
                ac++;
                c++;
            }
            *ac = NULL;
        }
        return;
    }
    if (ac != NULL)
        *ac = NULL;
    while (c->handler != NULL) {
        if (strcmp(cmd, c->cmd) == 0) {
            if ((ac == NULL) || (c->type == SHELL_CMD_SUBCMD))
                c->handler(args, data);
            break;
        }
        c++;
    }
}

#define BACKSPACE   8
#define TAB         9
#define CR          13
#define CTRL_R      18

void shell_parser(unsigned char *buf, unsigned int len)
{
    static unsigned char cmd_len = 0;
    static char cmd_line[MAX_SHELL_LINE_LEN];
    static char prev_cmd_line[MAX_SHELL_LINE_LEN];
    unsigned char i;
    char tmp[MAX_SHELL_LINE_LEN], *p;
    struct shell_cmdmap_s *ac[20], **a, **b;
    int size;
    
    for (i = 0; i < len; i++) {
        //printf("<%d>", buf[i]);
        switch (buf[i]) {
            case BACKSPACE:
                if (cmd_len > 0) {
                    cmd_len--;
                    shell_putc(buf[i]);
                }
                continue;
            case CTRL_R:
                strcpy(cmd_line, prev_cmd_line);
                cmd_len = strlen(cmd_line);
                shell_printf("\r\n> %s", cmd_line);
                continue;
            case TAB:
                cmd_line[cmd_len] = '\0';
                strcpy(tmp, cmd_line);
                p = strrchr(tmp, ' ');
                if (p == NULL) {
                    strcpy(tmp, "help");
                    p = tmp;
                } else {
                    strcpy(p, " help");
                    p++;
                }
                shell_exec(tmp, root_cmdmap, ac);
                strcpy(tmp, cmd_line);
                size = strlen(p);
                if (size > 0) {
                    a = b = ac;
                    while (*a != NULL) {
                        if (strncmp((*a)->cmd, p, size) == 0)
                            *b++ = *a;
                        a++;
                    }
                    *b = NULL;
                }
                a = ac;
                if (*a == NULL) {
                    shell_printf("%c", 7);
                } else if (*(a+1) == NULL) {
                    p = &cmd_line[cmd_len];
                    strcat(cmd_line, (*a)->cmd + size);
                    cmd_len += strlen((*a)->cmd) - size;
                    cmd_line[cmd_len++] = ' ';
                    cmd_line[cmd_len] = '\0';
                    shell_printf("%s", p);
                } else {
                    shell_printf("\r\n");
                    while (*a != NULL)
                        shell_printf("%s ", (*a++)->cmd);
                    shell_printf("\r\n> %s", cmd_line);
                }
                continue;
            case CR:
                cmd_line[cmd_len] = '\0';
                strcpy(prev_cmd_line, cmd_line);
                shell_exec(cmd_line, root_cmdmap, NULL);
                cmd_len = 0;
                shell_printf("\r\n> ");
                break;
            default:
                cmd_line[cmd_len] = buf[i];
                if (cmd_len < MAX_SHELL_LINE_LEN-1)
                    cmd_len++;
                shell_putc(buf[i]);
                break;
        }

    }
}
