/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Include
 *---------------------------------------------------------------------*/
#include "tusb_option.h"

#if TUSB_OPT_DEVICE_ENABLED && CFG_TUSB_MCU == OPT_MCU_HPM

#include "board.h"
#include "hpm_usb_device.h"
#include "common/tusb_common.h"
#include "device/dcd.h"

/*---------------------------------------------------------------------*
 * Macro Enum Declaration
 *---------------------------------------------------------------------*/
/* USBSTS, USBINTR */
enum {
    intr_usb         = HPM_BITSMASK(1, 0),
    intr_error       = HPM_BITSMASK(1, 1),
    intr_port_change = HPM_BITSMASK(1, 2),
    intr_reset       = HPM_BITSMASK(1, 6),
    intr_sof         = HPM_BITSMASK(1, 7),
    intr_suspend     = HPM_BITSMASK(1, 8),
    intr_nak         = HPM_BITSMASK(1, 16)
};

/*---------------------------------------------------------------------*
 * Macro Typedef Declaration
 *---------------------------------------------------------------------*/
typedef struct {
    USB_Type* regs;         /* register base*/
    const uint32_t irqnum;  /* IRQ number */
    const uint8_t ep_count; /* Max bi-directional Endpoints */
} dcd_controller_t;

/*---------------------------------------------------------------------*
 * Variable Definitions
 *---------------------------------------------------------------------*/
static const dcd_controller_t _dcd_controller[] =
{
    { .regs = (USB_Type*) HPM_USB0_BASE, .irqnum = IRQn_USB0, .ep_count = USB_SOC_DCD_MAX_ENDPOINT_COUNT },
#ifdef HPM_USB1_BASE
    { .regs = (USB_Type*) HPM_USB1_BASE, .irqnum = IRQn_USB1, .ep_count = USB_SOC_DCD_MAX_ENDPOINT_COUNT }
#endif
};

static ATTR_PLACE_AT_NONCACHEABLE usb_device_handle_t usb_device_handle[USB_SOC_MAX_COUNT];

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT) dcd_data_t _dcd_data;

/*---------------------------------------------------------------------*
 * Device API
 *---------------------------------------------------------------------*/

/* USB bus reset */
static void bus_reset(uint8_t rhport)
{
    usb_device_bus_reset(&usb_device_handle[rhport], CFG_TUD_ENDPOINT0_SIZE);
}

/* Initialize controller to device mode */
void dcd_init(uint8_t rhport)
{
    uint32_t int_mask;

    usb_device_handle[rhport].regs     = _dcd_controller[rhport].regs;
    usb_device_handle[rhport].dcd_data =  &_dcd_data;

    int_mask = (USB_USBINTR_UE_MASK | USB_USBINTR_UEE_MASK | USB_USBINTR_PCE_MASK | USB_USBINTR_URE_MASK

    #if (defined(USB_DEVICE_CONFIG_LOW_POWER_MODE) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
                    | USB_USBINTR_SLE_MASK
    #endif /* USB_DEVICE_CONFIG_LOW_POWER_MODE */
                    );

    usb_device_init(&usb_device_handle[rhport], int_mask);

    return;
}

/* De-initialize controller */
void dcd_deinit(uint8_t rhport)
{
    usb_device_deinit(&usb_device_handle[rhport]);
}

/* Enable device interrupt */
void dcd_int_enable(uint8_t rhport)
{
    intc_m_enable_irq(_dcd_controller[rhport].irqnum);
}

/* Disable device interrupt */
void dcd_int_disable(uint8_t rhport)
{
    intc_m_disable_irq(_dcd_controller[rhport].irqnum);
}

/* Receive Set Address request, mcu port must also include status IN response */
void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    usb_device_set_address(&usb_device_handle[rhport], dev_addr);
}

/* Wake up host */
void dcd_remote_wakeup(uint8_t rhport)
{
    usb_device_remote_wakeup(&usb_device_handle[rhport]);
}

/* Connect by enabling internal pull-up resistor on D+/D- */
void dcd_connect(uint8_t rhport)
{
    usb_device_connect(&usb_device_handle[rhport]);
}

/* Disconnect by disabling internal pull-up resistor on D+/D- */
void dcd_disconnect(uint8_t rhport)
{
    usb_device_disconnect(&usb_device_handle[rhport]);
}

/*---------------------------------------------------------------------*
 * Endpoint API
 *---------------------------------------------------------------------*/
/* Configure endpoint's registers according to descriptor */
bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_endpoint_desc)
{
    usb_endpoint_config_t ep_cfg;

    ep_cfg.xfer            = p_endpoint_desc->bmAttributes.xfer;
    ep_cfg.ep_addr         = p_endpoint_desc->bEndpointAddress;
    ep_cfg.max_packet_size = p_endpoint_desc->wMaxPacketSize;

    return usb_device_edpt_open(&usb_device_handle[rhport], &ep_cfg);
}

/* Submit a transfer, When complete dcd_event_xfer_complete() is invoked to notify the stack */
bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    return usb_device_edpt_xfer(&usb_device_handle[rhport], ep_addr, buffer, total_bytes);
}

/* Stall endpoint */
void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    usb_device_edpt_stall(&usb_device_handle[rhport], ep_addr);
}

