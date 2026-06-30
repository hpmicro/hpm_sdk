/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "usbd_core.h"
#include "usbd_dfu.h"
#include "dfu.h"
#include "board.h"

#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + 9 + 9 + 9)

#ifndef DFU_XPI_FLASH_ERASE_SIZE
#define DFU_XPI_FLASH_ERASE_SIZE (16 * 1024)
#endif

#ifndef DFU_FLASH_SIZE
#define DFU_FLASH_SIZE BOARD_FLASH_SIZE
#endif

static char flash_internal_desc_str[128];
static char option_bytes_desc_str[64];

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x00, USB_DEVICE_CLASS_APP_SPECIFIC, 0x01, 0x02, 0x04),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x01, 0x00, USB_DEVICE_CLASS_APP_SPECIFIC, 0x01, 0x02, 0x05),
    0x09,
    0x21,
    0x0B,
    WBVAL(0x00ff),
    WBVAL(USBD_DFU_XFER_SIZE),
    WBVAL(0x011a)
};

static const uint8_t device_quality_descriptor[] = {
    /* device qualifier descriptor */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB DFU DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
    flash_internal_desc_str,
    option_bytes_desc_str
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;
    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor dfu_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

struct usbd_interface intf0;

void dfu_init(uint8_t busid, uint32_t reg_base)
{
    uint32_t dfu_app_size = DFU_FLASH_SIZE - (USBD_DFU_APP_DEFAULT_ADD - BOARD_FLASH_BASE_ADDRESS);

    (void)snprintf(flash_internal_desc_str,
        sizeof(flash_internal_desc_str),
        "@Internal Flash /0x%08lX/%lu*%luKg",
        (unsigned long)(USBD_DFU_APP_DEFAULT_ADD),
        (unsigned long)dfu_app_size / DFU_XPI_FLASH_ERASE_SIZE,
        (unsigned long)DFU_XPI_FLASH_ERASE_SIZE / 1024);
    (void)snprintf(option_bytes_desc_str,
        sizeof(option_bytes_desc_str),
        "@Option bytes /0x00000000/01*512 g");
    printf("%s\n", flash_internal_desc_str);
    printf("%s\n", option_bytes_desc_str);
    usbd_desc_register(busid, &dfu_descriptor);
    usbd_add_interface(busid, usbd_dfu_init_intf(&intf0));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
