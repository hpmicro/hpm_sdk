/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void hid_mouse_init(void);
extern void hid_mouse_test(void);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb_pins();
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb hid_mouse device sample.\n");

    hid_mouse_init();
    while (1) {
        hid_mouse_test();
    }

    return 0;
}
