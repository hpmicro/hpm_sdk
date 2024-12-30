/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern void msc_ram_init(uint8_t busid, uint32_t reg_base);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb msc_ram sample.\n");

    msc_ram_init(0, CONFIG_HPM_USBD_BASE);
    while (1) {
    }

    return 0;
}
