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


#define X_SIZE  192
#define Y_SIZE  160



struct widget_priv {
    struct point3d ls, uav;
};

static void pre_render(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    
    mavlink_attitude_t *att = mavdata_get(MAVLINK_MSG_ID_ATTITUDE);
    mavlink_vfr_hud_t *hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

        
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;
    
    
    
    add_timer(TIMER_WIDGET, 50, pre_render, w);
    return 0;
}

s16 XX,YY,ZZ = 100;
struct point p;
struct point3d ls, eye;

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    struct home_data *home = get_home_data();
    struct flight_stats *fstats = get_flight_stats();

    mavlink_attitude_t *att = mavdata_get(MAVLINK_MSG_ID_ATTITUDE);
    mavlink_vfr_hud_t *hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    unsigned long d = (unsigned long) home->distance;
    unsigned int r = (w->ca.width/2)-2;
    u8 i;
    
    struct point ils_screen[5];
    struct point ils_points[5] = { {-35, -100}, {-35, 100}, {0, 130}, {35, 100}, {35, -100} };
    struct polygon ils = {
        .len = 5,
        .points = ils_points,
    };

    
    
    float x,y,z;
    
    eye.x = 0; //X_SIZE/2;
    eye.y = 0; //Y_SIZE/2;
    eye.z = 10;

    x = sin(DEG2RAD(home->direction)) * d;
    z = cos(DEG2RAD(home->direction)) * d;
    y = home->altitude;
    
    ls.x = (s32) x;
    ls.y = (s32) y;
    ls.z = (s32) z;
    
    if (ls.z <= 0)
        return;
    
    //ls.x = XX;
    //ls.y = YY;
    //ls.z = ZZ;
    
    eye.z = ZZ;
    
    p.x = (eye.z * (ls.x-eye.x)) / (eye.z + ls.z) + eye.x;
    p.y = (eye.z * (ls.y-eye.y)) / (eye.z + ls.z) + eye.y;
    
    draw_circle(X_SIZE/2 + p.x, Y_SIZE/2 + p.y, 5, 1, ca);

    
    transform_polygon(&ils, x, z,  - (fstats->launch_heading - hud->heading));
    for (i = 0; i < 5; i++) {
        ils_screen[i].x = (eye.z * (ils_points[i].x-eye.x)) / (eye.z + ils_points[i].y) + eye.x;
        ils_screen[i].y = (eye.z * (ls.y-eye.y)) / (eye.z + ils_points[i].y) + eye.y;
    }
    ils.points = ils_screen;
    transform_polygon(&ils, X_SIZE/2, Y_SIZE/2, 0);
    draw_polygon(&ils, 1, ca);

    
}

const struct widget_ops ils_widget_ops = {
    .name = "Imaginary LS",
    .mavname = "ILS",
    .id = WIDGET_ILS_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
