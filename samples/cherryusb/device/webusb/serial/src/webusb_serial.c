/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

enum {
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  ITF_NUM_VENDOR,
  ITF_NUM_TOTAL
};

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + 9 + 7 + 7)

#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x82
#define WEBUSB_OUT_EP (0x03)
#define WEBUSB_IN_EP  (0x83)

#define USBD_WEBUSB_VENDOR_CODE  (0x01)
#define USBD_WINUSB_VENDOR_CODE  (0x02)

#define USBD_WINUSB_DESC_SET_LEN (0xB2)

#define WEBUSB_URL_STRINGS 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 't', 'i', 'n', 'y', 'u', 's', 'b', '.', 'o', 'r', 'g', '/',\
                           'w', 'e', 'b', 'u', 's', 'b', '-', 's', 'e', 'r', 'i', 'a', 'l', '/', 'i', 'n', 'd', 'e', 'x', '.', 'h', 't', 'm', 'l'
#define URL_DESCRIPTOR_LENGTH  (3 + 44)

const uint8_t USBD_WinUSBDescriptorSetDescriptor[USBD_WINUSB_DESC_SET_LEN] = {
    /*
     * WCID20 descriptor set descriptor
     */
    0x0A, 0x00,                     /* Descriptor size (10 bytes) */
    0x00, 0x00,                     /* MS OS 2.0 descriptor set header */
    0x00, 0x00, 0x03, 0x06,         /* Windows version (8.1) (0x06030000) */
    USBD_WINUSB_DESC_SET_LEN, 0x00, /* Size, MS OS 2.0 descriptor set */

    /*
     * WCID20 configuration subset descriptor
     */
    0x08, 0x00, /* wLength */
    0x01, 0x00, /* wDescriptorType */
    0x00,       /* configuration No.0 */
    0x00,       /* bReserved */
    0XA8, 0X00, /* Size, MS OS 2.0 configuration subset */

    /*
     * WCID20 function subset descriptor
     */
    0x08, 0x00,           /* Descriptor size (8 bytes) */
    0x02, 0x00,           /* MS OS 2.0 function subset header  */
    ITF_NUM_VENDOR,       /* bFirstInterface */
    0x00,                 /* bReserved */
    0xA0, 0x00,           /* wSubsetLength */

    /*
     * WCID20 compatible ID descriptor
     */
    0x14, 0x00, /* wLength  20 */
    0x03, 0x00, /* MS_OS_20_FEATURE_COMPATIBLE_ID */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,       /* cCID_8 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cSubCID_8 */

    /*
     * WCID20 registry property descriptor
     */
    0x84, 0x00, /* wLength: 132 */
    0x04, 0x00, /*  wDescriptorType: MS_OS_20_FEATURE_REG_PROPERTY: 0x04 (Table 9) */
    0x07, 0x00, /* wPropertyDataType: REG_MULTI_SZ (Table 15) */
    0x2a, 0x00, /* wPropertyNameLength: */
    /* bPropertyName: “DeviceInterfaceGUID”  */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
    'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,
    0x00, 0x00,
    0x50, 0x00, /* wPropertyDataLength  */
    /* bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}” */
    '{', 0x00, '9', 0x00, 'd', 0x00, '7', 0x00, 'd', 0x00, 'e', 0x00, 'b', 0x00, 'b', 0x00, 'c', 0x00, '-', 0x00,
    'c', 0x00, '8', 0x00, '5', 0x00, 'd', 0x00, '-', 0x00, '1', 0x00, '1', 0x00, 'd', 0x00, '1', 0x00, '-', 0x00,
    '9', 0x00, 'e', 0x00, 'b', 0x00, '4', 0x00, '-', 0x00, '0', 0x00, '0', 0x00, '6', 0x00, '0', 0x00, '0', 0x00,
    '8', 0x00, 'c', 0x00, '3', 0x00, 'a', 0x00, '1', 0x00, '9', 0x00, 'a', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t USBD_WebUSBURLDescriptor[URL_DESCRIPTOR_LENGTH] = {
    URL_DESCRIPTOR_LENGTH,
    WEBUSB_URL_TYPE,
    WEBUSB_URL_SCHEME_HTTPS,
    WEBUSB_URL_STRINGS
};

#define USBD_BOS_WTOTALLENGTH 0x39

#define LANDING_PAGE          0x01
uint8_t USBD_BinaryObjectStoreDescriptor[USBD_BOS_WTOTALLENGTH] = {
    /*
     * WCID20 BOS descriptor
     */
    0x05,                        /* bLength */
    0x0F,                        /* bDescriptorType */
    USBD_BOS_WTOTALLENGTH, 0x00, /* wTotalLength */
    0x02,                        /* bNumDeviceCaps */

    /*
     * WCID20 WebUSB Platform capability descriptor
     */
    0x18, /* Descriptor size (24 bytes) */
    0x10, /* Descriptor type (Device Capability) */
    0x05, /* Capability type (Platform) */
    0x00, /* Reserved */

    /* WebUSB Platform Capability ID (3408b638-09a9-47a0-8bfd-a0768815b665) */
    0x38, 0xB6, 0x08, 0x34,
    0xA9, 0x09,
    0xA0, 0x47,
    0x8B, 0xFD,
    0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65,

    0x00, 0x01,              /* WebUSB version 1.0 */
    USBD_WEBUSB_VENDOR_CODE, /* Vendor-assigned WebUSB request code */
    LANDING_PAGE,            /* Landing page */

    /*
     *WCID20 device capability descriptor
     */
    0x1C, /* Descriptor size (28 bytes) */
    0x10, /* Descriptor type (Device Capability) */
    0x05, /* Capability type (Platform) */
    0x00, /* Reserved */

    0xDF, 0x60, 0xDD, 0xD8, /* PlatformCapabilityUUID */
    0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D,
    0x9E, 0x64, 0x8A, 0x9F,

    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 * dwWindowsVersion */

    USBD_WINUSB_DESC_SET_LEN, 0X00, /* wDescriptorSetTotalLength */

    USBD_WINUSB_VENDOR_CODE, /* bVendorCode */
    0X00                     /* bAltEnumCode */
};

struct usb_webusb_descriptor webusb_url_desc = {
    .vendor_code = USBD_WEBUSB_VENDOR_CODE,
    .string = USBD_WebUSBURLDescriptor,
    .string_len = USBD_WINUSB_DESC_SET_LEN
};

struct usb_msosv2_descriptor msosv2_desc = {
    .vendor_code = USBD_WINUSB_VENDOR_CODE,
    .compat_id = USBD_WinUSBDescriptorSetDescriptor,
    .compat_id_len = USBD_WINUSB_DESC_SET_LEN,
};

struct usb_bos_descriptor bos_desc = {
    .string = USBD_BinaryObjectStoreDescriptor,
    .string_len = USBD_BOS_WTOTALLENGTH
};

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_1, 0xEF, 0x02, 0x01, 0xcafe, USBD_PID, 0x0002, 0x01)
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, ITF_NUM_TOTAL, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(ITF_NUM_CDC, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x04),
    USB_INTERFACE_DESCRIPTOR_INIT(ITF_NUM_VENDOR, 0x00, 0x02, USB_DEVICE_CLASS_VEND_SPECIFIC, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, ITF_NUM_TOTAL, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(ITF_NUM_CDC, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x04),
    USB_INTERFACE_DESCRIPTOR_INIT(ITF_NUM_VENDOR, 0x00, 0x02, USB_DEVICE_CLASS_VEND_SPECIFIC, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, ITF_NUM_TOTAL, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(ITF_NUM_CDC, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x04),
    USB_INTERFACE_DESCRIPTOR_INIT(ITF_NUM_VENDOR, 0x00, 0x02, USB_DEVICE_CLASS_VEND_SPECIFIC, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, ITF_NUM_TOTAL, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(ITF_NUM_CDC, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x04),
    USB_INTERFACE_DESCRIPTOR_INIT(ITF_NUM_VENDOR, 0x00, 0x02, USB_DEVICE_CLASS_VEND_SPECIFIC, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WEBUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
};

static const char * const string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro WEBUSB DEMO",        /* Product */
    "2025022609",                 /* Serial Number */
    "HPMicro CDC",
    "HPMicro WebUSB",
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void) speed;

    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor webusb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
    .msosv2_descriptor = &msosv2_desc,
    .webusb_url_descriptor = &webusb_url_desc,
    .bos_descriptor = &bos_desc
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer_1[512];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer_2[512];

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        /* setup first out ep read transfer */
        usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer_1, usbd_get_ep_mps(busid, CDC_OUT_EP));
        usbd_ep_start_read(busid, WEBUSB_OUT_EP, read_buffer_2, usbd_get_ep_mps(busid, WEBUSB_OUT_EP));
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    usbd_ep_start_write(busid, CDC_IN_EP, read_buffer_1, nbytes);  /* echo back */
    usbd_ep_start_read(busid, ep, read_buffer_1, usbd_get_ep_mps(busid, ep));
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

void usbd_webusb_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    usbd_ep_start_write(busid, WEBUSB_IN_EP, read_buffer_2, nbytes);  /* echo back */
    /* setup next out ep read transfer */
    usbd_ep_start_read(busid, ep, read_buffer_2, usbd_get_ep_mps(busid, ep));
}

