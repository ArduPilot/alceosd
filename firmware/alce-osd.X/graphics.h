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

#ifndef GRAPHICS_H
#define	GRAPHICS_H

#include "videocore.h"


struct point {
    int x, y;
};

struct polygon {
    struct point *points;
    unsigned char len;
};


/* in assembly */
extern void set_pixel(unsigned int x, unsigned int y, unsigned int v, struct canvas *ca);
extern void draw_hline(int x0, int x1, int y, unsigned char p, struct canvas *ca);
extern void draw_vline(int x, int y0, int y1, unsigned char p, struct canvas *ca);

/* in C */
void draw_line(int x0, int y0, int x1, int y1, unsigned char v, struct canvas *ca);
void draw_oline(int x0, int y0, int x1, int y1, unsigned char v, struct canvas *ca);
void draw_ohline(int x0, int x1, int y, unsigned char p, unsigned char b, struct canvas *ca);

void draw_rect(int x0, int y0, int x1, int y1, unsigned char p, struct canvas *ca);
void draw_frect(int x0, int y0, int x1, int y1, unsigned char p, struct canvas *ca);

void draw_chr(char c, int x, int y, struct canvas *ca);
void draw_str(char *buf, int x, int y, struct canvas *ca);
void draw_str3(char *buf, int x, int y, struct canvas *ca);

void draw_circle(int xm, int ym, int r, unsigned char p, struct canvas *ca);

void transform_polygon(struct polygon *p, int x, int y, int rot);
void move_polygon(struct polygon *p, int x, int y);
void draw_polygon(struct polygon *p, unsigned char v, struct canvas *ca);

#endif
