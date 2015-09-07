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


float earth_distance(struct gps_coord *c1, struct gps_coord *c2)
{
    float dphi, dlam, sin_dphi, sin_dlam;
    float a, c;

    dphi = c2->lat - c1->lat;
    dlam = c2->lon - c1->lon;
    sin_dphi = sin(dphi/2);
    sin_dlam = sin(dlam/2);
    a = sin_dphi * sin_dphi + cos(c1->lat) * cos(c2->lat) * sin_dlam * sin_dlam;
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    c = c * ((float) EARTH_RADIUS);

    return c;
}

float get_bearing(struct gps_coord *c1, struct gps_coord *c2)
{
    float y = sin(c2->lon - c1->lon) * cos(c2->lat);
    float x = cos(c1->lat)*sin(c2->lat) - sin(c1->lat)*cos(c2->lat)*cos(c2->lon - c1->lon);
    return RAD2DEG(atan2(y, x));
}