/* clear stall, data toggle is also reset to DATA0 */
void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    usb_device_edpt_clear_stall(&usb_device_handle[rhport], ep_addr);
}

/* close a specified endpoint */
void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    usb_device_edpt_close(&usb_device_handle[rhport], ep_addr);
}

/* Close all non-control endpoints, cancel all pending transfers if any */
void dcd_edpt_close_all(uint8_t rhport)
{
    usb_device_edpt_close_all(&usb_device_handle[rhport]);
}
/*---------------------------------------------------------------------*
 * Internal API
 *---------------------------------------------------------------------*/
/* Index to bit position in register */
static inline uint8_t ep_idx2bit(uint8_t ep_idx)
{
    return ep_idx / 2 + ((ep_idx % 2) ? 16 : 0);
}

/*---------------------------------------------------------------------*
 * ISR
 *---------------------------------------------------------------------*/
void dcd_int_handler(uint8_t rhport)
{
    uint32_t int_status;
    uint32_t speed;
    uint32_t transfer_len;
    uint8_t result = XFER_RESULT_SUCCESS;
    bool qtd_active;
    usb_device_handle_t *handle = &usb_device_handle[rhport];

    /* Acknowledge handled interrupt */
    int_status = usb_device_status_flags(handle);
    int_status &= usb_device_interrupts(handle);
    usb_device_clear_status_flags(handle, int_status);

    /* disabled interrupt sources */
    if (int_status == 0) {
        return;
    }

    if (int_status & intr_reset) {
        bus_reset(rhport);
        speed = usb_device_get_port_speed(handle);
        dcd_event_bus_reset(rhport, (tusb_speed_t)speed, true);
    }

    if (int_status & intr_suspend) {
         if (usb_device_get_suspend_status(handle)) {
            /* Note: Host may delay more than 3 ms before and/or after bus reset before doing enumeration. */
            if (usb_device_get_address(handle)) {
                dcd_event_bus_signal(rhport, DCD_EVENT_SUSPEND, true);
            }
        }
    }

    if (int_status & intr_port_change) {
        if (!usb_device_get_port_ccs(handle)) {
            dcd_event_t event = {.rhport = rhport, .event_id = DCD_EVENT_UNPLUGGED};
            dcd_event_handler(&event, true);
        } else {
            dcd_event_t event = {.rhport = rhport, .event_id = DCD_EVENT_PLUGGED};
            if (usb_device_get_port_reset_status(handle) == 0) {
                event.plugged.speed = usb_device_get_port_speed(handle);
                dcd_event_handler(&event, true);
            }
        }
    }

    if (int_status & intr_usb) {
        uint32_t const edpt_complete = usb_device_get_edpt_complete_status(handle);
        uint32_t const edpt_setup_status = usb_device_get_setup_status(handle);

        if (edpt_complete) {
            usb_device_clear_edpt_complete_status(handle, edpt_complete);
            for(uint8_t ep_idx = 0; ep_idx < USB_SOS_DCD_MAX_QHD_COUNT; ep_idx++) {
                if (tu_bit_test(edpt_complete, ep_idx2bit(ep_idx))) {
                    transfer_len = 0;
                    qtd_active = false;

                    /* Failed QTD also get ENDPTCOMPLETE set */
                    dcd_qhd_t *p_qhd = usb_device_qhd_get(&usb_device_handle[rhport], ep_idx);
                    dcd_qtd_t *p_qtd = p_qhd->attached_qtd;
                    while (1) {
                        if (p_qtd->halted) {
                            result = XFER_RESULT_STALLED;
                            break;
                        } else if (p_qtd->xact_err || p_qtd->buffer_err) {
                            result = XFER_RESULT_FAILED;
                            break;
                        } else if (p_qtd->active) {
                            qtd_active = true;
                            break;
                        } else {
                            transfer_len += p_qtd->expected_bytes - p_qtd->total_bytes;
                            p_qtd->in_use = false;
                        }

                        if (p_qtd->next == USB_SOC_DCD_QTD_NEXT_INVALID){
                            break;
                        } else {
                            p_qtd = (dcd_qtd_t *)p_qtd->next;
                        }
                    }

                    if (!qtd_active) {
                        uint8_t const ep_addr = (ep_idx/2) | ( (ep_idx & 0x01) ? TUSB_DIR_IN_MASK : 0);
                        dcd_event_xfer_complete(rhport, ep_addr, transfer_len, result, true);
                    }
                }
            }
        }

        if (edpt_setup_status) {
            /*------------- Set up Received -------------*/
            usb_device_clear_setup_status(handle, edpt_setup_status);
            dcd_qhd_t *qhd0 = usb_device_qhd_get(&usb_device_handle[rhport], 0);
            dcd_event_setup_received(rhport, (uint8_t*) &qhd0->setup_request, true);
        }

    }

    if (int_status & intr_sof) {
        dcd_event_bus_signal(rhport, DCD_EVENT_SOF, true);
    }

    if (int_status & intr_nak) {}
    if (int_status & intr_error) TU_ASSERT(false, );
}

SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usb0)
void isr_usb0(void)
{
    dcd_int_handler(0);
}

#ifdef HPM_USB1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_USB1, isr_usb1)
void isr_usb1(void)
{
    dcd_int_handler(1);
}
#endif

#endif
