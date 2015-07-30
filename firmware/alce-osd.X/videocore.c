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


/* enable ram output */
#define OE_RAM      LATBbits.LATB8
#define OE_RAM_DIR  TRISBbits.TRISB8

/* sram commands */
#define SRAM_READ   0x03
#define SRAM_WRITE  0x02
#define SRAM_DIO    0x3b
#define SRAM_QIO    0x38
#define SRAM_RSTIO  0xFF
#define SRAM_RMODE  0x05
#define SRAM_WMODE  0x01

#define SRAM_SIZE (0x20000)

/* chip select */
#define CS_DIR TRISCbits.TRISC9
#define CS_LOW LATCbits.LATC9 = 0
#define CS_HIGH LATCbits.LATC9 = 1
/* clock */
#define CLK_DIR TRISCbits.TRISC8
#define CLK_LOW LATCbits.LATC8 = 0
#define CLK_HIGH LATCbits.LATC8 = 1
/* data direction */
#define SRAM_SPI TRISCbits.TRISC0 = 0; TRISCbits.TRISC1 = 1
#define SRAM_OUT TRISC &= 0xfffc
#define SRAM_IN TRISC |= 0x000f
#define SRAM_OUTQ TRISC &= 0xfff0
#define SPI_O LATCbits.LATC0
#define SPI_I PORTCbits.RC1
/* spi2 clock pin */
#define SCK2_O 0x09

extern struct alceosd_config config;

void video_apply_config_cbk(void);

const struct param_def params_video[] = {
    PARAM("VIDEO_STD", MAV_PARAM_TYPE_UINT8, &config.video.standard, NULL),
    PARAM("VIDEO_XSIZE", MAV_PARAM_TYPE_UINT8, &config.video.x_size_id, video_apply_config_cbk),
    PARAM("VIDEO_YSIZE", MAV_PARAM_TYPE_UINT16, &config.video.y_size, NULL),
    PARAM("VIDEO_XOFFSET", MAV_PARAM_TYPE_UINT16, &config.video.x_offset, NULL),
    PARAM("VIDEO_YOFFSET", MAV_PARAM_TYPE_UINT16, &config.video.y_offset, NULL),
    PARAM("VIDEO_BRIGHT", MAV_PARAM_TYPE_UINT16, &config.video.brightness, video_apply_config_cbk),
    PARAM_END,
};


volatile unsigned char sram_busy = 0;
volatile unsigned int line, last_line = 200, last_line_cnt = 0;
volatile unsigned int ticks = 0;
volatile unsigned char odd = 0;
static unsigned char render_state = 0;



const struct osd_xsize_tbl video_xsizes[] = {
    { .xsize = 420, .clk_ps = 0 } ,
    { .xsize = 480, .clk_ps = 1 } ,
    { .xsize = 560, .clk_ps = 2 } ,
    { .xsize = 672, .clk_ps = 3 } ,
};


#define MAX_CANVAS_PIPE_MASK (0x3f)

static struct canvas_pipe_s {
    struct canvas *ca[MAX_CANVAS_PIPE_MASK+1];
    unsigned char prd, pwr;
} canvas_pipe = {
    .pwr = 0,
    .prd = 0,
};


#define SCRATCHPAD_SIZE 0x4000
__eds__ unsigned char scratchpad1[SCRATCHPAD_SIZE]  __attribute__ ((eds, noload, address(0x4000)));
__eds__ unsigned char scratchpad2[SCRATCHPAD_SIZE]  __attribute__ ((eds, noload, address(0x8000)));

struct scratchpad_s {
    __eds__ unsigned char *mem;
    unsigned int alloc_size;
};

struct scratchpad_s scratchpad[2] = {
    {   .mem = scratchpad1,
        .alloc_size = 0,     },
    {   .mem = scratchpad2,
        .alloc_size = 0,     },
};


unsigned char sram_byte_spi(unsigned char b);
extern void sram_byteo_sqi(unsigned char b);
extern __eds__ unsigned char* copy_line(__eds__ unsigned char *buf, unsigned int count);
extern void clear_canvas(__eds__ unsigned char *buf, unsigned int count, unsigned char v);



static void sram_exit_sqi(void)
{
    CS_LOW;
    SRAM_OUTQ;
    LATC = (LATC & 0xfff0) | 0x000f;
    CLK_HIGH;CLK_LOW;
    CLK_HIGH;CLK_LOW;
    CS_HIGH;
    SRAM_SPI;
}

void sram_exit_sdi(void)
{
    CS_LOW;
    SRAM_OUTQ;
    LATC = (LATC & 0xfff0) | 0x000f;
    CLK_HIGH;CLK_LOW;
    CLK_HIGH;CLK_LOW;
    CLK_HIGH;CLK_LOW;
    CLK_HIGH;CLK_LOW;
    CS_HIGH;
    SRAM_SPI;
}

