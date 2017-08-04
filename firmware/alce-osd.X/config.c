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

#define CONFIG_ADDR_START   (0x1000L)
#define CONFIG_ADDR_END     (0x3fffL)
#define CONFIG_ADDR_PAGE    (0x800)
#define CONFIG_PAGE_SIZE    (0x400)

#define CONFIG_VERSION_SIG  (0xffffff-12)

//#define DEBUG_CONFIG

static unsigned long valid_config_addr = 0;

/* default configuration */
struct alceosd_config config = {
    .uart = {
        { .mode = UART_CLIENT_MAVLINK, .baudrate = UART_BAUD_115200, .pins = UART_PINS_TELEMETRY },
        { .mode = UART_CLIENT_MAVLINK, .baudrate = UART_BAUD_115200, .pins = UART_PINS_CON2 },
        { .mode = UART_CLIENT_MAVLINK, .baudrate = UART_BAUD_115200, .pins = UART_PINS_CON3 },
        { .mode = UART_CLIENT_MAVLINK, .baudrate = UART_BAUD_115200, .pins = UART_PINS_ICSP },
    },
  
    .video_profile = {
        {
            .mode = VIDEO_SCAN_PROGRESSIVE,
            .x_offset = 40,
            .y_toffset = 40,
            .x_size_id = VIDEO_XSIZE_480,
            .y_boffset = 0,
        },
        {
            .mode = VIDEO_SCAN_INTERLACED,
            .x_offset = 40,
            .y_toffset = 40,
            .x_size_id = VIDEO_XSIZE_672,
            .y_boffset = 0,
        },
    },
    .video = {
        .white_lvl = 0x3ff >> 4,
        .gray_lvl = 0x2d0 >> 4,
        .black_lvl = 0x190 >> 4,
        .vref.raw = (0x04 << 4) | 0x04,
    },
    .video_sw = {
        .ch = 7,
        .ch_min = 1000,
        .ch_max = 2000,
        .mode = SW_MODE_CHANNEL,
        .time = 20,
    },
    .tab_sw = {
        .ch = 5,
        .ch_min = 1000,
        .ch_max = 2000,
        .mode = SW_MODE_CHANNEL,
        .time = 20,
    },

    .mav = {
        .streams = {3, 1, 4, 1, 4, 10, 10, 1},
        .shell_rate = 0,
        .osd_sysid = 200,
        .uav_sysid = 1,
        .heartbeat = 1,
    },
    
    .rssi = {
        .mode.source = RSSI_SOURCE_MAVLINK,
        .mode.units = RSSI_UNITS_PERCENT,
        .min = 0,
        .max = 255,
    },
    
    .flight_alarm = {
        { .props.id = FL_ALARM_ID_RSSI, .props.mode = FL_ALARM_MODE_LOW, .value = 35, .timer = 0},
        { .props.id = FL_ALARM_ID_MAVBAT, .props.mode = FL_ALARM_MODE_LOW, .value = 11700, .timer = 0},
        { .props.id = FL_ALARM_ID_GPS, .props.mode = FL_ALARM_MODE_LOW, .value = 3, .timer = 0},
        { .props.id = FL_ALARM_ID_ALT, .props.mode = FL_ALARM_MODE_LOW, .value = 15, .timer = 0},
        { .props.id = FL_ALARM_ID_END },
    },
    
    .default_units = UNITS_METRIC,

