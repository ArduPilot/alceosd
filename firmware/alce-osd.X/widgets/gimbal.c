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

#define GIMBAL_SYSID 71
#define GIMBAL_READY 1
#define GIMBAL_BLINK 2

#define X_SIZE  48
#define Y_SIZE  48

static struct gimbal_priv {
    unsigned int heading;
    unsigned int yaw0_heading;
    unsigned char state;
} g_priv;

struct widget_priv {
    int yaw;
};

static void mav_callback_vfr_hud(mavlink_message_t *msg, void *d)
{
    g_priv.heading = (int) mavlink_msg_vfr_hud_get_heading(msg);
    if ((g_priv.state & GIMBAL_READY) == 0)
        g_priv.yaw0_heading = g_priv.heading;
}

static void mav_callback_gimbal_hb(mavlink_message_t *msg, void *d)
{
    unsigned char s = mavlink_msg_heartbeat_get_system_status(msg);
    if (s > MAV_STATE_CALIBRATING) {
        if ((g_priv.state & GIMBAL_READY) == 0) {
            g_priv.state |= GIMBAL_READY;
            console_printf("gimbal: ready status=%d\n", s);
        }
    } else {
        g_priv.state &= ~GIMBAL_READY;
        g_priv.state ^= GIMBAL_BLINK;
    }
}

static void mav_callback_gimbal_att(mavlink_message_t *msg, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    float yaw_f;

    yaw_f = mavlink_msg_attitude_get_yaw(msg);
    priv->yaw = (int) RAD2DEG(yaw_f);

    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->yaw = 0;
    
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;

    add_mavlink_callback_sysid(GIMBAL_SYSID, MAVLINK_MSG_ID_ATTITUDE, mav_callback_gimbal_att, CALLBACK_WIDGET, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    struct point uav_points[4] = { {0, 0}, {6, 8}, {0, -8}, {-6, 8} };
    struct polygon uav = {
        .len = 4,
        .points = uav_points,
    };
    struct point camera_points[8] = { {-5, 0}, {-5, 6}, {-2, 6}, {-5, 10},
                                      {5, 10}, {2, 6}, {5, 6}, {5, 0} };
    struct polygon camera = {
        .len = 8,
        .points = camera_points,
    };

    move_polygon(&uav, ca->width >> 1, ca->height >> 1);
    draw_polygon(&uav, 3, ca);
    move_polygon(&uav, -1, -1);
    draw_polygon(&uav, 1, ca);

    if (g_priv.state != 0) {
        move_polygon(&camera, 0, 11);
        transform_polygon(&camera, ca->width >> 1, ca->height >> 1, (g_priv.yaw0_heading - g_priv.heading) + priv->yaw - 180);
        draw_polygon(&camera, 3, ca);
        move_polygon(&camera, -1, -1);
        draw_polygon(&camera, 1, ca);
        
    }
}

void init(void)
{
    g_priv.state = 0;
    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback_vfr_hud, CALLBACK_PERSISTENT, NULL);
    add_mavlink_callback_sysid(GIMBAL_SYSID, MAVLINK_MSG_ID_HEARTBEAT, mav_callback_gimbal_hb, CALLBACK_PERSISTENT, NULL);
}

const struct widget_ops gimbal_widget_ops = {
    .name = "Gimbal",
    .mavname = "GIMBAL",
    .id = WIDGET_GIMBAL_ID,
    .init = init,
    .open = open,
    .render = render,
    .close = NULL,
};
