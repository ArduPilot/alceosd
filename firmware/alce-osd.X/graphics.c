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


extern const struct font font_ab[];

const struct font* get_font(unsigned char size)
{
    if (size > 2)
        size = 2;
    
    return &font_ab[size];
}

void draw_line(int x0, int y0, int x1, int y1,
        unsigned char v, struct canvas *ca)
{
    if (x0 == x1) {
        draw_vline(x0, y0, y1, v, ca);
        return;
    } else if (y0 == y1) {
        draw_hline(x0, x1, y0, v, ca);
        return;
    }

    int aux;
    if (x0 > x1) {
        aux = x1; x1 = x0; x0 = aux;
        aux = y1; y1 = y0; y0 = aux;
    }

    int dx =  abs(x1-x0);
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;

    for(;;){
        set_pixel(x0, y0, v, ca);
        if (x0==x1 && y0==y1) break;

        e2 = 2*err;
        
        if (e2 >= dy) {
            err += dy;
            x0 ++;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_ohline(int x0, int x1, int y, unsigned char p, unsigned char b, struct canvas *ca)
{
    draw_hline(x0, x1, y-1, b, ca);
    draw_hline(x0, x1, y, p, ca);
    draw_hline(x0, x1, y+1, b, ca);
}

void draw_oline(int x0, int y0, int x1, int y1,
        unsigned char v, struct canvas *ca)
{
    double slope;
    int aux, x, y;

    if (abs(x1-x0) >= abs(y1-y0)) {
        if (x1 < x0) {
            aux = x0;
            x0 = x1;
            x1 = aux;
        }
        slope = 1.0*(y1-y0)/(x1-x0);
        set_pixel(x0-1, y0, 3, ca);
        for (x = x0; x <= x1; x++) {
            y = y0 + ((int) (slope*(x-x0)));
            set_pixel(x, y-1, 3, ca);
            set_pixel(x, y+1, 3, ca);
            set_pixel(x, y, v, ca);
        }
        set_pixel(x1+1, y1, 3, ca);
    } else {
        if (y1 < y0) {
            aux = y0;
            y0 = y1;
            y1 = aux;
        }
        slope = 1.0*(x1-x0)/(y1-y0);
        set_pixel(x0, y0-1, 3, ca);
        for (y = y0; y <= y1; y++) {
            x = x0 + ((int) (slope*(y-y0)));
            set_pixel(x-1, y, 3, ca);
            set_pixel(x+2, y, 3, ca);
            set_pixel(x, y, v, ca);
        }
        set_pixel(x1, y1+1, 3, ca);
    }
}


void draw_rect(int x0, int y0, int x1, int y1, unsigned char p, struct canvas *ca)
{
    draw_hline(x0, x1, y0, p, ca);
    draw_hline(x0, x1, y1, p, ca);
    draw_vline(x0, y0, y1, p, ca);
    draw_vline(x1, y0, y1, p, ca);
}
void draw_frect(int x0, int y0, int x1, int y1, unsigned char p, struct canvas *ca)
{
    int y;
    for (y = y0; y <= y1; y++)
        draw_hline(x0, x1, y, p, ca);
}


void draw_circle(int xm, int ym, int r, unsigned char p, struct canvas *ca)
{
   int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
   do {
      set_pixel(xm-x, ym+y, p, ca); /*   I. Quadrant */
      set_pixel(xm-y, ym-x, p, ca); /*  II. Quadrant */
      set_pixel(xm+x, ym-y, p, ca); /* III. Quadrant */
      set_pixel(xm+y, ym+x, p, ca); /*  IV. Quadrant */
      r = err;
      if (r <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (r > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
   } while (x < 0);
}


void transform_polygon(struct polygon *p, int x, int y, int rot)
{
    struct point *pt = p->points;
    unsigned char i;
    float angle = DEG2RAD(rot);
    float cos_rot = cos(angle) * 10000;
    float sin_rot = sin(angle) * 10000;
    long cos_l = (long) cos_rot;
    long sin_l = (long) sin_rot;

    long xr, yr;

    for (i = 0; i < p->len; i++) {
        xr = ((cos_l * pt->x) - (sin_l * pt->y));
        xr = (xr + 5000) / 10000;
        yr = ((cos_l * pt->y) + (sin_l * pt->x));
        yr = (yr + 5000) / 10000;
        pt->x = (int) xr;
        pt->y = (int) yr;
        pt->x += x;
        pt->y += y;
        pt++;
    }
}

void move_polygon(struct polygon *p, int x, int y)
{
    struct point *pt = p->points;
    unsigned char i;

    for (i = 0; i < p->len; i++) {
        pt->x += x;
        pt->y += y;
        pt++;
    }
}

void draw_polygon(struct polygon *p, unsigned char v, struct canvas *ca)
{
   unsigned char i;
   struct point *pt1 = p->points;
   struct point *pt2 = pt1+1;

    for (i = 1; i < p->len; i++) {
        draw_line(pt1->x, pt1->y, pt2->x, pt2->y, v, ca);
        pt1++;
        pt2++;
    }
    pt2 = p->points;
    draw_line(pt1->x, pt1->y, pt2->x, pt2->y, v, ca);
}



static unsigned char draw_chr0(char c, int x, int y, struct canvas *ca, const struct font *f)
{
    unsigned int ch = (c - 0x20);
    const struct font_char *fc = &f->chars[ch];
    unsigned int idx = fc->data;
    const char *b = &f->font_data[idx];
    
    unsigned char i, j, k, p;
    int x0;

    k = 8;
    y = y + (char) fc->oy;
    for (i = 0; i < fc->h; i++) {
        x0 = x;// + (char) fc->ox;
        for (j = 0; j < fc->w; j++) {
            k = k - 2;
            p = (*b >> k) & 3;
            if (p > 0)
                set_pixel(x0, y, p, ca);
            
            x0++;
            if (k == 0) {
                k = 8;
                b++;
            }
        }
        y++;
    }
    return fc->w;
}





void draw_chr(char c, int x, int y, struct canvas *ca, unsigned char size)
{
    const struct font *f = get_font(size);
    draw_chr0(c, x, y, ca, f);
}


void draw_str(char *buf, int x, int y, struct canvas *ca,
    unsigned char size)
{
    const struct font *f = get_font(size);
    int x0 = x;

    while (*buf != '\0') {
        if (*buf == '\n') {
            x = x0;
            y += f->size;
            buf++;
        } else {
            x += draw_chr0(*buf++, x, y, ca, f);
        }
    }
}


unsigned int get_str_width(char *buf, const struct font *f)
{
    unsigned char ch;
    unsigned int wid = 0;
    
    while ((*buf != '\0') && (*buf != '\n')) {
        ch = (*buf - 0x20);
        wid += f->chars[ch].w;
        buf++;
    }
    return wid-1;
}


void draw_jstr(char *buf, int x, int y, unsigned char just, struct canvas *ca,
    unsigned char size)
{
    const struct font *f = get_font(size);
    int sw = get_str_width(buf, f);
    int x0 = x;

    if (just & JUST_VCENTER)    
        y -= f->size/2 + 1;
    else if (just & JUST_BOT)
        y -= f->size;

    if (just & JUST_HCENTER)    
        x -= sw/2;
    else if (just & JUST_RIGHT)
        x -= sw;
    
    while (*buf != '\0') {
        if (*buf == '\n') {
            buf++;
            sw = get_str_width(buf, f);
            if (just & JUST_HCENTER)    
                x = x0 - sw/2;
            else if (just & JUST_RIGHT)
                x = x0 - sw;
            
            y += f->size;
        } else {
            x += draw_chr0(*buf++, x, y, ca, f);
        }
    }
}


#if 0
void draw_line_wd(int x0, int y0, int x1, int y1, unsigned char v, unsigned char wd)
{
   int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
   int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
   int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
   float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

   for (wd = (wd+1)/2; ; ) {                                   /* pixel loop */
      set_pixel(x0, y0, v);
      e2 = err; x2 = x0;
      if (2*e2 >= -dx) {                                           /* x step */
         for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
            set_pixel(x0, y2 += sy, v);
         if (x0 == x1) break;
         e2 = err; err -= dy; x0 += sx;
      }
      if (2*e2 <= dy) {                                            /* y step */
         for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
            set_pixel(x2 += sx, y0, v);
         if (y0 == y1) break;
         err += dx; y0 += sy;
      }
   }
}




void draw_ellipse_rect(int x0, int y0, int x1, int y1, unsigned char v)
{
   int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;

   do {
       set_pixel(x1, y0, v); /*   I. Quadrant */
       set_pixel(x0, y0, v); /*  II. Quadrant */
       set_pixel(x0, y1, v); /* III. Quadrant */
       set_pixel(x1, y1, v); /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
   } while (x0 <= x1);

   while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
       set_pixel(x0-1, y0, v); /* -> finish tip of ellipse */
       set_pixel(x1+1, y0++, v);
       set_pixel(x0-1, y1, v);
       set_pixel(x1+1, y1--, v);
   }
}
#endif

