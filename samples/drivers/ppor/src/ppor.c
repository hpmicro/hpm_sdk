/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_ppor_drv.h"



void reset_soc(void)
{
    printf("%s\n", __func__);
    ppor_sw_reset(HPM_PPOR, 10);
}

int main(void)
{
    board_init();
    printf("ppor example\n");

    /*Enable software reset*/
    ppor_reset_mask_set_source_enable(HPM_PPOR, ppor_reset_software);
    /*If a software reset occurs, clear the reset flag and print the message*/
    if (ppor_reset_get_flags(HPM_PPOR) & ppor_reset_software) {
        ppor_reset_clear_flags(HPM_PPOR, ppor_reset_software);
        ppor_reset_mask_clear_source_enable(HPM_PPOR, ppor_reset_software);
        printf("Software reset has occurred\n");
        while (1) {
        };
    }

    reset_soc();
    while (1) {
    }
    return 0;
}
