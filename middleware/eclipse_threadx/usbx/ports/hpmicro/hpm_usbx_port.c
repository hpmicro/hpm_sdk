/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_usbx_port.h"
#include "assert.h"
#include "board.h"
#include "hpm_usb_device.h"
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"

/* USBSTS, USBINTR */
enum {
    INTR_UI = HPM_BITSMASK(1, 0), /* transfer complete isr */
    INTR_UEI = HPM_BITSMASK(1, 1), /* transfer error isr */
    INTR_PCI = HPM_BITSMASK(1, 2), /* port change isr */
    INTR_URI = HPM_BITSMASK(1, 6), /* reset isr */
    INTR_SRI = HPM_BITSMASK(1, 7), /* sof isr */
    INTR_SLI = HPM_BITSMASK(1, 8), /* suspend isr */
    INTR_NAKI = HPM_BITSMASK(1, 16) /* nak isr */
};

usb_device_handle_t *deviceHandle;

void hpm_usbd_send(usb_device_handle_t *handle, uint8_t endpointAddress, uint8_t *buffer, uint32_t length)
{
    usb_device_edpt_xfer(handle, (endpointAddress & 0x0F) | (0x80), buffer, length);
}

void hpm_usbd_recv(usb_device_handle_t *handle, uint8_t endpointAddress, uint8_t *buffer, uint32_t length)
{
    usb_device_edpt_xfer(handle, endpointAddress & 0x0F, buffer, length);
}

void usb_device_setup(void)
{
    _hpm_usbd_initialize(0, &deviceHandle);

    intc_m_enable_irq(IRQn_USB0);
}

static void USBD_IRQHandler(usb_device_handle_t *handle)
{
    uint32_t transfer_len;
    uint32_t int_status;
    bool ep_cb_req;

    if (handle == NULL) {
        return;
    }

    /* Acknowledge handled interrupt */
    int_status = usb_device_status_flags(handle);
    int_status &= usb_device_interrupts(handle);
    usb_device_clear_status_flags(handle, int_status);

    /* disabled interrupt sources */
    if (int_status == 0) {
        return;
    }

    /* Reset event!!!! */
    if (int_status & INTR_URI) {
        _hpm_usbd_callback(handle, USBD_EVENT_BUS_RESET, NULL);
        usb_device_bus_reset(handle, 64);
    }

    /* Suspend event!!!! */
    if (int_status & INTR_SLI) {
        if (usb_device_get_suspend_status(handle)) {
            /* Note: Host may delay more than 3 ms before and/or after bus reset
             * before doing enumeration. */
            if (usb_device_get_address(handle)) {
            }
        }
    }

    /* Port change event!!!! */
    if (int_status & INTR_PCI) {
        if (!usb_device_get_port_ccs(handle)) {
        } else {
            /* if it is not port reset */
            if (usb_device_get_port_reset_status(handle) == 0) {
            }
        }
    }

    /* Transfer complete event!!!! */
    if (int_status & INTR_UI) {
        usb_message_t msg = {0};
        uint32_t const edpt_complete = usb_device_get_edpt_complete_status(handle);
        uint32_t const edpt_setup_status = usb_device_get_setup_status(handle);

        if (edpt_complete) {
            usb_device_clear_edpt_complete_status(handle, edpt_complete);
            for (uint8_t ep_idx = 0; ep_idx < USB_SOS_DCD_MAX_QHD_COUNT; ep_idx++) {
                if (edpt_complete & (1 << ep_idx2bit(ep_idx))) {
                    transfer_len = 0;
                    ep_cb_req = true;

                    /* Failed QTD also get ENDPTCOMPLETE set */
                    dcd_qhd_t *p_qhd = usb_device_qhd_get(handle, ep_idx);
                    dcd_qtd_t *p_qtd = p_qhd->attached_qtd;
                    while (1) {
                        if (p_qtd->halted || p_qtd->xact_err || p_qtd->buffer_err) {
                            USB_LOG_ERR("usbd transfer error!\r\n");
                            ep_cb_req = false;
                            break;
                        } else if (p_qtd->active) {
                            ep_cb_req = false;
                            break;
                        } else {
                            transfer_len += p_qtd->expected_bytes - p_qtd->total_bytes;
                            p_qtd->in_use = false;
                        }

                        if (p_qtd->next == USB_SOC_DCD_QTD_NEXT_INVALID) {
                            break;
                        } else {
                            p_qtd = (dcd_qtd_t *)p_qtd->next;
                        }
                    }

                    if (ep_cb_req) {
                        uint8_t const ep_addr = (ep_idx / 2) | ((ep_idx & 0x01) ? 0x80 : 0);
                        dcd_qhd_t *qhd0 = usb_device_qhd_get(handle, 0);
                        if ((ep_addr & 0x0F) == 0) {
                            msg.is_setup_packet = 0;
                            msg.buffer = (uint8_t *)p_qhd->attached_buffer;
                            msg.length = transfer_len;
                            _hpm_usbd_ctl_control_callback(&msg, ep_addr);
                        } else {
                            msg.is_setup_packet = 0;
                            msg.buffer = (uint8_t *)p_qhd->attached_buffer;
                            msg.length = transfer_len;
                            _hpm_usbd_transfer_complete_callback(&msg, ep_addr);
                        }
                    }
                }
            }
        }

        if (edpt_setup_status) {
            /*------------- Set up Received -------------*/
            usb_device_clear_setup_status(handle, edpt_setup_status);
            msg.is_setup_packet = 1;
            msg.buffer = (uint8_t *)&usb_device_qhd_get(handle, 0)->setup_request;
            _hpm_usbd_ctl_control_callback(&msg, 0); /* When setup is set, ep_addr is not used */
        }
    }
}

SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usb0)
void isr_usb0(void)
{
    USBD_IRQHandler(deviceHandle);
}