unsigned char sram_byte_spi(unsigned char b)
{
    unsigned char i, out = 0;

    for (i = 0; i < 8; i++) {
        out <<= 1;
        CLK_LOW;
        if (b & 0x80)
            SPI_O = 1;
        else
            SPI_O = 0;
        b <<= 1;
        CLK_HIGH;
        if (SPI_I)
            out |= 1;
    }
    /* clk = 0 */
    CLK_LOW;
    return out;
}

void sram_byteo_sdi(unsigned char b)
{
    unsigned int _LATC = LATC & 0xfefc; /* data clear and clk low*/

    LATC = _LATC | ((b >> 6));
    CLK_HIGH;
    LATC = _LATC | ((b >> 4) & 3);
    CLK_HIGH;
    LATC = _LATC | ((b >> 2) & 3);
    CLK_HIGH;
    LATC = _LATC | (b & 3);
    CLK_HIGH;
    CLK_LOW;
}


void clear_sram(void)
{
    unsigned long i;

    CS_LOW;
    sram_byteo_sqi(SRAM_WRITE);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    for (i = 0; i < SRAM_SIZE; i++) {
        sram_byteo_sqi(0x00);
    }
    CS_HIGH;
}

void clear_video(void)
{
    int ipl;

    while (sram_busy);
    SET_AND_SAVE_CPU_IPL(ipl, 7);
    clear_sram();
    RESTORE_CPU_IPL(ipl);
}


static void video_init_sram(void)
{
    /* cs as output, set high */
    CS_DIR = 0;
    CS_HIGH;
    /* clock as output, set low */
    CLK_DIR = 0;
    CLK_LOW;
    _RP56R = 0;

    /* force a spi mode from sdi */
    sram_exit_sdi();
    /* force a spi mode from sqi */
    sram_exit_sqi();

    /* IOs are now in spi mode, set SQI */
    SRAM_SPI;
    CS_LOW;
    sram_byte_spi(SRAM_QIO);
    CS_HIGH;
    SRAM_OUTQ;

    /* set ram to zeros */
    clear_sram();

#if 0
    unsigned long i, k;

    CS_LOW;
    sram_byteo_sqi(SRAM_WRITE);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    //for (i = 0; i < OSD_YSIZE; i++) {
        for (k = 0; k < OSD_XSIZE/8; k++) {
            //sram_byteo_sqi(0x41);
            sram_byteo_sqi(0x05);
            sram_byteo_sqi(0xaf);
       }
   // }
    CS_HIGH;
#endif
}



static void video_init_hw(void)
{
    SPI2CON1bits.CKP = 0; /* idle low */
    SPI2CON1bits.CKE = 1;
    SPI2CON1bits.MSTEN = 1;
    SPI2CON1bits.DISSDO = 1;
    SPI2CON1bits.DISSCK = 0;
    SPI2CON1bits.PPRE = 3;
    SPI2CON1bits.SPRE = video_xsizes[config.video.x_size_id].clk_ps;
    SPI2CON1bits.MODE16 = 1;
    SPI2CON2bits.FRMEN = 1;
    /* pins as ports */
    //_RP12R = SCK2_O;
    //_RP0R = SDO2_O;
    SPI2STATbits.SPIROV = 0;
    SPI2STATbits.SPIEN = 0;

    OE_RAM_DIR = 0;
    OE_RAM = 1;

    /* csync, vsync, frame */
    TRISBbits.TRISB13 = 1;
    TRISBbits.TRISB14 = 1;
    TRISBbits.TRISB15 = 1;

    /* CSYNC - INT2 */
    RPINR1bits.INT2R = 45;
    /* falling edge */
    INTCON2bits.INT2EP = 1;
    /* priority */
    IPC7bits.INT2IP = 4;
    /* enable */
    IEC1bits.INT2IE = 1;

    /* VSYNC - INT1 */
    RPINR0bits.INT1R = 46;
    /* falling edge */
    INTCON2bits.INT1EP = 1;
    /* priority */
    IPC5bits.INT1IP = 3;
    /* enable int1 */
    IEC1bits.INT1IE = 1;

    /* LINE TIMER - TIMER1 */
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    PR2 = 280*12 + 5;
    T2CONbits.TON = 0;
    IPC1bits.T2IP = 4;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;


    /* brightness */
    /* OC1 pin */
    TRISBbits.TRISB7 = 0;
    _RP39R = 0x10;

    T3CONbits.TCKPS = 0;
    T3CONbits.TCS = 0;
    T3CONbits.TGATE = 0;
    TMR3 = 0x00;
    PR3 = 0x7fff;
    T3CONbits.TON = 1;

    OC1CON1bits.OCM = 0;
    OC1CON1bits.OCTSEL = 1;
    OC1CON2bits.SYNCSEL = 0x1f;
    OC1R = 0x100;
    OC1RS = 0x100 + config.video.brightness;
    OC1CON1bits.OCM = 0b110;
}


