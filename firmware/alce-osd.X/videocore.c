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
#define OE_RAM2     LATAbits.LATA9
#define OE_RAM_DIR2 TRISAbits.TRISA9


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


#define LINE_TMR (278*12)+5


#define CTRL_SYNCGEN        0x01
#define CTRL_COMPSYNC       0x02
#define CTRL_PWMBRIGHT      0x04
#define CTRL_DACBRIGHT      0x08


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


static volatile unsigned int ext_sync_cnt = 0, int_sync_cnt = 0;
volatile unsigned char sram_busy = 0;
volatile unsigned int line, last_line_cnt = 0;
volatile unsigned char odd = 0;
static struct canvas *rendering_canvas = NULL;

static unsigned char videocore_ctrl = 0;




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
extern unsigned char sram_bytei_sqi(void);
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

static void sram_exit_sdi(void)
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
    register unsigned char i;
    unsigned char out = 0;

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
    register unsigned long i;

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

static unsigned char test_sram_0(unsigned char b)
{
    register unsigned long i;
    register unsigned char r;

    CS_LOW;
    sram_byteo_sqi(SRAM_WRITE);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    for (i = 0; i < SRAM_SIZE; i++) {
        sram_byteo_sqi(b);
    }
    CS_HIGH;
    
    CS_LOW;
    sram_byteo_sqi(SRAM_READ);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    sram_byteo_sqi(0);
    SRAM_IN;
    r = sram_bytei_sqi();
    for (i = 0; i < SRAM_SIZE; i++) {
        r = sram_bytei_sqi();
        if (r != b) {
            CS_HIGH;
            SRAM_OUTQ;
            printf("test_ram: expected = 0x%02x read = 0x%02x\n", b, r);
            return 1;
        }
    }
    CS_HIGH;
    SRAM_OUTQ;
    return 0;
}

static void test_sram(void)
{
    unsigned char test[4] = {0x00, 0x5a, 0xa5, 0xff};
    unsigned char i, j;
    printf("Testing SRAM\n");
    
    for (i = 0; i < 4; i++) {
        j = test[i];
        if (test_sram_0(j))
            printf("0x%02x (fail)\n", j);
        else
            printf("0x%02x (pass)\n", j);
    }
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

    /* test SRAM */
    test_sram();

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

static void video_dac_read(void)
{
    unsigned char dac_status[3*2*4];
    unsigned char i;
    
    printf("Reading DAC values...\n");
    
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN == 1);
    
    I2C1TRN = 0xc1;
    while (I2C1STATbits.TRSTAT == 1);

    for (i = 0; i < 3*2*4; i++) {
        I2C1CONbits.RCEN = 1;
        while (I2C1CONbits.RCEN == 1);
        
        dac_status[i] = I2C1RCV;
        
        I2C1CONbits.ACKEN = 1;
        while (I2C1CONbits.ACKEN == 1);
    }

    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN == 1);
    
    for (i = 0; i < 3*2*4; i++) {
        printf("0x%02x ", dac_status[i]);
        if (((i+1) % 3) == 0)
            printf("\n");
    }
    
    
}

static void video_init_dac(void)
{
    /* init (a)i2c1*/
    _ODCB8 = 1;
    _ODCB9 = 1;
    
    I2C1CON = 0x8000;
    
    I2C1ADD = 0;
    I2C1MSK = 0;
    
    I2C1BRG = 0x1ff;
}


static void video_update_dac(void)
{
    unsigned int dac_values[4] = { 0x3ff, 0x2d0, 0x190, 0x000};
    unsigned char i;
    
    printf("Setting DAC values...\n");
    
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN == 1);
    
    I2C1TRN = 0xc0;
    while (I2C1STATbits.TRSTAT == 1);

    for (i = 0; i < 4; i++) {
        I2C1TRN = (dac_values[i] >> 8) & 0x0f;
        while (I2C1STATbits.TRSTAT == 1);
    
        I2C1TRN = dac_values[i] & 0xff;
        while (I2C1STATbits.TRSTAT == 1);

    }
    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN == 1);
}



extern unsigned char hw_rev;

