#include <stdio.h>
#include <string.h>

#include "config.h"

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  64
#define Y_SIZE  16

#define RSSI_MAX 255

static struct widget_priv {
    unsigned char rssi, last_rssi;
    struct canvas ca;
} priv;

const struct widget rssi_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.rssi = mavlink_msg_rc_channels_raw_get_rssi(msg);

    if (priv.rssi ==  priv.last_rssi)
        return;

    schedule_widget(&rssi_widget);
    priv.last_rssi = priv.rssi;
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.last_rssi = 0xff;

    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned char i, x;
    char buf[5];

    if (init_canvas(ca, 0))
        return 1;

    x = 0;
    for (i = 0; i < (5 * priv.rssi)/(RSSI_MAX-RSSI_MAX/5); i++) {
        draw_vline(x, Y_SIZE-1 - i*3, Y_SIZE-1, 3, ca);
        draw_vline(x+1, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        draw_vline(x+2, Y_SIZE-1 - i*3, Y_SIZE-1, 1, ca);
        x += 4;
    }

    sprintf(buf, "%3d", (priv.rssi * 100) / RSSI_MAX);
    draw_str(buf, 25, 4, ca, 2);

    schedule_canvas(ca);
    return 0;
}


const struct widget rssi_widget = {
    .name = "RSSI",
    .id = WIDGET_RSSI_ID,
    .init = init,
    .render = render,
};