void init_video(void)
{
    video_init_sram();
    video_init_hw();

    params_add(params_video);
}

static int ipl;

void video_pause(void)
{
    while (sram_busy);
    SET_AND_SAVE_CPU_IPL(ipl, 7);
}

void video_resume(void)
{
    RESTORE_CPU_IPL(ipl);
}


void video_apply_config(struct video_config *cfg)
{
    /* brightness */
    OC1RS = 0x100 + cfg->brightness;

    /* pixel clock */
    INTCON2bits.GIE = 0;
    SPI2STATbits.SPIEN = 0;
    SPI2CON1bits.SPRE = video_xsizes[cfg->x_size_id].clk_ps;
    INTCON2bits.GIE = 1;

}

void video_apply_config_cbk(void)
{
    struct video_config *c = &config.video;
    if (c->x_size_id >= VIDEO_XSIZE_END)
        c->x_size_id = VIDEO_XSIZE_END - 1;
    video_apply_config(c);
}


void free_mem(void)
{
    scratchpad[0].alloc_size = 0;
    scratchpad[1].alloc_size = 0;

    canvas_pipe.prd = canvas_pipe.pwr = 0;
    render_state = 0;
}

void video_get_size(unsigned int *xsize, unsigned int *ysize)
{
    *xsize = video_xsizes[config.video.x_size_id].xsize;
    *ysize = config.video.y_size;
    if (config.video.standard & VIDEO_STANDARD_SCAN_MASK)
        *ysize *= 2;
}


int alloc_canvas(struct canvas *c, void *widget_cfg)
{
    unsigned int osdxsize, osdysize, i;
    struct widget_config *wcfg = widget_cfg;
    video_get_size(&osdxsize, &osdysize);

    c->width = (c->width & 0xfffc);
    c->rwidth = c->width >> 2;
    c->size = c->rwidth * c->height;

    for (i = 0; i < 2; i++) {
        if ((scratchpad[i].alloc_size + c->size) < SCRATCHPAD_SIZE)
            break;
    }
    if (i == 2) {
        c->lock = 1;
        return -1;
    }

    switch (wcfg->props.vjust) {
        case VJUST_TOP:
        default:
            c->y = wcfg->y;
            break;
        case VJUST_BOT:
            c->y = osdysize - c->height + wcfg->y;
            break;
        case VJUST_CENTER:
            c->y = (osdysize - c->height)/2 + wcfg->y;
            break;
    }

    switch (wcfg->props.hjust) {
        case HJUST_LEFT:
        default:
            c->x = wcfg->x;
            break;
        case HJUST_RIGHT:
            c->x = osdxsize - c->width + wcfg->x;
            break;
        case HJUST_CENTER:
            c->x = (osdxsize - c->width)/2 + wcfg->x;
            break;
    }

    c->buf = &scratchpad[i].mem[scratchpad[i].alloc_size];
    scratchpad[i].alloc_size += c->size;

    c->lock = 0;
    return 0;
}


int init_canvas(struct canvas *ca, unsigned char b)
{
    if (ca->lock) {
        //U1TXREG = 'l';
        return -1;
    }
    clear_canvas(ca->buf, ca->size, b);
    return 0;
}


void schedule_canvas(struct canvas *ca)
{
    canvas_pipe.ca[canvas_pipe.pwr++] = ca;
    canvas_pipe.pwr &= MAX_CANVAS_PIPE_MASK;
    ca->lock = 1;
}


void render_process(void)
{
    static struct canvas *ca;
    static unsigned int y1, y;
    __eds__ static unsigned char *b;
    static union sram_addr addr;
    static unsigned int xsize;

    unsigned int x;

    for (;;) {
        if (render_state == 0) {
            if (canvas_pipe.prd == canvas_pipe.pwr)
                return;

            ca = canvas_pipe.ca[canvas_pipe.prd];

            y = ca->y;
            y1 = ca->y + ca->height;
            x = ca->x >> 2;
            b = ca->buf;

            xsize = (video_xsizes[config.video.x_size_id].xsize) >> 2;
            addr.l = x + ((unsigned long) xsize *  y);
            render_state = 1;
        }
        if (render_state == 1) {
            /* render */
            for (;;) {
                if (sram_busy)
                    return;

                CS_LOW;
                sram_byteo_sqi(SRAM_WRITE);
                sram_byteo_sqi(addr.b2);
                sram_byteo_sqi(addr.b1);
                sram_byteo_sqi(addr.b0);
                b = copy_line(b, ca->rwidth);
                CS_HIGH;

                if (++y == y1)
                    break;

                addr.l += xsize;
            }
            ca->lock = 0;
            //U1TXREG = 'U';
            canvas_pipe.prd++;
            canvas_pipe.prd &= MAX_CANVAS_PIPE_MASK;
            render_state = 0;
        }
    }
}


