#include "hpm_common.h"
#include "hpm_soc.h"
#include "ehci_api.h"
#include "host/hcd.h"
#include "hpm_usb_drv.h"
#include "hpm_clock_drv.h"


typedef struct
{
    USB_Type *regs;            /* register base */
    const uint32_t irqnum;     /* IRQ number */
} hcd_controller_t;

/*---------------------------------------------------------------------*
 * Variable Definitions
 *---------------------------------------------------------------------*/
static const hcd_controller_t _hcd_controller[] =
{
    { .regs = (USB_Type *) HPM_USB0_BASE, .irqnum = IRQn_USB0 },
    #ifdef HPM_USB1_BASE
    { .regs = (USB_Type *) HPM_USB1_BASE, .irqnum = IRQn_USB1 }
    #endif
};

/*---------------------------------------------------------------------*
 * Function Definitions
 *---------------------------------------------------------------------*/
static void usb_host_mode_init(USB_Type *ptr)
{
    /* Stop */
    ptr->USBCMD &= ~USB_USBCMD_RS_MASK;

    /* Reset controller */
    ptr->USBCMD |= USB_USBCMD_RST_MASK;
    while (USB_USBCMD_RST_GET(ptr->USBCMD)) {
    }

    /* Set mode to host, must be set immediately after reset */
    ptr->USBMODE &= ~USB_USBMODE_CM_MASK;
    ptr->USBMODE |= USB_USBMODE_CM_SET(3);

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

bool hcd_init(uint8_t rhport)
{
    if (rhport > USB_SOC_MAX_COUNT) {
        return false;
    }

    usb_phy_init(_hcd_controller[rhport].regs, true);
    usb_host_mode_init(_hcd_controller[rhport].regs);
    ehci_init(rhport, ((uint32_t)(_hcd_controller[rhport].regs) + 0x100u), (uint32_t)(&_hcd_controller[rhport].regs->USBCMD));

    return true;
}

void hcd_int_enable(uint8_t rhport)
{
    intc_m_enable_irq(_hcd_controller[rhport].irqnum);
}

void hcd_int_disable(uint8_t rhport)
{
    intc_m_disable_irq(_hcd_controller[rhport].irqnum);
}

USB_Type *usb_get_hcd_controller_instance(uint8_t rhport)
{
    return _hcd_controller[rhport].regs;
}

SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usb0)
void isr_usb0(void)
{
    hcd_int_handler(0, true);
}

#ifdef HPM_USB1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_USB1, isr_usb1)
void isr_usb1(void)
{
    hcd_int_handler(1, true);
}
#endif
