/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_l1c_drv.h"
#include "ramstress_lib.h"

int main(void)
{
    board_init();
    disable_global_irq(CSR_MSTATUS_MIE_MASK);

    if (!l1c_dc_is_enabled()) {
        l1c_dc_enable();
        l1c_dc_invalidate_all();
    }
    __asm volatile("fence rw, rw");

    printf("SDRAM stress start: 0x%X, length: 0x%X\n", 
            BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);
    if (ramstress_run(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE)) {
        printf("ramstress_run: FAILED!!!\n");
        while (1);
    }

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback_all();
        __asm volatile("fence rw, rw");
        l1c_dc_disable();
    }
    __asm volatile("fence rw, rw");

    printf("SDRAM(nocache) stress start: 0x%X, length: 0x%X\n", 
            BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE);
    if (ramstress_run(BOARD_SDRAM_ADDRESS, BOARD_SDRAM_SIZE)) {
        printf("ramstress_run(nocache): FAILED!!!\n");
        while (1);
    }

    while (1);
    return 0;
}
