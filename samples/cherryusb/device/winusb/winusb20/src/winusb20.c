/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"

#ifndef DOUBLE_WINUSB
#define DOUBLE_WINUSB 1
#endif

#define USBD_WINUSB_VENDOR_CODE 0x17

#define WINUSB_IN_EP  0x81
#define WINUSB_OUT_EP 0x01

#if DOUBLE_WINUSB == 0
    #define USBD_PID_WINUSB20 0x1236
    #define USB_CONFIG_SIZE (9 + 9 + 7 + 7)
    #define INTF_NUM 1
#else
    #define USBD_PID_WINUSB20 0x1237
    #define WINUSB_IN_EP2  0x82
    #define WINUSB_OUT_EP2 0x02
    #define USB_CONFIG_SIZE (9 + 9 + 7 + 7 + 9 + 7 + 7)
    #define INTF_NUM 2
#endif

static const uint8_t USBD_WinUSBDescriptorSetDescriptor[] = {
#if INTF_NUM == 1
    USB_MSOSV2_COMP_ID_SET_HEADER_DESCRIPTOR_INIT(WINUSB_DESCRIPTOR_SET_HEADER_SIZE + USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_SINGLE_DESCRIPTOR_LEN),
    USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_SINGLE_DESCRIPTOR_INIT(),
#else
    USB_MSOSV2_COMP_ID_SET_HEADER_DESCRIPTOR_INIT(WINUSB_DESCRIPTOR_SET_HEADER_SIZE + INTF_NUM * USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_MULTI_DESCRIPTOR_LEN),
    USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_MULTI_DESCRIPTOR_INIT(0x00),
    USB_MSOSV2_COMP_ID_FUNCTION_WINUSB_MULTI_DESCRIPTOR_INIT(0x01),
#endif
};

static const uint8_t USBD_BinaryObjectStoreDescriptor[] = {
    USB_BOS_HEADER_DESCRIPTOR_INIT(5 + USB_BOS_CAP_PLATFORM_WINUSB_DESCRIPTOR_LEN, 1),
    USB_BOS_CAP_PLATFORM_WINUSB_DESCRIPTOR_INIT(USBD_WINUSB_VENDOR_CODE, sizeof(USBD_WinUSBDescriptorSetDescriptor)),
};

const struct usb_msosv2_descriptor msosv2_desc = {
    .vendor_code = USBD_WINUSB_VENDOR_CODE,
    .compat_id = USBD_WinUSBDescriptorSetDescriptor,
    .compat_id_len = sizeof(USBD_WinUSBDescriptorSetDescriptor),
};

const struct usb_bos_descriptor bos_desc = {
    .string = USBD_BinaryObjectStoreDescriptor,
    .string_len = sizeof(USBD_BinaryObjectStoreDescriptor)
};

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_1, 0x00, 0x00, 0x00, USBD_VID, USBD_PID_WINUSB20, 0x0100, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0xff, 0x00, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
#if DOUBLE_WINUSB == 1
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0xff, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
#endif
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0xff, 0x00, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
#if DOUBLE_WINUSB == 1
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0xff, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
#endif
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0xff, 0x00, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
#if DOUBLE_WINUSB == 1
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0xff, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_FS, 0x00),
#endif
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xff, 0xff, 0x00, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
#if DOUBLE_WINUSB == 1
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0xff, 0xff, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_IN_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(WINUSB_OUT_EP2, USB_ENDPOINT_TYPE_BULK, USB_BULK_EP_MPS_HS, 0x00),
#endif
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro WinUSB Demo",        /* Product */
    "2024051702",                 /* Serial Number */
    "HPMicro WinUSB0",
    "HPMicro WinUSB1",
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

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

const struct usb_descriptor winusbv2_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
    .msosv2_descriptor = &msosv2_desc,
    .bos_descriptor = &bos_desc,
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];

volatile bool ep_tx_busy_flag;

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
        ep_tx_busy_flag = false;
        /* setup first out ep read transfer */
        usbd_ep_start_read(busid, WINUSB_OUT_EP, read_buffer, 2048);
#if DOUBLE_WINUSB == 1
        usbd_ep_start_read(busid, WINUSB_OUT_EP2, read_buffer, 2048);
#endif
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_winusb_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);

    usbd_ep_start_write(busid, WINUSB_IN_EP, read_buffer, nbytes);
    /* setup next out ep read transfer */
    usbd_ep_start_read(busid, ep, read_buffer, 2048);
}

void usbd_winusb_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

struct usbd_endpoint winusb_out_ep1 = {
    .ep_addr = WINUSB_OUT_EP,
    .ep_cb = usbd_winusb_out
};

struct usbd_endpoint winusb_in_ep1 = {
    .ep_addr = WINUSB_IN_EP,
    .ep_cb = usbd_winusb_in
};

struct usbd_interface winusb_intf1;

#if DOUBLE_WINUSB == 1

void usbd_winusb_out2(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);

    usbd_ep_start_write(busid, WINUSB_IN_EP2, read_buffer, nbytes);
    /* setup next out ep read transfer */
    usbd_ep_start_read(busid, ep, read_buffer, 2048);
}

void usbd_winusb_in2(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

struct usbd_endpoint winusb_out_ep2 = {
    .ep_addr = WINUSB_OUT_EP2,
    .ep_cb = usbd_winusb_out2
};

struct usbd_endpoint winusb_in_ep2 = {
    .ep_addr = WINUSB_IN_EP2,
    .ep_cb = usbd_winusb_in2
};

struct usbd_interface winusb_intf2;

#endif

void winusbv2_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &winusbv2_descriptor);

    usbd_add_interface(busid, &winusb_intf1);
    usbd_add_endpoint(busid, &winusb_out_ep1);
    usbd_add_endpoint(busid, &winusb_in_ep1);
#if DOUBLE_WINUSB == 1
    usbd_add_interface(busid, &winusb_intf2);
    usbd_add_endpoint(busid, &winusb_out_ep2);
    usbd_add_endpoint(busid, &winusb_in_ep2);
#endif
    usbd_initialize(busid, reg_base, usbd_event_handler);
}
