#include <math.h>
#include <string.h>
#include <stdio.h>
#include <p33Exxxx.h>

#include "mavlink.h"
#include "graphics.h"
#include "widgets.h"


#define X_SHIFT 32

#define X_SIZE  (192 - X_SHIFT)
#define Y_SIZE  140

#define RANGE       Y_SIZE
#define SCALE       5
#define MINOR_TICK  5
#define MAJOR_TICK  10
#define Y_CENTER    Y_SIZE/2
#define X_CENTER    (X_SIZE/2 - (X_SHIFT/2))

#define ROLL_RANGE  90
#define MINOR_ROLL_TICK  5
#define MAJOR_ROLL_TICK  15


static struct widget_priv {
    float pitch, roll;

    int pitch_deg, roll_deg;
    float cos_roll, sin_roll;

    struct canvas canvas;
} priv;


#define RAD2DEG(x)                  (x*57.2957795131)
#define DEG2RAD(x)                  (x*0.0174532925199)

const struct widget horizon_widget;

static void mav_callback(mavlink_message_t *msg, mavlink_status_t *status)
{
    priv.pitch = mavlink_msg_attitude_get_pitch(msg);
    priv.pitch_deg = RAD2DEG(priv.pitch * SCALE);

    priv.roll = mavlink_msg_attitude_get_roll(msg);
    priv.roll_deg  = RAD2DEG(priv.roll);
    priv.cos_roll = cos(priv.roll);
    priv.sin_roll = -1 * sin(priv.roll);

    schedule_widget(&horizon_widget);
}

static void init(struct widget_config *wcfg)
{
    struct canvas *ca = &priv.canvas;

    priv.roll = 0;
    priv.pitch = 0;
    priv.cos_roll = cos(0);
    priv.sin_roll = sin(0);
    add_mavlink_callback(MAVLINK_MSG_ID_ATTITUDE, mav_callback, CALLBACK_WIDGET);

    alloc_canvas(ca, wcfg, X_SIZE, Y_SIZE);
}


static int render(void)
{
    struct canvas *ca = &priv.canvas;
    int y, i, j;
    int x0, x1, y0, y1, offset, cx, cy;
    unsigned char size, gap;
    char buf[10];

    if (init_canvas(ca, 0))
        return 1;

    for (i = -RANGE/2; i <= RANGE/2; i++) {
        y = Y_CENTER - i;
        //j = priv.pitch_deg*SCALE + i;
        j = priv.pitch_deg + i;

        if (j % (MINOR_TICK*SCALE) == 0) {
            if (j == 0) {
                size = 40;
                gap = 10;
            } else {
                if (j % (MAJOR_TICK*SCALE) == 0)
                    size = 20;
                else
                    size = 10;
                gap = 10;
            }

            cx = X_CENTER + (int) (i * priv.sin_roll);
            cy = y + i - (int) (i * priv.cos_roll);

            
            offset = (int) gap * priv.cos_roll;
            x0 = cx + offset;
            offset = (int) size * priv.cos_roll;
            x1 = x0 + offset;

            
            offset = (int) gap * priv.sin_roll;
            y0 = cy + offset;
            offset = (int) size * priv.sin_roll;
            y1 = y0 + offset;
            
            if (j == 0) {
                draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
                //draw_line(x0, y0-1, x1, y1-1, 1, ca);
            }
            draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
            draw_line(x0, y0, x1, y1, 1, ca);

            if ((j != 0) && (j % (MAJOR_TICK*SCALE) == 0)) {
                sprintf(buf, "%d", j / SCALE);
                draw_str(buf, x1, y1, ca);
            }


            offset = (int) gap * priv.cos_roll;
            x0 = cx - offset;
            offset = (int) size * priv.cos_roll;
            x1 = x0 - offset;


            offset = (int) gap * priv.sin_roll;
            y0 = cy - offset;
            offset = (int) size * priv.sin_roll;
            y1 = y0 - offset;

            if (j == 0) {
                draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
                //draw_line(x0, y0-1, x1, y1-1, 1, ca);
            }
            draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
            draw_line(x0, y0, x1, y1, 1, ca);



        }

        //get_vec(50, 50, priv.cos_roll, priv.sin_roll, cpitch + (i-1)*10, 40, &l);
        //draw_line(&l[i], 1, &ca);
    }

    draw_vline(X_CENTER+1, Y_CENTER - 3+1, Y_CENTER + 3+1, 3, ca);
    draw_hline(X_CENTER - 4, X_CENTER + 4, Y_CENTER+1, 3, ca);

    draw_vline(X_CENTER, Y_CENTER - 3, Y_CENTER + 3, 1, ca);
    draw_hline(X_CENTER - 4, X_CENTER + 4, Y_CENTER, 1, ca);

    float cos_i, sin_i;

    for (i = -ROLL_RANGE/2; i <= ROLL_RANGE/2; i++) {
        y = Y_CENTER - i;
        //j = priv.pitch_deg*SCALE + i;
        //j = priv.roll_deg + i;

        if (i % (MINOR_ROLL_TICK) == 0) {
            gap = 70;
            if (i == 0) {
                size = 10;
            } else {
                if (i % (MAJOR_ROLL_TICK) == 0)
                    size = 10;
                else
                    size = 5;
            }

            cos_i = cos(DEG2RAD(i));
            sin_i = sin(DEG2RAD(i));
            
            cx = X_CENTER;// + (int) (gap * cos_i);
            cy = Y_CENTER;// - (int) (gap * sin_i);


            offset = (int) gap * cos_i;
            x0 = cx + offset;
            offset = (int) size * cos_i;
            x1 = x0 + offset;


            offset = (int) gap * sin_i;
            y0 = cy - offset;
            offset = (int) size * sin_i;
            y1 = y0 - offset;

            draw_line(x0, y0, x1, y1, 1, ca);

            if ((i != 0) && (i % (MAJOR_ROLL_TICK) == 0)) {
                sprintf(buf, "%d", i);
                draw_str(buf, x1, y1, ca);
            }
        }



        //get_vec(50, 50, priv.cos_roll, priv.sin_roll, cpitch + (i-1)*10, 40, &l);
        //draw_line(&l[i], 1, &ca);
    }


    cx = X_CENTER; // + (int) (gap * priv.sin_roll);
    cy = Y_CENTER ; //- (int) (gap * priv.cos_roll);
    size = 10;

    offset = (int) (gap-size) * priv.cos_roll;
    x0 = cx + offset;
    offset = (int) size * priv.cos_roll;
    x1 = x0 + offset;


    offset = (int) (gap-size) * priv.sin_roll;
    y0 = cy + offset;
    offset = (int) size * priv.sin_roll;
    y1 = y0 + offset;


    draw_line(x0-1, y0-1, x1+1, y1-1, 3, ca);
    draw_line(x0-1, y0+1, x1+1, y1+1, 3, ca);
    draw_line(x0, y0, x1, y1, 1, ca);
    
    schedule_canvas(ca);
    return 0;
}

const struct widget horizon_widget = {
    .name = "Articial Horizon",
    .id = WIDGET_HORIZON_ID,
    .init = init,
    .render = render,
};
