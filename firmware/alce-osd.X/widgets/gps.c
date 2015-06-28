#include <stdio.h>
#include <math.h>
#include <string.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"



#define X_SIZE  60 + 32 + 20 + 32
#define Y_SIZE  18

#define SAT_X   60
#define SAT_Y   5

#ifdef GPS_SAT_ICON
const char sat_ico[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,3,3,0,3,3,3,3,3,3,3,0,0,0,0,0,
    0,0,0,0,3,3,3,3,1,1,1,1,1,1,3,3,0,0,0,0,
    0,0,0,0,3,3,3,3,3,3,3,3,3,3,1,3,3,0,0,0,
    0,0,0,0,3,3,3,3,1,1,1,1,1,1,1,3,3,3,0,0,
    0,0,0,3,1,3,1,3,3,1,1,1,1,1,1,1,3,3,0,0,
    0,0,3,1,1,3,1,1,3,3,1,1,1,1,1,3,3,3,0,0,
    0,3,1,1,1,3,1,1,1,3,3,1,1,1,1,3,3,3,0,0,
    0,3,1,1,1,3,1,1,1,1,3,3,1,1,1,3,3,3,0,0,
    0,3,1,1,1,3,1,1,1,1,1,3,1,1,3,3,3,3,0,0,
    0,3,1,1,1,3,1,1,1,1,1,1,1,3,3,3,3,3,0,0,
    0,3,3,1,1,1,1,1,1,1,1,3,3,3,3,3,3,0,0,0,
    0,3,3,3,1,1,1,1,1,1,3,3,3,3,3,3,3,0,0,0,
    0,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,
    0,0,0,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,
    0,0,0,0,3,3,3,3,3,3,3,3,3,3,0,3,0,0,0,0,
    0,0,0,0,0,0,3,3,3,3,3,0,0,0,3,3,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,
    0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,0,0,
    0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,
    0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,3,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif

static struct widget_priv {
    float gps_lat, gps_lon, gps_eph;
    unsigned int gps_cog;
    unsigned char gps_nrsats, gps_fix_type;

    struct canvas ca;
} priv;

const struct widget gps_info_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.gps_lat = mavlink_msg_gps_raw_int_get_lat(msg) / 10000000.0;
    priv.gps_lon = mavlink_msg_gps_raw_int_get_lon(msg) / 10000000.0;
    priv.gps_fix_type = mavlink_msg_gps_raw_int_get_fix_type(msg);
    priv.gps_nrsats = mavlink_msg_gps_raw_int_get_satellites_visible(msg);
    //priv.gps_cog = mavlink_msg_gps_raw_int_get_cog(msg);
    priv.gps_eph = (float) mavlink_msg_gps_raw_int_get_eph(msg) / 100.0;

    schedule_widget(&gps_info_widget);
}


static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.ca;

    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
    add_mavlink_callback(MAVLINK_MSG_ID_GPS_RAW_INT, mav_callback, CALLBACK_WIDGET);
}


static int render(void)
{
    struct canvas *ca = &priv.ca;
    char buf[20], i, j;

    if (init_canvas(ca, 0))
        return 1;

    sprintf(buf, "%10.6f", priv.gps_lat);
    draw_str(buf, 0, 0, ca, 0);

    sprintf(buf, "%10.6f", priv.gps_lon);
    draw_str(buf, 0, 9, ca, 0);

#ifdef GPS_SAT_ICON
    for (i = 0; i < 22; i++)
        for (j = 0; j < 20; j++)
            set_pixel(SAT_X+j, SAT_Y+i, sat_ico[i * 20 + j], ca);
#endif

    sprintf(buf, "%2d", priv.gps_nrsats);
    draw_str(buf, SAT_X + 20 + 1, 0, ca, 0);

    buf[1] = 'D';
    switch (priv.gps_fix_type) {
    default:
    case 0:
    case 1:
        buf[0] = ' ';
        buf[1] = ' ';
    break;
    case 2:
        buf[0] = '2';
        break;
    case 3:
        buf[0] = '3';
        break;
    }
    draw_chr(buf[0], SAT_X + 20 + 1, 9, ca, 0);
    draw_chr(buf[1], SAT_X + 20 + 1 + 6, 9, ca, 0);

    strcpy(buf, "HDP");
    draw_str(buf, SAT_X + 20 + 1 + 18, 0, ca, 0);
    sprintf(buf, "%2.1f", priv.gps_eph);
    draw_str(buf, SAT_X + 20 + 1 + 18, 9, ca, 0);

    schedule_canvas(ca);
    return 0;
}


const struct widget gps_info_widget = {
    .name = "GPS stats",
    .id = WIDGET_GPS_INFO_ID,
    .init = init,
    .render = render,
};
