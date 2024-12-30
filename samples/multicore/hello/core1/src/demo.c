/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"

#define LED_FLASH_PERIOD_IN_MS 300

int main(void)
{
    board_init_core1();
    board_init_led_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    return 0;
}
