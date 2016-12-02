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

#define VIN_SEL LATAbits.LATA4

#define LINE_TMR_ (278*12)-2
static unsigned int LINE_TMR = LINE_TMR_;

#define CNT_INT_MODE    (10 * 1000)

#define CTRL_SYNCGEN        0x01
#define CTRL_COMPSYNC       0x02
#define CTRL_PWMBRIGHT      0x04
#define CTRL_DACBRIGHT      0x08
#define CTRL_EXTVREF        0x10
#define CTRL_DUALVIN        0x20


extern struct alceosd_config config;
extern unsigned char hw_rev;

void video_apply_config_cbk(void);

const struct param_def params_video[] = {
    PARAM("VIDE0_STD", MAV_PARAM_TYPE_UINT8, &config.video[0].mode, NULL),
    PARAM("VIDE0_XSIZE", MAV_PARAM_TYPE_UINT8, &config.video[0].x_size_id, video_apply_config_cbk),
    PARAM("VIDE0_YSIZE", MAV_PARAM_TYPE_UINT16, &config.video[0].y_size, NULL),
    PARAM("VIDE0_XOFFSET", MAV_PARAM_TYPE_UINT16, &config.video[0].x_offset, NULL),
    PARAM("VIDE0_YOFFSET", MAV_PARAM_TYPE_UINT16, &config.video[0].y_offset, NULL),

    PARAM("VIDE1_STD", MAV_PARAM_TYPE_UINT8, &config.video[1].mode, NULL),
    PARAM("VIDE1_XSIZE", MAV_PARAM_TYPE_UINT8, &config.video[1].x_size_id, video_apply_config_cbk),
    PARAM("VIDE1_YSIZE", MAV_PARAM_TYPE_UINT16, &config.video[1].y_size, NULL),
    PARAM("VIDE1_XOFFSET", MAV_PARAM_TYPE_UINT16, &config.video[1].x_offset, NULL),
    PARAM("VIDE1_YOFFSET", MAV_PARAM_TYPE_UINT16, &config.video[1].y_offset, NULL),

    PARAM_END,
};
const struct param_def params_video0v1[] = {
    PARAM("VIDE0_BRIGHT", MAV_PARAM_TYPE_UINT16, &config.video[0].brightness, video_apply_config_cbk),
    PARAM("VIDE1_BRIGHT", MAV_PARAM_TYPE_UINT16, &config.video[1].brightness, video_apply_config_cbk),
    PARAM_END,
};
const struct param_def params_video0v3[] = {
    PARAM("VIDE0_WHITE", MAV_PARAM_TYPE_UINT8, &config.video[0].white_lvl, video_apply_config_cbk),
    PARAM("VIDE0_GRAY", MAV_PARAM_TYPE_UINT8, &config.video[0].gray_lvl, video_apply_config_cbk),
    PARAM("VIDE0_BLACK", MAV_PARAM_TYPE_UINT8, &config.video[0].black_lvl, video_apply_config_cbk),

    PARAM("VIDE1_WHITE", MAV_PARAM_TYPE_UINT8, &config.video[1].white_lvl, video_apply_config_cbk),
    PARAM("VIDE1_GRAY", MAV_PARAM_TYPE_UINT8, &config.video[1].gray_lvl, video_apply_config_cbk),
    PARAM("VIDE1_BLACK", MAV_PARAM_TYPE_UINT8, &config.video[1].black_lvl, video_apply_config_cbk),
    PARAM_END,
};

const struct param_def params_video0v4[] = {
    PARAM("VIDEO_CHMODE", MAV_PARAM_TYPE_UINT8, &config.video_sw.mode, NULL),
    PARAM("VIDEO_CHTIME", MAV_PARAM_TYPE_UINT8, &config.video_sw.time, NULL),
    PARAM("VIDEO_CH", MAV_PARAM_TYPE_UINT8, &config.video_sw.ch, NULL),
    PARAM("VIDEO_CHMAX", MAV_PARAM_TYPE_UINT16, &config.video_sw.ch_max, NULL),
    PARAM("VIDEO_CHMIN", MAV_PARAM_TYPE_UINT16, &config.video_sw.ch_min, NULL),
    PARAM_END,
};

static volatile unsigned int int_sync_cnt = 0;
volatile unsigned char sram_busy = 0;
volatile unsigned int line, last_line_cnt = 0;
volatile unsigned char odd = 0;
static struct canvas *rendering_canvas = NULL;

static unsigned char videocore_ctrl = 0;

unsigned long nbusy_time = 0, render_time = 0, wait_time = 0, mpw = 0, mpwt = 0;
int video_pid = -1;

static struct video_config *cfg = &config.video[0];

const struct osd_xsize_tbl video_xsizes[] = {
    { .xsize = 420, .clk_ps = 0 } ,
    { .xsize = 480, .clk_ps = 1 } ,
    { .xsize = 560, .clk_ps = 2 } ,
    { .xsize = 672, .clk_ps = 3 } ,
};


#define MAX_CANVAS_PIPE_MASK (0x1f)

static struct canvas_pipe_s {
    struct canvas *ca[MAX_CANVAS_PIPE_MASK+1];
    unsigned char prd, pwr;
    unsigned char peak;
} canvas_pipe = {
    .pwr = 0,
    .prd = 0,
};


#define SCRATCHPAD1_SIZE 0x5000
#define SCRATCHPAD2_SIZE 0x2000
__eds__ unsigned char scratchpad1[SCRATCHPAD1_SIZE]  __attribute__ ((eds, noload, address(0x8000)));
__eds__ unsigned char scratchpad2[SCRATCHPAD2_SIZE]  __attribute__ ((eds, noload, address(0x6000)));

struct scratchpad_s {
    __eds__ unsigned char *mem;
    unsigned int alloc_size;
    unsigned int alloc_max;
};

struct scratchpad_s scratchpad[2] = {
    {   .mem = scratchpad2,
        .alloc_size = 0, .alloc_max = SCRATCHPAD2_SIZE },
    {   .mem = scratchpad1,
        .alloc_size = 0, .alloc_max = SCRATCHPAD1_SIZE },
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
    LATC |= 0x000f;
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
            shell_printf("test_ram: addr 0x05x expected = 0x%02x read = 0x%02x\n", i, b, r);
            return 1;
        }
    }
    CS_HIGH;
    SRAM_OUTQ;
    return 0;
}

