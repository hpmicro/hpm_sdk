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
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "tusb_option.h"

#if TUSB_OPT_HOST_ENABLED && CFG_TUSB_MCU == OPT_MCU_HPM

#include "board.h"
#include "common/tusb_common.h"
#include "hpm_usb_host.h"
#include "host/usbh_hcd.h"
#include "host/hcd.h"
#include "ff.h"
#include "hpm_interrupt.h"
/*---------------------------------------------------------------------*
 * Enum Declaration
 *---------------------------------------------------------------------*/
typedef enum {
  hcd_int_mask_usb                   = HPM_BITSMASK(1, 0),
  hcd_int_mask_error                 = HPM_BITSMASK(1, 1),
  hcd_int_mask_port_change           = HPM_BITSMASK(1, 2),

  hcd_int_mask_framelist_rollover    = HPM_BITSMASK(1, 3),
  hcd_int_mask_pci_host_system_error = HPM_BITSMASK(1, 4),
  hcd_int_mask_async_advance         = HPM_BITSMASK(1, 5),
  hcd_int_mask_sof                   = HPM_BITSMASK(1, 7),

  hcd_int_mask_async                 = HPM_BITSMASK(1, 18),
  hcd_int_mask_periodic              = HPM_BITSMASK(1, 19),

  hcd_int_mask_all                   = hcd_int_mask_usb | hcd_int_mask_error | hcd_int_mask_port_change |
                                       hcd_int_mask_framelist_rollover | hcd_int_mask_pci_host_system_error |
                                       hcd_int_mask_async_advance | hcd_int_mask_sof |
                                       hcd_int_mask_async | hcd_int_mask_periodic
} usb_interrupt_mask_t;
typedef struct
{
    USB_Type *regs;            /* register base */
    const uint32_t irqnum;     /* IRQ number */
}hcd_controller_t;

static const hcd_controller_t _hcd_controller[] =
{
    { .regs = (USB_Type *) HPM_USB0_BASE, .irqnum = IRQn_USB0 },
    #ifdef HPM_USB1_BASE
    { .regs = (USB_Type *) HPM_USB1_BASE, .irqnum = IRQn_USB1 }
    #endif
};

/*---------------------------------------------------------------------*
 * Variable Definitions
 *---------------------------------------------------------------------*/
ATTR_PLACE_AT_NONCACHEABLE static usb_host_handle_t usb_host_handle;
ATTR_PLACE_AT_NONCACHEABLE static bool hcd_int_sta;
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT) static hcd_data_t _hcd_data;

bool hcd_init(uint8_t rhport)
{
    uint32_t int_mask;

    if (rhport > USB_SOC_MAX_COUNT) {
        return false;
    }

    usb_host_handle.rhport   = rhport;
    usb_host_handle.regs     = _hcd_controller[rhport].regs;
    usb_host_handle.hcd_data = &_hcd_data;
    usb_host_handle.hcd_vbus_ctrl_cb = board_usb_vbus_ctrl;

    int_mask = hcd_int_mask_error | hcd_int_mask_port_change | hcd_int_mask_async_advance |
               hcd_int_mask_periodic | hcd_int_mask_async | hcd_int_mask_framelist_rollover;

    usb_host_init(&usb_host_handle, int_mask, USB_HOST_FRAMELIST_SIZE);

    return true;
}

void hcd_int_enable(uint8_t rhport)
{
    intc_m_enable_irq(_hcd_controller[rhport].irqnum);
    hcd_int_sta  = true;
}

void hcd_int_disable(uint8_t rhport)
{
    intc_m_disable_irq(_hcd_controller[rhport].irqnum);
    hcd_int_sta = false;
}

bool hcd_int_status(void)
{
    return hcd_int_sta;
}

uint32_t hcd_uframe_number(uint8_t rhport)
{
    return usb_host_uframe_number(&usb_host_handle);
}

void hcd_port_reset(uint8_t hostid)
{
    usb_host_port_reset(&usb_host_handle);
}

