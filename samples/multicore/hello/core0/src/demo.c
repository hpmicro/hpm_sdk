/*
 * Copyright (c) 2022 HPMicro
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
#include "multicore_common.h"

static void secondary_core_image_load_and_run(void);

int main(void)
{
    int u;
    board_init();

    secondary_core_image_load_and_run();

    printf("Hello world for multicore! Secondary core control RGB leds blinking...\n");

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
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
}
