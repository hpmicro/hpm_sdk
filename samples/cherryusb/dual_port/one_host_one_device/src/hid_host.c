/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbh_core.h"
#include "usbh_hid.h"

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_keyboard_buffer[64];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_mouse_buffer[64];

void usbh_hid_keyboard_callback(void *arg, int nbytes)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;

    if (nbytes > 0) {
        for (int i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_keyboard_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", nbytes);
        usbh_submit_urb(&hid_class->intin_urb);
    }
}

void usbh_hid_mouse_callback(void *arg, int nbytes)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)arg;

    if (nbytes > 0) {
        for (int i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_mouse_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", nbytes);
        usbh_submit_urb(&hid_class->intin_urb);
    }
}

void usbh_hid_run(struct usbh_hid *hid_class)
{
    if (hid_class != NULL) {
        if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_KEYBOARD) {
            usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, hid_keyboard_buffer, hid_class->intin->wMaxPacketSize, 0, usbh_hid_keyboard_callback, hid_class);
            usbh_submit_urb(&hid_class->intin_urb);
            USB_LOG_RAW("mount a keyboard\r\n");
        } else if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_MOUSE) {
            usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, hid_mouse_buffer, hid_class->intin->wMaxPacketSize, 0, usbh_hid_mouse_callback, hid_class);
            usbh_submit_urb(&hid_class->intin_urb);
            USB_LOG_RAW("mount a mouse\r\n");
        } else {
            ;
        }
    }
}
