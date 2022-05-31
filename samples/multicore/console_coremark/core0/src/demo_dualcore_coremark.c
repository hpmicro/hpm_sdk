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
#include "hpm_mchtmr_drv.h"
#include "sec_core_img.h"
#include "core_portme.h"

#ifndef TIMER_CLK_NAME
#define TIMER_CLK_NAME clock_mchtmr0
#endif


extern coremark_context_t g_coremark_ctx;
#define CORE_MARK_CTX_ADDR (uint32_t)&g_coremark_ctx

extern int main(void);

#define COREMARK_MAIN main

void run_coremark_for_cpu(uint32_t index);
static void secondary_core_image_load_and_run(void);
void reset_handler(void);


  volatile coremark_context_t *core0_cm_ctx;
  volatile coremark_context_t *core1_cm_ctx;

int app_main(void)
{
    board_init();

    printf("Dual-core Coremark example\n");

    core0_cm_ctx = (volatile coremark_context_t*)CORE_MARK_CTX_ADDR;
    core1_cm_ctx = (volatile coremark_context_t*)core_local_mem_to_sys_address(1, CORE_MARK_CTX_ADDR);


    while (1) {
        run_coremark_for_cpu(1);
        run_coremark_for_cpu(0);

        while (!core0_cm_ctx->has_done){
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

        } while(!core1_cm_ctx->has_done);
        
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

void reset_handler(void)
{
    l1c_dc_disable();
#if !defined(__SEGGER_RTL_VERSION) || defined(__GNU_LINKER)
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
            secondary_core_image_load_and_run();
            HPM_SYSCTL->CPU[1].GPR[0] = (uint32_t) SEC_CORE_IMG_START;
            HPM_SYSCTL->CPU[1].GPR[1] = 0xc1bef1a9;
            printf("Run Coremark on Core1...\n\n");
            HPM_SYSCTL->CPU[1].LP &= ~SYSCTL_CPU_LP_HALT_MASK;
            board_delay_ms(100);

        } else {
            printf("Run Coremark on Core1...\n\n");
            HPM_SYSCTL->CPU[1].GPR[3] = 1;
            board_delay_ms(100);
        }
    } else {

    }
}

static void secondary_core_image_load_and_run(void)
{
#ifdef SEC_CORE_IMG_START
    if (!sysctl_is_cpu1_released(HPM_SYSCTL)) {
        printf("\n\n");
        printf("Copying secondary core image to destination memory...\n");
        uint32_t sec_core_img_sys_addr = core_local_mem_to_sys_address(HPM_CORE1, (uint32_t)SEC_CORE_IMG_START);
        memcpy((void *)sec_core_img_sys_addr, sec_core_img, sec_core_img_size);
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(sec_core_img_sys_addr);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(sec_core_img_sys_addr + sec_core_img_size);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
        printf("Starting secondary core...\n");
        sysctl_set_cpu1_entry(HPM_SYSCTL, SEC_CORE_IMG_START);
        sysctl_release_cpu1(HPM_SYSCTL);
    }
#endif
}
