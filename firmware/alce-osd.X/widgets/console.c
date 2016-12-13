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

#define PERSISTENT_BUFFER

#define ROWS    (10)
#define COLS    (45)

#define X_SIZE  (8*COLS)
#define Y_SIZE  (13*ROWS)

static struct widget *console = NULL;

struct widget_priv {
    unsigned char x, y;
    char buf[ROWS][COLS];
    s8 line0;
};

#ifdef PERSISTENT_BUFFER
static struct widget_priv priv;
#endif

static inline void print_chr(char chr)
{
#ifdef PERSISTENT_BUFFER
    struct widget_priv *wp = &priv;
#else
    struct widget_priv *wp = console->priv;
#endif

    if ((chr == '\n') || (wp->x == COLS-1)) {
        wp->buf[wp->y][wp->x] = '\0';
        wp->x = 0;

        if (++(wp->y) == ROWS)
            wp->y = 0;

        if (++(wp->line0) == ROWS)
            wp->line0 = 0;
    }
    if (chr > 13) {
        wp->buf[wp->y][(wp->x)++] = chr;
        wp->buf[wp->y][wp->x] = '\0';
    }
}

void console_printn(char *str, unsigned int len)
{
#ifndef PERSISTENT_BUFFER
    if (console == NULL)
        return;
#endif

    while (len-- != 0)
        print_chr(*(str++));

#ifdef PERSISTENT_BUFFER
    if (console == NULL)
        return;
#endif
    schedule_widget(console);
}

void console_print(char *str)
{
#ifndef PERSISTENT_BUFFER
    if (console == NULL)
        return;
#endif

    while (*str != '\0')
        print_chr(*(str++));
    
#ifdef PERSISTENT_BUFFER
    if (console == NULL)
        return;
#endif
    schedule_widget(console);
}

int console_printf(const char *fmt, ...)
{
    char buf[MAX_LINE_LENGTH];
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vsprintf(buf, fmt, ap);
    va_end(ap);
    if (ret > 0) {
        console_print(buf);
    }
    return ret;
}

static void mav_callback(mavlink_message_t *msg, void *d)
{
    mavlink_statustext_t s;
    
    mavlink_msg_statustext_decode(msg, &s);
    console_printf("[%d] %s\n", s.severity, s.text);
}

static int open(struct widget *w)
{
    struct widget_priv *p;

    /* don't allow more than 1 instance */
    if (console != NULL)
        return -2;

#ifdef PERSISTENT_BUFFER
    p = &priv;
#else
    p = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (p == NULL)
        return -1;
#endif
    w->priv = p;
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;

#ifndef PERSISTENT_BUFFER
    memset(priv.buf, 0, ROWS*COLS);
    priv.line0 = -ROWS;
    add_mavlink_callback(MAVLINK_MSG_ID_STATUSTEXT, mav_callback, CALLBACK_WIDGET, NULL);
#endif
    
    console = w;
    return 0;
}


static void close(struct widget *w)
{
    console = NULL;
}

static void init(void)
{
    console = NULL;
#ifdef PERSISTENT_BUFFER
    add_mavlink_callback(MAVLINK_MSG_ID_STATUSTEXT, mav_callback, CALLBACK_PERSISTENT, NULL);
    memset(priv.buf, 0, ROWS*COLS);
    priv.line0 = -ROWS;
#endif
}

static void render(struct widget *w)
{
    struct widget_priv *wp = w->priv;
    struct canvas *ca = &w->ca;
    unsigned char y;
    u8 line0 = max(wp->line0, 0);

    for (y = 0; y < ROWS; y++) {
        draw_str(wp->buf[line0], 0, y*12, ca, 1);
        if (++line0 == ROWS)
            line0 = 0;
    }
}


const struct widget_ops console_widget_ops = {
    .name = "Console",
    .mavname = "CONSOLE",
    .id = WIDGET_CONSOLE_ID,
    .init = init,
    .open = open,
    .render = render,
    .close = close,
};
