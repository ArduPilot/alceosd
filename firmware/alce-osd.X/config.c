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


/* settings are stored in the program flash
 * a simple algorithm to avoid flash wear is implemented */

/* WARNING: MESSING WITH THIS CODE MIGHT BRICK YOUR ALCEOSD */
/* RECOVERY IS ONLY POSSIBLE USING A PICKIT3 TO RE-FLASH THE BOOTLOADER */

#define CONFIG_ADDR_START   (0x1000)
#define CONFIG_ADDR_END     (0x3fff)
#define CONFIG_ADDR_PAGE    (0x800)
#define CONFIG_PAGE_SIZE    (0x400)

#define CONFIG_VERSION_SIG  (0xfffff-4)

static unsigned long valid_config_addr = 0;


/* default configuration */
struct alceosd_config config = {
    .baudrate = UART_57600,
    
    .mavlink_default_sysid = 1,

    .video.standard = VIDEO_STANDARD_PAL,
    .video.scan = VIDEO_SCAN_PROGRESSIVE,
    .video.brightness = 600, //0x50,
    .video.x_offset = 85,
    .video.y_offset = 40,

    .video.x_size = VIDEO_XSIZE_480,
    .video.y_size = 260,

    .tab_change.tab_change_ch_min = 1000,
    .tab_change.tab_change_ch_max = 2000,
    .tab_change.ch = 7,
    .tab_change.mode = TAB_CHANGE_CHANNEL,
    .tab_change.time_window = 20,

    
    .default_units = UNITS_METRIC,
    .home_lock_sec = 15,

    .widgets = {
        { 1, WIDGET_ALTITUDE_ID,        0,   0, {JUST_VCENTER | JUST_RIGHT}},
        { 1, WIDGET_BATTERY_INFO_ID,    0,   0, {JUST_TOP     | JUST_LEFT}},
        { 1, WIDGET_COMPASS_ID,         0,   0, {JUST_BOT     | JUST_HCENTER}},
        { 1, WIDGET_FLIGHT_MODE_ID,     0, -32, {JUST_BOT     | JUST_LEFT}},
        { 1, WIDGET_GPS_INFO_ID,        0,   0, {JUST_BOT     | JUST_LEFT}},
        { 1, WIDGET_HORIZON_ID,        16,   0, {JUST_VCENTER | JUST_HCENTER}},
        { 1, WIDGET_RSSI_ID,            0,   0, {JUST_TOP     | JUST_RIGHT}},
        { 1, WIDGET_SPEED_ID,           0,   0, {JUST_VCENTER | JUST_LEFT}},
        { 1, WIDGET_THROTTLE_ID,       70,   0, {JUST_TOP     | JUST_LEFT}},
        { 1, WIDGET_VARIOMETER_ID,      0,  -5, {JUST_BOT     | JUST_RIGHT}},
        { 1, WIDGET_WIND_ID,             0, 30, {JUST_TOP     | JUST_RIGHT}},

        { 1, WIDGET_HOME_INFO_ID,       80,  0, {JUST_TOP     | JUST_LEFT}},
        { 1, WIDGET_RADAR_ID,           60,-44, {JUST_BOT     | JUST_LEFT}},

        { 2, WIDGET_RC_CHANNELS_ID,      0,  0, {JUST_VCENTER | JUST_LEFT}},
        { 2, WIDGET_RADAR_ID,            0,  0, {JUST_TOP     | JUST_HCENTER}},

        { 3, WIDGET_FLIGHT_INFO_ID,      0,  0, {JUST_VCENTER | JUST_HCENTER}},

        { TABS_END, 0, 0, 0, {0}},
    }
};


/* flash operation functions */
int erase_page(unsigned long erase_address)
{
    u32union addr;
    addr.l = erase_address & 0xfff800;

    NVMADRU = addr.w[1];
    NVMADR = addr.w[0];
    NVMCON = 0x4003;

    __builtin_write_NVM();
    while (NVMCONbits.WR == 1) {}

    return NVMCONbits.WRERR;
}

