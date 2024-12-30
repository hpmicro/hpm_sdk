/*
 * Copyright (c) 2024 HPMicro
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

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start usb host task...\r\n");
    usbh_initialize(0, CONFIG_HPM_USBH_BASE);

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}