/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_touch.h"

int main(void)
{
    hpm_stat_t stat;
    uint8_t i = 0;
    uint8_t num_of_points = 0;

    touch_point_t points[HPM_TOUCH_MAX_POINTS];

    board_init();
    board_init_console();
    board_init_lcd();
    board_init_cap_touch();

    printf("touch example\n");
    stat = touch_init(BOARD_CAP_I2C_BASE);
    if (stat != status_success) {
        printf("touch init failed %d\n", stat);
        while(1);
    }

    while(1) {
        stat = touch_get_data(points, &num_of_points);
        if (stat == status_fail) {
            printf("touch read data failed\n");
            while(1);
        } else if ((stat == status_touch_buffer_no_ready) ||
                     stat == status_touch_points_over_number) {
            continue;
        }
        for (i = 0; i < num_of_points; i++) {
            printf("[%d]: x: %d, y: %d\n", i, points[i].x, points[i].y);
        }
    }

    while(1);
}