int write_dword(unsigned long addr, unsigned long data0, unsigned long data1)
{
    u32union wr_addr;
    u32union wr_data0, wr_data1;
    unsigned int tmp;

    wr_addr.l = addr;
    wr_data0.l = data0;
    wr_data1.l = data1;


    NVMCON = 0x4001;
    NVMADRU = wr_addr.w[1];
    NVMADR = wr_addr.w[0];

    tmp = TBLPAG;
    
    TBLPAG = 0xFA;
    __builtin_tblwtl(0, wr_data0.w[0]);
    __builtin_tblwth(1, wr_data0.w[1]);
    __builtin_tblwtl(2, wr_data1.w[0]);
    __builtin_tblwth(3, wr_data1.w[1]);

    //INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);

    TBLPAG = tmp;

    return NVMCONbits.WRERR;
}

static void write_word(unsigned long addr, unsigned long data)
{
    if ((addr % 4) == 0)
        write_dword(addr, data, 0xffffff);
    else
        write_dword(addr, 0xffffff, data);
}

static void read_flash(unsigned long addr, unsigned int size, unsigned char *buf)
{
    unsigned int tmp;
    unsigned int data;
    unsigned int i;
    unsigned int instr = size / 3;
    unsigned int left = size % 3;

    tmp = TBLPAG;
    TBLPAG = addr >> 16;
    for (i = 0; i < instr; i++, addr += 2) {
        data = __builtin_tblrdl(addr & 0xFFFF);
        *buf++ = data & 0xFF;
        *buf++ = data >> 8;
        data = __builtin_tblrdh(addr & 0xFFFF);
        //printf("read1.2 %x %x\n", (unsigned int) data >> 16, (unsigned int) data);
        *buf++ = data & 0xFF;
    }
    if (left >= 1) {
        data = __builtin_tblrdl(addr & 0xFFFF);
        *buf++ = data & 0xFF;
        if(left >= 2)
            *buf = data >> 8;
    }
    TBLPAG = tmp;
}


void load_config(void)
{
    unsigned long status;
    unsigned long addr;
    unsigned char buf[3];
    int ipl;

    SET_AND_SAVE_CPU_IPL(ipl, 7);
    
    /* find first valid config */
    addr = CONFIG_ADDR_START;    

    while (addr < CONFIG_ADDR_END) {
        read_flash(addr, 3, buf);
        status = ((unsigned long) buf[0]) | 
                 ((unsigned long) buf[1] << 8) |
                 ((unsigned long) buf[2] << 16);
        //printf("signature: %4x%4x\n", (unsigned int) (status>>16), (unsigned int) status);
        if (status == CONFIG_VERSION_SIG)
            break;

        addr += CONFIG_PAGE_SIZE;
    }

    if (addr >= CONFIG_ADDR_END) {
        //printf("no valid config found\n");
        video_apply_config(&config.video);
        return;
    }

    //printf("valid config found at %4x\n", (unsigned int) addr);
    valid_config_addr = addr;

    read_flash(addr + 4, sizeof(struct alceosd_config), (unsigned char *) &config);

    RESTORE_CPU_IPL(ipl);
    return;
}

void write_config(void)
{
    unsigned long addr;
    unsigned char *b;
    int left;
    unsigned long data;
    int ipl;

    SET_AND_SAVE_CPU_IPL(ipl, 7);

    if (valid_config_addr != 0) {
        /* find a spot */
        if ((valid_config_addr + CONFIG_PAGE_SIZE) < CONFIG_ADDR_END) {
            /* still room for one more, invalidate current config */
            write_word(valid_config_addr, 0xffeeee);
            valid_config_addr += CONFIG_PAGE_SIZE;
        } else {
            //printf("no more space, reseting addr\n");
            valid_config_addr = 0;
        }
    }

    if (valid_config_addr == 0) {
        /* end of config memory or no more space, erase pages start over*/
        for (addr = CONFIG_ADDR_START; addr < CONFIG_ADDR_END; addr += CONFIG_ADDR_PAGE) {
            //printf("erasing page %4x\n", (unsigned int) addr);
            erase_page(addr);
        }

        valid_config_addr = CONFIG_ADDR_START;
    }

    /* write config */
    /* flag valid config */
    write_word(valid_config_addr, CONFIG_VERSION_SIG);

    b = (unsigned char*) &config;
    left = sizeof(struct alceosd_config);
    addr = valid_config_addr + 4;
    while (left > 0) {
        data = (unsigned long) (*b++);
        if (left > 1) {
            data |= (unsigned long) (*b++) << 8;
            if (left > 2)
                data |= (unsigned long) (*b++) << 16;
            else
                data |= 0xffff0000;
        } else {
            data |= 0xffffff00;
        }

        write_word(addr, data);

        left -= 3;
        addr += 2;
    }

    RESTORE_CPU_IPL(ipl);
}



