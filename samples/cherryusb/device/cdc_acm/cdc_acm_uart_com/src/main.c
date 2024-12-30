/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usbd_core.h"
#include "usbd_cdc_acm.h"
#include "cdc_acm.h"
#include "uart_dma.h"

int main(void)
{
    board_init();

    printf("cherry usb cdc_acm uart device sample.\n");
    printf("please use USB_CDC_ACM_UART pins.\n");

    init_board_app_uart();
    init_board_app_dma();

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 3);
    cdc_acm_init(USB_BUS_ID, CONFIG_HPM_USBD_BASE);

    board_timer_create(5, uart_task_entry_5ms);

    while (1) {
        ;
    }
    return 0;
}
