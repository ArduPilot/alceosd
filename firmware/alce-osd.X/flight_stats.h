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


struct flight_stats {
    int launch_heading;
    unsigned long total_distance;
    unsigned long flight_start, flight_end;

    int max_altitude;
    unsigned int max_gnd_speed, max_air_speed;    
};

void init_flight_stats_process(void);
struct flight_stats* get_flight_stats(void);

#endif	/* FLIGHT_STATS_H */

