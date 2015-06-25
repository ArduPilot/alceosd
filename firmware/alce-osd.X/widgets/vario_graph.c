#include <stdio.h>
#include <math.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  80
#define Y_SIZE  80

#ifndef OSD_SMALL
#undef Y_SIZE
#define Y_SIZE  50
#endif


#define CLIMBRATE_EMA 8
#define ALPHA (1.0/CLIMBRATE_EMA)
#define SCALE 5

static struct widget_priv {
    float climb;
    int avg;
    int hist[X_SIZE-1];
    int y;
    struct canvas ca;
} priv;

const struct widget vario_graph_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.climb = mavlink_msg_vfr_hud_get_climb(msg) * 60.0;
    priv.avg = priv.avg - (int) (((float) priv.avg - priv.climb) * ALPHA);

    priv.y = -(priv.avg / SCALE) + (Y_SIZE/2)-1;
    if (priv.y > Y_SIZE-1)
        priv.y = Y_SIZE-1;
    else if (priv.y < 0)
        priv.y = 0;

    schedule_widget(&vario_graph_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    unsigned char i;
    for (i = 0; i < X_SIZE-1; i++)
        priv.hist[i] = Y_SIZE/2 - 1;
    priv.y = Y_SIZE/2 - 1;


    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);
}

static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned char i;
    char buf[6];

    if (init_canvas(ca, 0))
        return 1;

    for (i = 0; i < X_SIZE-2; i++)
        priv.hist[i] = priv.hist[i+1];
    priv.hist[X_SIZE-2] = priv.y;

    draw_vline(X_SIZE-1, 0, Y_SIZE-1, 1, ca);

    for (i = 0; i < X_SIZE-1; i++) {
        draw_vline(i, Y_SIZE/2-1, priv.hist[i], 2, ca);
        set_pixel(i, priv.hist[i], 1, ca);
    }

    sprintf(buf, "%4d", priv.avg);
#ifdef OSD_SMALL
    draw_str3(buf, X_SIZE - 12*4, 2, ca);
#else
    draw_str(buf, X_SIZE - 6*5, 2, ca, 0);
#endif


    schedule_canvas(ca);
    return 0;
}


const struct widget vario_graph_widget = {
    .name = "Variometer chart",
    .id = WIDGET_VARIOMETER_ID,
    .init = init,
    .render = render,
};