static void test_sram(void)
{
    unsigned char test[4] = {0xff, 0x5a, 0xa5, 0x00};
    unsigned char i, j;
    shell_printf("Testing SRAM\n");
    
    for (i = 0; i < 4; i++) {
        j = test[i];
        if (test_sram_0(j))
            shell_printf("0x%02x (fail)\n", j);
        else
            shell_printf("0x%02x (pass)\n", j);
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
    if (hw_rev >= 0x03)
        _RP56R = 0;
    else
        _RP54R = 0;

    /* force a spi mode from sdi */
    sram_exit_sdi();
    /* force a spi mode from sqi */
    sram_exit_sqi();

    /* set mode register (sequential rw mode) */
    CS_LOW;
    sram_byte_spi(SRAM_WMODE);
    sram_byte_spi(0x40);
    CS_HIGH;

    /* IOs are now in spi mode, set SQI */
    CS_LOW;
    sram_byte_spi(SRAM_QIO);
    CS_HIGH;
    SRAM_OUTQ;

    /* test SRAM */
    //test_sram();

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

#define I2C_STATE_START     0
#define I2C_STATE_TRMT      1
#define I2C_STATE_RECV      2
#define I2C_STATE_ACK       3
#define I2C_STATE_STOP      4
int i2c_wait(u8 state)
{
    u8 done = 0;
    u16 timeout = 20;
    do {
        switch (state) {
            case I2C_STATE_START:
                done = (I2C1CONbits.SEN == 0);
                break;
            case I2C_STATE_TRMT:
                done = (I2C1STATbits.TRSTAT == 0);
                break;
            case I2C_STATE_RECV:
                done = (I2C1CONbits.RCEN == 0);
                break;
            case I2C_STATE_ACK:
                done = (I2C1CONbits.ACKEN == 0);
                break;
            case I2C_STATE_STOP:
                done = (I2C1CONbits.PEN == 0);
                break;
            default:
                return 1;
        }
        if (done)
            return 0;
        mdelay(10);
    } while (--timeout > 0);
    return 1;
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

#define VIDEO_DAC_BUF_SIZE  (3*2*4)
static int video_read_dac(unsigned char *dac_status)
{
    unsigned char i;
    
    I2C1CONbits.SEN = 1;
    if (i2c_wait(I2C_STATE_START))
        return 1;
    
    I2C1TRN = 0xc1;
    if (i2c_wait(I2C_STATE_TRMT))
        return 1;

    for (i = 0; i < VIDEO_DAC_BUF_SIZE; i++) {
        I2C1CONbits.RCEN = 1;
        if (i2c_wait(I2C_STATE_RECV))
            return 1;
        
        dac_status[i] = I2C1RCV;

        I2C1CONbits.ACKEN = 1;
        if (i2c_wait(I2C_STATE_ACK))
            return 1;
    }

    I2C1CONbits.PEN = 1;
    if (i2c_wait(I2C_STATE_STOP))
        return 1;
    
    return 0;
}

static int video_update_dac(struct video_config *cfg)
{
    unsigned int dac_values[4];
    unsigned char i;
    
    dac_values[0] = cfg->white_lvl << 4;
    dac_values[1] = cfg->gray_lvl << 4;
    dac_values[2] = cfg->black_lvl << 4;
    dac_values[3] = 0;

    I2C1CONbits.SEN = 1;
    if (i2c_wait(I2C_STATE_START))
        return 1;
    
    I2C1TRN = 0xc0;
    if (i2c_wait(I2C_STATE_TRMT))
        return 1;

    for (i = 0; i < 4; i++) {
        I2C1TRN = (dac_values[i] >> 8) & 0x0f;
        if (i2c_wait(I2C_STATE_TRMT))
            return 1;
    
        I2C1TRN = dac_values[i] & 0xff;
        if (i2c_wait(I2C_STATE_TRMT))
            return 1;

    }
    I2C1CONbits.PEN = 1;
    if (i2c_wait(I2C_STATE_STOP))
        return 1;

    return 0;
}

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
        case 0x04:
            videocore_ctrl |= CTRL_SYNCGEN;
            videocore_ctrl |= CTRL_DACBRIGHT;
            videocore_ctrl |= CTRL_COMPSYNC;
            videocore_ctrl |= CTRL_EXTVREF;
            videocore_ctrl |= CTRL_DUALVIN;
            break;
    }


    SPI2CON1bits.CKP = 0; /* idle low */
    SPI2CON1bits.CKE = 1;
    SPI2CON1bits.MSTEN = 1;
    SPI2CON1bits.DISSDO = 1;
    SPI2CON1bits.DISSCK = 0;
    SPI2CON1bits.PPRE = 3;
    SPI2CON1bits.SPRE = video_xsizes[cfg->x_size_id].clk_ps;
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
        _LATA2 = 0;
        _LATA3 = 0;
        /* pull downs */
        _CNPUA2 = 0;
        _CNPDA2 = 1;
        _CNPUA3 = 0;
        _CNPDA3 = 1;
    }
    
    /* generic line timer */
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
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
        OC1RS = 0x100 + cfg->brightness;
        OC1CON1bits.OCM = 0b110;
    }

    
    if (videocore_ctrl & CTRL_SYNCGEN) {
        /* sync pin */
        switch (hw_rev) {
            default:
                _TRISA9 = 0;
                _LATA9 = 1;
                break;
            case 0x03:
                _TRISA4 = 0;
                _LATA4 = 1;
                break;
            case 0x04:
                _TRISA7 = 0;
                _LATA7 = 0;
                break;
        }

        /* timer */
        T4CON = 0x8010;
        _T4IP = 3;
        /* period = 1 / (70000000 / 8) * 56 = 6.4us */
        PR4 = 56;
    }

    if (videocore_ctrl & CTRL_COMPSYNC) {
        /* analog input */
        TRISBbits.TRISB0 = 1;
        ANSELBbits.ANSB0 = 1;

        /* vref */
#if defined (__dsPIC33EP512GM604__)
        if (videocore_ctrl & CTRL_EXTVREF) {
            CVR1CONbits.CVRR = 0;
            CVR1CONbits.CVRR1 = 0;
            CVR1CONbits.CVRSS = 1;
            CVR1CONbits.CVR = 2;
        } else {
            CVR1CONbits.CVRR = 1;
            CVR1CONbits.CVRR1 = 0;
            CVR1CONbits.CVRSS = 0;
            CVR1CONbits.CVR = 4;
        }
        CVR1CONbits.CVROE = 0;
        CVR1CONbits.VREFSEL = 0;
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

        _RP54R = 0x19;
        _IC1R = 54;
        /* input capture */
        IC1CON1bits.ICM = 0b000;
        IC1CON1bits.ICTSEL = 7;
        IC1CON2bits.ICTRIG = 0;
        IC1CON2bits.SYNCSEL = 0;
        IC1CON1bits.ICI = 0;
        IPC0bits.IC1IP = 4;
        IFS0bits.IC1IF = 0;
        IEC0bits.IC1IE = 1;
        IC1CON1bits.ICM = 0b001;

        /* comp */
        CM2CONbits.COE = 1;
        CM2CONbits.CPOL = 1;
        CM2CONbits.OPMODE = 0;
        CM2CONbits.COUT = 0;
        CM2CONbits.EVPOL = 0b00;
        CM2CONbits.CREF = 1;
        CM2CONbits.CCH = 0b00;
        CM2FLTRbits.CFLTREN = 0;
        CM2CONbits.CON = 1;

        T5CON = 0x0010;
        /* period = 1 / (70000000 / 8) * PR5 */
        PR5 = 500;
        _T5IP = 7;
        _T5IF = 0;
        _T5IE = 1;
    }
    
    if (videocore_ctrl & CTRL_DACBRIGHT) {
        video_init_dac();
    }
    
    if (videocore_ctrl & CTRL_DUALVIN) {
        _TRISA4 = 0;
        VIN_SEL = 1;
    }
}

