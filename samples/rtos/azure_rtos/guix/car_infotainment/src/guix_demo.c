/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* This is a small demo of the high-performance GUIX graphics framework. */

#include "demo_guix_car_infotainment.h"


/******************************************************************************************/
/* Application entry.                                                                     */
/******************************************************************************************/
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