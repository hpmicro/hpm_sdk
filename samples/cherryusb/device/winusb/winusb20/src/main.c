/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usb_config.h"

extern void winusbv2_init(uint8_t busid, uint32_t reg_base);

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);

    printf("cherryusb device winusb 2.0 sample.\n");

    winusbv2_init(0, CONFIG_HPM_USBD_BASE);

    while (1) {
        ;
    }
    return 0;
}
