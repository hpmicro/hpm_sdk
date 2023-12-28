/*
 * Copyright (c) 2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "gx_api.h"
#include "tx_api.h"
#include "hpm_soc.h"
#include "hpm_touch.h"


#define TOUCH_STATE_TOUCHED     1
#define TOUCH_STATE_RELEASED    2

#define MIN_DRAG_DELTA          10
#define TOUCH_THREAD_SIZE       1024

TX_THREAD touch_thread;
UCHAR touch_thread_stack[TOUCH_THREAD_SIZE];

struct guix_touch_t {
    uint8_t touch_state;
    GX_VALUE last_pos_x;
    GX_VALUE last_pos_y; 
};

struct guix_touch_t guix_touch;

static VOID gx_send_pen_down_event(GX_VALUE x, GX_VALUE y)
{
    GX_EVENT event;

    event.gx_event_type                                  = GX_EVENT_PEN_DOWN;
    event.gx_event_payload.gx_event_pointdata.gx_point_x = x;
    event.gx_event_payload.gx_event_pointdata.gx_point_y = y;
    event.gx_event_sender                                = 0;
    event.gx_event_target                                = 0;
    event.gx_event_display_handle                        = 0;
    gx_system_event_send(&event);

    guix_touch.last_pos_x = x;
    guix_touch.last_pos_y = y;
}

static VOID gx_send_pen_up_event(VOID)
{
    GX_EVENT event;

    event.gx_event_type                                  = GX_EVENT_PEN_UP;
    event.gx_event_payload.gx_event_pointdata.gx_point_x = guix_touch.last_pos_x;
    event.gx_event_payload.gx_event_pointdata.gx_point_y = guix_touch.last_pos_y;
    event.gx_event_sender                                = 0;
    event.gx_event_target                                = 0;
    event.gx_event_display_handle                        = 0;
    gx_system_event_send(&event);
}

static VOID gx_send_pen_drag_event(GX_VALUE x, GX_VALUE y)
{
    GX_EVENT event;

    event.gx_event_type                                  = GX_EVENT_PEN_DRAG;
    event.gx_event_payload.gx_event_pointdata.gx_point_x = x;
    event.gx_event_payload.gx_event_pointdata.gx_point_y = y;
    event.gx_event_sender                                = 0;
    event.gx_event_target                                = 0;
    event.gx_event_display_handle                        = 0;
    gx_system_event_fold(&event);

    guix_touch.last_pos_x = x;
    guix_touch.last_pos_y = y;
}

static void touch_thread_entry(ULONG thread_input)
{
    (void)thread_input;
    hpm_stat_t status;
    touch_point_t touch_points[HPM_TOUCH_MAX_POINTS];
    uint8_t num_of_points;
    int x_delta, y_delta;
    struct guix_touch_t *touch = &guix_touch; 

    touch->touch_state = TOUCH_STATE_RELEASED;
    for(; ;) {
        tx_thread_sleep(2);
        status = touch_get_data(touch_points, &num_of_points);
        if ((status == status_success) &&\
            (num_of_points < HPM_TOUCH_MAX_POINTS) &&\
            num_of_points) {
            if (touch->touch_state == TOUCH_STATE_RELEASED) {
                touch->touch_state = TOUCH_STATE_TOUCHED;
                gx_send_pen_down_event(touch_points[0].x, touch_points[0].y);
            } else {
                x_delta = abs(touch_points[0].x - touch->last_pos_x);
                y_delta = abs(touch_points[0].y - touch->last_pos_y);
                if (x_delta > MIN_DRAG_DELTA || y_delta > MIN_DRAG_DELTA)
                    gx_send_pen_drag_event(touch_points[0].x, touch_points[0].y);
            }
        } else {
            if (touch->touch_state == TOUCH_STATE_TOUCHED) {
                touch->touch_state = TOUCH_STATE_RELEASED;
                gx_send_pen_up_event();
            }
        }
    }
}

void hpm_touchpad_init(void)
{
    hpm_stat_t stat;
    stat = touch_init(BOARD_CAP_I2C_BASE);
    if (stat != status_success)
        while(1);
}

void start_touch_thread(void)
{
    tx_thread_create(&touch_thread, "GUIX Touch Thread", touch_thread_entry, 0,
                     touch_thread_stack, sizeof(touch_thread_stack),
                     GX_SYSTEM_THREAD_PRIORITY - 2,GX_SYSTEM_THREAD_PRIORITY - 2,
                     TX_NO_TIME_SLICE, TX_AUTO_START);
}