bool hcd_port_connect_status(uint8_t hostid)
{
    return usb_host_get_port_ccs(&usb_host_handle);
}

tusb_speed_t hcd_port_speed_get(uint8_t hostid)
{
    return usb_host_get_port_speed(&usb_host_handle);
}

void hcd_device_close(uint8_t rhport, uint8_t dev_addr)
{
    usb_host_device_close(&usb_host_handle, dev_addr);
}

bool hcd_edpt_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr, uint8_t * buffer, uint16_t buflen)
{
    return usb_host_edpt_xfer(&usb_host_handle, dev_addr, ep_addr, buffer, buflen);
}

bool hcd_setup_send(uint8_t rhport, uint8_t dev_addr, uint8_t const setup_packet[8])
{
    return usb_host_setup_send(&usb_host_handle, dev_addr, setup_packet);
}

bool hcd_edpt_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_endpoint_t const * ep_desc)
{
    usb_host_handle.ep_speed = _usbh_devices[dev_addr].speed;
    usb_host_handle.hub_addr = _usbh_devices[dev_addr].hub_addr;
    usb_host_handle.hub_port = _usbh_devices[dev_addr].hub_port;

    return usb_host_edpt_open(&usb_host_handle, dev_addr, (usb_desc_endpoint_t const *)ep_desc);
}

bool hcd_pipe_queue_xfer(uint8_t dev_addr, uint8_t ep_addr, uint8_t buffer[], uint16_t total_bytes)
{
    return usb_host_pipe_queue_xfer(&usb_host_handle, dev_addr, ep_addr, buffer, total_bytes);
}

bool hcd_edpt_busy(uint8_t dev_addr, uint8_t ep_addr)
{
    return usb_host_edpt_busy(&usb_host_handle, dev_addr, ep_addr);
}

bool hcd_edpt_stalled(uint8_t dev_addr, uint8_t ep_addr)
{
    return usb_host_edpt_stalled(&usb_host_handle, dev_addr, ep_addr);
}

bool hcd_edpt_clear_stall(uint8_t dev_addr, uint8_t ep_addr)
{
    return usb_host_edpt_clear_stall(&usb_host_handle, dev_addr, ep_addr);
}

/*---------------------------------------------------------------------*
 * HCD Interrupt Handler
 *---------------------------------------------------------------------*/
/* async_advance is handshake between usb stack & ehci controller.
 * This isr mean it is safe to modify previously removed queue head from async list.
 * In tinyusb, queue head is only removed when device is unplugged.
 */
static void async_advance_isr(usb_host_handle_t *handle)
{
    hcd_qhd_t* qhd_pool = handle->hcd_data->qhd_pool;

    for(uint32_t i = 0; i < HCD_MAX_ENDPOINT; i++) {
        if (qhd_pool[i].removing) {
            qhd_pool[i].removing = 0;
            qhd_pool[i].used     = 0;
        }
    }
}

static void port_connect_status_change_isr(usb_host_handle_t *handle)
{
    /* NOTE There is an sequence plug->unplug->…..-> plug if device is powering with pre-plugged device */
    if (usb_host_get_port_ccs(handle)) {
        usb_host_port_reset(handle);
        hcd_event_device_attach(handle->rhport, true);
    } else { /* device unplugged */
        hcd_event_device_remove(handle->rhport, true);
    }
}

static void qhd_xfer_complete_isr(hcd_qhd_t * p_qhd)
{
    bool is_ioc;

    while(p_qhd->p_qtd_list_head != NULL && !p_qhd->p_qtd_list_head->active) {
        /* TD need to be freed and removed from qhd, before invoking callback */
        is_ioc = (p_qhd->p_qtd_list_head->int_on_complete != 0);
        p_qhd->total_xferred_bytes += p_qhd->p_qtd_list_head->expected_bytes - p_qhd->p_qtd_list_head->total_bytes;

        p_qhd->p_qtd_list_head->used = 0; /* free QTD */
        usb_host_qtd_remove_1st_from_qhd(p_qhd);

        if (is_ioc) {
            hcd_event_xfer_complete(p_qhd->dev_addr, tu_edpt_addr(p_qhd->ep_number, p_qhd->qtd_overlay.pid == usb_pid_in ? 1 : 0), p_qhd->total_xferred_bytes, XFER_RESULT_SUCCESS, true);
            p_qhd->total_xferred_bytes = 0;
        }
    }
}

