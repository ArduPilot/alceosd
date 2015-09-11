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

static int open(struct widget *w)
{
    w->ca.height = 120;
    w->ca.width = 120;
    return 0;
}


static void render(struct widget *w)
{
    struct canvas *ca = &w->ca;

    draw_rect(0, 0, 119, 119, 3, ca);
    draw_frect(1, 1, 118, 118, 1, ca);

    draw_frect(9, 59, 59, 109, 0, ca);
    draw_frect(9, 9, 59, 59, 1, ca);
    draw_frect(59, 9, 109, 59, 2, ca);
    draw_frect(59, 59, 109, 109, 3, ca);

    draw_str("White", 10, 10, ca, 1);
    draw_str("Gray", 60, 10, ca, 1);
    draw_str("Black", 60, 60, ca, 1);
}


const struct widget_ops videolvl_widget_ops = {
    .name = "Video levels",
    .mavname = "VIDLVL",
    .id = WIDGET_VIDEOLVL_ID,
    .init = NULL,
    .open = open,
    .render = render,
    .close = NULL,
};
