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


#define X_SIZE  80
#define Y_SIZE  50


#define CLIMBRATE_EMA 8
#define ALPHA (1.0/CLIMBRATE_EMA)
#define SCALE 5

struct widget_priv {
    float climb;
    int avg;
    int hist[X_SIZE-1];
    int y;
};

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;
    
    priv->climb = mavlink_msg_vfr_hud_get_climb(msg) * 60.0;
    priv->avg = priv->avg - (int) (((float) priv->avg - priv->climb) * ALPHA);

    priv->y = -(priv->avg / SCALE) + (Y_SIZE/2)-1;
    if (priv->y > Y_SIZE-1)
        priv->y = Y_SIZE-1;
    else if (priv->y < 0)
        priv->y = 0;

    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;
    unsigned char i;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    for (i = 0; i < X_SIZE-1; i++)
        priv->hist[i] = Y_SIZE/2 - 1;
    priv->y = Y_SIZE/2 - 1;

    w->cfg->w = X_SIZE;
    w->cfg->h = Y_SIZE;

    add_mavlink_callback(MAVLINK_MSG_ID_VFR_HUD, mav_callback, CALLBACK_WIDGET, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned char i;
    char buf[6];

    for (i = 0; i < X_SIZE-2; i++)
        priv->hist[i] = priv->hist[i+1];
    priv->hist[X_SIZE-2] = priv->y;

    draw_vline(X_SIZE-1, 0, Y_SIZE-1, 1, ca);

    for (i = 0; i < X_SIZE-1; i++) {
        draw_vline(i, Y_SIZE/2-1, priv->hist[i], 2, ca);
        set_pixel(i, priv->hist[i], 1, ca);
    }

    sprintf(buf, "%4d", priv->avg);
    draw_str(buf, X_SIZE - 6*5, 2, ca, 0);
}


const struct widget_ops vario_graph_widget_ops = {
    .name = "Variometer chart",
    .mavname = "VARIO",
    .id = WIDGET_VARIOMETER_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
