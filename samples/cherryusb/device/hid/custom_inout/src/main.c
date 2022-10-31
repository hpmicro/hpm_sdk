/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"


#define LED_FLASH_PERIOD_IN_MS 300

extern void hid_custom_init(void);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb hid_custom in/out device sample.\n");

    hid_custom_init();
    while (1) {
        ;
    }

    return 0;
}
