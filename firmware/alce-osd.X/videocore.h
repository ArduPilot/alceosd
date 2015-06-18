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

#ifndef VIDEOCORE_H
#define	VIDEOCORE_H

#include "widgets.h"


enum {
    VIDEO_XSIZE_420 = 0,
    VIDEO_XSIZE_480,
    VIDEO_XSIZE_560,
    VIDEO_XSIZE_672,
    VIDEO_XSIZE_END,
};

enum {
    VIDEO_STANDARD_PAL = 0,
    VIDEO_STANDARD_NTSC,
    VIDEO_STANDARD_END,
};

enum {
    VIDEO_SCAN_PROGRESSIVE = 0,
    VIDEO_SCAN_INTERLACED,
    VIDEO_SCAN_END,
};


union sram_addr {
    unsigned long l;
    struct {
        unsigned int w0:16;
        unsigned int w1:16;
    };
    struct {
        unsigned char b0:8;
        unsigned char b1:8;
        unsigned char b2:8;
        unsigned char b3:8;
    };
};


struct osd_xsize_tbl {
    unsigned int xsize;
    unsigned int clk_ps;
};

struct canvas {
    unsigned int x, y;
    unsigned int width, height;
    unsigned int rwidth;
    unsigned int size;
    unsigned char *buf;
    unsigned char lock;
};

struct video_config {
    /* video standard */
    unsigned int standard;
    /* video scan */
    unsigned int scan;
    /* video X resolution */
    unsigned int x_size;
    /* video Y resolution */
    unsigned int y_size;
    /* video X resolution */
    unsigned int x_offset;
    /* video Y resolution */
    unsigned int y_offset;
    /* video brightness */
    unsigned int brightness;
};



void init_video(void);
void render_process(void);

/* canvas related functions */
int alloc_canvas(struct canvas *c,
        struct widget_config *wcfg,
        unsigned int w, unsigned int h);
int init_canvas(struct canvas *ca, unsigned char b);
void schedule_canvas(struct canvas *ca);
void render_canvas(struct canvas *ca);
void free_mem(void);

/* clear up display */
void clear_video(void);


void video_apply_config(struct video_config *cfg);
void video_get_size(unsigned int *xsize, unsigned int *ysize);


#endif