void video_apply_config(unsigned char profile)
{
    if (profile < CONFIG_MAX_VIDEO)
        cfg = &config.video[profile];

    /* brightness */
    if (hw_rev < 0x03) {
        OC1RS = 0x100 + cfg->brightness;
    } else {
        if (video_update_dac(cfg))
            shell_printf("error updating DAC values\n");
    }
    
    if (hw_rev == 0x04) {
        VIN_SEL = (cfg->mode & VIDEO_MODE_INPUT_MASK) ? 0 : 1;
    }

    /* pixel clock */
    INTCON2bits.GIE = 0;
    SPI2STATbits.SPIEN = 0;
    SPI2CON1bits.SPRE = video_xsizes[cfg->x_size_id].clk_ps;
    INTCON2bits.GIE = 1;

    if (cfg->mode & VIDEO_MODE_SYNC_MASK) {
        _T4IF = 0;
        _T4IE = 1;
    } else {
        _T4IE = 0;
    }
}

void video_apply_config_cbk(void)
{
    if (cfg->x_size_id >= VIDEO_XSIZE_END)
        cfg->x_size_id = VIDEO_XSIZE_END - 1;
    video_apply_config(VIDEO_ACTIVE_CONFIG);
}

void video_get_size(unsigned int *xsize, unsigned int *ysize)
{
    *xsize = video_xsizes[cfg->x_size_id].xsize;
    *ysize = cfg->y_size;
    if (cfg->mode & VIDEO_MODE_SCAN_MASK)
        *ysize *= 2;
}

void free_mem(void)
{
    scratchpad[0].alloc_size = 0;
    scratchpad[1].alloc_size = 0;

    canvas_pipe.prd = canvas_pipe.pwr = 0;
    rendering_canvas = NULL;
}

static void set_canvas_pos(struct canvas *c, struct widget_config *wcfg)
{
    u16 osdxsize, osdysize;
    video_get_size(&osdxsize, &osdysize);
    
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
}

int alloc_canvas(struct canvas *c, void *widget_cfg)
{
    u16 i;
    struct widget_config *wcfg = widget_cfg;

    c->width = (c->width & 0xfffc);
    c->rwidth = c->width >> 2;
    c->size = c->rwidth * c->height;

    if (c->size == 0) {
        c->lock = 1;
        return 0;
    }
    
    for (i = 0; i < 2; i++) {
        if ((scratchpad[i].alloc_size + c->size) < scratchpad[i].alloc_max)
            break;
    }
    if (i == 2) {
        c->lock = 1;
        return -1;
    }
    c->buf = &scratchpad[i].mem[scratchpad[i].alloc_size];
    scratchpad[i].alloc_size += c->size;
    set_canvas_pos(c, wcfg);
    c->lock = 0;
    
    c->buf_nr = i;
    
    return 0;
}

void free_canvas(struct canvas *c)
{
    struct scratchpad_s *s = &scratchpad[c->buf_nr];
    __eds__ u8 *dst = c->buf;
    __eds__ u8 *src = c->buf + c->size;
    __eds__ u8 *end = &s->mem[s->alloc_size];
    
    do {
        *(dst++) = *(src++);
    } while (src != end);
    
    s->alloc_size -= c->size;
    
}

int init_canvas(struct canvas *ca)
{
    if (ca->lock)
        return -1;
    clear_canvas(ca->buf, ca->size, 0);
    return 0;
}


void schedule_canvas(struct canvas *ca)
{
    canvas_pipe.ca[canvas_pipe.pwr++] = ca;
    canvas_pipe.pwr &= MAX_CANVAS_PIPE_MASK;
    canvas_pipe.peak = max(canvas_pipe.peak, canvas_pipe.pwr - canvas_pipe.prd);
    ca->lock = 1;
}


