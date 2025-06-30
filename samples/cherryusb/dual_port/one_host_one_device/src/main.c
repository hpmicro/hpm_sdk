/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "usbh_core.h"

extern void msc_ram_init(uint8_t busid, uint32_t reg_base);

int main(void)
{
    board_init();
    board_init_usb(HPM_USB0);
    board_init_usb(HPM_USB1);

    printf("Cherryusb one host one device sample.\n");
    printf("USB0 as host, support HID class. USB1 as a MSC device.\r\n");

    /* set irq priority */
    intc_set_irq_priority(IRQn_USB0, 1);
    intc_set_irq_priority(IRQn_USB1, 2);

    /* usb host init */
    usbh_initialize(0, HPM_USB0_BASE);

    /* usb device init */
    msc_ram_init(0, HPM_USB1_BASE);

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
