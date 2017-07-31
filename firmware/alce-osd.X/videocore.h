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

#include "alce-types.h"

enum {
    VIDEO_XSIZE_420 = 0,
    VIDEO_XSIZE_480,
    VIDEO_XSIZE_560,
    VIDEO_XSIZE_672,
    VIDEO_XSIZE_END,
};

#define VIDEO_STATUS_SYNC_MASK 0x01
#define VIDEO_STATUS_EXTSYNC   0x00
#define VIDEO_STATUS_INTSYNC   0x01
#define VIDEO_STATUS_SYNC_BIT  0

#define VIDEO_STATUS_STD_MASK  0x02
#define VIDEO_STATUS_STD_PAL   0x00
#define VIDEO_STATUS_STD_NTSC  0x02
#define VIDEO_STATUS_STD_BIT   1

#define VIDEO_STATUS_SOURCE_MASK    0x04
#define VIDEO_STATUS_SOURCE_0       0x00
#define VIDEO_STATUS_SOURCE_1       0x04
#define VIDEO_STATUS_SOURCE_BIT     2

#define VIDEO_STATUS_VREF0_MASK    0x0f00
#define VIDEO_STATUS_VREF1_MASK    0xf000
#define VIDEO_STATUS_VREF0_BIT     8
#define VIDEO_STATUS_VREF1_BIT     12

#define VIDEO_SCAN_PROGRESSIVE 0
#define VIDEO_SCAN_INTERLACED  1

#define VIDEO_ACTIVE_CONFIG         0xff

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
} __attribute__((aligned(2)));


struct osd_xsize_tbl {
    unsigned int xsize;
    unsigned int clk_ps;
};

struct canvas {
    unsigned int x, y;
    unsigned int width, height;
    unsigned int rwidth;
    unsigned int size;
    __eds__ unsigned char *buf;
    unsigned char lock;
    
    u8 buf_nr;
};

typedef union {
    u8 raw;
    struct  {
        unsigned scan_mode:1;
        unsigned :7;
    };
} video_mode_t;

struct video_config_profile {
    /* video scan mode */
    video_mode_t mode;
    /* video X resolution id */
    unsigned char x_size_id;
    /* video X left offset */
    unsigned int x_offset;
    /* video Y top offset */
    unsigned int y_toffset;
    /* video Y bottom offset */
    unsigned int y_boffset;
};

typedef union {
    u8 raw;
    struct  {
        unsigned vin0:4;
        unsigned vin1:4;
    };
} video_vref_t;

struct video_config {
    /* video voltage levels */
    unsigned char white_lvl;
    unsigned char gray_lvl;
    unsigned char black_lvl;
    
    /* input source */
    video_vref_t vref;
};


void init_video(void);

/* canvas related functions */
int alloc_canvas(struct canvas *ca, void *widget_cfg);
void free_canvas(struct canvas *c);
void reconfig_canvas(struct canvas *ca, void *widget_cfg);
int init_canvas(struct canvas *ca);
void schedule_canvas(struct canvas *ca);
void free_mem(void);

/* clear up display */
void clear_sram(void);


void video_apply_config(unsigned char profile);
void video_get_size(unsigned int *xsize, unsigned int *ysize);


void video_pause(void);
void video_resume(void);

void shell_cmd_video(char *args, void *data);

extern const struct osd_xsize_tbl video_xsizes[];
#endif