enum {
    MENU_MAIN,
    MENU_VIDEO,
    MENU_TABS,
    MENU_TAB_WIDGETS,
    MENU_ADD_WIDGET,
    MENU_EDIT_WIDGET,
};

const char menu_main[] = "\n\n"
                         "AlceOSD setup\n\n"
                         "1 - Video config\n"
                         "2 - Telemetry UART speed: %u%u\n"
                         "3 - Configure tabs\n"
                         "4 - Units (global setting): %s\n"
                         "q/w - Decrease/increase home locking timer: %d\n"
                         "\ns - Save settings to FLASH\n"
                         "x - Exit config\n";

const char menu_video[] = "\n\nAlceOSD :: VIDEO setup\n\n"
                          "1 - Video scan: %s\n"
                          "2/3 - Adjust video brightness: %u\n"
                          "q/a - Adjust video window vertically: %d\n"
                          "d/f - Adjust video window horizontally: %d\n"
                          "e/r - Decrease/increase video X size: %d\n"
                          "s/w - Decrease/increase video Y size: %d\n"
                          "x - Go back\n";

const char menu_tabs[] = "\n\nAlceOSD :: TAB config\n\n"
                         "1/2 - Change active tab: %d\n"
                         "3/4 - Tab change mode: %s\n";
const char menu_tabs_mode_ch[] = "5/6 - Tab change channel: RC%d\n";
const char menu_tabs_mode_tmr[] = "7/8 - Change time window: %d00ms\n";
const char menu_tabs_end[] = "e - Edit tab\n"
                             "x - Go back\n";

const char menu_tab_widgets[] = "\n\nAlceOSD :: TAB %d widgets config\n\n"
                                "0 - Add widget\n"
                                "x - Go Back\n\n";

const char menu_add_widgets[] = "\n\nAlceOSD :: Add widget\n\n"
                                "0 - Go back\n";

const char menu_edit_widget[] = "\n\nAlceOSD :: Edit widget\n\n"
                                "1 - Horizontal justification: %d\n"
                                "2 - Vertical justification: %d\n"
                                "3/4 - Mode: %d\n"
                                "5/6 - Units: %d\n"
                                "7/8 - Source: %d\n"
                                "w/e - X position: %d\n"
                                "q/a - Y position: %d\n"
                                "0 - Remove\n"
                                "\nx - Go back\n";


extern const struct widget_ops *all_widget_ops[];

