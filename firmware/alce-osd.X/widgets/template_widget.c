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
 * 2) Change all references to template_widget to "<name>_widget"
 * 3) Add an ID in widgets.h for your new widget call it "WIDGET_<NAME>_ID"
 * 4) Modify the struct at the end of this file: give it a proper name an ID
 * 4) Add your widget to widgets.c:
 *          extern struct widget <name>_widget;
 *    And add it to the list of active widgets:
 *          all_widgets[] = {
 *              ...
 *              &<name>_widget,
 *              NULL,
 *          }
 *
 * The widget will be now available in the config menus to add in any tab.
 * 
 */


#define X_SIZE  64
#define Y_SIZE  16


/* widget private variables */
static struct widget_priv {
    unsigned char var1;
    struct canvas ca;
} priv;


/* rename "template" to widget name */
const struct widget template_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    /*
       do stuff in here with the mavlink message data
     */

    priv.var1++;
    
    /* draw the widget (as soon as possible) */
    schedule_widget(&template_widget);
}

static void timer_callback(struct timer *t, void *d)
{
    /* do stuff if needed (read adc, digital i/o, ... */

    priv.var1--;

    schedule_widget(&template_widget);
}

/* called once - init the widget */
static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    /* init private variables */
    priv.var1 = 0;

    /* init the widget drawing area and position on screen */
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);

    /* create a callback that will trigger when a sepecific message ID arrrives */
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET);

    /* or/and trigger the widget rendering with a timer */
    /* in this case with a refresh rate of 200 msec */
    add_timer(TIMER_WIDGET, 2, timer_callback, NULL);
}


/* renders the widget on its private canvas */
static int render(void)
{
    struct canvas *ca = &priv.ca;
    char buf[10];

    /* init the canvas */
    /* if the canvas is still in the rendering fifo come back later */
    if (init_canvas(ca, 0))
        return 1;

    /* do stuff */
    sprintf(buf, "AlceOSD");
    draw_str(buf, 0, 0, ca, 1);

    /* send the canvas to the rendering fifo */
    schedule_canvas(ca);
    return 0;
}

/* dummy define */
/* the ID should be assigned in the widgets.h file and removed from here */
#define WIDGET_TEMPLATE_ID -1
/* REMEMBER TO REMOVE AFTER CREATED IN the "widgets.h" file */


const struct widget template_widget = {
    .name = "Template widget",
    .id = WIDGET_TEMPLATE_ID,
    .init = init,
    .render = render,
};
