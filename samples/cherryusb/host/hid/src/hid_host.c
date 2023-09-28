/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbh_core.h"
#include "usbh_hid.h"
#include "usb_ehci_priv.h"

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_keyboard_buffer[64];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_mouse_buffer[64];
static struct usbh_urb intin_urb[2];

void usbh_hid_keyboard_callback(void *arg, int nbytes)
{
    if (nbytes >= 0) {
        for (int i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_keyboard_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", nbytes);
    }
    usbh_submit_urb(&intin_urb[0]);
}

void usbh_hid_mouse_callback(void *arg, int nbytes)
{
    if (nbytes > 0) {
        for (int i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_mouse_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", nbytes);
    }
    usbh_submit_urb(&intin_urb[1]);
}

void usbh_hid_run(struct usbh_hid *hid_class)
{
    struct ehci_pipe *ppipe;

    if (hid_class != NULL) {
        ppipe = (struct ehci_pipe *)hid_class->intin;
        if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_KEYBOARD) {
            usbh_int_urb_fill(&intin_urb[0], hid_class->intin, hid_keyboard_buffer, ppipe->ep_mps, 0, usbh_hid_keyboard_callback, hid_class);
            usbh_submit_urb(&intin_urb[0]);
            USB_LOG_RAW("mount a keyboard\r\n");
        } else if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_MOUSE) {
            usbh_int_urb_fill(&intin_urb[1], hid_class->intin, hid_mouse_buffer, ppipe->ep_mps, 0, usbh_hid_mouse_callback, hid_class);
            usbh_submit_urb(&intin_urb[1]);
            USB_LOG_RAW("mount a mouse\r\n");
        } else {
            ;
        }
    }
}

void usbh_class_test(void)
{
    printf("hid host test\r\n");
}
