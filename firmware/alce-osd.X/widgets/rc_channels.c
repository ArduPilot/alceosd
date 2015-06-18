#include <stdio.h>
#include <string.h>

#include "config.h"

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  84
#define Y_SIZE  64
#define BAR_SIZE 25


static struct widget_priv {
    unsigned int ch_raw[8];
    struct canvas ca;
} priv;

const struct widget rc_channels_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.ch_raw[0] = mavlink_msg_rc_channels_raw_get_chan1_raw(msg);
    priv.ch_raw[1] = mavlink_msg_rc_channels_raw_get_chan2_raw(msg);
    priv.ch_raw[2] = mavlink_msg_rc_channels_raw_get_chan3_raw(msg);
    priv.ch_raw[3] = mavlink_msg_rc_channels_raw_get_chan4_raw(msg);
    priv.ch_raw[4] = mavlink_msg_rc_channels_raw_get_chan5_raw(msg);
    priv.ch_raw[5] = mavlink_msg_rc_channels_raw_get_chan6_raw(msg);
    priv.ch_raw[6] = mavlink_msg_rc_channels_raw_get_chan7_raw(msg);
    priv.ch_raw[7] = mavlink_msg_rc_channels_raw_get_chan8_raw(msg);

    schedule_widget(&rc_channels_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;
    unsigned int i;

    for (i = 0; i < 8; i++)
        priv.ch_raw[i] = 1500;

    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_RC_CHANNELS_RAW, mav_callback);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    unsigned char i;
    int x;
    char buf[10];

    if (init_canvas(ca, 0))
        return 1;


    for (i = 0; i < 8; i++) {
        sprintf(buf, "CH%u %4d", i+1, priv.ch_raw[i]);
        draw_str(buf, 0, i*8+1, ca);

        x = priv.ch_raw[i] - 1000;
        if (x < 0)
            x = 0;
        else if (x > 1000)
            x = 1000;

        x = (x * BAR_SIZE) / 1000;

        draw_rect(X_SIZE-BAR_SIZE-1,        i*8, X_SIZE-1, i*8+6, 3, ca);
        draw_rect(X_SIZE-BAR_SIZE-1,      i*8+1, X_SIZE-1, i*8+5, 1, ca);
        
        draw_vline(X_SIZE-BAR_SIZE-1+x,   i*8+1, i*8+5, 1, ca);
        draw_vline(X_SIZE-BAR_SIZE-1+x-1, i*8+1, i*8+5, 3, ca);
        draw_vline(X_SIZE-BAR_SIZE-1+x+1, i*8+1, i*8+5, 3, ca);
    }

    schedule_canvas(ca);
    return 0;
}


const struct widget rc_channels_widget = {
    .name = "RC Channels",
    .id = WIDGET_RC_CHANNELS_ID,
    .init = init,
    .render = render,
};
