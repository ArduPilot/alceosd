#include <stdio.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"
#include "config.h"


#ifdef OSD_SMALL
#define X_SIZE  170
#define Y_SIZE  17
#define X_POS   0
#define Y_POS   OSD_YSIZE - 32 - 28

#else
#define X_SIZE  120
#define Y_SIZE  7


#endif

extern struct alceosd_config config;

static struct widget_priv {
    unsigned int custom_mode, prev_custom_mode;
    struct canvas ca;
} priv;

const struct widget flightmode_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.custom_mode = mavlink_msg_heartbeat_get_custom_mode(msg);

    if (priv.custom_mode == priv.prev_custom_mode)
        return;
    priv.prev_custom_mode = priv.custom_mode;
    schedule_widget(&flightmode_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    priv.prev_custom_mode = 0xff;
    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_HEARTBEAT, mav_callback);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    char mode[17];
    unsigned int cust_mode;

    if (init_canvas(ca, 0))
        return 1;

    cust_mode = priv.custom_mode;
    if (config.vehicle == APM_COPTER)
        cust_mode += 100;

    switch (cust_mode) {
    case PLANE_MODE_MANUAL:
        strcpy(mode, "manual");
        break;
    case PLANE_MODE_CIRCLE:
    case COPTER_MODE_CIRCLE:
        strcpy(mode, "circle");
        break;
    case PLANE_MODE_STABILIZE:
    case COPTER_MODE_STABILIZE:
        strcpy(mode, "stabilize");
        break;
    case PLANE_MODE_TRAINING:
        strcpy(mode, "training");
        break;
    case PLANE_MODE_ACRO:
    case COPTER_MODE_ACRO:
        strcpy(mode, "acro");
        break;
    case PLANE_MODE_FBWA:
        strcpy(mode, "fly-by-wire a");
        break;
    case PLANE_MODE_FBWB:
        strcpy(mode, "fly-by-wire b");
        break;
    case PLANE_MODE_CRUISE:
        strcpy(mode, "cruise");
        break;
    case PLANE_MODE_AUTOTUNE:
    case COPTER_MODE_AUTOTUNE:
        strcpy(mode, "auto tune");
        break;
    case PLANE_MODE_AUTO:
    case COPTER_MODE_AUTO:
        strcpy(mode, "auto");
        break;
    case PLANE_MODE_RTL:
    case COPTER_MODE_RTL:
        strcpy(mode, "return to launch");
        break;
    case PLANE_MODE_LOITER:
    case COPTER_MODE_LOITER:
        strcpy(mode, "loiter");
        break;
    case PLANE_MODE_INIT:
        strcpy(mode, "initializing");
        break;
    case PLANE_MODE_GUIDED:
    case COPTER_MODE_GUIDED:
        strcpy(mode, "guided");
        break;
    case COPTER_MODE_ALTHOLD:
        strcpy(mode, "altitude hold");
        break;
    case COPTER_MODE_LAND:
        strcpy(mode, "land");
        break;
    case COPTER_MODE_OF_LOITER:
        strcpy(mode, "of loiter");
        break;
    case COPTER_MODE_DRIFT:
        strcpy(mode, "drift");
        break;
    case COPTER_MODE_SPORT:
        strcpy(mode, "sport");
        break;
    case COPTER_MODE_FLIP:
        strcpy(mode, "flip");
        break;
    case COPTER_MODE_POSHOLD:
        strcpy(mode, "position hold");
        break;
    default:
        strcpy(mode, "unknown mode");
        break;
    }

#ifdef OSD_SMALL
    draw_str3(mode, 0, 0, ca);
#else
    draw_str(mode, 0, 0, ca);
#endif

    schedule_canvas(ca);
    return 0;
}


const struct widget flightmode_widget = {
    .name = "Flight mode",
    .id = WIDGET_FLIGHT_MODE_ID,
    .init = init,
    .render = render,
};
