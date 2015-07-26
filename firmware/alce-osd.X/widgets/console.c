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


#define ROWS    (15)
#define COLS    (40)

#define X_SIZE  (8*COLS)
#define Y_SIZE  (8*ROWS)


static struct widget *console = NULL;

struct widget_priv {
    unsigned char x, y;
    char buf[ROWS][COLS];
};


static inline void print_chr(char chr)
{
    struct widget_priv *wp = console->priv;
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
    if (console == NULL)
        return;

    while (len-- != 0)
        print_chr(*(str++));
    schedule_widget(console);
}


void console_print(char *str)
{
    if (console == NULL)
        return;

    while (*str != '\0')
        print_chr(*(str++));
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
    struct widget_priv *priv;

    /* don't allow more than 1 instance */
    if (console != NULL)
        return -2;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;
    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;

    memset(priv->buf, ' ', ROWS*COLS);

    console = w;
    return 0;
}


static void close(struct widget *w)
{
    console = NULL;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned char x, y;

    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++)
            draw_chr(priv->buf[y][x], x*8, y*8, ca, 1);
    }
}


const struct widget_ops console_widget_ops = {
    .name = "Console",
    .mavname = "CONSOLE",
    .id = WIDGET_CONSOLE_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = close,
};
