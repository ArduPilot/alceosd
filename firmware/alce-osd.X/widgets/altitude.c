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

#define X_SIZE  48
#define Y_SIZE  100

/* range in degrees */
#define RANGE       100
#define MAJOR_TICK  20
#define MINOR_TICK  5
#define X_CENTER    (X_SIZE/2) - 15
#define Y_CENTER    (Y_SIZE/2) - 1


static struct widget_priv {
    float altitude;
    int altitude_i;
    struct canvas ca;
} priv;

const struct widget altitude_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.altitude = mavlink_msg_gps_raw_int_get_alt(msg) / 1000.0;
    priv.altitude_i = (int) priv.altitude;

    schedule_widget(&altitude_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, mav_callback);
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    int i, j, y;
    char buf[10];

    if (init_canvas(ca, 0))
        return 1;


    for(i = -RANGE / 2; i < RANGE / 2; i++) {
        y = Y_CENTER - i;
        j = priv.altitude_i + i;
        if(j % MAJOR_TICK == 0) {
            draw_ohline(X_CENTER + 2, X_CENTER - 4, y, 1, 3, ca);
            sprintf(buf, "%4d", j);
            draw_str(buf, X_CENTER + 10, y-3, ca);
        } else if(j % MINOR_TICK == 0) {
            draw_ohline(X_CENTER + 2, X_CENTER - 2, y, 1, 3, ca);
        }
    }

    draw_frect(X_CENTER + 10, Y_CENTER-3, X_SIZE-2, Y_CENTER + 3, 0, ca);
    sprintf(buf, "%4d", priv.altitude_i);
    draw_str(buf, X_CENTER + 10, Y_CENTER - 2, ca);

    draw_hline(X_CENTER + 10, X_SIZE - 1, Y_CENTER-4, 1, ca);
    draw_hline(X_CENTER + 10, X_SIZE - 1, Y_CENTER+4, 1, ca);
    draw_vline(X_SIZE - 1, Y_CENTER-3, Y_CENTER+3, 1, ca);

    /* draw arrow */
    draw_line(X_CENTER+10, Y_CENTER-4, X_CENTER+10-4, Y_CENTER, 1, ca);
    draw_line(X_CENTER+10, Y_CENTER+4, X_CENTER+10-4, Y_CENTER, 1, ca);

    schedule_canvas(ca); //render_canvas(ca);

    return 0;
}


const struct widget altitude_widget = {
    .name = "Altitude MSL",
    .id = WIDGET_ALTITUDE_ID,
    .init = init,
    .render = render,
};
