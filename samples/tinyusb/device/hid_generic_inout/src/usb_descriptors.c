/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tusb.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ((CFG_TUD_##itf) << (n))
#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                           _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

/*---------------------------------------------------------------------*
 * Device Descriptors
 *---------------------------------------------------------------------*/
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0xCafe,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

/* Invoked when received GET DEVICE DESCRIPTOR
 * Application return pointer to descriptor
 */
uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const *) &desc_device;
}

/*---------------------------------------------------------------------*
 * HID Report Descriptor
 *---------------------------------------------------------------------*/
uint8_t const desc_hid_report[] =
{
    #if   (CFG_TUSB_REPORT_ID_COUNT == 2)
        TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE - 1 , HID_REPORT_ID(REPORT_ID_IN) HID_REPORT_ID(REPORT_ID_OUT))
    #elif (CFG_TUSB_REPORT_ID_COUNT == 1)
        TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE - 1, HID_REPORT_ID(REPORT_ID))
    #elif (CFG_TUSB_REPORT_ID_COUNT == 0)
        TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE)
    #endif
};

/* Invoked when received GET HID REPORT DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
    (void) itf;
    return desc_hid_report;
}

/*---------------------------------------------------------------------*
 * Configuration Descriptor
 *---------------------------------------------------------------------*/
enum {
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

uint8_t desc_fs_configuration[] =
{
    /* Config number, interface count, string index, total length, attribute, power in mA */
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    /* Interface number, string index, protocol, report descriptor len, EP In & Out address, size & polling interval */
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), CFG_TUD_EPNUM_HID, 0x80 | CFG_TUD_EPNUM_HID, 64, 1)
};

#if TUD_OPT_HIGH_SPEED
uint8_t desc_hs_configuration[] =
{
    /* Config number, interface count, string index, total length, attribute, power in mA */
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    /* Interface number, string index, protocol, report descriptor len, EP In & Out address, size & polling interval */
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), CFG_TUD_EPNUM_HID, 0x80 | CFG_TUD_EPNUM_HID, 1024, 4)
};
#endif

/* Invoked when received GET CONFIGURATION DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void) index; /* for multiple configurations */

#if TUD_OPT_HIGH_SPEED
    // Although we are highspeed, host may be fullspeed.
    return (tud_speed_get() == TUSB_SPEED_HIGH) ?  desc_hs_configuration : desc_fs_configuration;
#else
    return desc_fs_configuration;
#endif
}

/*---------------------------------------------------------------------*
 * String Descriptors
 *---------------------------------------------------------------------*/

/* array of pointer to string descriptors */
char const* string_desc_arr [] =
{
    (const char[]){0x09, 0x04}, /* 0: is supported language is English (0x0409) */
    "TinyUSB",                     /* 1: Manufacturer */
    "TinyUSB Device",              /* 2: Product */
    "123456",                      /* 3: Serials, should use chip ID */
};

static uint16_t _desc_str[32];

/* Invoked when received GET STRING DESCRIPTOR request
 * Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
 */
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) {
            return NULL;
        }

        const char* str = string_desc_arr[index];

        /* Cap at max char */
        chr_count = strlen(str);
        if (chr_count > 31) {
            chr_count = 31;
        }

        /* Convert ASCII string into UTF-16 */
        for(uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    /* first byte is length (including header), second byte is string type */
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2 * chr_count + 2);

    return _desc_str;
}
