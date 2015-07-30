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

#ifndef PARAMS_H
#define	PARAMS_H


struct param_value {
    union {
            float param_float;
            long param_int32;
            unsigned long param_uint32;
            int param_int16;
            unsigned int param_uint16;
            char param_int8;
            unsigned char param_uint8;
    };
};

struct param_def {
    /* param name */
    char name[17];
    /* param type */
    unsigned char type;
    /* param value pointer */
    void *value;
    /* callback to trigger when parameter changes */
    void (*cbk)(void);
};

struct param_dynamic_def {
    int (*set)(struct param_def *p);
    void (*get)(int idx, struct param_def *p);
    unsigned int (*total)(void);
};

#define PARAM(n, t, p, c) { .name = n, .type = t, .value = (void *) p, .cbk = c }
#define PARAM_END { .name = "" }


void params_add(const struct param_def *p);
void params_set_dynamic_params(const struct param_dynamic_def *p);
unsigned int params_get_total(void);
float params_get_value(int idx, char *name);
int params_set_value(char *name, float value, unsigned char trigger_cbk);


#endif	/* PARAMS_H */

