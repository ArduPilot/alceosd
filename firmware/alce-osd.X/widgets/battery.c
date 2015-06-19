#include <stdio.h>
#include <math.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SIZE  64
#define Y_SIZE  45


#define BAT_BAR_X   3
#define BAT_BAR_Y   0
#define BAT_BAR_W   58
#define BAT_BAR_H   10



static struct widget_priv {
    float bat_voltage, bat_current;
    int bat_remaining;
    struct canvas ca;
} priv;

const struct widget bat_info_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.bat_voltage = mavlink_msg_sys_status_get_voltage_battery(msg) / 1000.0;
    priv.bat_current = mavlink_msg_sys_status_get_current_battery(msg) / 100.0;
    priv.bat_remaining = (int) mavlink_msg_sys_status_get_battery_remaining(msg);

    schedule_widget(&bat_info_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    add_mavlink_callback(MAVLINK_MSG_ID_SYS_STATUS, mav_callback, CALLBACK_WIDGET);
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    char buf[10];
    int i;

    if (init_canvas(ca, 0))
        return 1;

    sprintf(buf, "%5.2fv", priv.bat_voltage);
    draw_str3(buf, 0, BAT_BAR_Y + BAT_BAR_H + 2, ca);
    sprintf(buf, "%5.2fa", priv.bat_current);
    draw_str3(buf, 0, BAT_BAR_Y + BAT_BAR_H + 18, ca);

    draw_rect(BAT_BAR_X, BAT_BAR_Y, BAT_BAR_X + BAT_BAR_W, BAT_BAR_Y + BAT_BAR_H, 3, ca);
    draw_rect(BAT_BAR_X+1, BAT_BAR_Y+1, BAT_BAR_X + BAT_BAR_W-1, BAT_BAR_Y + BAT_BAR_H-1, 1, ca);
    draw_vline(BAT_BAR_X - 1, BAT_BAR_Y + 4, BAT_BAR_Y + BAT_BAR_H - 4, 1, ca);
    draw_vline(BAT_BAR_X - 2, BAT_BAR_Y + 4, BAT_BAR_Y + BAT_BAR_H - 4, 1, ca);
    draw_vline(BAT_BAR_X - 3, BAT_BAR_Y + 4, BAT_BAR_Y + BAT_BAR_H - 4, 3, ca);

    for (i = 0; i < (priv.bat_remaining*BAT_BAR_W)/100 - 2; i++) {
        draw_vline(BAT_BAR_X + 2 + i, BAT_BAR_Y + 2, BAT_BAR_Y + BAT_BAR_H - 2, 2, ca);
    }

    sprintf(buf, "%d", priv.bat_remaining);
    draw_str(buf, BAT_BAR_X + BAT_BAR_W/2 - 6, BAT_BAR_Y + 3, ca);

    schedule_canvas(ca);
    return 0;
}


const struct widget bat_info_widget = {
    .name = "Battery info",
    .id = WIDGET_BATTERY_INFO_ID,
    .init = init,
    .render = render,
};
