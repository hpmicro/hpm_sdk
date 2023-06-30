/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void msc_ram_init(void);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb msc_ram freertos sample.\n");

    /* threads are created by the function */
    msc_ram_init();

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
