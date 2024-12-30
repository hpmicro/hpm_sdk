/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_usb_drv.h"
#include "usb_util.h"
#include "usb_def.h"

static void usb_host_mode_init(USB_Type *ptr)
{
    /* Set the endian */
    ptr->USBMODE &= ~USB_USBMODE_ES_MASK;

    /* Set parallel interface signal */
    ptr->PORTSC1 &= ~USB_PORTSC1_STS_MASK;

    /* Set parallel transceiver width */
    ptr->PORTSC1 &= ~USB_PORTSC1_PTW_MASK;

#ifdef CONFIG_USB_HOST_FORCE_FULL_SPEED
    /* Set usb forced to full speed mode */
    ptr->PORTSC1 |= USB_PORTSC1_PFSC_MASK;
#endif

    /* Not use interrupt threshold. */
    ptr->USBCMD &= ~USB_USBCMD_ITC_MASK;
}

void usb_hc_low_level_init(void)
{
    usb_phy_init((USB_Type *)HPM_USB0_BASE, true);
    intc_m_enable_irq(IRQn_USB0);
}

void usb_hc_low_level2_init(void)
{
    usb_host_mode_init((USB_Type *)HPM_USB0_BASE);
}

uint8_t usbh_get_port_speed(const uint8_t port)
{
    uint8_t speed;
    (void)port;
    speed = usb_get_port_speed((USB_Type *)HPM_USB0_BASE);

    if (speed == 0x00) {
        return USB_SPEED_FULL;
    }
    if (speed == 0x01) {
        return USB_SPEED_LOW;
    }
    if (speed == 0x02) {
        return USB_SPEED_HIGH;
    }

    return 0;
}

extern void _ux_hcd_ehci_interrupt_handler(void);

SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usb0)
void isr_usb0(void)
{
    _ux_hcd_ehci_interrupt_handler();
}

#ifdef HPM_USB1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_USB1, isr_usb1)
void isr_usb1(void)
{
}
#endif