    .widgets = {
        { 1, 0, WIDGET_ALTITUDE_ID,        0,   0, {JUST_VCENTER | JUST_RIGHT}},
        { 1, 0, WIDGET_BATTERY_INFO_ID,    0,   0, {JUST_TOP     | JUST_LEFT}},
        { 1, 0, WIDGET_COMPASS_ID,         0,   0, {JUST_BOT     | JUST_HCENTER}},
        { 1, 0, WIDGET_FLIGHT_MODE_ID,     0, -32, {JUST_BOT     | JUST_LEFT}},
        { 1, 0, WIDGET_GPS_INFO_ID,        0,   0, {JUST_BOT     | JUST_LEFT}},
//        { 1, 0, WIDGET_ILS_ID,             0,   0, {JUST_VCENTER | JUST_HCENTER}},
        { 1, 0, WIDGET_HORIZON_ID,        16,   0, {JUST_VCENTER | JUST_HCENTER}},
        { 1, 0, WIDGET_RSSI_ID,            0,   0, {JUST_TOP     | JUST_RIGHT}, {0, 255}},
        { 1, 0, WIDGET_SPEED_ID,           0,   0, {JUST_VCENTER | JUST_LEFT}},
        { 1, 0, WIDGET_THROTTLE_ID,       70,   0, {JUST_TOP     | JUST_LEFT}},
        { 1, 0, WIDGET_VARIOMETER_ID,      0,  -5, {JUST_BOT     | JUST_RIGHT}},
        { 1, 0, WIDGET_WIND_ID,            0,  30, {JUST_TOP     | JUST_RIGHT}},

        { 1, 0, WIDGET_HOME_INFO_ID,      88,   0, {JUST_TOP     | JUST_LEFT}},
        { 1, 0, WIDGET_RADAR_ID,          60, -44, {JUST_BOT     | JUST_LEFT}},
        { 1, 0, WIDGET_ALARMS_ID,          0,   0, {JUST_TOP     | JUST_HCENTER}},

        { 2, 0, WIDGET_RC_CHANNELS_ID,     0,   0, {JUST_TOP     | JUST_LEFT}},
        { 2, 1, WIDGET_RADAR_ID,           0,   0, {JUST_TOP     | JUST_HCENTER}},
        { 2, 0, WIDGET_CONSOLE_ID,         0,   0, {JUST_BOT     | JUST_LEFT}},

        { 3, 0, WIDGET_FLIGHT_INFO_ID,     0,   0, {JUST_VCENTER | JUST_HCENTER}},

        { TABS_END, 0, 0, 0, 0, {0}},
    },
};

const struct param_def params_config[] = {
    PARAM("OSD_UNITS", MAV_PARAM_TYPE_UINT8, &config.default_units, NULL),
    PARAM_END,
};


unsigned char get_units(struct widget_config *cfg)
{
    if (cfg->props.units == UNITS_DEFAULT)
        return config.default_units;
    else
        return cfg->props.units;
}

/* return rc_channel in percentage according to switch config values */
unsigned char get_sw_state(struct ch_switch *sw, u32 *store_age)
{
    unsigned int *val;
    void *rc;
    long x;    
    u32 age = mavdata_age(MAVLINK_MSG_ID_RC_CHANNELS);

    if (age < 5000) {
        rc = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS);
        *store_age = age;
    } else {
        rc = mavdata_get(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
        *store_age = mavdata_age(MAVLINK_MSG_ID_RC_CHANNELS_RAW);
    }
    val = (unsigned int*) (rc + 4 + 2 * sw->ch);
    x = (long) *(val);
    x = ( ((x - sw->ch_min) * 100) /
          (sw->ch_max - sw->ch_min));
    if (x < 0)
        x = 0;
    else if (x > 100)
        x = 100;
    return (unsigned char) x;
}

static void load_config(void)
{
    unsigned long status;
    unsigned long addr;
    unsigned char buf[3];
    int ipl;

    /* find first valid config */
    addr = CONFIG_ADDR_START;

    SET_AND_SAVE_CPU_IPL(ipl, 7);
    while (addr < CONFIG_ADDR_END) {
        read_flash(addr, 3, buf);
        status = ((unsigned long) buf[0]) |
                 ((unsigned long) buf[1] << 8) |
                 ((unsigned long) buf[2] << 16);
        //shell_printf("signature: %4x%4x\n", (unsigned int) (status>>16), (unsigned int) status);
        if (status == CONFIG_VERSION_SIG)
            break;

        addr += CONFIG_PAGE_SIZE;
    }
    if (addr < CONFIG_ADDR_END) {
        //shell_printf("valid config found at %4x\n", (unsigned int) addr);
        valid_config_addr = addr;
        read_flash(addr + 4, sizeof(struct alceosd_config), (unsigned char *) &config);
    }
    RESTORE_CPU_IPL(ipl);

    /* setup video */
    video_apply_config(VIDEO_ACTIVE_CONFIG);

    /* setup serial ports */
    uart_set_config_pins();
    uart_set_config_baudrates();
}


static void write_config(void)
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
#ifdef DEBUG_CONFIG
    shell_printf("last_addr=%04x%04x\n", (unsigned int) (valid_config_addr>>16), (unsigned int) (valid_config_addr));
#endif
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

#ifdef DEBUG_CONFIG
    shell_printf("last_addr=%04x%04x\n", (unsigned int) (addr>>16), (unsigned int) (addr));
#endif
    RESTORE_CPU_IPL(ipl);
}