static void async_list_xfer_complete_isr(hcd_qhd_t * const async_head)
{
    hcd_qhd_t *p_qhd = async_head;

    do {
        if ( !p_qhd->qtd_overlay.halted ) { /* halted or error is processed in error isr */
            qhd_xfer_complete_isr(p_qhd);
        }

        p_qhd = usb_host_qhd_next(p_qhd);
        p_qhd = (hcd_qhd_t *)sys_address_to_core_local_mem(USB_HOST_MCU_CORE, (uint32_t)p_qhd);

    } while(p_qhd != async_head); /* async list traversal, stop if loop around */
}

static void period_list_xfer_complete_isr(usb_host_handle_t *handle, uint8_t interval_ms)
{
    uint16_t max_loop = 0;
    uint32_t const period_1ms_addr = (uint32_t) usb_host_get_period_head(handle, 1);
    hcd_link_t next_item = *usb_host_get_period_head(handle, interval_ms);
    hcd_qhd_t *p_qhd_int;

    /* TODO abstract max loop guard for period */
    while(!next_item.terminate &&
          !(interval_ms > 1 && period_1ms_addr == tu_align32(next_item.address)) &&
          max_loop < (HCD_MAX_ENDPOINT + usb_max_itd + usb_max_sitd) * CFG_TUSB_HOST_DEVICE_MAX) {

        switch ( next_item.type ) {
            case usb_qtype_qhd:
                p_qhd_int = (hcd_qhd_t *)tu_align32(next_item.address);
                if (!p_qhd_int->qtd_overlay.halted) {
                    qhd_xfer_complete_isr(p_qhd_int);
                }

                break;

            case usb_qtype_itd:
            case usb_qtype_sitd:
            case usb_qtype_fstn:

            default: break;
        }

        next_item = *usb_host_list_next(&next_item);
        max_loop++;
    }
}

static void qhd_xfer_error_isr(usb_host_handle_t *handle, hcd_qhd_t * p_qhd)
{
    xfer_result_t error_event;
    hcd_qtd_t *p_setup;

    if ( (p_qhd->dev_addr != 0 && p_qhd->qtd_overlay.halted) || /* addr0 cannot be protocol STALL */
          usb_host_qhd_has_xact_error(p_qhd)) {
        /* no error bits are set, endpoint is halted due to STALL */
        error_event = usb_host_qhd_has_xact_error(p_qhd) ? XFER_RESULT_FAILED : XFER_RESULT_STALLED;

        p_qhd->total_xferred_bytes += p_qhd->p_qtd_list_head->expected_bytes - p_qhd->p_qtd_list_head->total_bytes;

        /* TODO skip unplugged device */

        p_qhd->p_qtd_list_head->used = 0; /* free QTD */
        usb_host_qtd_remove_1st_from_qhd(p_qhd);

        if (0 == p_qhd->ep_number) {
            /* control cannot be halted --> clear all qtd list */
            p_qhd->p_qtd_list_head = NULL;
            p_qhd->p_qtd_list_tail = NULL;

            p_qhd->qtd_overlay.next.terminate      = 1;
            p_qhd->qtd_overlay.alternate.terminate = 1;
            p_qhd->qtd_overlay.halted              = 0;

            p_setup = usb_host_qtd_control(handle, p_qhd->dev_addr);
            p_setup->used = 0;
        }

        /* invoke USBH callback */
        hcd_event_xfer_complete(p_qhd->dev_addr, tu_edpt_addr(p_qhd->ep_number, p_qhd->qtd_overlay.pid == usb_pid_in ? 1 : 0), p_qhd->total_xferred_bytes, error_event, true);

        p_qhd->total_xferred_bytes = 0;
    }
}

