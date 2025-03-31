/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usb_config.h"

extern void webusb_init(uint8_t busid, uint32_t reg_base);

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);

    printf("cherryusb device webusb sample.\n");

    webusb_init(0, CONFIG_HPM_USBD_BASE);

    while (1) {
        ;
    }
    return 0;
}
