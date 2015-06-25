#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  40
#define Y_SIZE  100

#define RANGE       100
#define MAJOR_TICK  20
#define MINOR_TICK  5
#define X_CENTER    (X_SIZE/2) + 12
#define Y_CENTER    (Y_SIZE/2) - 1

static struct widget_priv {
    float speed;
    int speed_i;
    struct canvas ca;
} priv;

const struct widget speed_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.speed = mavlink_msg_vfr_hud_get_airspeed(msg) * 3600 / 1000.0;
    priv.speed_i = (int) priv.speed;

    schedule_widget(&speed_widget);
}

static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);
}

static int render(void)
{
    struct canvas *ca = &priv.ca;
    int i, j, y;
    char buf[10];
    
    if (init_canvas(ca, 0))
        return 1;

    for(i = -RANGE / 2; i < (RANGE / 2); i++) {
        y = Y_CENTER - i;
        j = priv.speed_i + i;
        if(j < 0)
            continue;
        if(j % MAJOR_TICK == 0) {
            sprintf(buf, "%3d", j);
            draw_str(buf, 2, y - 2, ca, 0);
            draw_ohline(X_CENTER - 2, X_CENTER + 4, y, 1, 3, ca);
        } else if(j % MINOR_TICK == 0) {
            draw_ohline(X_CENTER - 2, X_CENTER + 2, y, 1, 3, ca);
        }
    }

    draw_frect(1, Y_CENTER-3, X_CENTER - 10, Y_CENTER + 3, 0, ca);
    sprintf(buf, "%3d", (int) priv.speed_i);
    draw_str(buf, 2, Y_CENTER-2, ca, 0);

    draw_hline(0, X_CENTER - 10, Y_CENTER - 4, 1, ca);
    draw_hline(0, X_CENTER - 10, Y_CENTER + 4, 1, ca);
    draw_vline(0, Y_CENTER - 3 , Y_CENTER + 3, 1, ca);

    draw_line(X_CENTER-10, Y_CENTER-4, X_CENTER-10+4, Y_CENTER, 1, ca);
    draw_line(X_CENTER-10, Y_CENTER+4, X_CENTER-10+4, Y_CENTER, 1, ca);

    schedule_canvas(ca);
    return 0;
}


const struct widget speed_widget = {
    .name = "Air speed",
    .id = WIDGET_SPEED_ID,
    .init = init,
    .render = render,
};