static void render_process(void)
{
    static unsigned int y1, y;
    __eds__ static unsigned char *b;
    static union sram_addr addr;
    static unsigned int xsize;
    
    static unsigned long t = 0, t2 = 0, t3 = 0;
    

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

            xsize = (video_xsizes[cfg->x_size_id].xsize) >> 2;
            addr.l = x + ((unsigned long) xsize *  y);
            
            t3 = get_micros();
            if (sram_busy)
                t2 = t3;
        } else {
            /* render */
            t = get_micros();
            
            if ((t2 > 0) && (!sram_busy)) {
                wait_time += (t - t2);
                t2 = 0;
            }
            
            for (;;) {
                if (sram_busy) {
                    render_time += (get_micros() - t);
                    return;
                }

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

            mpw += (get_micros() - t3);
            mpwt++;
            
            render_time += (get_micros() - t);
        }
    }
}


/* blocking render */
static void render_canvas(struct canvas *ca)
{
    __eds__ unsigned char *b = ca->buf;
    union sram_addr addr;
    u16 osdxsize, osdysize, h;
    u32 y_offset;
    
    video_get_size(&osdxsize, &osdysize);

    y_offset = (u32) osdxsize >> 2;
    addr.l = (u32) ca->y * y_offset + (ca->x >> 2);

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
        addr.l += y_offset;
    }
}

void reconfig_canvas(struct canvas *ca, void *widget_cfg)
{
    struct widget_config *cfg = (struct widget_config*) widget_cfg;
    clear_canvas(ca->buf, ca->size, 0);
    render_canvas(ca);
    set_canvas_pos(ca, cfg);
}


#define VIDEO_TIMER_IDLE   (0xff)

static void video_switch_task(struct timer *t, void *d)
{
    static unsigned char tmr = VIDEO_TIMER_IDLE;
    static unsigned char prev_val = 255, source_mode = 0xff;
    struct ch_switch *sw = d;
    unsigned int val;
    u32 ch_age;
    
    switch (sw->mode) {
        case SW_MODE_CHANNEL:
        default:
            val = get_sw_state(sw, &ch_age);
            if (val > 50)
                val = 1;
            else
                val = 0;
            if ((unsigned char) val != prev_val) {
                /* change video input */
                if (val)
                    cfg->mode |= VIDEO_MODE_INPUT_MASK;
                else
                    cfg->mode &= ~VIDEO_MODE_INPUT_MASK;
                video_apply_config_cbk();
                prev_val = (unsigned char) val;
            }
            break;
        case SW_MODE_TOGGLE:
            val = get_sw_state(sw, &ch_age);
            if (ch_age > 2000)
                break;

            if (val < 50)
                val = 1;
            else
                val = 2;            

            /* store idle position */
            if (prev_val == 255)
                prev_val = (unsigned char) val;
            
            /* idle */
            if (tmr == VIDEO_TIMER_IDLE) {
                if ((unsigned char) val != prev_val)
                    tmr = 0;
            } else if (tmr < sw->time) {
                tmr++;
                /* switch returned to idle position */
                if (prev_val == (unsigned char) val) {
                    /* swap video in */
                    if (cfg->mode & VIDEO_MODE_INPUT_MASK)
                        cfg->mode &= ~VIDEO_MODE_INPUT_MASK;
                    else
                        cfg->mode |= VIDEO_MODE_INPUT_MASK;
                    video_apply_config_cbk();
                    tmr = VIDEO_TIMER_IDLE;
                }
            } else {
                /* wait until switch returns to idle state */
                if ((unsigned char) val == prev_val)
                    tmr = VIDEO_TIMER_IDLE;
            }            
            break;
        case SW_MODE_FLIGHTMODE:
        {
            mavlink_heartbeat_t *hb = mavdata_get(MAVLINK_MSG_ID_HEARTBEAT);
            unsigned char i;
            const unsigned char mode_ignore_list[] = {
                PLANE_MODE_CIRCLE, PLANE_MODE_AUTO,
                PLANE_MODE_RTL, PLANE_MODE_LOITER,
                COPTER_MODE_AUTO, COPTER_MODE_LOITER,
                COPTER_MODE_RTL, COPTER_MODE_CIRCLE,
            };

            if (tmr < sw->time)
                tmr++;
            else
                tmr = VIDEO_TIMER_IDLE;
            
            val = (unsigned char) hb->custom_mode;
            if (hb->type != MAV_TYPE_FIXED_WING)
                val += 100;

            /* don't switch input in case failsafe triggers */
            for (i = 0; i < sizeof(mode_ignore_list); i++)
                if (mode_ignore_list[i] == (unsigned char) val)
                    return;

            if ((unsigned char) val != prev_val) {
                /* mode changed */
                if (tmr != VIDEO_TIMER_IDLE) {
                    if (source_mode == (unsigned char) val) {
                        /* swap video in */
                        if (cfg->mode & VIDEO_MODE_INPUT_MASK)
                            cfg->mode &= ~VIDEO_MODE_INPUT_MASK;
                        else
                            cfg->mode |= VIDEO_MODE_INPUT_MASK;
                        video_apply_config_cbk();
                    }
                    tmr = VIDEO_TIMER_IDLE;
                } else {
                    tmr = 0;
                    source_mode = prev_val;
                }
                prev_val = (unsigned char) val;
            }            
            break;
        }
        case SW_MODE_DEMO:
            tmr++;
            if (tmr > sw->time) {
                /* next video input */
                if (cfg->mode & VIDEO_MODE_INPUT_MASK)
                    cfg->mode &= ~VIDEO_MODE_INPUT_MASK;
                else
                    cfg->mode |= VIDEO_MODE_INPUT_MASK;
                video_apply_config_cbk();
                tmr = 0;
            }
            break;
    }
}

void init_video(void)
{
    video_init_sram();
    video_init_hw();

    params_add(params_video);
    if (hw_rev < 0x03)
        params_add(params_video0v1);
    else
        params_add(params_video0v3);
    
    if (hw_rev >= 0x04) {
        params_add(params_video0v4);
        add_timer(TIMER_ALWAYS, 100, video_switch_task, &config.video_sw);
    }
    
    video_pid = process_add(render_process, "RENDER", 100);
}

void video_pause(void)
{
    while (sram_busy);
    
    process_remove(video_pid);

    if (videocore_ctrl & CTRL_SYNCGEN) {
        _T4IE = 0;
    }

    if (videocore_ctrl & CTRL_COMPSYNC) {
        _IC1IE = 0;
    } else {
        _INT2IE = 0;
        _INT1IE = 0;
    }
}

