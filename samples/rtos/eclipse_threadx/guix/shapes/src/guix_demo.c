/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tx_api.h"
#include "board.h"
#include "guix_touch.h"
#include "guix_display_driver_565rgb.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)**argv;
    board_init();
    board_init_lcd();
    board_init_cap_touch();
    hpm_touchpad_init();
    printf("board pins init ok\n");

    tx_kernel_enter();
    return 0;
}

