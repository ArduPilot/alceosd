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


enum {
    VIDEO_XSIZE_420 = 0,
    VIDEO_XSIZE_480,
    VIDEO_XSIZE_560,
    VIDEO_XSIZE_672,
    VIDEO_XSIZE_END,
};

enum {
    VIDEO_STANDARD_PAL_P  = 0,
    VIDEO_STANDARD_PAL_I  = 1,
    VIDEO_STANDARD_NTSC_P = 2,
    VIDEO_STANDARD_NTSC_I = 3,
    VIDEO_STANDARD_END    = 8,
};

#define VIDEO_MODE_SCAN_MASK        (0x01)
#define VIDEO_MODE_STANDARD_MASK    (0x02)
#define VIDEO_MODE_SYNC_MASK        (0x04)


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
};

struct video_config {
    /* video standard and scan */
    unsigned char mode;
    /* video X resolution id */
    unsigned char x_size_id;
    /* video Y resolution */
    unsigned int y_size;
    /* video X resolution */
    unsigned int x_offset;
    /* video Y resolution */
    unsigned int y_offset;
    /* video brightness */
    unsigned int brightness;

    /* video voltage levels */
    unsigned char white_lvl;
    unsigned char gray_lvl;
    unsigned char black_lvl;
};



void init_video(void);

/* canvas related functions */
int alloc_canvas(struct canvas *ca, void *widget_cfg);
int init_canvas(struct canvas *ca, unsigned char b);
void schedule_canvas(struct canvas *ca);
void render_canvas(struct canvas *ca);
void free_mem(void);

/* clear up display */
void clear_sram(void);


void video_apply_config(struct video_config *cfg);
void video_get_size(unsigned int *xsize, unsigned int *ysize);


void video_pause(void);
void video_resume(void);

void shell_cmd_video(char *args, void *data);

#endif
