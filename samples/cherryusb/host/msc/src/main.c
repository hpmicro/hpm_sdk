/*
 * Copyright (c) 2022 HPMicro
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

extern void usbh_class_test(void);

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start usb host task...\r\n");
    usbh_initialize(0, CONFIG_HPM_USBH_BASE);
    usbh_class_test();
    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}