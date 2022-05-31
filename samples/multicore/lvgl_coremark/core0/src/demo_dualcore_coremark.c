/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_mchtmr_drv.h"
#include "dualcore_widgets.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#define LV_TICK 10

#ifndef TIMER_CLK_NAME
#define TIMER_CLK_NAME clock_mchtmr0
#endif

uint32_t mchtmr_freq_in_khz;

lv_coremark_ctx_t g_lv_cm_ctx;

void isr_mchtmr(void)
{
    lv_tick_inc(LV_TICK);
    mchtmr_delay(HPM_MCHTMR, mchtmr_freq_in_khz * LV_TICK);
}
SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)

void reset_handler(void);

void update_coremark_result(lv_coremark_ctx_t *cm_ctx);

int app_main(void)
{
    board_init();
    board_init_cap_touch();
    board_init_lcd();

    mchtmr_freq_in_khz = clock_get_frequency(TIMER_CLK_NAME) / 1000;

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    printf("littlevgl example\n");
    printf("%d color depth @%d\n", LV_COLOR_SIZE, clock_get_frequency(clock_display));
    init_coremark_context();
    init_coremark_result(&g_lv_cm_ctx);
    lv_dualcore_coremark_demo();

    mchtmr_delay(HPM_MCHTMR, mchtmr_freq_in_khz * LV_TICK);
    enable_mchtmr_irq();

    while (1) {
        lv_task_handler();
        refresh_coremark_info();
    }
    return 0;
}

void init_coremark_result(lv_coremark_ctx_t *cm_ctx)
{
    cm_ctx->core_freq[0] = clock_get_frequency(clock_cpu0);
    cm_ctx->core_freq[1] = clock_get_frequency(clock_cpu1);
    cm_ctx->bus_freq[0] = clock_get_frequency(clock_axi0);
    cm_ctx->bus_freq[1] = clock_get_frequency(clock_axi0);
    cm_ctx->result_ready[0] = false;
    cm_ctx->result_ready[1] = false;
}

void reset_handler(void)
{
    l1c_dc_disable();
#ifndef __SEGGER_RTL_VERSION
    /*
     * Initialize LMA/VMA sections.
     * Relocation for any sections that need to be copied from LMA to VMA.
     */
    extern void c_startup(void);
    c_startup();
#endif

    /* Call platform specific hardware initialization */
    extern void system_init(void);
    system_init();

#ifdef __cplusplus
    /* Do global constructors */
    __libc_init_array();
#endif

    /* Entry function */
    app_main();
}
