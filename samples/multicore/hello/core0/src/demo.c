/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_debug_console.h"
#include "sec_core_img.h"

static void secondary_core_image_load_and_run(void);

int main(void)
{
    int u;
    board_init();

    secondary_core_image_load_and_run();

    printf("hello world from primary core\n");

    while (1) {
        u = getchar();
        if (u == '\r') {
            u = '\n';
        }
        printf("%c", u);
    }
    return 0;
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
        printf("Secondary core started, RGB leds are blinking...\n\n");
    }
#endif
}
