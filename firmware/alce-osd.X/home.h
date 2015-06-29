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

#ifndef HOME_H
#define	HOME_H

#define HOME_LOCK_FIX    0x01
#define HOME_LOCK_POS    0x02
#define HOME_LOCK_ALT    0x04
#define HOME_LOCK_DONE   0x08
#define HOME_LOCKED      0x0f


struct home_data {
    int altitude;
    int direction, uav_bearing;
    float distance;

    unsigned char lock_sec;
    unsigned char lock;
};

void init_home_process(void);
struct home_data* get_home_data(void);

#endif	/* HOME_H */

