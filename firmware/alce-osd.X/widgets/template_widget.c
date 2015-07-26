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


/* Creating widgets:
 *
 * 1) Make a copy of this file and give it a "<name>".
 * 2) Add an ID in widgets.h for your new widget call it "WIDGET_<NAME>_ID"
 * 3) Change the reference to template_widget_ops to "<name>_widget_ops" at
 *    the bottom of this file. Take the chance to give it a proper name an ID.
 * 4) Add your widget_ops to widgets.c:
 *          extern const struct widget <name>_widget_ops;
 *    And add it to the list of all widgets:
 *          all_widgets[] = {
 *              ...
 *              &<name>_widget_ops,
 *              NULL,
 *          }
 *
 * The widget will be now available in the config menus to add in any tab.
 * 
 */


#define X_SIZE  64
#define Y_SIZE  16


/* widget private variables */
struct widget_priv {
    unsigned char var1;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    /*
       do stuff in here with the mavlink message data
     */

    priv->var1++;
    
    /* draw the widget (as soon as possible) */
    schedule_widget(w);
}

static void timer_callback(struct timer *t, void *d)
{
    struct widget *w = (struct widget*) d;
    struct widget_priv *priv = (struct widget_priv*) w->priv;

    /* do stuff if needed (read adc, digital i/o, ... */

    priv->var1--;

    schedule_widget(w);
}

/* called when the widget is shown on the screen */
/* should return 0 if the init was successeful */
static int open(struct widget *w)
{
    struct widget_priv *priv;

    /* allocate space for private variables */
    /* may not be needed -> check home_info.c */
    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    /* init private variables */
    priv->var1 = 0;

    /* set the widget size */
    /* this can be left to be setup by the user */
    /* using the config menus if needed */
    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;

    /* create a callback that will trigger when a sepecific message ID arrrives */
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET, w);

    /* or/and trigger the widget rendering with a timer */
    /* in this case with a refresh rate of 500 msec */
    add_timer(TIMER_WIDGET, 5, timer_callback, w);

    return 0;
}

/* renders the widget on its canvas */
static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[10];

    /* do stuff */
    sprintf(buf, "AlceOSD var1=%d", priv->var1);
    draw_str(buf, 0, 0, ca, 1);
}

/* called each time the widget is removed from screen - changing tabs */
static void close(struct widget *w)
{
    /* uninit stuff here */
}

/* called only once on system startup */
static void init(void)
{
    /* init stuff here */
}


/* dummy define */
/* the ID should be assigned in the widgets.h file and removed from here */
#define WIDGET_TEMPLATE_ID -1
/* REMEMBER TO REMOVE AFTER CREATED IN the "widgets.h" file */

const struct widget_ops template_widget_ops = {
    .name = "Template widget",
    .mavname = "TPLT",
    .id = WIDGET_TEMPLATE_ID,
    .init = init,
    .open = open,
    .render = render,
    .close = close,
};