static void video_init_hw(void)
{
    switch (hw_rev) {
        case 0x01:
        default:
            videocore_ctrl |= CTRL_PWMBRIGHT;
            break;
        case 0x02:
            videocore_ctrl |= CTRL_PWMBRIGHT;
            videocore_ctrl |= CTRL_SYNCGEN;
            break;
        case 0x03:
            videocore_ctrl |= CTRL_SYNCGEN;
            videocore_ctrl |= CTRL_DACBRIGHT;
            videocore_ctrl |= CTRL_COMPSYNC;
            break;
    }


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

    if (hw_rev <= 0x02) {
        OE_RAM_DIR = 0;
        OE_RAM = 1;
    } else {
        OE_RAM_DIR2 = 0;
        OE_RAM2 = 1;
        /* mux as input */
        _TRISA2 = 1;
        _TRISA3 = 1;
    }
    
    /* generic line timer */
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    PR2 = LINE_TMR;
    T2CONbits.TON = 0;
    _T2IP = 6;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;


    if ((videocore_ctrl & CTRL_COMPSYNC) == 0) {
        /* csync, vsync, frame */
        TRISBbits.TRISB13 = 1;
        TRISBbits.TRISB14 = 1;
        TRISBbits.TRISB15 = 1;

        /* CSYNC - INT2 */
        RPINR1bits.INT2R = 45;
        /* falling edge */
        INTCON2bits.INT2EP = 1;
        /* priority */
        _INT2IP = 4;

        /* VSYNC - INT1 */
        RPINR0bits.INT1R = 46;
        /* falling edge */
        INTCON2bits.INT1EP = 1;
        /* priority */
        _INT1IP = 3;
        /* enable int1 */
        _INT1IE = 1;
    }


    if (videocore_ctrl & CTRL_PWMBRIGHT) {
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

    
    if (videocore_ctrl & CTRL_SYNCGEN) {
        /* sync pin */
        if (hw_rev <= 0x02) {
            _TRISA9 = 0;
            _LATA9 = 1;
        } else {
            _TRISA4 = 0;
            _LATA4 = 1;
        }

        /* timer */
        T4CON = 0x8010;
        _T4IP = 3;
        _T4IE = 1;
        _T4IF = 0;
        /* period = 1 / (70000000 / 8) * 56 = 6.4us */
        PR4 = 56;
    }

    if (videocore_ctrl & CTRL_COMPSYNC) {
        /* analog input */
        TRISBbits.TRISB0 = 1;
        ANSELBbits.ANSB0 = 1;
        
        /* pull downs */
        _CNPUA2 = 0;
        _CNPDA2 = 1;
        _CNPUA3 = 0;
        _CNPDA3 = 1;
        

        /* vref */
#if defined (__dsPIC33EP512GM604__)
        CVR1CONbits.CVR1OE = 0;
//        CVR1CONbits.CVR2OE = 0;
        CVR1CONbits.VREFSEL = 0;
        CVR1CONbits.CVRR = 1;
        CVR1CONbits.CVRSS = 0;
        CVR1CONbits.CVR = 3;
        CVR1CONbits.CVREN = 1;
#else
        CVRCONbits.CVR1OE = 0;
        CVRCONbits.CVR2OE = 0;
        CVRCONbits.VREFSEL = 0;
        CVRCONbits.CVRR = 1;
        CVRCONbits.CVRSS = 0;
        CVRCONbits.CVR = 3;
        CVRCONbits.CVREN = 1;
#endif
        
        /* comp */
        CM2CONbits.COE = 0;
        CM2CONbits.CPOL = 1;
        CM2CONbits.OPMODE = 0;
        //CM2CONbits.COUT = 0;
        CM2CONbits.EVPOL = 0b10;
        CM2CONbits.CREF = 1;
        CM2CONbits.CCH = 0b00;

        CM2FLTRbits.CFLTREN = 1;
        CM2FLTRbits.CFSEL = 1;
        CM2FLTRbits.CFDIV = 2;

        CMSTATbits.PSIDL = 0;

        _CMIP = 5;

        CM2CONbits.CEVT = 0;
        IFS1bits.CMIF = 0;
        _CMIE = 1;
        CM2CONbits.CON = 1;
    }
    
    if (videocore_ctrl & CTRL_DACBRIGHT) {
        video_init_dac();
        
        video_dac_read();
        video_update_dac();
        video_dac_read();
    }
}

void video_apply_config(struct video_config *cfg)
{
    /* brightness */
    if (hw_rev < 0x03) {
        OC1RS = 0x100 + cfg->brightness;
    } else {
        video_update_dac();
    }

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

void video_get_size(unsigned int *xsize, unsigned int *ysize)
{
    *xsize = video_xsizes[config.video.x_size_id].xsize;
    *ysize = config.video.y_size;
    if (config.video.standard & VIDEO_STANDARD_SCAN_MASK)
        *ysize *= 2;
}

void video_pause(void)
{
    while (sram_busy);

    if (videocore_ctrl & CTRL_SYNCGEN) {
        _T4IE = 0;
    }

    if (videocore_ctrl & CTRL_COMPSYNC) {
        _CMIE = 0;
        CM2CONbits.CEVT = 0;
        _CMIF = 0;
    } else {
        _INT2IE = 0;
        _INT1IE = 0;
    }
}

void video_resume(void)
{
    if (videocore_ctrl & CTRL_COMPSYNC) {
        line = 0;
        CM2CONbits.CEVT = 0;
        _CMIF = 0;
        _CMIE = 1;
    } else {
        _INT1IF = 0;
        _INT1IE = 1;
    }

    if (videocore_ctrl & CTRL_SYNCGEN) {
        _T4IF = 0;
        _T4IE = 1;
    }
}

void free_mem(void)
{
    scratchpad[0].alloc_size = 0;
    scratchpad[1].alloc_size = 0;

    canvas_pipe.prd = canvas_pipe.pwr = 0;
    rendering_canvas = NULL;
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
    if (ca->lock)
        return -1;
    clear_canvas(ca->buf, ca->size, b);
    return 0;
}


void schedule_canvas(struct canvas *ca)
{
    canvas_pipe.ca[canvas_pipe.pwr++] = ca;
    canvas_pipe.pwr &= MAX_CANVAS_PIPE_MASK;
    ca->lock = 1;
}


static void render_process(void)
{
    static unsigned int y1, y;
    __eds__ static unsigned char *b;
    static union sram_addr addr;
    static unsigned int xsize;

    unsigned int x;

    for (;;) {
        if (rendering_canvas == NULL) {
            if (canvas_pipe.prd == canvas_pipe.pwr)
                return;

            rendering_canvas = canvas_pipe.ca[canvas_pipe.prd];

            y = rendering_canvas->y;
            y1 = rendering_canvas->y + rendering_canvas->height;
            x = rendering_canvas->x >> 2;
            b = rendering_canvas->buf;

            xsize = (video_xsizes[config.video.x_size_id].xsize) >> 2;
            addr.l = x + ((unsigned long) xsize *  y);
        } else {
            /* render */
            for (;;) {
                if (sram_busy)
                    return;

                CS_LOW;
                sram_byteo_sqi(SRAM_WRITE);
                sram_byteo_sqi(addr.b2);
                sram_byteo_sqi(addr.b1);
                sram_byteo_sqi(addr.b0);
                b = copy_line(b, rendering_canvas->rwidth);
                CS_HIGH;

                if (++y == y1)
                    break;

                addr.l += xsize;
            }
            rendering_canvas->lock = 0;
            canvas_pipe.prd++;
            canvas_pipe.prd &= MAX_CANVAS_PIPE_MASK;
            rendering_canvas = NULL;
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


void init_video(void)
{
    video_init_sram();
    video_init_hw();

    params_add(params_video);
    process_add(render_process);
}


static char line_busy = 0;

/* line timer */
void __attribute__((__interrupt__, auto_psv )) _T2Interrupt()
{
    if (PR2 != LINE_TMR) {
        T2CONbits.TON = 0;
        if (hw_rev <= 0x02)
            OE_RAM = 0;
        else
            OE_RAM2 = 0;
        SPI2STATbits.SPIEN = 1;
        PR2 = LINE_TMR;
        T2CONbits.TON = 1;
    } else {
        if (hw_rev <= 0x02)
            OE_RAM = 1;
        else
            OE_RAM2 = 1;
        _RP56R = 0;
        CS_HIGH;
        SRAM_OUT;
        SPI2STATbits.SPIEN = 0;
        T2CONbits.TON = 0;
        line_busy = 0;
    }
    _T2IF = 0;
}

/* vsync interrupt */
void __attribute__((__interrupt__, auto_psv )) _INT1Interrupt()
{
    if (sram_busy) {
        if (T2CONbits.TON == 1) {
            T2CONbits.TON = 0;
            _T2IF = 0;
            OE_RAM = 1;
            _RP56R = 0;
            CS_HIGH;
            SRAM_OUT;
            SPI2STATbits.SPIEN = 0;
        }

        if (line >= config.video.y_offset) {
            sram_exit_sdi();
            CS_LOW;
            sram_byte_spi(SRAM_QIO);
            CS_HIGH;
            SRAM_OUTQ;
        }
        sram_busy = 0;
    }

    last_line_cnt = line;
    line = 0;
    int_sync_cnt = 0;
    _INT2IF = 0;
    _INT2IE = 1;
    LED = 1;
    _INT1IF = 0;
}


#define INT_X_OFFSET    (45)
#define CNT_INT_MODE    (10 * 1000)

static inline void render_line(void)
{
    static union sram_addr addr __attribute__((aligned(2)));
    static unsigned int osdxsize;
    static unsigned int x_offset;
    static unsigned int last_line = 200;

    if (line < config.video.y_offset-2) {
        /* do nothing */
    } else if (line < config.video.y_offset-1) {
        /* T-2: setup vars */
        osdxsize = video_xsizes[config.video.x_size_id].xsize;

        /* calc last_line */
        last_line = config.video.y_size + config.video.y_offset;

        /* avoid sram_busy soft-locks */
        if (last_line > last_line_cnt - 20)
            last_line = last_line_cnt - 20;

        /* auto detect video standard */
#if 0
        if (last_line_cnt < 300)
            config.video.standard |= VIDEO_STANDARD_MASK;
        else
            config.video.standard &= ~VIDEO_STANDARD_MASK;
#endif
        
        sram_busy = 1;
        addr.l = 0;

        /* TODO: Fix for compsync mode */
        if (config.video.standard && VIDEO_STANDARD_SCAN_MASK) {
            if (int_sync_cnt < CNT_INT_MODE)
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

        x_offset = config.video.x_offset;
        if (int_sync_cnt > CNT_INT_MODE)
            x_offset += INT_X_OFFSET;

    } else if (line < last_line) {
        /* render */
        line_busy = 1;
        
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

        /* start x_offset timer */
        PR2 = x_offset * 5;
        T2CONbits.TON = 1;

        /* calc next address */
        if (config.video.standard & VIDEO_STANDARD_SCAN_MASK) {
            addr.l += (unsigned long) ((osdxsize/4) * 2);
        } else {
            addr.l += (unsigned long) (osdxsize/4);
        }
    } else if (line == last_line ) {
        /* switch sram back to sqi mode */
        sram_exit_sdi();
        CS_LOW;
        sram_byte_spi(SRAM_QIO);
        CS_HIGH;
        SRAM_OUTQ;
        sram_busy = 0;
    }
}


void __attribute__((__interrupt__, auto_psv )) _INT2Interrupt()
{
    line++;
    render_line();
    _INT2IF = 0;
}


void __attribute__((interrupt, auto_psv)) _CM1Interrupt(void)
{
    static unsigned int last_cnt = 0;
    static unsigned char vcnt = 0;

    unsigned int cnt = ext_sync_cnt - last_cnt;
    last_cnt = ext_sync_cnt;

    if (line_busy) {
        CM2CONbits.CEVT = 0;
        _CMIF = 0;
        _CMIE = 0;
        return;
    }

    //if(CMSTATbits.C2EVT)
    //{
        //ext_sync_cnt = 0;
        //printf("%d\n", last_cnt);

        if ((cnt > 3) && (cnt < 7)) {
            vcnt++;
        } else if ((cnt > 8) && (cnt < 12)) {
            if (vcnt > 13) {
                
                if (sram_busy) {
                    sram_exit_sdi();
                    CS_LOW;
                    sram_byte_spi(SRAM_QIO);
                    CS_HIGH;
                    SRAM_OUTQ;
                    sram_busy = 0;
                }
                
                
                /* vsync */
                last_line_cnt = line + 13;
                line = 10;
                int_sync_cnt = 0;
                last_cnt = 0;
                LED = 1;
                _CMIP = 5;
                _T4IP = 4;
                
                /* pull downs */
                _CNPUA2 = 0;
                _CNPDA2 = 1;
                _CNPUA3 = 0;
                _CNPDA3 = 1;
            } else {
                line++;
                render_line();
            }
            vcnt = 0;
        } else {
            vcnt = 0;
            if (_CMIP == 4) {
                _CNPDA2 = 0;
                _CNPUA2 = 1;
                _CNPDA3 = 0;
                _CNPUA3 = 1;
                _CMIE = 0;
            }
        }
        CM2CONbits.CEVT = 0;
    //}

    _CMIF = 0;
}


void __attribute__((__interrupt__, auto_psv )) _T4Interrupt()
{
    static unsigned char cnt;

    ext_sync_cnt++;

    if (int_sync_cnt < CNT_INT_MODE) {
        /* ext sync */
        int_sync_cnt++;
    } else if (int_sync_cnt < CNT_INT_MODE + 1) {
        /* prepare int sync */
        last_line_cnt = 312;
        line = 0;
        odd = 1;
        int_sync_cnt++;
        _INT2IE = 0;
        LED = 0;
        cnt = 0;
        _CMIP = 4;
        _T4IP = 5;

        /* pull ups */
        _CNPDA2 = 0;
        _CNPUA2 = 1;
        _CNPDA3 = 0;
        _CNPUA3 = 1;

        _CMIE = 0;
        _CMIF = 0;
    } else {
        /* int sync */
        cnt++;
        
        if (hw_rev <= 0x02) {
            if (odd == 1) {
                if (line < 2) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA9 = 1;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 5) || (cnt == 7)) {
                        _LATA9 = 1;
                    }
                } else if ((line < 5) || (line > 309)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA9 = 1;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA9 = 0;
                    } else if (cnt == 2) {
                        _LATA9 = 1;
                    }
                }
            } else {
                if (line < 1) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 2) || (cnt == 10)) {
                        _LATA9 = 1;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA9 = 1;
                    }

                } else if ((line < 5) || (line > 308)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA9 = 0;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA9 = 1;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA9 = 0;
                    } else if (cnt == 2) {
                        _LATA9 = 1;
                    }
                }
            }
        } else {
            if (odd == 1) {
                if (line < 2) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA4 = 1;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 5) || (cnt == 7)) {
                        _LATA4 = 1;
                    }
                } else if ((line < 5) || (line > 309)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA4 = 1;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA4 = 0;
                    } else if (cnt == 2) {
                        _LATA4 = 1;
                    }
                }
            } else {
                if (line < 1) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 2) || (cnt == 10)) {
                        _LATA4 = 1;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA4 = 1;
                    }
                } else if ((line < 5) || (line > 308)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA4 = 0;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA4 = 1;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA4 = 0;
                    } else if (cnt == 2) {
                        _LATA4 = 1;
                    }
                }
            }
        }
        if (cnt == 1)
            render_line();

        if (cnt > 9) {
            line++;
            cnt = 0;
            if (line == 312) {
                last_line_cnt = line;
                line = 0;
                odd = odd ^ 1;
            }

            if (odd && (line < 100) ) {
                CM2CONbits.CEVT = 0;
                _CMIF = 0;
                _CMIE = 1;
            } else {
                _CMIE = 0;
            }
        }
    }

    _T4IF = 0;
}
