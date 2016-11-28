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


#define X_SHIFT 32

#define X_SIZE  (192 - X_SHIFT)
#define Y_SIZE  154

#define RANGE       Y_SIZE
#define SCALE       5
#define MINOR_TICK  5
#define MAJOR_TICK  10
#define Y_CENTER    Y_SIZE/2
#define X_CENTER    (X_SIZE/2 - (X_SHIFT/2))

#define ROLL_RANGE  90
#define MINOR_ROLL_TICK  5
#define MAJOR_ROLL_TICK  15


struct widget_priv {
    int pitch_deg, roll_deg;
    float cos_roll, sin_roll;
    int heading;
};

static void pre_render(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    
    mavlink_attitude_t *att = mavdata_get(MAVLINK_MSG_ID_ATTITUDE);
    mavlink_vfr_hud_t *hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
    
    priv->pitch_deg = RAD2DEG(att->pitch * SCALE);
    priv->roll_deg  = RAD2DEG(att->roll);
    priv->cos_roll = cos(att->roll);
    priv->sin_roll = -1 * sin(att->roll);
    priv->heading = hud->heading;
    
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->cos_roll = cos(0);
    priv->sin_roll = sin(0);
        
    w->ca.width = X_SIZE;
    w->ca.height = Y_SIZE;
        
    add_timer(TIMER_WIDGET, 50, pre_render, w);
    return 0;
}

static void render_0(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    int y, i, j;
    int x0, x1, y0, y1, offset, cx, cy;
    unsigned char size, gap;
    char buf[10];

    for (i = -RANGE/2; i <= RANGE/2; i++) {
        y = Y_CENTER - i;
        //j = priv->pitch_deg*SCALE + i;
        j = priv->pitch_deg + i;

        if (j % (MINOR_TICK*SCALE) == 0) {
            if (j == 0) {
                size = 40;
                gap = 10;
            } else {
                if (j % (MAJOR_TICK*SCALE) == 0)
                    size = 20;
                else
                    size = 10;
                gap = 10;
            }

            cx = X_CENTER + (int) (i * priv->sin_roll);
            cy = y + i - (int) (i * priv->cos_roll);

            if ((j != 0) && (j % (MAJOR_TICK*SCALE) == 0)) {
                sprintf(buf, "%d", j / SCALE);
                draw_jstr(buf, cx, cy, JUST_HCENTER | JUST_VCENTER, ca, 0);
            }
            
            offset = (int) gap * priv->cos_roll;
            x0 = cx + offset;
            offset = (int) size * priv->cos_roll;
            x1 = x0 + offset;
            
            offset = (int) gap * priv->sin_roll;
            y0 = cy + offset;
            offset = (int) size * priv->sin_roll;
            y1 = y0 + offset;
            
            if (j == 0) {
                draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
                //draw_line(x0, y0-1, x1, y1-1, 1, ca);
            }
            draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
            draw_line(x0, y0, x1, y1, 1, ca);

            offset = (int) gap * priv->cos_roll;
            x0 = cx - offset;
            offset = (int) size * priv->cos_roll;
            x1 = x0 - offset;

            offset = (int) gap * priv->sin_roll;
            y0 = cy - offset;
            offset = (int) size * priv->sin_roll;
            y1 = y0 - offset;

            if (j == 0) {
                draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
                //draw_line(x0, y0-1, x1, y1-1, 1, ca);
            }
            draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
            draw_line(x0, y0, x1, y1, 1, ca);

        }
    }

    draw_oline(X_CENTER - 3, Y_CENTER, X_CENTER + 3, Y_CENTER, 1, ca);
    draw_vline(X_CENTER, Y_CENTER - 3, Y_CENTER + 3, 1, ca);
    
    float cos_i, sin_i;

    for (i = -ROLL_RANGE/2; i <= ROLL_RANGE/2; i++) {
        y = Y_CENTER - i;
        //j = priv->pitch_deg*SCALE + i;
        //j = priv->roll_deg + i;

        if (i % (MINOR_ROLL_TICK) == 0) {
            gap = 70;
            if (i == 0) {
                size = 10;
            } else {
                if (i % (MAJOR_ROLL_TICK) == 0)
                    size = 10;
                else
                    size = 5;
            }

            cos_i = cos(DEG2RAD(i));
            sin_i = sin(DEG2RAD(i));
            
            cx = X_CENTER;// + (int) (gap * cos_i);
            cy = Y_CENTER;// - (int) (gap * sin_i);


            offset = (int) gap * cos_i;
            x0 = cx + offset;
            offset = (int) size * cos_i;
            x1 = x0 + offset;


            offset = (int) gap * sin_i;
            y0 = cy - offset;
            offset = (int) size * sin_i;
            y1 = y0 - offset;

            draw_line(x0, y0, x1, y1, 1, ca);

            if ((i != 0) && (i % (MAJOR_ROLL_TICK) == 0)) {
                sprintf(buf, "%d", i);
                draw_jstr(buf, x1, y1, JUST_VCENTER, ca, 0);
            }
        }
    }

    cx = X_CENTER; // + (int) (gap * priv->sin_roll);
    cy = Y_CENTER ; //- (int) (gap * priv->cos_roll);
    size = 10;

    offset = (int) (gap-size) * priv->cos_roll;
    x0 = cx + offset;
    offset = (int) size * priv->cos_roll;
    x1 = x0 + offset;

    offset = (int) (gap-size) * priv->sin_roll;
    y0 = cy + offset;
    offset = (int) size * priv->sin_roll;
    y1 = y0 + offset;

    draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
    draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
    draw_line(x0, y0, x1, y1, 1, ca);
}

