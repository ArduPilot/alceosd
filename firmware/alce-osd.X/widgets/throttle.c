#include <stdio.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#ifdef OSD_SMALL
#define X_SIZE  12
#define Y_SIZE  104
#define X_POS   0
#define Y_POS   50

#else
#define X_SIZE  8
#define Y_SIZE  45

#endif


static struct widget_priv {
    unsigned char throttle, last_throttle;
    struct canvas ca;
} priv;

const struct widget throttle_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.throttle = (unsigned char) mavlink_msg_vfr_hud_get_throttle(msg);

    if (priv.throttle ==  priv.last_throttle)
        return;

    schedule_widget(&throttle_widget);
    priv.last_throttle = priv.throttle;
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.last_throttle = 0xff;
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned v = priv.throttle * (Y_SIZE-4) / 100;
    
    if (init_canvas(ca, 0))
        return 1;

    draw_rect(0, 0, X_SIZE-1, Y_SIZE-1, 3, ca);
    draw_rect(1, 1, X_SIZE-2, Y_SIZE-2, 1, ca);
    if (priv.throttle > 0) {
        draw_frect(2, Y_SIZE-3 - v, X_SIZE-3, Y_SIZE-3, 1, ca);
    }

    schedule_canvas(ca);
    return 0;
}


const struct widget throttle_widget = {
    .name = "Throttle",
    .id = WIDGET_THROTTLE_ID,
    .init = init,
    .render = render,
};
