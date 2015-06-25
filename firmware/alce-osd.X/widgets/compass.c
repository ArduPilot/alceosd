#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  100
#define Y_SIZE  30

/* range in degrees */
#define RANGE       90
#define MAJOR_TICK  45
#define MINOR_TICK  15
#define X_CENTER    (X_SIZE/2) - 2
#define Y_CENTER    (Y_SIZE/2) - 1

static struct widget_priv {
    int heading;
    char heading_s[4];
    struct canvas ca;
} priv;

const struct widget compass_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.heading = mavlink_msg_vfr_hud_get_heading(msg);
    priv.heading_s[0] = '0' + (priv.heading / 100);
    priv.heading_s[1] = '0' + ((priv.heading % 100) / 10);
    priv.heading_s[2] = '0' + (priv.heading % 10);

    schedule_widget(&compass_widget);
}

static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.heading_s[3] = '\0';
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
}

static int render(void)
{
    struct canvas *ca = &priv.ca;
    const char cardinals[] = {'N', 'E', 'S', 'W'};
    int i, j, x;

    if (init_canvas(ca, 0))
        return 1;

    draw_str(priv.heading_s, X_CENTER-12, 0, ca, 1);
    set_pixel(X_CENTER, Y_CENTER-1, 1, ca);
    draw_hline(X_CENTER-1, X_CENTER+1, Y_CENTER-2, 1, ca);
    draw_hline(X_CENTER-2, X_CENTER+2, Y_CENTER-3, 1, ca);
    for(i = -RANGE / 2; i < RANGE / 2; i++) {
        x = X_CENTER + i;
        j = (priv.heading + i + 360) % 360;
        if (j == 0 || j == 90 || j == 180 || j == 270) {
            draw_vline(x,   Y_CENTER, Y_CENTER + 5, 1, ca);
            draw_vline(x-1, Y_CENTER, Y_CENTER + 5, 3, ca);
            draw_vline(x+1, Y_CENTER, Y_CENTER + 5, 3, ca);
            draw_chr(cardinals[j / 90], x-2, Y_CENTER + 6, ca, 0);
        } else if(j % MAJOR_TICK == 0) {
            draw_vline(x,   Y_CENTER, Y_CENTER + 5, 1, ca);
            draw_vline(x-1, Y_CENTER, Y_CENTER + 5, 3, ca);
            draw_vline(x+1, Y_CENTER, Y_CENTER + 5, 3, ca);
        } else if(j % MINOR_TICK == 0) {
            draw_vline(x,   Y_CENTER, Y_CENTER + 3, 1, ca);
            draw_vline(x-1, Y_CENTER, Y_CENTER + 3, 3, ca);
            draw_vline(x+1, Y_CENTER, Y_CENTER + 3, 3, ca);
        }
    }

    schedule_canvas(ca);
    return 0;
}


const struct widget compass_widget = {
    .name = "Compass",
    .id = WIDGET_COMPASS_ID,
    .init = init,
    .render = render,
};
