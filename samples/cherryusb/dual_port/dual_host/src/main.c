/*
 * Copyright (c) 2023-2025 HPMicro
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
#include "hpm_gpio_drv.h"
#include "usbh_core.h"

extern void usbh_hid_class_test(void);
extern void usbh_msc_class_test(void);

int main(void)
{
    board_init();
    board_init_usb(HPM_USB0);
    board_init_usb(HPM_USB1);

    /* set irq priority */
    intc_set_irq_priority(IRQn_USB0, 1);
    intc_set_irq_priority(IRQn_USB1, 2);

    printf("Cherryusb dual port host sample. Support HID and MSC class.\n");

    usbh_initialize(0, HPM_USB0_BASE);
    usbh_initialize(1, HPM_USB1_BASE);

    printf("Start usb host task...\r\n");

    usbh_hid_class_test();
    usbh_msc_class_test();

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}