void usbd_webusb_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    }
}

struct usbd_endpoint webusb_out_ep = {
    .ep_addr = WEBUSB_OUT_EP,
    .ep_cb = usbd_webusb_out
};

struct usbd_endpoint webusb_in_ep = {
    .ep_addr = WEBUSB_IN_EP,
    .ep_cb = usbd_webusb_in
};

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;

static int vendor_class_interface_request_handler(uint8_t busid, struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    (void) busid;
    (void) data;
    (void) len;

    USB_LOG_DBG("Vendor Class request: "
                "bRequest 0x%02x\r\n",
                setup->bRequest);

    switch (setup->bRequest) {
    case 0x22:
        if (setup->wValue != 0) {
            board_led_write(!board_get_led_gpio_off_level());
        } else {
            board_led_write(board_get_led_gpio_off_level());
        }
        break;

    default:
        USB_LOG_WRN("Unhandled Vendor Class bRequest 0x%02x\r\n", setup->bRequest);
        return -1;
    }

    return 0;
}

void webusb_init(uint8_t busid, uintptr_t reg_base)
{
    usbd_desc_register(busid, &webusb_descriptor);

    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_endpoint(busid, &cdc_out_ep);
    usbd_add_endpoint(busid, &cdc_in_ep);

    intf2.class_interface_handler = vendor_class_interface_request_handler;
    usbd_add_interface(busid, &intf2);
    usbd_add_endpoint(busid, &webusb_out_ep);
    usbd_add_endpoint(busid, &webusb_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