#define MAX_LINE_LEN 50
static u16 load_config_text(unsigned char *buf, unsigned int len, void *data)
{
    static unsigned char line[MAX_LINE_LEN];
    static unsigned char llen = 0;

    char param[20];
    float value;
    unsigned int i = 0;

    while (*buf != '\n') {
        line[llen++] = *(buf++);
        if (++i == len)
            return len;
    }
    line[llen] = '\0';
    i++;

    if (llen == 0)
        return len;

    /* the end */
    if (line[0] == '.') {
        llen = 0;
        load_tab(1);
        return len | SHELL_GET_EXIT;
    }

    /* reset widgets config */
    if (memcmp("==", line, 2) == 0) {
        config.widgets[0].tab = TABS_END;
    } else {
        int ret = sscanf((const char *) line, "%20s = %f", param, &value);
        if (ret == 2) {
            params_set_value(param, value, 0);
            shell_printf("ret=%d, got: '%s' = '%f'\n", ret, param, (double) value);
        }
    }
    llen = 0;
    return len;
}

static void shell_cmd_stats(char *args, void *data)
{
    shell_printf("Config stats:\n");
    shell_printf(" Signature        0x%06lx\n", CONFIG_VERSION_SIG);
    shell_printf(" Address range    0x%06lx-0x%06lx\n", CONFIG_ADDR_START, CONFIG_ADDR_END);
    if (valid_config_addr == 0)
        shell_printf(" Address          (none)\n");
    else
        shell_printf(" Address          0x%06lx\n", valid_config_addr);
    shell_printf(" Size             %u/%u (0x%x/0x%x)\n",
            (unsigned int) sizeof(struct alceosd_config),
            (unsigned int) CONFIG_PAGE_SIZE,
            (unsigned int) sizeof(struct alceosd_config),
            (unsigned int) CONFIG_PAGE_SIZE);
}

static void shell_cmd_defaults(char *args, void *data)
{
    unsigned long addr;
    shell_printf("Restored default config... reboot to apply changes\n");
    for (addr = CONFIG_ADDR_START; addr < CONFIG_ADDR_END; addr += CONFIG_ADDR_PAGE) {
        //shell_printf("erasing page %4x\n", (unsigned int) addr);
        erase_page(addr);
    }
}

static void shell_cmd_savecfg(char *args, void *data)
{
    shell_printf("Saving config...\n");
    write_config();
}

static void shell_cmd_loadcfg(char *args, void *data)
{
    shell_printf("Loading config...\n");
    shell_get(load_config_text, NULL);
}

