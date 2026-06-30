/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_usb_drv.h"
#include "dfu.h"
#include "hpm_dfu_trigger.h"

void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid; /* Unused parameter */
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

/* Flash operations are implemented in usb_dc_hpm.c */

int main(void)
{
    board_init();
    cherryusb_dfu_jump_app();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 3);

    printf("HPMicro Cherryusb DFU Demo\n");
    /* Initialize USB DFU */
    dfu_init(0, CONFIG_HPM_USBD_BASE);

    while (1) {
        /* Main loop */
    }
}
