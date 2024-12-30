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
#include "usbh_core.h"

#if !defined(USBH_USE_CUSTOM_ISR) || !USBH_USE_CUSTOM_ISR
#error "this sample must set USBH_USE_CUSTOM_ISR=1"
#endif

#if !defined(USBD_USE_CUSTOM_ISR) || !USBD_USE_CUSTOM_ISR
#error "this sample must set USBD_USE_CUSTOM_ISR=1"
#endif

static uint8_t usbh_busid;
static uint8_t usbd_busid;

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

    usbh_busid = 0;
    usbh_initialize(usbh_busid, HPM_USB0_BASE);
    usbd_busid = 0;
    msc_ram_init(usbd_busid, HPM_USB1_BASE);

    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}

extern void USBH_IRQHandler(uint8_t busid);
SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usbh0)
void isr_usbh0(void)
{
    USBH_IRQHandler(usbh_busid);
}


extern void USBD_IRQHandler(uint8_t busid);
SDK_DECLARE_EXT_ISR_M(IRQn_USB1, isr_usbd1)
void isr_usbd1(void)
{
    USBD_IRQHandler(usbd_busid);
}