void video_resume(void)
{
    if (videocore_ctrl & CTRL_COMPSYNC) {
        _IC1IF = 0;
        _IC1IE = 1;
    } else {
        _INT1IF = 0;
        _INT1IE = 1;
    }

    if ((videocore_ctrl & CTRL_SYNCGEN) && (cfg->mode & VIDEO_MODE_SYNC_MASK)) {
        _T4IF = 0;
        _T4IE = 1;
    }
    video_pid = process_add(render_process, "RENDER", 100);
}

/* line timer */
void __attribute__((__interrupt__, auto_psv )) _T2Interrupt()
{
    /* stop timer */
    T2CONbits.TON = 0;
    _T2IF = 0;
    
    if (PR2 != LINE_TMR) {
        if (hw_rev <= 0x02)
            OE_RAM = 0;
        else
            OE_RAM2 = 0;
        SPI2STATbits.SPIEN = 1;
        PR2 = LINE_TMR;
        T2CONbits.TON = 1;
    } else {
        if (hw_rev <= 0x02) {
            OE_RAM = 1;
        } else {
            OE_RAM2 = 1;

            if (int_sync_cnt >= CNT_INT_MODE) {
                _TRISA2 = 1;
                _TRISA3 = 1;
                _LATA2 = 1;
                _LATA3 = 1;
            } else {
                _TRISA2 = 0;
                _TRISA3 = 0;
                _TRISA2 = 1;
                _TRISA3 = 1;
            }
        }
        CS_HIGH;
        SRAM_OUT;
        if (hw_rev >= 0x03)
            _RP56R = 0;
        else
            _RP54R = 0;
        SPI2STATbits.SPIEN = 0;
    }
}

static void render_line(void)
{
    static union sram_addr addr __attribute__((aligned(2)));
    static unsigned int osdxsize;
    static unsigned int last_line = 200;
    static unsigned long t = 0;
    unsigned int x_offset;
    
    if (line < cfg->y_offset-2) {
        /* do nothing */
    } else if (line < cfg->y_offset-1) {
        /* setup vars */
        osdxsize = video_xsizes[cfg->x_size_id].xsize;

        /* calc last_line */
        last_line = cfg->y_size + cfg->y_offset;

        /* avoid sram_busy soft-locks */
        if (last_line > last_line_cnt - 10)
            last_line = last_line_cnt - 10;

        /* auto detect video standard */
#if 0
        if (last_line_cnt < 300)
            cfg->mode |= VIDEO_MODE_STANDARD_MASK;
        else
            cfg->mode &= ~VIDEO_MODE_STANDARD_MASK;
#endif
        
        sram_busy = 1;
        nbusy_time += (get_micros() - t);
        addr.l = 0;

        if ((videocore_ctrl & CTRL_COMPSYNC) == 0) {
            if (int_sync_cnt < CNT_INT_MODE)
                odd = PORTBbits.RB15;
        }
            
        if (cfg->mode & VIDEO_MODE_SCAN_MASK) {
            if (odd == 0) {
                addr.l += (osdxsize/4);
            }
        }
    } else if (line < cfg->y_offset) {
        sram_exit_sqi();
        /* make sure we are in sequential mode */
        CS_LOW;
        sram_byte_spi(SRAM_WMODE);
        sram_byte_spi(0x40);
        CS_HIGH;
        /* switch sram to sdi mode */
        CS_LOW;
        sram_byte_spi(SRAM_DIO);
        CS_HIGH;
        SRAM_OUT;
    } else if (line < last_line) {
        /* render */
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
        if (hw_rev >= 0x03)
            _RP56R = SCK2_O;
        else
            _RP54R = SCK2_O;

        /* line start timer - x_offset */
        x_offset = cfg->x_offset;

        if (hw_rev < 0x03) {
            x_offset += 75;
        }
        if (int_sync_cnt >= CNT_INT_MODE) {
            if (hw_rev < 0x03)
                x_offset += 50;
            else
                x_offset += 105;
        }
        PR2 = x_offset * 5;
        T2CONbits.TON = 1;

        /* calc next address */
        if (cfg->mode & VIDEO_MODE_SCAN_MASK) {
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
        t = get_micros();
    }
}


/* external sync */
void __attribute__((__interrupt__, auto_psv )) _INT1Interrupt()
{
    last_line_cnt = line;
    line = 0;
    int_sync_cnt = 0;
    _INT2IF = 0;
    _INT2IE = 1;
    _INT1IF = 0;
    _T4IP = 3;
}
void __attribute__((__interrupt__, auto_psv )) _INT2Interrupt()
{
    line++;
    render_line();
    _INT2IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv )) _T5Interrupt()
{
    T5CONbits.TON = 0;
    IC1CON1bits.ICM = 1;
    _T5IF = 0;
}

/* comparator + input capture sync */
void __attribute__((__interrupt__, auto_psv )) _IC1Interrupt(void)
{
    static unsigned int tp = 0xffff, last_cnt;
    static unsigned char vsync = 0;
    unsigned int cnt, t;
    unsigned char std = 0;

    /* empty fifo */
    do {
        cnt = IC1BUF;
    } while (IC1CON1bits.ICBNE != 0);
    IFS0bits.IC1IF = 0;
    
    t = cnt - last_cnt;
    if (t < 100)
        return;
    
    last_cnt = cnt;

    if (PORTCbits.RC6) {
        /* rising edge */
        /* vsync detector */
        if (abs(((long) t) - 1900) < 200)
            tp = (tp << 1) | 1;
        else if (abs(((long) t) - 170) < 70)
            tp = (tp << 1);
        else {
            tp = 0xffff;
        }

        switch (tp) {
            case 0b1000001111100000:
                std = 1; // pal
                break;
            case 0b0000111111000000:
                std = 2; // ntsc
                break;
            default:
                std = 0;
                break;
        }
        
        if (std > 0) {
            /* pull downs - input video */
            _LATA2 = 0;
            _LATA3 = 0;
            _CNPUA2 = 0;
            _CNPDA2 = 1;
            _CNPUA3 = 0;
            _CNPDA3 = 1;
            _T4IP = 3;
            vsync = std;
            last_line_cnt = line+10;
            line = 10;
            odd = 0;
            int_sync_cnt = 0;
            tp = 0xffff;
            return;
        }

        if (vsync) {
            if (abs(((long) t) - 329) < 100) {
                if (((vsync == 1) && (line < 314)) ||
                    ((vsync == 2) && (line < 264))) {
                    IC1CON1bits.ICM = 0;
                    TMR5 = 0;
                    T5CONbits.TON = 1;
                }
                line++;
                render_line();
            } else {
                /* lost sync */
                vsync = 0;
            }
        }
    } else {
        /* falling edge */
        if (vsync && (line == 10))
            if (abs(((long) t) - 2050) < 100)
                odd = 1;

    }
}