static void shell_cmd_dumpcfg(char *args, void *data)
{
    char param_name[17];
    float value;
    u16 i, t = params_get_total();
    
    shell_printf("AlceOSD config hw%dv%d fw%d.%d.%d\n==\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);

    for (i = 0; i < t; i++) {
        value = params_get_value(i, param_name);
        shell_printf("%s = %f\n", param_name, (double) value);
    }
    shell_printf("--\n");
}

static void shell_cmd_dumpcfg2(char *args, void *data)
{
    u16 i;
    
    shell_printf("# AlceOSD config hw%dv%d fw%d.%d.%d\n", hw_rev >> 4, hw_rev & 0xf, VERSION_MAJOR, VERSION_MINOR, VERSION_DEV);
    shell_printf("echo 0\n\n");

    /* uart */
    shell_printf("# UART\n");
    for (i = 0; i < 4; i++) {
        shell_printf("uart config -p%u -b%lu -c%s -i%s\n", i,
                uart_get_baudrate(config.uart[i].baudrate),
                UART_CLIENT_NAMES[config.uart[i].mode],
                UART_PIN_NAMES[config.uart[i].pins]);
    }
    
    /* video */
    shell_printf("\n# VIDEO\n");
    shell_printf("video config -w%u -g%u -b%u -r%u -f%u\n", config.video.white_lvl,
            config.video.gray_lvl, config.video.black_lvl,
            config.video.vref.vin0, config.video.vref.vin1);
    shell_printf("# VIDEO_SW\n");
    shell_printf("video sw -m%u -c%u -l%u -h%u -t%u\n", config.video_sw.mode,
            config.video_sw.ch, config.video_sw.ch_min,
            config.video_sw.ch_max, config.video_sw.time * 100);
    shell_printf("# VIDEO_PROFILES\n");
    for (i = 0; i < 2; i++) {
        shell_printf("video config -p%u -m%c -x%u -y%u -h%u -v%u\n", i,
                //config.video_profile[i].mode & VIDEO_MODE_SCAN_MASK ? 'n' : 'p',
                config.video_profile[i].mode.scan_mode == VIDEO_SCAN_INTERLACED ? 'i' : 'p',
                video_xsizes[config.video_profile[i].x_size_id].xsize,
                config.video_profile[i].y_boffset,
                config.video_profile[i].x_offset,
                config.video_profile[i].y_toffset);
    }

    /* tab switch */
    shell_printf("\n# TAB_SW\n");
    shell_printf("tabs config -m%u -c%u -l%u -h%u -t%u\n", config.tab_sw.mode,
            config.tab_sw.ch, config.tab_sw.ch_min,
            config.tab_sw.ch_max, config.tab_sw.time);
    
    /* mavlink */
    shell_printf("\n# MAVLINK\n");
    shell_printf("mavlink config -i%u -u%u -h%u\n", config.mav.osd_sysid,
            config.mav.uav_sysid, config.mav.heartbeat);

    shell_printf("\n# MAVLINK STREAM RATES\n");
    for (i = 0; i < 8; i++)
        shell_printf("mavlink rates -s%u -r%u\n", i+1, config.mav.streams[i]);
    
    /* rssi */
    shell_printf("\n# RSSI\n");
    shell_printf("flight rssi -s%u -u%u -l%u -h%u\n", config.rssi.mode.source,
            config.rssi.mode.units, config.rssi.min, config.rssi.max);
    
    /* alarms */
    shell_printf("\n# FLIGHT ALARMS\n");
    shell_printf("flight alarms -a3\n");
    i = 0;
    while (config.flight_alarm[i].props.id != FL_ALARM_ID_END) {
        shell_printf("flight alarms -a%u -i%u -v%.4f -t%u\n",
                config.flight_alarm[i].props.mode,
                config.flight_alarm[i].props.id,
                config.flight_alarm[i].value,
                config.flight_alarm[i].timer);
        i++;
    }
    
    /* default units */
    shell_printf("\n# DEFAULT UNITS\n");
    shell_printf("config units %u\n", config.default_units - 1);
    
    /* widgets */
    shell_printf("\n# WIDGETS\n");
    shell_printf("tabs load -t0\n");
    shell_printf("widgets rm all\n");
    i = 0;
    while (config.widgets[i].tab != TABS_END) {
        shell_printf("widgets cfg -i%u -t%u -m%u -s%u -u%u -h%u -v%u -x%d -y%d -a%u -b%u -c%u -d%u\n",
                config.widgets[i].widget_id,
                config.widgets[i].tab,
                config.widgets[i].props.mode,
                config.widgets[i].props.source,
                config.widgets[i].props.units,
                config.widgets[i].props.hjust,
                config.widgets[i].props.vjust,
                config.widgets[i].x,
                config.widgets[i].y,
                config.widgets[i].params[0],
                config.widgets[i].params[1],
                config.widgets[i].params[2],
                config.widgets[i].params[3]);
        i++;
    }
    shell_printf("tabs load -t1\n");
    shell_printf("echo 1\n");
    shell_printf("# end\n");
    
}

static void shell_cmd_units(char *args, void *data)
{
    u16 v;
    if (strlen(args) > 0) {
        v = atoi(args) + 1;
        v = TRIM(v, 1, 2);
        config.default_units = v;
    } else {
        shell_printf("syntax: config units <i>\n");
        shell_printf("        0 - metric\n");
        shell_printf("        1 - imperial\n\n");
    }
    shell_printf("Units: %s\n", (config.default_units == UNITS_METRIC) ? "metric" : "imperial");
}

static const struct shell_cmdmap_s config_cmdmap[] = {
    {"dump", shell_cmd_dumpcfg, "dump", SHELL_CMD_SIMPLE},
    {"dump2", shell_cmd_dumpcfg2, "dump2", SHELL_CMD_SIMPLE},
    {"load", shell_cmd_loadcfg, "load", SHELL_CMD_SIMPLE},
    {"save", shell_cmd_savecfg, "save", SHELL_CMD_SIMPLE},
    {"stats", shell_cmd_stats, "config statistics", SHELL_CMD_SIMPLE},
    {"units", shell_cmd_units, "Default units", SHELL_CMD_SIMPLE},
    {"defaults", shell_cmd_defaults, "Restore default config", SHELL_CMD_SIMPLE},
    {"", NULL, ""},
};

void shell_cmd_cfg(char *args, void *data)
{
    shell_exec(args, config_cmdmap, data);
}

void config_init(void)
{
    params_add(params_config);
    load_config();
}
