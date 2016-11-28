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


struct widget_priv {
    float gps_lat, gps_lon, gps_eph;
    unsigned char gps_nrsats, gps_fix_type;
    unsigned char font_id;
};

static void pre_render(struct timer *t, void *d)
{
    struct widget *w = d;
    struct widget_priv *priv = w->priv;

    if (w->cfg->props.source == 0) {
        mavlink_gps_raw_int_t *gps = mavdata_get(MAVLINK_MSG_ID_GPS_RAW_INT);
        priv->gps_lat = gps->lat / 10000000.0;
        priv->gps_lon = gps->lon / 10000000.0;
        priv->gps_fix_type = gps->fix_type;
        priv->gps_nrsats = gps->satellites_visible;
        priv->gps_eph = (float) gps->eph / 100.0;
    } else {
        mavlink_gps2_raw_t *gps2 = mavdata_get(MAVLINK_MSG_ID_GPS2_RAW);
        priv->gps_lat = gps2->lat / 10000000.0;
        priv->gps_lon = gps2->lon / 10000000.0;
        priv->gps_fix_type = gps2->fix_type;
        priv->gps_nrsats = gps2->satellites_visible;
        priv->gps_eph = (float) gps2->eph / 100.0;
    }
    schedule_widget(w);
}

static int open(struct widget *w)
{
    struct widget_priv *priv;
    unsigned char m = w->cfg->props.mode;
    const struct font *f;

    priv = (struct widget_priv*) widget_malloc(sizeof(struct widget_priv));
    if (priv == NULL)
        return -1;
    w->priv = priv;

    if (m > 2)
        m = 2;

    priv->font_id = m;
    f = get_font(m);
    w->ca.height = (f->size + 2) * 2;
    w->ca.width = f->size * 13;
    
    add_timer(TIMER_WIDGET, 1000, pre_render, w);
    return 0;
}


static void render(struct widget *w)
{
    struct widget_priv *priv = w->priv;
    struct canvas *ca = &w->ca;
    char buf[50], buf2[5];

    snprintf(buf, 50, "%10.6f\n%10.6f", (double) priv->gps_lat, (double) priv->gps_lon);
    draw_str(buf, 0, 0, ca, priv->font_id);

    if (priv->gps_fix_type < 2) {
        buf2[0] = '\0';
    } else if (priv->gps_fix_type < 4) {
        sprintf(buf2, "%dD", priv->gps_fix_type);
    } else if (priv->gps_fix_type < 5) {
        strcpy(buf2, "DGPS");
    } else if (priv->gps_fix_type < 6) {
        strcpy(buf2, "RTK");
    } else {
        strcpy(buf2, "?");
    }

    snprintf(buf, 50, "%d %s\n%2.1f HDP",
                priv->gps_nrsats, buf2,
                (double) priv->gps_eph);
    draw_jstr(buf, ca->width, 0, JUST_RIGHT, ca, priv->font_id);
}


const struct widget_ops gps_info_widget_ops = {
    .name = "GPS stats",
    .mavname = "GPSINFO",
    .id = WIDGET_GPS_INFO_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
