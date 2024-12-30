/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void hid_mouse_init(uint8_t busid, uint32_t reg_base);
extern void hid_mouse_test(uint8_t busid);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb hid_mouse device remote wakeup sample.\n");

    hid_mouse_init(0, CONFIG_HPM_USBD_BASE);
    while (1) {
        hid_mouse_test(0);
    }

    return 0;
}