static void render_1(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    int x0, x1, y0, y1, offset, cx, cy, x, y, i;

    cx = X_SIZE / 2;
    cy = Y_CENTER;
    
    i = priv->pitch_deg;

    y = cy + (int) i * priv->cos_roll * 0.8;
    x = cx - (int) i * priv->sin_roll * 0.8;

    offset = (int) (X_SIZE/2 - 5) * priv->cos_roll;
    x0 = x - offset; x1 = x + offset;
    offset = (int) (X_SIZE/2 - 5) * priv->sin_roll;
    y0 = y - offset; y1 = y + offset;
    draw_oline(x0, y0, x1, y1, 1, ca);

    i += 10;
    
    y = cy + (int) i * priv->cos_roll * 0.8;
    x = cx - (int) i * priv->sin_roll * 0.8;

    offset = (int) (X_SIZE/2 - 30) * priv->cos_roll;
    x0 = x - offset; x1 = x + offset;
    offset = (int) (X_SIZE/2 - 30) * priv->sin_roll;
    y0 = y - offset; y1 = y + offset;
    draw_oline(x0, y0, x1, y1, 1, ca);

    /* compass */
    for (i = 0; i < 89; i += 15) {
        offset = i + priv->heading;
        x = 70 * sin(DEG2RAD(offset));
        y = 70 * cos(DEG2RAD(offset));
        
        if (i == 0) {
            draw_jstr("S", cx + x, cy + y, JUST_VCENTER | JUST_HCENTER, ca, 2);
            draw_jstr("E", cx + y, cy - x, JUST_VCENTER | JUST_HCENTER, ca, 2);
            draw_jstr("N", cx - x, cy - y, JUST_VCENTER | JUST_HCENTER, ca, 2);
            draw_jstr("W", cx - y, cy + x, JUST_VCENTER | JUST_HCENTER, ca, 2);
        } else {
            set_pixel(cx + x, cy + y, 1, ca);
            set_pixel(cx - x, cy - y, 1, ca);
            set_pixel(cx - y, cy + x, 1, ca);
            set_pixel(cx + y, cy - x, 1, ca);
            draw_circle(cx + x, cy + y, 2, 3, ca);
            draw_circle(cx - x, cy - y, 2, 3, ca);
            draw_circle(cx - y, cy + x, 2, 3, ca);
            draw_circle(cx + y, cy - x, 2, 3, ca);
        }
    }

    struct home_data *home = get_home_data();

    offset = home->direction - 90;
    x = 70 * cos(DEG2RAD(offset));
    y = 70 * sin(DEG2RAD(offset));
    draw_jstr("H", cx + x, cy + y, JUST_VCENTER | JUST_HCENTER, ca, 1);

    draw_circle(cx, cy, 5, 1, ca);
    draw_circle(cx, cy, 6, 3, ca);
}

void render(struct widget *w)
{
    switch (w->cfg->props.mode) {
        default:
        case 0:
            render_0(w);
            break;
        case 1:
            render_1(w);
            break;
    }
}

const struct widget_ops horizon_widget_ops = {
    .name = "Artificial Horizon",
    .mavname = "HORIZON",
    .id = WIDGET_HORIZON_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
