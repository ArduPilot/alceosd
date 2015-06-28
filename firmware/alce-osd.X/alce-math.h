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

#ifndef ALCE_MATH_H
#define	ALCE_MATH_H


#define EARTH_RADIUS                (6371000) /* meters */
#define PI                          (3.1415926535897931)
#define RAD2DEG(x)                  (x * (180.0 / PI))
#define DEG2RAD(x)                  (x * (PI / 180.0))



struct gps_coord {
    float lat, lon;
};

float earth_distance(struct gps_coord *c1, struct gps_coord *c2);
float get_bearing(struct gps_coord *c1, struct gps_coord *c2);


#endif	/* ALCE_MATH_H */

