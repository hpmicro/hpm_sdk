/*
 * Copyright (c) 2025 HPMicro
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
#include "hpm_usb_drv.h"
#include "hpm_gpio_drv.h"
#include "usbotg_core.h"

extern void usbd_event_handler(uint8_t busid, uint8_t event);
extern void cdc_acm_init(uint8_t busid);

int main(void)
{
    uint8_t usb_role;

    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBOTG_BASE);

    printf("CherryUSB OTG Sample...\r\n");

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBOTG_IRQn, 1);

    cdc_acm_init(0);

    usb_role = gpio_read_pin(BOARD_USB_ID_GPIO_CTRL, BOARD_USB_ID_GPIO_INDEX, BOARD_USB_ID_GPIO_PIN) ? USBOTG_MODE_DEVICE : USBOTG_MODE_HOST;
    usbotg_initialize(0, CONFIG_HPM_USBOTG_BASE, usbd_event_handler, NULL, usb_role);

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}