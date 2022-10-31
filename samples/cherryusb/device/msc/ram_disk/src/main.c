/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void msc_ram_init(void);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb msc_ram sample.\n");

    msc_ram_init();
    while (1) {
    }

    return 0;
}
