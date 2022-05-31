/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_lcdc_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demo_widgets.h"

#define LV_TICK 5 

#ifndef TIMER_CLK_NAME
#define TIMER_CLK_NAME clock_mchtmr0
#endif

uint32_t mchtmr_freq_in_khz = 0;

void isr_mchtmr(void)
{
    lv_tick_inc(LV_TICK);
    mchtmr_delay(HPM_MCHTMR, mchtmr_freq_in_khz * LV_TICK);
}
SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)

int main(void)
{
    board_init();
    board_init_cap_touch();
    board_init_lcd();

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    mchtmr_freq_in_khz = clock_get_frequency(TIMER_CLK_NAME) / 1000;
    printf("littlevgl widgets\n");
    printf("%d color depth @%d\n", LV_COLOR_SIZE, clock_get_frequency(clock_display));

    lv_demo_widgets();

    mchtmr_delay(HPM_MCHTMR, mchtmr_freq_in_khz * LV_TICK);
    enable_mchtmr_irq();

    while(1)
    {
        lv_task_handler();
    }
    return 0;
}
