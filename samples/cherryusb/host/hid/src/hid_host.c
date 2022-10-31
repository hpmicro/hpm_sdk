/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbh_core.h"
#include "usbh_hid.h"

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_buffer[128];
static USB_NOCACHE_RAM_SECTION struct usbh_hid *hid_class;
static struct usbh_urb intin_urb;

void usbh_hid_callback(void *arg, int nbytes)
{
    if (nbytes > 0) {
        for (int i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_buffer[i]);
        }

        USB_LOG_RAW("nbytes:%d\r\n", nbytes);

        usbh_int_urb_fill(&intin_urb, hid_class->intin, hid_buffer, 8, 0, usbh_hid_callback, hid_class);
        usbh_submit_urb(&intin_urb);
    }
}

void usbh_device_mount_done_callback(struct usbh_hubport *hport)
{
    hid_class = (struct usbh_hid *)usbh_find_class_instance("/dev/input0");

    if (hid_class != NULL) {
        if ((hid_class->report_desc[0] == 0x05) && (hid_class->report_desc[1] == 0x01) && (hid_class->report_desc[2] == 0x09)) {
            if (hid_class->report_desc[3] == 0x06) {
                USB_LOG_RAW("mount a keyboard\r\n");
            } else if (hid_class->report_desc[3] == 0x02) {
                USB_LOG_RAW("mount a mouse\r\n");
            } else {
                ;
            }
        }
        usbh_int_urb_fill(&intin_urb, hid_class->intin, hid_buffer, 8, 0, usbh_hid_callback, NULL);
        usbh_submit_urb(&intin_urb);
    }
}

void usbh_device_unmount_done_callback(struct usbh_hubport *hport)
{
}

void usbh_class_test(void)
{
    printf("hid host test\r\n");
}
