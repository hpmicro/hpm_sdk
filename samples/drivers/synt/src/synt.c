/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_synt_drv.h"

int main(void)
{
    uint32_t i, j;
    board_init();
    synt_reset_counter(HPM_SYNT);
    synt_enable_counter(HPM_SYNT, true);
    synt_set_reload(HPM_SYNT, 0x888888U);

    printf("synt example\n");
    for (i = 0; i < 100; i++) {
        printf("[%d]: 0x%x\n", i, synt_get_current_count(HPM_SYNT));
        for (j = 0; j < 500000; j++) {
            __asm("nop");
        }
    }

    synt_enable_counter(HPM_SYNT, false);
    while(1);
    return 0;
}
