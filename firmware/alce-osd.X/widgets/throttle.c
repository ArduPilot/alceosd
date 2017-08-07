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

#define X_SIZE  12
#define Y_SIZE  45

struct widget_priv {
    u16 throttle, last_throttle;
};

static void render_timer(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    switch (w->cfg->props.source) {
        default:
        case 0:
        {
            /* throttle setting from VFR_HUD (percent) */
            mavlink_vfr_hud_t *vfr_hud = mavdata_get(MAVLINK_MSG_ID_VFR_HUD);
            priv->throttle = (unsigned char) vfr_hud->throttle;
            break;
        }
        case 1:
        {
            /* RC channel throttle stick */
            /* param1 = rc channel nr */
            mavlink_rc_channels_raw_t *rcr;
            mavlink_rc_channels_t *rc;
            u16 *val_ptr;
            if (mavdata_age(MAVLINK_MSG_ID_RC_CHANNELS) < 5000) {
                rc = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS);
                val_ptr = &rc->chan1_raw;
            } else {
                rcr = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
                val_ptr = &rcr->chan1_raw;
            }
            u16 ch = TRIM(w->cfg->params[0], 1, 18) - 1;
            priv->throttle = *(val_ptr + ch);
            break;
        }
    }

    if ((w->cfg->props.units == 0) && (w->cfg->params[3] > 0)) {
        /* percentage */
        /* param3=min, param4=max */
        u16 min = w->cfg->params[2];
        u16 max = w->cfg->params[3];
        u32 thr = TRIM(priv->throttle, min, max);
        thr = ((thr - min) * 100) / (max - min);
        priv->throttle = (u16)thr;
    }

    if (priv->throttle == priv->last_throttle)
        return;

    priv->last_throttle = priv->throttle;
    schedule_widget(w);
}


static int open(struct widget *w)
{
    struct widget_priv *priv;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    priv->last_throttle = 0xff;
    
    switch (w->cfg->props.mode) {
        default:
        case 0:
            /* 0-100% vertical bar */
            w->ca.width = X_SIZE;
            w->ca.height = Y_SIZE;
            break;
        case 1:
            /* 0-100% horizontal bar */
            w->ca.width = Y_SIZE;
            w->ca.height = X_SIZE;
            break;
        case 2:
            /* text only */
            w->ca.width = (18*4);
            w->ca.height = 16;
            break;
    }
    add_timer(TIMER_WIDGET, 250, render_timer, w);
    return 0;
}

static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    unsigned v = priv->throttle * (Y_SIZE-4) / 100;
    char buf[20];
    
    switch (w->cfg->props.mode) {
        default:
        case 0:
            draw_rect(0, 0, X_SIZE-1, Y_SIZE-1, 3, ca);
            draw_rect(1, 1, X_SIZE-2, Y_SIZE-2, 1, ca);
            if (priv->throttle > 0) {
                draw_frect(2, Y_SIZE-3 - v, X_SIZE-3, Y_SIZE-3, 2, ca);
            }
            snprintf(buf, 5, "%d", priv->throttle);
            draw_jstr(buf, ca->width/2, ca->height/2, JUST_HCENTER|JUST_VCENTER, ca, 0);
            break;
        case 1:
            draw_rect(0, 0, Y_SIZE-1, X_SIZE-1, 3, ca);
            draw_rect(1, 1, Y_SIZE-2, X_SIZE-2, 1, ca);
            if (priv->throttle > 0) {
                draw_frect(2, 2, Y_SIZE-3 - v, X_SIZE-3, 2, ca);
            }
            snprintf(buf, 5, "%d", priv->throttle);
            draw_jstr(buf, ca->width/2, ca->height/2, JUST_HCENTER|JUST_VCENTER, ca, 0);
            break;
        case 2:
            snprintf(buf, 20, "Thr(%s) %d%c", (w->cfg->props.source == 0 ? "o" : "i"), priv->throttle, (w->cfg->props.units == 0 ? '%' : ' '));
            draw_jstr(buf, ca->width/2, ca->height/2, JUST_HCENTER|JUST_VCENTER, ca, 0);
            break;
    }
}


const struct widget_ops throttle_widget_ops = {
    .name = "Throttle bar",
    .mavname = "THROTTL",
    .id = WIDGET_THROTTLE_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
