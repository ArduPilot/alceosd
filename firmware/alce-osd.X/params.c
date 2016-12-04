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


#define MAX_CONFIG_PARAMS   (50)
static struct param_def *all_params[MAX_CONFIG_PARAMS];
static unsigned int nr_params = 0;
static struct param_dynamic_def *dynamic_params;

const char *mavdata_type_name[] = {
    "CHAR", "UINT8_T", "INT8_T",
    "UINT16_T", "INT16_T",
    "UINT32_T", "INT32_T",
    "UINT64_T", "INT64_T",
    "FLOAT", "DOUBLE"
};

float cast2float(void *value, u8 type)
{
    switch (type) {
        case MAV_PARAM_TYPE_UINT8:
            return (float) *((unsigned char*) (value));
        case MAV_PARAM_TYPE_INT8:
            return (float) *((char*) (value));
        case MAV_PARAM_TYPE_UINT16:
            return (float) *((unsigned int*) (value));
        case MAV_PARAM_TYPE_INT16:
            return (float) *((int*) (value));
        case MAV_PARAM_TYPE_REAL32:
            return (float) *((float*) (value));
        default:
            return 0;
    }
}

static void cast2param(struct param_def *p, float v)
{
    switch (p->type) {
        case MAV_PARAM_TYPE_UINT8:
            *((unsigned char*) (p->value)) = (unsigned char) v;
            break;
        case MAV_PARAM_TYPE_INT8:
            *((char*) (p->value)) = (char) v;
            break;
        case MAV_PARAM_TYPE_UINT16:
            *((unsigned int*) (p->value)) = (unsigned int) v;
            break;
        case MAV_PARAM_TYPE_INT16:
            *((int*) (p->value)) = (int) v;
            break;
        case MAV_PARAM_TYPE_REAL32:
            *((float*) (p->value)) = (float) v;
            break;
        default:
            break;
    }
}


static unsigned int param_find(char *name)
{
    unsigned int idx;
    for (idx = 0; idx < nr_params; idx++) {
        if (strcmp(name, all_params[idx]->name) == 0)
            break;
    }
    return idx;
}


void params_set_dynamic_params(const struct param_dynamic_def *p)
{
    dynamic_params = (struct param_dynamic_def*) p;
}

void params_add(const struct param_def *p)
{
    while (p->name[0] != '\0')
        all_params[nr_params++] = (struct param_def*) p++;
}

unsigned int params_get_total(void)
{
    return nr_params + dynamic_params->total();
}

float params_get_value(int idx, char *name)
{
    struct param_def p;
    struct param_value pv;

    p.value = &pv;

    if (idx == -1)
        idx = param_find(name);

    if (idx < nr_params) {
        strcpy(name, all_params[idx]->name);
        return cast2float(all_params[idx]->value, all_params[idx]->type);
    } else {
        /* TODO: implement get dynamic param by name */
        dynamic_params->get(idx - nr_params, &p);
        strcpy(name, p.name);
        return cast2float(p.value, p.type);
    }
}

int params_set_value(char *name, float value, unsigned char trigger_cbk)
{
    struct param_def *p, sp;
    int idx;

    idx = param_find(name);
    //printf("name='%s' idx=%d\n", name, idx);
    if (idx < nr_params) {
        p = all_params[idx];
        cast2param(p, value);
        if ((p->cbk != NULL) && trigger_cbk)
            p->cbk();
    } else {
        p = &sp;
        strcpy(sp.name, name);
        sp.value = (void*) &value;
        idx = dynamic_params->set(p) + nr_params;
    }
    return idx;
}
