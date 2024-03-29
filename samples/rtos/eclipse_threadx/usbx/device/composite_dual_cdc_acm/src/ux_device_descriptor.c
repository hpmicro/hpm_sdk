/*
 * Copyright 2021 NXP
 *           2023 Hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ux_device_descriptor.h"

/* Vendor ID */
#define USB_DEVICE_VID (0x34B7U)
/* Product ID */
#define USB_DEVICE_PID (0xFFFFU)

/* High Speed */
#define USB_HS_BCD_VERSION (0x0200U)
#define USB_HS_CONTROL_MAX_PACKET_SIZE (64U)
#define USB_HS_CDC_ACM_BULK_PACKET_SIZE (512U)

/* Full Speed */
#define USB_FS_BCD_VERSION (0x0101U)
#define USB_FS_CONTROL_MAX_PACKET_SIZE (64U)
#define USB_FS_CDC_ACM_BULK_PACKET_SIZE (64U)

#define D3(d) ((unsigned char)((d) >> 24))
#define D2(d) ((unsigned char)((d) >> 16))
#define D1(d) ((unsigned char)((d) >> 8))
#define D0(d) ((unsigned char)((d) >> 0))

unsigned char device_framework_high_speed[] = {
    /* Device Descriptor */
    0x12,                           /* bLength */
    0x01,                           /* bDescriptorType */
    D0(USB_HS_BCD_VERSION),         /* bcdUSB lo */
    D1(USB_HS_BCD_VERSION),         /* bcdUSB hi */
    0xEF,                           /* DeviceClass */
    0x02,                           /* DeviceSubClass */
    0x01,                           /* DeviceProtocol */
    USB_HS_CONTROL_MAX_PACKET_SIZE, /* bMaxPacketSize */
    D0(USB_DEVICE_VID),             /* idVendor lo */
    D1(USB_DEVICE_VID),             /* idVendor hi */
    D0(USB_DEVICE_PID),             /* idProduct lo */
    D1(USB_DEVICE_PID),             /* idProduct hi */
    0x00,                           /* bcdDevice lo */
    0x01,                           /* bcdDevice hi */
    0x01,                           /* iManufacturer*/
    0x02,                           /* iProduct */
    0x03,                           /* iSerialNumber */
    0x01,                           /* bNumConfigurations */

    /* Device Qualifier Descriptor */
    0x0A,                   /* bLength */
    0x06,                   /* bDescriptorType */
    D0(USB_HS_BCD_VERSION), /* bcdUSB lo */
    D1(USB_HS_BCD_VERSION), /* bcdUSB hi */
    0x00,                   /* DeviceClass */
    0x00,                   /* DeviceSubClass */
    0x00,                   /* DeviceProtocol */
    0x40,                   /* bMaxPacketSize 8 */
    0x01,                   /* bNumConfigurations */
    0x00,                   /* bReserved */

    /* Configuration Descriptor */
    0x09, /* bLength */
    0x02, /* bDescriptorType */
    0x8D, /* wTotalLength lo */
    0x00, /* wTotalLength hi */
    0x04, /* bNumInterfaces */
    0x01, /* bConfigurationValue */
    0x00, /* iConfiguration */
    0x40, /* bmAttributes */
    0x32, /* bMaxPower 0x32 * 2ma */

    /***** ACM_1 descriptor *****/
    /* Interface Association Descriptor (IAD) */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass  */
    0x02, /* bFunctionSubClass */
    0x00, /* bFunctionProtocol */
    0x00, /* iFunction */

    /* Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptorType */
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x01, /* bNumEndpoints */
    0x02, /* bInterfaceClass - CDC */
    0x02, /* bInterfaceSubClass - Abstract Control Model */
    0x01, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* CDC Class-Specific descriptor */
    /* Header Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x00, /* bDescriptorSubtype */
    0x10, /* bcdCDC_lo */
    0x01, /* bcdCDC_hi - specification version 1.10 */

    /* ACM Functional Descriptor */
    0x04, /* bLength */
    0x24, /* bDescriptorType */
    0x02, /* bDescriptorSubtype */
    0x0f, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x06, /* bDescriptorSubtype */
    0x00, /* bMasterInterface */
    0x01, /* bSlaveInterface0 */

    /* Call Management Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x01, /* bDescriptorSubtype */
    0x00, /* bmCapabilities */
    0x01, /* bDataInterface */

    /* Notification Endpoint descriptor */
    0x07, /* bLength */
    0x05, /* bDescriptorType */
    0x81, /* bEndpointAddress */
    0x03, /* bmAttribute */
    0x08, /* wMAXPacketSize_lo */
    0x00, /* wMAXPacketSize_hi */
    0x08, /* bInterval */

    /* Data Class Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptor */
    0x01, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x02, /* bNumEndpoints */
    0x0A, /* bInterfaceClass */
    0x00, /* bInterfaceSubClass */
    0x00, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* Bulk IN Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x82,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /* Bulk OUT Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x02,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /***** ACM_2 descriptor *****/
    /* Interface Association Descriptor (IAD) */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x02, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass  */
    0x02, /* bFunctionSubClass */
    0x00, /* bFunctionProtocol */
    0x00, /* iFunction */

    /* Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptorType */
    0x02, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x01, /* bNumEndpoints */
    0x02, /* bInterfaceClass - CDC */
    0x02, /* bInterfaceSubClass - Abstract Control Model */
    0x01, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* CDC Class-Specific descriptor */
    /* Header Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x00, /* bDescriptorSubtype */
    0x10, /* bcdCDC_lo */
    0x01, /* bcdCDC_hi - specification version 1.10 */

    /* ACM Functional Descriptor */
    0x04, /* bLength */
    0x24, /* bDescriptorType */
    0x02, /* bDescriptorSubtype */
    0x0f, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x06, /* bDescriptorSubtype */
    0x02, /* bMasterInterface */
    0x03, /* bSlaveInterface0 */

    /* Call Management Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x01, /* bDescriptorSubtype */
    0x00, /* bmCapabilities */
    0x01, /* bDataInterface */

    /* Notification Endpoint descriptor */
    0x07, /* bLength */
    0x05, /* bDescriptorType */
    0x83, /* bEndpointAddress */
    0x03, /* bmAttribute */
    0x08, /* wMAXPacketSize_lo */
    0x00, /* wMAXPacketSize_hi */
    0x08, /* bInterval */

    /* Data Class Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptor */
    0x03, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x02, /* bNumEndpoints */
    0x0A, /* bInterfaceClass */
    0x00, /* bInterfaceSubClass */
    0x00, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* Bulk IN Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x84,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /* Bulk OUT Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x04,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_HS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */
};