static void xfer_error_isr(usb_host_handle_t *handle)
{
    hcd_qhd_t * const async_head = usb_host_qhd_async_head(handle);
    hcd_qhd_t *p_qhd = async_head;
    hcd_qhd_t *p_qhd_int;
    hcd_link_t next_item, *p;

    /*------------- async list -------------*/
    do {
        qhd_xfer_error_isr(handle, p_qhd);
        p_qhd = usb_host_qhd_next(p_qhd);
        p_qhd = (hcd_qhd_t *)sys_address_to_core_local_mem(USB_HOST_MCU_CORE, (uint32_t)p_qhd);
    } while(p_qhd != async_head); /* async list traversal, stop if loop around */

    /*------------- TODO refractor period list -------------*/
    uint32_t const period_1ms_addr = (uint32_t)usb_host_get_period_head(handle, 1);
    for (uint8_t interval_ms = 1; interval_ms <= USB_HOST_FRAMELIST_SIZE; interval_ms *= 2) {
        next_item = *usb_host_get_period_head(handle, interval_ms);

        /* TODO abstract max loop guard for period */
        while(!next_item.terminate &&
              !(interval_ms > 1 && period_1ms_addr == tu_align32(next_item.address)) ) {
            switch (next_item.type) {
                case usb_qtype_qhd:
                    p_qhd_int = (hcd_qhd_t *)tu_align32(next_item.address);
                    qhd_xfer_error_isr(handle, p_qhd_int);
                    break;

                case usb_qtype_itd:
                case usb_qtype_sitd:
                case usb_qtype_fstn:

                default:
                    break;
            }

            p = usb_host_list_next(&next_item);
            p = (hcd_link_t *)sys_address_to_core_local_mem(USB_HOST_MCU_CORE, (uint32_t)p);
            next_item = *p;
        }
    }
}

/*------------- Host Controller Driver's Interrupt Handler -------------*/
void hcd_int_handler(uint8_t rhport)
{
    uint32_t status;
    usb_host_handle_t *handle = &usb_host_handle;

    /* Acknowledge handled interrupt */
    status = usb_host_status_flags(handle);
    status &= usb_host_interrupts(handle);
    usb_host_clear_status_flags(handle, status);

    if (status == 0) {
        return;
    }

    if (status & hcd_int_mask_framelist_rollover) {
        handle->hcd_data->uframe_number += (USB_HOST_FRAMELIST_SIZE << 3);
    }

    if (status & hcd_int_mask_port_change) {
        if (usb_host_port_csc(handle)) {
            port_connect_status_change_isr(handle);
        }
    }

    if (status & hcd_int_mask_error) {
        xfer_error_isr(handle);
    }

    /*------------- some QTD/SITD/ITD with IOC set is completed -------------*/
    if (status & hcd_int_mask_async) {
        async_list_xfer_complete_isr(usb_host_qhd_async_head(handle));
    }

    if (status & hcd_int_mask_periodic) {
        for (uint8_t i = 1; i <= USB_HOST_FRAMELIST_SIZE; i *= 2) {
            period_list_xfer_complete_isr(handle, i);
        }
    }

    /*------------- There is some removed async previously -------------*/
    if (status & hcd_int_mask_async_advance) { /* need to place after EHCI_INT_MASK_ASYNC */
        async_advance_isr(handle);
    }
}

void isr_usb0(void)
{
    hcd_int_handler(0);
}
SDK_DECLARE_EXT_ISR_M(IRQn_USB0, isr_usb0)

#ifdef HPM_USB1_BASE
void isr_usb1(void)
{
    hcd_int_handler(1);
}
SDK_DECLARE_EXT_ISR_M(IRQn_USB1, isr_usb1)
#endif

#endif
