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


#define X_SIZE  128
#define Y_SIZE  20

struct widget_priv {
    s16 x, i;
    u8 blink;
    u8 wait;
    u16 active_alarms;
    u16 display[MAX_FLIGHT_ALARMS];
};

#define BLINK_RATE          200 /* milliseconds */
#define REFRESH_RATE        50 /* milliseconds */
#define MIN_DISPLAY_TIME    2  /* seconds */
#define SCROLL_STEP         3
#define END_STOP_TIME       10

#define DISPLAY_TIME        (MIN_DISPLAY_TIME * 1000 / REFRESH_RATE)
#define BLINK_CNT           (BLINK_RATE / REFRESH_RATE)

static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = (struct widget*) d;
    struct widget_priv *priv = (struct widget_priv*) w->priv;
    struct flight_stats *f = get_flight_stats();
    u8 id;

    for (id = 0; id < MAX_FLIGHT_ALARMS; id++) {
        if (f->fl_alarms[id].ctrl & FL_ALARM_ACTIVE) {
            priv->active_alarms |= (1 << id);
            priv->display[id] = 0;
        } else if (priv->display[id] > (DISPLAY_TIME-1)) {
            priv->active_alarms &= ~(1 << id);
        } else {
            priv->display[id]++;
        }
    }
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    memset(priv, 0, sizeof(struct widget_priv));
    priv->i = -SCROLL_STEP;
    
    /* set the widget size */
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;

    add_timer(TIMER_WIDGET, 50, timer_callback, w);
    return 0;
}

#define MAX_WARN_LINE_SIZE  200
static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    struct flight_stats *f = get_flight_stats();
    char buf[MAX_WARN_LINE_SIZE];
    int ptr = 0;
    u8 id;
    u16 z;

    if (priv->active_alarms != 0) {
        if (priv->blink < BLINK_CNT) {
            draw_rect(0, 0, X_SIZE-1, Y_SIZE-1, 3, ca);
            draw_rect(1, 1, X_SIZE-2, Y_SIZE-2, 1, ca);
            draw_frect(2, 2, X_SIZE-3, Y_SIZE-3, 2, ca);
        } else {
            draw_rect(0, 0, X_SIZE-1, Y_SIZE-1, 3, ca);
            draw_rect(1, 1, X_SIZE-2, Y_SIZE-2, 1, ca);
            draw_frect(2, 2, X_SIZE-3, Y_SIZE-3, 3, ca);
        }

        buf[ptr++] = ' ';
        for (id = 0; id < MAX_FLIGHT_ALARMS; id++) {
            if (priv->active_alarms & (1 << id)) {
                switch (f->fl_alarms[id].cfg->props.mode) {
                    default:
                    case FL_ALARM_MODE_LOW:
                        ptr += snprintf(buf + ptr, MAX_WARN_LINE_SIZE - ptr, "LOW ");
                        break;
                    case FL_ALARM_MODE_HIGH:
                        ptr += snprintf(buf + ptr, MAX_WARN_LINE_SIZE - ptr, "HIGH ");
                        break;
                }
                ptr += snprintf(buf + ptr, MAX_WARN_LINE_SIZE - ptr,
                        "%s | ", f->fl_alarms[id].info->name);
            }
        }
        if (ptr > 0)
            buf[ptr-2] = '\0';

        z = (int) get_str_width(buf, get_font(1));
        if (z > X_SIZE) {
            draw_str(buf, priv->x, 2, ca, 1);
            
            /* scroll logic */
            if (priv->x > 0) {
                if (++priv->wait > END_STOP_TIME) {
                    priv->i = -SCROLL_STEP;
                    priv->wait = 0;
                } else {
                    priv->i = 0;
                }
            } else if (priv->x < (X_SIZE - z)) {
                if (++priv->wait > END_STOP_TIME) {
                    priv->i = SCROLL_STEP;
                    priv->wait = 0;
                } else {
                    priv->i = 0;
                }
            }
            priv->x += priv->i;
        } else {
            draw_jstr(buf, X_SIZE/2, 2, JUST_HCENTER | JUST_TOP, ca, 1);
        }
        
        if (++priv->blink > (BLINK_CNT * 2 - 1))
            priv->blink = 0;
    } else {
        priv->blink = 0;
        priv->x = 0;
    }
}

const struct widget_ops alarms_widget_ops = {
    .name = "Flight Alarms",
    .mavname = "ALARMS",
    .id = WIDGET_ALARMS_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