/* internal sync generator */
void __attribute__((__interrupt__, auto_psv )) _T4Interrupt()
{
    static unsigned char cnt;

    _T4IF = 0;

    if (int_sync_cnt < CNT_INT_MODE) {
        /* ext sync */
        int_sync_cnt++;
    } else if (int_sync_cnt < CNT_INT_MODE + 1) {
        /* prepare internal sync */

        last_line_cnt = 312;
        line = 0;
        odd = 1;
        int_sync_cnt++;
        if (hw_rev < 0x03) {
            _INT2IE = 0;
        } else {
            /* pull ups - black level */
            _CNPDA2 = 0;
            _CNPUA2 = 1;
            _CNPDA3 = 0;
            _CNPUA3 = 1;
        }
        cnt = 0;
        _T4IP = 5;
    } else {
        /* internal sync */
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
        } else if (hw_rev == 0x03) {
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
            }
        }  else if (hw_rev == 0x04) {
            if (odd == 1) {
                if (line < 2) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA7 = 0;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 5) || (cnt == 7)) {
                        _LATA7 = 0;
                    }
                } else if ((line < 5) || (line > 309)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA7 = 0;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA7 = 1;
                    } else if (cnt == 2) {
                        _LATA7 = 0;
                    }
                }
            } else {
                if (line < 1) {
                    /* vsync sync pulses */
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 2) || (cnt == 10)) {
                        _LATA7 = 0;
                    }
                } else if (line < 3) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 5) || (cnt == 10)) {
                        _LATA7 = 0;
                    }
                } else if ((line < 5) || (line > 309)) {
                    if ((cnt == 1) || (cnt == 6)) {
                        _LATA7 = 1;
                    } else if ((cnt == 2) || (cnt == 7)) {
                        _LATA7 = 0;
                    }
                } else {
                    /* normal sync pulse */
                    if (cnt == 1) {
                        _LATA7 = 1;
                    } else if (cnt == 2) {
                        _LATA7 = 0;
                    }
                }
            }
        }
        if (cnt == 1)
            render_line();

        if (cnt > 9) {
            line++;
            cnt = 0;
            if (((line == 312) && (odd == 1)) ||
                ((line == 313) && (odd == 0))) {
                last_line_cnt = line;
                line = 0;
                odd = odd ^ 1;
            }
        }
    }
}


static void shell_cmd_stats(char *args, void *data)
{
    unsigned char dac_status[VIDEO_DAC_BUF_SIZE], i;
    float f;
    
    shell_printf("Video config:\n");
    shell_printf(" standard=%s,%s\n",
        (cfg->mode & VIDEO_MODE_STANDARD_MASK) != 0 ? "ntsc" : "pal",
        (cfg->mode & VIDEO_MODE_SCAN_MASK) != 0 ? "interlaced" : "progressive");
    shell_printf(" internal sync=%u\n", (cfg->mode & VIDEO_MODE_SYNC_MASK) != 0 ? 1 : 0);
    if (hw_rev < 0x03) {
        shell_printf(" brightness=%u\n", cfg->brightness);
    } else {
        shell_printf(" levels: white=%u gray=%u %black=%u\n",
                cfg->white_lvl,
                cfg->gray_lvl,
                cfg->black_lvl);
        
        shell_printf(" dac settings:\n  ");
        if (video_read_dac(dac_status))
            shell_printf("error reading DAC values\n");
        for (i = 0; i < VIDEO_DAC_BUF_SIZE; i++) {
            shell_printf("0x%02x ", dac_status[i]);
            if (((i+1) % 3) == 0)
                shell_printf("\n  ");
        }
        shell_printf("\n");
    }

    shell_printf(" offset: x=%u y=%u\n",
            cfg->x_offset, cfg->y_offset);
    shell_printf(" size: x=%u y=%u\n",
            video_xsizes[cfg->x_size_id].xsize, cfg->y_size);
    
    shell_printf("\nVideocore stats:\n");
    shell_printf(" scratchpad memory: A=%u/%u B=%u/%u\n",
                scratchpad[0].alloc_size, scratchpad[0].alloc_max,
                scratchpad[1].alloc_size, scratchpad[1].alloc_max);
    shell_printf(" canvas fifo: size=%u peak=%u max=%u\n",
                (canvas_pipe.pwr - canvas_pipe.prd) & MAX_CANVAS_PIPE_MASK, canvas_pipe.peak, MAX_CANVAS_PIPE_MASK+1);
    shell_printf(" status: last_line_cnt=%u sram_busy=%u int_sync_cnt=%u\n",
                last_line_cnt, sram_busy, int_sync_cnt);
    shell_printf(" nbusy_time=%lu render_time=%lu wait_time=%lu R%=%.2f W%=%.2f\n",
                nbusy_time, render_time, wait_time,
                (float) (render_time * 100.0) / nbusy_time,
                (float) (wait_time * 100.0) / render_time);
    
    f = (float) mpw / ((float)mpwt);
    shell_printf(" mpw=%.2fus wps=%.2f\n",
                f, 1.0 / (f / 1e6));
    
}