/* blocking render */
void render_canvas(struct canvas *ca)
{
    unsigned int x, h;
    __eds__ unsigned char *b;
    union sram_addr addr;
    unsigned int osdxsize, osdysize;
    video_get_size(&osdxsize, &osdysize);

    x = ca->x >> 2;
    b = ca->buf;
    addr.l =  x + (osdxsize/4 * (unsigned long) ca->y);

    /* render */
    for (h = 0; h < ca->height; h++) {
        while (sram_busy);
        CS_LOW;
        sram_byteo_sqi(SRAM_WRITE);
        sram_byteo_sqi(addr.b2);
        sram_byteo_sqi(addr.b1);
        sram_byteo_sqi(addr.b0);
        b = copy_line(b, ca->rwidth);
        CS_HIGH;
        addr.l += osdxsize/4;
    }
}


/* line timer */
void __attribute__((__interrupt__, auto_psv )) _T2Interrupt()
{
    OE_RAM = 1;
    _RP56R = 0x0;
    CS_HIGH;
    SRAM_OUT;

    SPI2STATbits.SPIEN = 0;
    T2CONbits.TON = 0;

    if (line == last_line - 1) {
        /* switch sram back to sqi mode */
        sram_exit_sdi();
        CS_LOW;
        sram_byte_spi(SRAM_QIO);
        CS_HIGH;
        SRAM_OUTQ;
        sram_busy = 0;
    }
    IFS0bits.T2IF = 0;
}

/* frame interrupt */
void __attribute__((__interrupt__, auto_psv )) _INT1Interrupt()
{
    last_line_cnt = line;
    ticks++;
    line = 0;
    IFS1bits.INT1IF = 0;
}


void __attribute__((__interrupt__, auto_psv )) _INT2Interrupt()
{
    volatile unsigned int i;
    volatile static union sram_addr addr;
    static unsigned int osdxsize;
    
    line++;
    
    if (line < config.video.y_offset-2) {
        /* do nothing */
    } else if (line < config.video.y_offset-1) {
        /* T-2: setup vars */
        osdxsize = video_xsizes[config.video.x_size_id].xsize;

        /* calc last_line */
        last_line = config.video.y_size + config.video.y_offset;

        /* avoid sram_busy soft-locks */
        if (last_line > last_line_cnt)
            last_line = last_line_cnt - 20;

        /* auto detect video standard */
        if (last_line_cnt < 300)
            config.video.standard |= VIDEO_STANDARD_MASK;
        else
            config.video.standard &= ~VIDEO_STANDARD_MASK;

        sram_busy = 1;
        addr.l = 0;

        if (config.video.standard && VIDEO_STANDARD_SCAN_MASK) {
            odd = PORTBbits.RB15;
            if (odd == 0) {
                addr.l += (osdxsize/4);
            }
        }
    } else if (line < config.video.y_offset) {
        /* T-1: switch sram to sdi mode */
        sram_exit_sqi();
        CS_LOW;
        sram_byte_spi(SRAM_DIO);
        CS_HIGH;
        SRAM_IN;
        SRAM_OUT;
    } else if (line < last_line) {
        /* rendering */
        /* TODO: Make this dummy "for" a timer interrupt? */
        for (i = 0; i < config.video.x_offset; i++);

        /* setup sram for video output */
        CS_LOW;
        sram_byteo_sdi(SRAM_READ);
        sram_byteo_sdi(addr.b2);
        sram_byteo_sdi(addr.b1);
        sram_byteo_sdi(addr.b0);
        SRAM_IN;
        CLK_HIGH; CLK_LOW;
        CLK_HIGH; CLK_LOW;
        CLK_HIGH; CLK_LOW;
        CLK_HIGH; CLK_LOW;
        _RP56R = SCK2_O;
        OE_RAM = 0;

        SPI2STATbits.SPIEN = 1;
        T2CONbits.TON = 1;

        /* calc next address */
        if (config.video.standard & VIDEO_STANDARD_SCAN_MASK) {
            addr.l += (unsigned long) ((osdxsize/4) * 2);
        } else {
            addr.l += (unsigned long) (osdxsize/4);
        }
    }
    IFS1bits.INT2IF = 0;
}

