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

#define ROWS    (15)
#define COLS    (40)

#define X_SIZE  (8*COLS)
#define Y_SIZE  (8*ROWS)

static struct widget *console = NULL;

struct widget_priv {
    unsigned char x, y;
    char buf[ROWS][COLS];
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
    unsigned char i, j;

    if (chr == '\n') {
        wp->x = 0;
        wp->y++;
    } else {
        if (wp->x == COLS) {
            wp->x = 0;
            wp->y++;
        }
        if (wp->y == ROWS) {
            for (i = 1; i < ROWS; i++) {
                for (j = 0; j < COLS; j++)
                    wp->buf[i-1][j] = wp->buf[i][j];
            }
            for (j = 0; j < COLS; j++)
                wp->buf[ROWS-1][j] = ' ';
            wp->y --;
        }
        wp->buf[wp->y][wp->x] = chr;
        wp->x++;
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
    memset(priv->buf, ' ', ROWS*COLS);
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
    memset(priv.buf, ' ', ROWS*COLS);
#endif
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[COLS+1];
    unsigned char y;

    
    for (y = 0; y < ROWS; y++) {
        memcpy(buf, priv->buf[y], COLS);
        buf[COLS] = '\0';
        draw_str(buf, 0, y*12, ca, 1);
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