#define SHELL_CMD_CONFIG_ARGS   12
static void shell_cmd_config(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_CONFIG_ARGS+1], *p;
    unsigned char t, i, j;
    int int_var;
    float f, vref;
    u16 profile;
    struct video_config *vcfg = cfg;
    
    t = shell_arg_parser(args, argval, SHELL_CMD_CONFIG_ARGS);

    if (t < 1) {
        shell_printf("arguments:\n");
        shell_printf(" -p <profile>     video profile: 0 or 1 (default is active profile)\n");
        shell_printf(" -s <standard>    video standard: [p]al or [n]tsc\n");
        shell_printf(" -m <scan_mode>   video scan mode: [p]rogressive or [i]nterlaced\n");
        shell_printf(" -i <int_sync>    internal sync: 0 or 1\n");

        switch (hw_rev) {
            default:
                shell_printf(" -t <brightness>  brightness: 0 (max) to 1000 (min)\n");
                break;
            case 0x03:
                shell_printf(" -w <white_lvl>   white voltage level: 0 to \n");
                shell_printf(" -g <gray_lvl>    gray voltage level: 0 to \n");
                shell_printf(" -b <black_lvl>   black voltage level: 0 to \n");
                break;
        }
        shell_printf(" -x <x_size>    horizontal video resolution:");
        for (i = 0; i < VIDEO_XSIZE_END; i++)
            shell_printf(" %d", video_xsizes[i].xsize);
        shell_printf("\n -y <y_size>    vertical video resolution\n");
        shell_printf(" -h <x_offset>  horizontal video offset\n");
        shell_printf(" -v <y_offset>  vertical video offset\n");

        
        shell_printf("debug:\n");
        shell_printf(" -r <comp_lvl>  comparator level\n");
        shell_printf(" -c <comp_ref>  comparator reference\n");
        
    } else {
        p = shell_get_argval(argval, 'p');
        if (p != NULL) {
            profile = atoi(p->val);
            profile = profile ? 1 : 0;
            vcfg = &config.video[profile];
        }
        
        for (i = 0; i < t; i++) {
            switch (argval[i].key) {
                case 's':
                    if (strcmp(argval[i].val, "p") == 0)
                        vcfg->mode &= ~VIDEO_MODE_STANDARD_MASK;
                    else
                        vcfg->mode |= VIDEO_MODE_STANDARD_MASK;
                case 'm':
                    if (strcmp(argval[i].val, "p") == 0)
                        vcfg->mode &= ~VIDEO_MODE_SCAN_MASK;
                    else
                        vcfg->mode |= VIDEO_MODE_SCAN_MASK;
                    break;
                case 'i':
                    if (strcmp(argval[i].val, "0") == 0)
                        vcfg->mode &= ~VIDEO_MODE_SYNC_MASK;
                    else
                        vcfg->mode |= VIDEO_MODE_SYNC_MASK;
                    break;
                case 't':
                    int_var = atoi(argval[i].val);
                    vcfg->brightness = (unsigned int) int_var;
                    break;

                case 'w':
                    int_var = atoi(argval[i].val);
                    vcfg->white_lvl = (unsigned char) int_var;
                    break;
                case 'g':
                    int_var = atoi(argval[i].val);
                    vcfg->gray_lvl = (unsigned char) int_var;
                    break;
                case 'b':
                    int_var = atoi(argval[i].val);
                    vcfg->black_lvl = (unsigned char) int_var;
                    break;
                case 'x':
                    int_var = atoi(argval[i].val);
                    for (j = 0; j < VIDEO_XSIZE_END; j++) {
                        if (int_var == video_xsizes[j].xsize) {
                            vcfg->x_size_id = j;
                            break;
                        }
                    }
                    break;
                case 'y':
                    int_var = atoi(argval[i].val);
                    vcfg->y_size = (unsigned int) int_var;
                    break;
                case 'h':
                    int_var = atoi(argval[i].val);
                    vcfg->x_offset = (unsigned int) int_var;
                    break;
                case 'v':
                    int_var = atoi(argval[i].val);
                    vcfg->y_offset = (unsigned int) int_var;
                    break;

                case 'c':
                    int_var = atoi(argval[i].val);
                    CVR1CONbits.CVRR = (unsigned int) int_var & 0x1;
                    CVR1CONbits.CVRR1 = (unsigned int) (int_var >> 1) & 0x1;
                    break;
                case 'r':
                    int_var = atoi(argval[i].val);
                    CVR1CONbits.CVR = (unsigned int) int_var & 0xf;
                    
                    int_var = CVR1CONbits.CVRR | (CVR1CONbits.CVRR1 << 1);
                    
                    f = (float) CVR1CONbits.CVR;
                    if (videocore_ctrl & CTRL_EXTVREF)
                        vref = 3.3/2.0;
                    else
                        vref = 3.3;
                    switch (int_var) {
                        case 0:
                            f = f/32.0 * vref + (1/4.0) * vref;
                            break;
                        case 1:
                            f = f /24.0 * vref;
                            break;
                        case 2:
                            f = f/24.0 * vref + (1/3.0) * vref;
                            break;
                        case 3:
                            f = f / 16.0 * vref;
                            break;
                        default:
                            f = 0;
                            break;
                    }
                    shell_printf("vref=%0.3f\n", f);
                    break;
                case 'l':
                    int_var = atoi(argval[i].val);
                    PR5 = (unsigned int) int_var;
                    break;
                case 'k':
                    LINE_TMR = atoi(argval[i].val);
                    break;
                default:
                    break;
            }
        }
        video_apply_config_cbk();
    }
}


static void toggle_video_pins(struct timer *t, void *data)
{
    LATC =~ LATC;
    if (hw_rev >= 0x03) {
        _LATA2 =~ _LATA2;
        _LATA3 =~ _LATA3;
        _LATA9 =~ _LATA9;
    } else {
        _LATB8 =~ _LATB8;
    }
    if (hw_rev >= 0x04) {
        _LATA4 =~ _LATA4;
        _LATA7 =~ _LATA7;
    }
}

