/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usb_config.h"
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

extern void msc_device_init(uint8_t busid, uint32_t reg_base);

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    printf("cherry usb msc device host sample.\n");

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    msc_device_init(0, CONFIG_HPM_USBD_BASE);

    vTaskStartScheduler();
    while (1) {
    }

    return 0;
}
