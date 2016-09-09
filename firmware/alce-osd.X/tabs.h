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

#ifndef TABS_H
#define	TABS_H

enum {
    TAB_CHANGE_CHANNEL = 0,
    TAB_CHANGE_FLIGHTMODE,
    TAB_CHANGE_TOGGLE,
    TAB_CHANGE_DEMO,
    TAB_CHANGE_MODES_END,
};

struct tab_change_config {
    /* tab change channel settings */
    unsigned int tab_change_ch_min;
    unsigned int tab_change_ch_max;

    unsigned char time_window;
    /* tab change channel */
    unsigned char ch;
    /* tab change mode */
    unsigned char mode;
};

void tabs_init(void);
void load_tab(unsigned char tab);

void shell_cmd_tabs(char *args, void *data);

#endif	/* TABS_H */

