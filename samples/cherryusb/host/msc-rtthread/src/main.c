/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "usbh_core.h"
#include "rtt_port.h"

extern void usbh_class_test(void);

void rt_hw_board_init(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);
    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    rtt_base_init();
}

int main(void)
{
    printf("Start usb host task...\r\n");
    usbh_initialize(0, CONFIG_HPM_USBH_BASE);
    usbh_class_test();

    return 0;
}