/*
 * Copyright (c) 2025-2026 HPMicro
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

#define USBD_WINUSB_DESC_SET_LEN (10 + USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_MULTI_DESCRIPTOR_LEN)

#define WEBUSB_URL_STRINGS 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 't', 'i', 'n', 'y', 'u', 's', 'b', '.', 'o', 'r', 'g', '/',\
                           'w', 'e', 'b', 'u', 's', 'b', '-', 's', 'e', 'r', 'i', 'a', 'l', '/', 'i', 'n', 'd', 'e', 'x', '.', 'h', 't', 'm', 'l'
#define URL_DESCRIPTOR_LENGTH  (3 + 44)
#define USBD_BOS_WTOTALLENGTH (5 + USB_BOS_CAP_PLATFORM_WEBUSB_DESCRIPTOR_LEN + USB_BOS_CAP_PLATFORM_WINUSB_DESCRIPTOR_LEN)
#define LANDING_PAGE          0x01

const uint8_t USBD_WinUSBDescriptorSetDescriptor[USBD_WINUSB_DESC_SET_LEN] = {
    USB_MSOSV2_COMP_ID_SET_HEADER_DESCRIPTOR_INIT(USBD_WINUSB_DESC_SET_LEN),
    USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_MULTI_DESCRIPTOR_INIT(ITF_NUM_VENDOR),
};

const uint8_t USBD_WebUSBURLDescriptor[URL_DESCRIPTOR_LENGTH] = {
    URL_DESCRIPTOR_LENGTH,
    WEBUSB_URL_TYPE,
    WEBUSB_URL_SCHEME_HTTPS,
    WEBUSB_URL_STRINGS
};

__ALIGN_BEGIN uint8_t USBD_BinaryObjectStoreDescriptor[USBD_BOS_WTOTALLENGTH] = {
    USB_BOS_HEADER_DESCRIPTOR_INIT(USBD_BOS_WTOTALLENGTH, 0x02),
    USB_BOS_CAP_PLATFORM_WEBUSB_DESCRIPTOR_INIT(USBD_WEBUSB_VENDOR_CODE, LANDING_PAGE),
    USB_BOS_CAP_PLATFORM_WINUSB_DESCRIPTOR_INIT(USBD_WINUSB_VENDOR_CODE, USBD_WINUSB_DESC_SET_LEN),
};

struct usb_webusb_descriptor webusb_url_desc = {
    .vendor_code = USBD_WEBUSB_VENDOR_CODE,
    .string = USBD_WebUSBURLDescriptor,
    .string_len = URL_DESCRIPTOR_LENGTH
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