/* FS Device Descriptor */
unsigned char device_framework_full_speed[] = {
    /* Device Descriptor */
    0x12,                           /* bLength */
    0x01,                           /* bDescriptorType */
    D0(USB_FS_BCD_VERSION),         /* bcdUSB lo */
    D1(USB_FS_BCD_VERSION),         /* bcdUSB hi */
    0xEF,                           /* DeviceClass */
    0x02,                           /* DeviceSubClass */
    0x01,                           /* DeviceProtocol */
    USB_FS_CONTROL_MAX_PACKET_SIZE, /* bMaxPacketSize */
    D0(USB_DEVICE_VID),             /* idVendor lo */
    D1(USB_DEVICE_VID),             /* idVendor hi */
    D0(USB_DEVICE_PID),             /* idProduct lo */
    D0(USB_DEVICE_PID),             /* idProduct hi */
    0x00,                           /* bcdDevice lo */
    0x01,                           /* bcdDevice hi */
    0x01,                           /* iManufacturer*/
    0x02,                           /* iProduct */
    0x03,                           /* iSerialNumber */
    0x01,                           /* bNumConfigurations */

    /* Configuration Descriptor */
    0x09, /* bLength */
    0x02, /* bDescriptorType */
    0x8D, /* wTotalLength lo */
    0x00, /* wTotalLength hi */
    0x04, /* bNumInterfaces */
    0x01, /* bConfigurationValue */
    0x00 /* iConfiguration */,
    0x40, /* bmAttributes */
    0x32, /* bMaxPower 0x32 * 2ma */

    /***** ACM_1 descriptor *****/
    /* Interface Association Descriptor (IAD) */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass  */
    0x02, /* bFunctionSubClass */
    0x00, /* bFunctionProtocol */
    0x00, /* iFunction */

    /* Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptorType */
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x01, /* bNumEndpoints */
    0x02, /* bInterfaceClass - CDC */
    0x02, /* bInterfaceSubClass - Abstract Control Model */
    0x01, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* CDC Class-Specific descriptor */
    /* Header Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x00, /* bDescriptorSubtype */
    0x10, /* bcdCDC_lo */
    0x01, /* bcdCDC_hi - specification version 1.10 */

    /* ACM Functional Descriptor */
    0x04, /* bLength */
    0x24, /* bDescriptorType */
    0x02, /* bDescriptorSubtype */
    0x0f, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x06, /* bDescriptorSubtype */
    0x00, /* bMasterInterface */
    0x01, /* bSlaveInterface0 */

    /* Call Management Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x01, /* bDescriptorSubtype */
    0x00, /* bmCapabilities */
    0x01, /* bDataInterface */

    /* Notification Endpoint descriptor */
    0x07, /* bLength */
    0x05, /* bDescriptorType */
    0x81, /* bEndpointAddress */
    0x03, /* bmAttribute */
    0x08, /* wMAXPacketSize_lo */
    0x00, /* wMAXPacketSize_hi */
    0x08, /* bInterval */

    /* Data Class Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptor */
    0x01, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x02, /* bNumEndpoints */
    0x0A, /* bInterfaceClass */
    0x00, /* bInterfaceSubClass */
    0x00, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* Bulk IN Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x82,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /* Bulk OUT Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x02,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /***** ACM_2 descriptor *****/
    /* Interface Association Descriptor (IAD) */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x02, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass  */
    0x02, /* bFunctionSubClass */
    0x00, /* bFunctionProtocol */
    0x00, /* iFunction */

    /* Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptorType */
    0x02, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x01, /* bNumEndpoints */
    0x02, /* bInterfaceClass - CDC */
    0x02, /* bInterfaceSubClass - Abstract Control Model */
    0x01, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* CDC Class-Specific descriptor */
    /* Header Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x00, /* bDescriptorSubtype */
    0x10, /* bcdCDC_lo */
    0x01, /* bcdCDC_hi - specification version 1.10 */

    /* ACM Functional Descriptor */
    0x04, /* bLength */
    0x24, /* bDescriptorType */
    0x02, /* bDescriptorSubtype */
    0x0f, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x06, /* bDescriptorSubtype */
    0x02, /* bMasterInterface */
    0x03, /* bSlaveInterface0 */

    /* Call Management Functional Descriptor */
    0x05, /* bLength */
    0x24, /* bDescriptorType */
    0x01, /* bDescriptorSubtype */
    0x00, /* bmCapabilities */
    0x01, /* bDataInterface */

    /* Notification Endpoint descriptor */
    0x07, /* bLength */
    0x05, /* bDescriptorType */
    0x83, /* bEndpointAddress */
    0x03, /* bmAttribute */
    0x08, /* wMAXPacketSize_lo */
    0x00, /* wMAXPacketSize_hi */
    0x08, /* bInterval */

    /* Data Class Interface Descriptor */
    0x09, /* bLength */
    0x04, /* bDescriptor */
    0x03, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    0x02, /* bNumEndpoints */
    0x0A, /* bInterfaceClass */
    0x00, /* bInterfaceSubClass */
    0x00, /* bInterfaceProtocol */
    0x00, /* iInterface */

    /* Bulk IN Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x84,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */

    /* Bulk OUT Endpoint descriptor */
    0x07,                                /* bLength */
    0x05,                                /* bDescriptorType */
    0x04,                                /* bEndpointAddress */
    0x02,                                /* bmAttribute */
    D0(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_lo */
    D1(USB_FS_CDC_ACM_BULK_PACKET_SIZE), /* wMAXPacketSize_hi */
    0x00,                                /* bInterval */
};

unsigned char string_framework[] = {
    /* Manufacturer string descriptor : Index 1 */
    0x09,
    0x04,
    0x01,
    21U,
    'H',
    'P',
    'M',
    'I',
    'C',
    'R',
    'O',
    ' ',
    'S',
    'E',
    'M',
    'I',
    'C',
    'O',
    'N',
    'D',
    'U',
    'C',
    'T',
    'O',
    'R',

    /* Product string descriptor : Index 2 */
    0x09,
    0x04,
    0x02,
    18U,
    'U',
    'S',
    'B',
    ' ',
    'C',
    'O',
    'M',
    'P',
    'O',
    'S',
    'I',
    'T',
    'E',
    ' ',
    'D',
    'E',
    'M',
    'O',

    /* Serial Number string descriptor : Index 3 */
    0x09,
    0x04,
    0x03,
    0x04,
    0x30,
    0x30,
    0x30,
    0x31
};

/* Multiple languages are supported on the device, to add
   a language besides English, the Unicode language code must
   be appended to the language_id_framework array and the length
   adjusted accordingly. */
unsigned char language_id_framework[] = {
    /* English. */
    0x09,
    0x04
};

unsigned char *ux_get_hs_framework(void)
{
    return device_framework_high_speed;
}

unsigned long ux_get_hs_framework_length(void)
{
    return sizeof(device_framework_high_speed);
}

unsigned char *ux_get_fs_framework(void)
{
    return device_framework_full_speed;
}

unsigned long ux_get_fs_framework_length(void)
{
    return sizeof(device_framework_full_speed);
}

unsigned char *ux_get_string_framework(void)
{
    return string_framework;
}

unsigned long ux_get_string_framework_length(void)
{
    return sizeof(string_framework);
}

unsigned char *ux_get_language_framework(void)
{
    return language_id_framework;
}

unsigned long ux_get_language_framework_length(void)
{
    return sizeof(language_id_framework);
}