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

#ifndef FLIGHT_STATS_H
#define	FLIGHT_STATS_H


enum {
    FL_ALARM_ID_MAVBAT = 0,
    FL_ALARM_ID_BAT1,
    FL_ALARM_ID_BAT2,
    FL_ALARM_ID_RSSI,
    FL_ALARM_ID_DIST,
    FL_ALARM_ID_SPEED,
    FL_ALARM_ID_ALT,
    FL_ALARM_ID_GPS,
    FL_ALARM_ID_END
};

enum {
    FL_ALARM_MODE_LOW = 1,
    FL_ALARM_MODE_HIGH = 2,
};

#define FL_ALARM_ACTIVE             (0x8000)
#define MAX_FLIGHT_ALARMS           (10)

struct flight_alarm_props {
    unsigned id:5;
    unsigned mode:3;    
};

struct flight_alarm_config {
    struct flight_alarm_props props;
    float value;
    u16 timer;
};

struct flight_alarm_info {
    u8 type;
    char *name;
};

struct flight_alarm {
    const struct flight_alarm_info *info;
    struct flight_alarm_config *cfg;
    void *value;
    u16 ctrl;
};

struct flight_stats {
    int launch_heading;
    float total_distance;
    unsigned long flight_start, flight_end;

    int max_altitude, max_home_altitude;
    unsigned int max_gnd_speed, max_air_speed;
    unsigned int max_home_distance;

    unsigned int max_bat_current;
    float total_flight_mah, total_mah;
    
    u16 rssi;

    struct flight_alarm fl_alarms[MAX_FLIGHT_ALARMS];
};

void init_flight_stats(void);
struct flight_stats* get_flight_stats(void);
void shell_cmd_flight(char *args, void *data);

#endif	/* FLIGHT_STATS_H */

