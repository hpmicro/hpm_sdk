/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "board.h"
#include "lwip/tcpip.h"
#include "usbh_core.h"

/* Function Definition */

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start cherryusb host usbnet task...\r\n");

    printf("\r\n************************************************\r\n");
    printf(" DHCP example\r\n");
    printf("************************************************\r\n");

    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    usbh_initialize(0, CONFIG_HPM_USBH_BASE);

    vTaskStartScheduler();

    return 0;
}
