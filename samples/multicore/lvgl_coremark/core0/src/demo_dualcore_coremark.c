/*
 * Copyright (c) 2021-2023 HPMicro
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

#include "hpm_lvgl.h"

lv_coremark_ctx_t g_lv_cm_ctx;

int app_main(void)
{
    board_init();
    board_init_cap_touch();
    board_init_lcd();

    hpm_lvgl_init();

    printf("lvgl dualcore coremark example\n");
    init_coremark_cpu_freq();
    init_coremark_context();
    init_coremark_result(&g_lv_cm_ctx);
    lv_dualcore_coremark_demo();

    while (1) {
        lv_timer_periodic_handler();
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
