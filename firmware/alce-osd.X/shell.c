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

#define BELL        7
#define BACKSPACE   8
#define TAB         9
#define LF          10
#define CR          13
#define CTRL_R      18
#define CTRL_D      4

#define SERIAL_CONTROL_DEV_OSDSHELL 9

static struct uart_client shell_uart_client;
extern unsigned char hw_rev;
extern struct alceosd_config config;

static void shell_cmd_version(char *args, void *data)
{
    shell_printf("AlceOSD hw%dv%d fw%d.%d.%d\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
}

static void shell_cmd_reboot(char *args, void *data)
{
    shell_printf("Rebooting...\n");
    __asm__ volatile ("reset");
}

static const struct shell_cmdmap_s root_cmdmap[] = {
    {"clock", shell_cmd_clock, "Clock module", SHELL_CMD_SUBCMD},
    {"config", shell_cmd_cfg, "Config module", SHELL_CMD_SUBCMD},
    {"mavdata", shell_cmd_mavdata, "Mavlink data storage module", SHELL_CMD_SUBCMD},
    {"mavlink", shell_cmd_mavlink, "Mavlink module", SHELL_CMD_SUBCMD},
    {"process", shell_cmd_process, "Process module", SHELL_CMD_SUBCMD},
    {"reboot", shell_cmd_reboot, "Reboot AlceOSD", SHELL_CMD_SIMPLE},
    {"tabs", shell_cmd_tabs, "Tabs module", SHELL_CMD_SUBCMD},
    {"uart", shell_cmd_uart, "UART module", SHELL_CMD_SUBCMD},
    {"version", shell_cmd_version, "Display firmware version", SHELL_CMD_SIMPLE},
    {"video", shell_cmd_video, "Videocore module", SHELL_CMD_SUBCMD},
    {"widgets", shell_cmd_widgets, "Widgets module", SHELL_CMD_SUBCMD},
    {"", NULL, ""},
};

static u8 mavlink_shell_enabled = 0;

inline static void _shell_write_mavlink(u8 *buf, u16 len)
{
    mavlink_serial_control_t sc __attribute__((aligned(2)));
    mavlink_message_t msg __attribute__((aligned(2)));
    u16 wr;
    u8 *b = buf;
    do {
        wr = min(MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN, len);
        memcpy(sc.data, b, wr);
        sc.count = wr;
        sc.device = SERIAL_CONTROL_DEV_OSDSHELL;
        mavlink_msg_serial_control_encode(config.mav.osd_sysid, MAV_COMP_ID_OSD, &msg, &sc);
        mavlink_send_msg(&msg);
        b += wr;
        len -= wr;
    } while (len > 0);
}

inline static void _shell_write_uart(u8 *buf, u16 len)
{
    if (shell_uart_client.write == NULL)
        return;
    
    u16 wr;
    u8 *b = buf;
    do {
        wr = min(UART_TX_BUF_SIZE, len);
        shell_uart_client.write(b, wr);
        b += wr;
        len -= wr;
    } while (len > 0);
}

void shell_write(u8 *buf, u16 len)
{
    _shell_write_uart(buf, len);
    if (mavlink_shell_enabled)
        _shell_write_mavlink(buf, len);
}

#define EDS_TMP_BUF_SIZE (UART_TX_BUF_SIZE)
void shell_write_eds(__eds__ unsigned char *buf, u16 len)
{
    u8 _buf[EDS_TMP_BUF_SIZE];
    u16 wr, i;
    do {
        wr = min(EDS_TMP_BUF_SIZE, len);
        for (i = 0; i < wr; i++)
            _buf[i] = *(buf++);
        _shell_write_uart(_buf, wr);
        if (mavlink_shell_enabled)
            _shell_write_mavlink(_buf, wr);
        len -= wr;
    } while (len > 0);
}

inline static void shell_putc(unsigned char c)
{
    shell_write(&c, 1);
}

inline static void shell_puts(char *s)
{
    shell_write((u8*) s, strlen(s));
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

static struct shell_getter_s {
    unsigned char (*func)(unsigned char *buf, unsigned int len, unsigned long data);
    unsigned long data;
} shell_getter;

void shell_get(void *f, u32 data)
{
    shell_getter.func = f;
    shell_getter.data = data;
}

unsigned char shell_arg_parser(char *args, struct shell_argval *v, unsigned char max)
{
    char *p, *s;
    u8 i;
    u16 len = strlen(args);
    
    if (len == 0)
        return 0;

    /* deal with negative numbers replacing the minus with a marker (0xff) */
    for (i = 0; i < len - 1; i++) {
        if ((args[i] == '-') && (args[i+1] >= '0') && (args[i+1] <= '9')) {
            args[i] = 0xff;
        }
    }
    
    i = 0;
    p = strtok(args, "-");
    while (p != NULL) {
        v[i].key = *p++;
        while (*p == ' ')
            p++;
        strncpy(v[i].val, p, MAX_SHELL_ARGVAL_LEN);
        s = strchr(v[i].val, ' ');
        if (s != NULL)
            *s = '\0';
        /* replace the minus marker */
        s = strchr(v[i].val, 0xff);
        if (s != NULL)
            *s = '-';
        p = strtok(NULL, "-");
        i++;
        if (i == max)
            break;
    }
    v[i].key = '\0';
    return i;
}

struct shell_argval* shell_get_argval(struct shell_argval *v, char k)
{
    while (v->key != '\0') {
        if (v->key == k)
            return v;
        v++;
    }
    return NULL;
}

void shell_exec(char *cmd_line, const struct shell_cmdmap_s *c, void *data)
{
    char *args, *cmd = cmd_line;
    struct shell_cmdmap_s **ac = (struct shell_cmdmap_s **) data;

    args = strchr(cmd_line, ' ');
    if (args != NULL)
        *args++ = '\0';

    //shell_printf("\r\nexec: [%s] [%s] [%p]\r\n", cmd, args, data);
    if (strcmp(cmd, "help") == 0) {
        if (ac == NULL) {
            shell_putc(LF);
            while (c->handler != NULL) {
                shell_printf("%-10s : %s\n", c->cmd, c->usage);
                c++;
            }
        } else {
            while (c->handler != NULL) {
                (*ac) = (struct shell_cmdmap_s*) c;
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

static unsigned int shell_parser(struct uart_client *cli, unsigned char *buf, unsigned int len)
{
    static unsigned char cmd_len = 0;
    static char cmd_line[MAX_SHELL_LINE_LEN];
    static char prev_cmd_line[MAX_SHELL_LINE_LEN];
    unsigned char i;
    char tmp[MAX_SHELL_LINE_LEN], *p;
    struct shell_cmdmap_s *ac[20], **a, **b;
    int size;

    if (shell_getter.func != NULL) {
        i = shell_getter.func(buf, len, shell_getter.data);
        if (i)
            shell_getter.func = NULL;
        return len;
    }

    for (i = 0; i < len; i++) {
        //shell_printf("<%d>", buf[i]);
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
                shell_printf("\n> %s", cmd_line);
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
                    shell_putc(BELL);
                } else if (*(a+1) == NULL) {
                    p = &cmd_line[cmd_len];
                    strcat(cmd_line, (*a)->cmd + size);
                    cmd_len += strlen((*a)->cmd) - size;
                    cmd_line[cmd_len++] = ' ';
                    cmd_line[cmd_len] = '\0';
                    shell_puts(p);
                } else {
                    shell_putc(LF);
                    while (*a != NULL)
                        shell_printf("%s ", (*a++)->cmd);
                    shell_printf("\n> %s", cmd_line);
                }
                continue;
            case CR:
            case LF:
                shell_putc(LF);
                if (cmd_len > 0) {
                    cmd_line[cmd_len] = '\0';
                    strcpy(prev_cmd_line, cmd_line);
                    shell_exec(cmd_line, root_cmdmap, NULL);
                    cmd_len = 0;
                }
                shell_puts("> ");
                break;
            default:
                cmd_line[cmd_len] = buf[i];
                if (cmd_len < MAX_SHELL_LINE_LEN-1)
                    cmd_len++;
                shell_putc(buf[i]);
                break;
        }

    }
    return len;
}

void mavlink_serial_cmd(mavlink_message_t *msg, void *data)
{
    u8 buf[70];
    u8 len;

    if (mavlink_msg_serial_control_get_device(msg) != SERIAL_CONTROL_DEV_OSDSHELL)
        return;

    if (mavlink_msg_serial_control_get_baudrate(msg) == 0) {
        mavlink_shell_enabled = 0;
    } else {
        mavlink_shell_enabled = 1;
    }
    
    len = mavlink_msg_serial_control_get_count(msg);
    mavlink_msg_serial_control_get_data(msg, buf);
    shell_parser(NULL, buf, len);
}

void shell_init(void)
{
    add_mavlink_callback_sysid(MAV_SYS_ID_ANY, MAVLINK_MSG_ID_SERIAL_CONTROL, mavlink_serial_cmd, CALLBACK_PERSISTENT, NULL);
    
    memset(&shell_uart_client, 0, sizeof(struct uart_client));
    shell_uart_client.read = shell_parser;
    shell_uart_client.id = UART_CLIENT_SHELL;
    uart_add_client(&shell_uart_client);
}
