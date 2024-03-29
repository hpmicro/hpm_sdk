/*
 * Copyright (c) 2021 HPMicro
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
#include "hpm_mchtmr_drv.h"
#include "multicore_common.h"
#include "core_portme.h"

#ifndef TIMER_CLK_NAME
#define TIMER_CLK_NAME clock_mchtmr0
#endif

extern coremark_context_t g_coremark_ctx[2];

extern int main(void);

#define COREMARK_MAIN main

void run_coremark_for_cpu(uint32_t index);

volatile coremark_context_t *core0_cm_ctx;
volatile coremark_context_t *core1_cm_ctx;

int app_main(void)
{
    board_init();

    printf("Dual-core Coremark example\n");

    core0_cm_ctx = (volatile coremark_context_t *)&g_coremark_ctx[0];
    core1_cm_ctx = (volatile coremark_context_t *)&g_coremark_ctx[1];

    while (1) {
        run_coremark_for_cpu(1);
        run_coremark_for_cpu(0);

        while (!core0_cm_ctx->has_done) {
        }
        printf("------------------------------------------------------\n");
        printf("|                                                    |\n");
        printf("|          Core0 CoreMark Info:                      |\n");
        printf("|                                                    |\n");
        printf("------------------------------------------------------\n");
        printf("%s\n", core0_cm_ctx->s_buffer);
        printf("\n\n");

        do {
            uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(core1_cm_ctx);
            uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)core1_cm_ctx + sizeof(coremark_context_t));
            uint32_t aligned_size = aligned_end - aligned_start;
            l1c_dc_invalidate(aligned_start, aligned_size);
            fencei();
        } while (!core1_cm_ctx->has_done);

        printf("------------------------------------------------------\n");
        printf("|                                                    |\n");
        printf("|               Core1 CoreMark Info:                 |\n");
        printf("|                                                    |\n");
        printf("------------------------------------------------------\n");
        printf("%s\n", core1_cm_ctx->s_buffer);
        printf("\n\n");

        printf("Press any key to start a new round of coremark test\n");
        getchar();
        putchar('\n');
    }
    return 0;
}

void run_coremark_for_cpu(uint32_t index)
{
    if (index == 0) {
        extern int COREMARK_MAIN(void);
        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        printf("Run Coremark on Core0...\n\n");
        COREMARK_MAIN();
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
    } else if (index == 1) {
        if (HPM_SYSCTL->CPU[1].GPR[1] != 0xc1bef1a9) {
            multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
            printf("Run Coremark on Core1...\n\n");
            board_delay_ms(100);
        } else {
            printf("Run Coremark on Core1...\n\n");
            HPM_SYSCTL->CPU[1].GPR[3] = 1;
            board_delay_ms(100);
        }
    } else {
    }
}
