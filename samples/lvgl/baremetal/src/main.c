/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_lvgl.h"

void hpm_lvgl_demos(void);

int main(void)
{
    board_init();
    board_init_cap_touch();
    board_init_lcd();

    hpm_lvgl_init();
    hpm_lvgl_demos();

    while (1) {
        lv_timer_periodic_handler();
    }

    return 0;
}