int config_osd(void)
{
    static unsigned char state = MENU_MAIN, refresh_disp = 1;
    unsigned int osdxsize, osdysize;
    static unsigned int options[30];
    static unsigned char nr_opt = 0;

    video_get_size(&osdxsize, &osdysize);

    static unsigned char current_tab = 1;
    static struct widget_config *wcfg;

    char c;

    if (refresh_disp) {

    //printf("cfg size=%d\n", (unsigned int) sizeof (struct alceosd_config));
        switch (state) {
            case MENU_MAIN:
            default:
                printf(menu_main,
                        (unsigned int) (uart_get_baudrate(config.baudrate) / 1000),
                        (unsigned int) (uart_get_baudrate(config.baudrate) % 1000),
                        (config.default_units == UNITS_METRIC) ? "METRIC" : "IMPERIAL",
                        config.home_lock_sec);
                break;
            case MENU_VIDEO:
                printf(menu_video,
                        (config.video.scan == VIDEO_SCAN_PROGRESSIVE) ? "Progressive" : "Interlaced",
                        config.video.brightness,
                        config.video.y_offset,
                        config.video.x_offset,
                        osdxsize,
                        osdysize);

                break;
            case MENU_TABS:
                printf(menu_tabs, current_tab,
                        (config.tab_change.mode == TAB_CHANGE_CHANNEL) ? "RC CHANNEL PERCENT" :
                        (config.tab_change.mode == TAB_CHANGE_FLIGHTMODE) ? "FLIGHT MODE" :
                        (config.tab_change.mode == TAB_CHANGE_TOGGLE) ? "RC CHANNEL TOGGLE" : "????");

                switch (config.tab_change.mode) {
                    case TAB_CHANGE_CHANNEL:
                    default:
                        printf(menu_tabs_mode_ch, config.tab_change.ch + 1);
                        break;
                    case TAB_CHANGE_FLIGHTMODE:
                        printf(menu_tabs_mode_tmr, config.tab_change.time_window);
                        break;
                    case TAB_CHANGE_TOGGLE:
                        printf(menu_tabs_mode_ch, config.tab_change.ch + 1);
                        printf(menu_tabs_mode_tmr, config.tab_change.time_window);
                        break;
                }
                printf(menu_tabs_end);
                break;
            case MENU_TAB_WIDGETS: {
                wcfg = &config.widgets[0];
                const struct widget_ops *w_ops;
                char c = '1';
                nr_opt = 0;

                printf(menu_tab_widgets, current_tab);
                while (wcfg->tab != TABS_END) {
                    if (wcfg->tab == current_tab) {
                        w_ops = get_widget_ops(wcfg->widget_id);
                        if (w_ops != NULL) {
                            printf("%c - %s\n", c++, w_ops->name);
                            if (c == ('9'+1))
                                c = 'a';
                            options[nr_opt++] = (int) wcfg;
                        }
                    }
                    wcfg++;
                }
                break;
            }

            case MENU_ADD_WIDGET: {
                const struct widget_ops **w_ops;
                char c = '1';
                nr_opt = 0;

                printf(menu_add_widgets);

                w_ops = all_widget_ops;
                while (*w_ops != NULL) {
                    printf("%c - %s\n", c++, (*w_ops)->name);
                    if (c == ('9'+1))
                        c = 'a';
                    options[nr_opt++] = (*w_ops)->id;
                    w_ops++;
                }
                break;
            }
            case MENU_EDIT_WIDGET: {
                wcfg = (struct widget_config*) options[nr_opt];

                printf(menu_edit_widget,
                    wcfg->props.hjust,
                    wcfg->props.vjust,
                    wcfg->props.mode,
                    wcfg->props.units,
                    wcfg->props.source,
                    wcfg->x,
                    wcfg->y);
                break;
            }

        }
        refresh_disp = 0;
    }

    if (uart_getc2(&c) == 0)
        return 1;

    refresh_disp = 1;
    switch (state) {
        case MENU_MAIN:
        default:
            switch (c) {
                case '1':
                    state = MENU_VIDEO;
                    break;
                case '2':
                    config.baudrate++;
                    if (config.baudrate >= UART_BAUDRATES)
                            config.baudrate = 0;
                    break;
                case '3':
                    state = MENU_TABS;
                    break;
                case '4':
                    if (config.default_units == UNITS_METRIC)
                        config.default_units = UNITS_IMPERIAL;
                    else
                        config.default_units = UNITS_METRIC;
                    load_tab(current_tab);
                    break;
                case 'q':
                    if (config.home_lock_sec > 5)
                        config.home_lock_sec -= 5;
                    load_tab(current_tab);
                    break;
                case 'w':
                    if (config.home_lock_sec < 56)
                        config.home_lock_sec += 5;
                    load_tab(current_tab);
                    break;
                case 's':
                    printf("Saving config to FLASH...\n");
                    write_config();
                    break;
                case 'x':
                    return 0;
                default:
                    break;
            }

            break;
        case MENU_VIDEO:
            switch (c) {
                case '1':
                    config.video.scan += 1;
                    if (config.video.scan >= VIDEO_SCAN_END)
                        config.video.scan = 0;
                    load_tab(current_tab);
                    break;
                case '2':
                    if (config.video.brightness > 9)
                        config.video.brightness -= 10;
                    video_apply_config(&config.video);
                    break;
                case '3':
                    config.video.brightness += 10;
                    if (config.video.brightness > 1000)
                        config.video.brightness = 1000;
                    video_apply_config(&config.video);
                    break;
                case 'q':
                    if (config.video.y_offset > 0)
                        config.video.y_offset--;
                    break;
                case 'a':
                    if (config.video.y_offset < 500)
                        config.video.y_offset++;
                    break;
                case 'd':
                    if (config.video.x_offset > 0)
                        config.video.x_offset--;
                    break;
                case 'f':
                    if (config.video.x_offset < 1000)
                        config.video.x_offset++;
                    break;
                case 'e':
                    if (config.video.x_size > 0)
                        config.video.x_size--;
                    video_apply_config(&config.video);
                    load_tab(current_tab);
                    break;
                case 'r':
                    if (config.video.x_size < (VIDEO_XSIZE_END-1))
                        config.video.x_size++;
                    video_apply_config(&config.video);
                    load_tab(current_tab);
                    break;
                case 'w':
                    if (config.video.y_size < 1000)
                        config.video.y_size++;
                    load_tab(current_tab);
                    break;
                case 's':
                    if (config.video.y_size > 0)
                        config.video.y_size--;
                    load_tab(current_tab);
                    break;
                case 'x':
                    state = MENU_MAIN;
                    break;
                default:
                    break;
            }
            break;
        case MENU_TABS:
            switch (c) {
                case '1':
                    current_tab--;
                    load_tab(current_tab);
                    break;
                case '2':
                    current_tab++;
                    load_tab(current_tab);
                    break;
                case '3':
                    if (config.tab_change.mode > 0)
                        config.tab_change.mode--;
                    break;
                case '4':
                    if (config.tab_change.mode < (TAB_CHANGE_MODES_END-1))
                        config.tab_change.mode++;
                    break;
                case '5':
                    if (config.tab_change.ch > 0)
                        config.tab_change.ch--;
                    break;
                case '6':
                    if (config.tab_change.ch < 7)
                        config.tab_change.ch++;
                    break;
                case '7':
                    if (config.tab_change.time_window > 5)
                        config.tab_change.time_window--;
                    break;
                case '8':
                    if (config.tab_change.time_window < 50)
                        config.tab_change.time_window++;
                    break;
                case 'e':
                    state = MENU_TAB_WIDGETS;
                    break;
                case 'x':
                    state = MENU_MAIN;
                    break;
                default:
                    break;
            }
            break;
        case MENU_TAB_WIDGETS:
            switch (c) {
                case '0':
                    state = MENU_ADD_WIDGET;
                    break;
                case 'x':
                    state = MENU_TABS;
                    break;
                default:
                    if (c > '9')
                        c -= ('a' - 9);
                    else
                        c -= '1';
                    if (c > nr_opt)
                        break;
                    nr_opt = c;
                    state = MENU_EDIT_WIDGET;
            }
            break;
        case MENU_ADD_WIDGET:
            switch (c) {
                case '0':
                    state = MENU_TAB_WIDGETS;
                    break;
                default:
                    if (c > '9')
                        c -= ('a' - 9);
                    else
                        c -= '1';
                    if (c > nr_opt)
                        break;
                    nr_opt = c;

                    wcfg = &config.widgets[0];
                    while (wcfg->tab != TABS_END)
                        wcfg++;

                    wcfg->tab = current_tab;
                    wcfg->widget_id = options[nr_opt];
                    wcfg->x = 0;
                    wcfg->y = 0;
                    wcfg->props.raw = JUST_VCENTER | JUST_HCENTER;

                    wcfg++;

                    wcfg->tab = TABS_END;

                    load_tab(current_tab);

                    state = MENU_TAB_WIDGETS;
                    break;
            }
            break;
        case MENU_EDIT_WIDGET:
            switch (c) {
                case 'a':
                    wcfg->y++;
                    break;
                case 'q':
                    wcfg->y--;
                    break;
                case 'w':
                    wcfg->x-=4;
                    break;
                case 'e':
                    wcfg->x+=4;
                    break;
                case '1':
                    wcfg->props.hjust++;
                    if (wcfg->props.hjust > 2)
                        wcfg->props.hjust = 0;
                    break;
                case '2':
                    wcfg->props.vjust++;
                    if (wcfg->props.vjust > 2)
                        wcfg->props.vjust = 0;
                    break;
                case '3':
                    wcfg->props.mode--;
                    break;
                case '4':
                    wcfg->props.mode++;
                    break;
                case '5':
                    wcfg->props.units--;
                    break;
                case '6':
                    wcfg->props.units++;
                    break;
                case '7':
                    wcfg->props.source--;
                    break;
                case '8':
                    wcfg->props.source++;
                    break;
                case 'x':
                    state = MENU_TAB_WIDGETS;
                    break;
                case '0': {
                    struct widget_config *wcfg2 = config.widgets;
                    while (wcfg2 != wcfg)
                        wcfg2++;
                    wcfg++;
                    while (wcfg->tab != TABS_END) {
                        memcpy(wcfg2++, wcfg++, sizeof(struct widget_config));
                    }
                    wcfg2->tab = TABS_END;
                    state = MENU_TAB_WIDGETS;
                }
                default:
                    break;
            }
            load_tab(current_tab);
            break;
    }
    
    return 1;
}

unsigned char get_units(struct widget_config *cfg)
{
    if (cfg->props.units == UNITS_DEFAULT)
        return config.default_units;
    else
        return cfg->props.units;
}