#define SHELL_CMD_TEST_ARGS   2
static void shell_cmd_test(char *args, void *data)
{
    struct shell_argval argval[SHELL_CMD_TEST_ARGS+1];
    unsigned char t, i;
    int v, p;
    
    t = shell_arg_parser(args, argval, SHELL_CMD_TEST_ARGS);

    if (t < 1) {
        shell_printf("Videocore test arguments:\n");
        shell_printf(" -r               test SRAM\n");
        shell_printf(" -e <state>       start or stop engine (0 or 1)\n");
        shell_printf(" -a <amux>        analog mux state (0 to 3)\n");
        shell_printf(" -d <dmux>        digital mux state\n");
        shell_printf("                      bit1~0 <state>\n");
        shell_printf("                      bit2   <[en|dis]able>\n");
        shell_printf(" -q <spi>         sram spi\n");
        shell_printf("                      bit3~0 <data>\n");
        shell_printf("                      bit4   <clk>\n");
        shell_printf("                      bit5   <cs>\n");
        shell_printf(" -t <0|1>         toggle all videocore related pins at 100kHz\n");
    } else {
        for (i = 0; i < t; i++) {
            switch (argval[i].key) {
                case 'r':
                    video_pause();
                    test_sram();
                    video_resume();
                    break;
                case 'e':
                    v = atoi(argval[i].val);
                    if (v)
                        video_resume();
                    else
                        video_pause();
                    break;
                case 'd':
                    v = atoi(argval[i].val);
                    if (hw_rev <= 0x02) {
                        if (v & 0x04)
                            OE_RAM = 1;
                        else
                            OE_RAM = 0;
                    } else {
                        if (v & 0x04)
                            OE_RAM2 = 1;
                        else
                            OE_RAM2 = 0;

                         /* input */
                        _TRISA2 = 1;
                        _TRISA3 = 1;
                        
                        /* set value on sio<1:0> */
                        v &= 3;
                        LATC &= ~3;
                        LATC |= v;
                        mdelay(100);
                        p = (PORTA & 0xc) >> 2;
                        shell_printf("sio<1:0>=%u; b<1:0>=%u ", v, p);
                        if (v == p)
                            shell_printf("[OK]\n");
                        else
                            shell_printf("[FAIL]\n");
                    }
                    break;
                case 'a':
                    v = atoi(argval[i].val);
                    if (hw_rev <= 0x02) {
                        if (v & 0x04)
                            OE_RAM = 1;
                        else
                            OE_RAM = 0;
                    } else {
                        if (v & 0x04)
                            OE_RAM2 = 1;
                        else
                            OE_RAM2 = 0;
                        _TRISA2 = 1;
                        _TRISA3 = 1;
                    }
                    v &= 3;
                    LATC &= ~3;
                    LATC |= v;
                    shell_printf("analog mux output=");
                    switch(v) {
                        default:
                        case 0:
                            shell_printf("video input\n");
                            break;
                        case 1:
                            shell_printf("white level\n");
                            break;
                        case 2:
                            shell_printf("gray level\n");
                            break;
                        case 3:
                            shell_printf("sync/black level\n");
                            break;
                    }
                    
                    break;
                case 't':
                    v = atoi(argval[i].val);
                    SRAM_OUTQ;
                    if (hw_rev <= 0x02) {
                        OE_RAM_DIR = 0;
                    } else {
                        OE_RAM_DIR2 = 0;
                    }
                    if (v)
                        add_timer(100, 10, toggle_video_pins, NULL);
                    else
                        remove_timers(100);
                    break;
                default:
                    break;
            }
        }
    }
}

#define SHELL_CMD_CONFIGSW_ARGS 6
static void shell_cmd_swconfig(char *args, void *data)
{
    struct ch_switch *swcfg = &config.video_sw;
    struct shell_argval argval[SHELL_CMD_CONFIGSW_ARGS+1], *p;
    unsigned char t, i = 0;
    unsigned int w;

    t = shell_arg_parser(args, argval, SHELL_CMD_CONFIGSW_ARGS);
    if (t < 1) {
        shell_printf("Video switch:\n");
        shell_printf(" Mode:    %d (0:ch%; 1:flight mode; 2:toggle)\n", swcfg->mode);
        shell_printf(" Ch:   CH%d\n", swcfg->ch + 1);
        shell_printf(" Min:  %d\n", swcfg->ch_min);
        shell_printf(" Max:  %d\n", swcfg->ch_max);
        shell_printf(" Time: %d00ms\n", swcfg->time);
        shell_printf("\nopt: -m <mode> -c <ch> -l <min> -h <max> -t <time> -i <input>\n");
        shell_printf("    -i <input>      Change video input: 0 or 1\n", swcfg->time);
    } else {
        p = shell_get_argval(argval, 'm');
        if (p != NULL) {
            i = atoi(p->val);
            if (i < SW_MODE_END)
                swcfg->mode = i;
        }
        p = shell_get_argval(argval, 'c');
        if (p != NULL) {
            i = atoi(p->val);
            i = TRIM(i, 1, 18);
            swcfg->ch = i - 1;
        }
        p = shell_get_argval(argval, 'l');
        if (p != NULL) {
            w = atoi(p->val);
            w = TRIM(w, 900, 2100);
            swcfg->ch_min = w;
        }
        p = shell_get_argval(argval, 'h');
        if (p != NULL) {
            w = atoi(p->val);
            w = TRIM(w, 900, 2100);
            swcfg->ch_max = w;
        }
        p = shell_get_argval(argval, 't');
        if (p != NULL) {
            w = atoi(p->val);
            w = w / 100;
            swcfg->time = w;
        }
        p = shell_get_argval(argval, 'i');
        if (p != NULL) {
            w = atoi(p->val);
            if (w)
                cfg->mode |= VIDEO_MODE_INPUT_MASK;
            else
                cfg->mode &= ~VIDEO_MODE_INPUT_MASK;
            video_apply_config_cbk();
        }
    }
}


static const struct shell_cmdmap_s video_cmdmap[] = {
    {"test", shell_cmd_test, "Test video circuits", SHELL_CMD_SIMPLE},
    {"config", shell_cmd_config, "Configure video settings", SHELL_CMD_SIMPLE},
    {"stats", shell_cmd_stats, "Display statistics", SHELL_CMD_SIMPLE},
    {"sw", shell_cmd_swconfig, "Video sw", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_video(char *args, void *data)
{
    shell_exec(args, video_cmdmap, data);
}
