/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"

#ifndef DOUBLE_WINUSB
#define DOUBLE_WINUSB 1
#endif

#define WCID_VENDOR_CODE 0x17

#define WINUSB_IN_EP  0x81
#define WINUSB_OUT_EP 0x01

#if DOUBLE_WINUSB == 0
#define USBD_PID_WINUSB10 0x1234
#define USB_CONFIG_SIZE (9 + 9 + 7 + 7)
#define INTF_NUM 1
#else
#define USBD_PID_WINUSB10 0x1235
#define WINUSB_IN_EP2  0x82
#define WINUSB_OUT_EP2 0x02

#define USB_CONFIG_SIZE (9 + 9 + 7 + 7 + 9 + 7 + 7)
#define INTF_NUM 2
#endif

static const uint8_t WCID_StringDescriptor_MSOS[18] = {
    /*
     * MS OS string descriptor
     */
    0x12,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    /* MSFT100 */
    'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00, /* wcChar_7 */
    '1', 0x00, '0', 0x00, '0', 0x00,            /* wcChar_7 */
    WCID_VENDOR_CODE,                           /* bVendorCode */
    0x00,                                       /* bReserved */
};

#if DOUBLE_WINUSB == 0
static const uint8_t WINUSB_WCIDDescriptor[40] = {
    /*
     * WCID descriptor
     */
    0x28, 0x00, 0x00, 0x00,                   /* dwLength */
    0x00, 0x01,                               /* bcdVersion */
    0x04, 0x00,                               /* wIndex */
    0x01,                                     /* bCount */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* bReserved_7 */

    /*
     * WCID function descriptor
     */
    0x00, /* bFirstInterfaceNumber */
    0x01, /* bReserved */
    /* WINUSB */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, /* cCID_8 */
    /*  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cSubCID_8 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             /* bReserved_6 */
};
#else
static const uint8_t WINUSB_WCIDDescriptor[64] = {
    /*
     * WCID descriptor
     */
    0x40, 0x00, 0x00, 0x00,                   /* dwLength */
    0x00, 0x01,                               /* bcdVersion */
    0x04, 0x00,                               /* wIndex */
    0x02,                                     /* bCount */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* bReserved_7 */

    /*
     * WCID function descriptor
     */
    0x00, /* bFirstInterfaceNumber */
    0x01, /* bReserved */
    /* WINUSB */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, /* cCID_8 */
    /*  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cSubCID_8 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             /* bReserved_6 */

    /*
     * WCID function descriptor
     */
    0x01, /* bFirstInterfaceNumber */
    0x01, /* bReserved */
    /* WINUSB */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, /* cCID_8 */
    /*  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* cSubCID_8 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             /* bReserved_6 */
};
#endif

static const uint8_t WINUSB_IF0_WCIDProperties[142] = {
    /*
     * WCID property descriptor
     */
    0x8e, 0x00, 0x00, 0x00,                           /* dwLength */
    0x00, 0x01,                                       /* bcdVersion */
    0x05, 0x00,                                       /* wIndex */
    0x01, 0x00,                                       /* wCount */

    /*
     * registry propter descriptor
     */
    0x84, 0x00, 0x00, 0x00,                           /* dwSize */
    0x01, 0x00, 0x00, 0x00,                           /* dwPropertyDataType */
    0x28, 0x00,                                       /* wPropertyNameLength */
    /* DeviceInterfaceGUID */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcName_20 */
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcName_20 */
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcName_20 */
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcName_20 */
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00,      /* wcName_20 */
    0x4e, 0x00, 0x00, 0x00,                           /* dwPropertyDataLength */
    /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
    '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcData_39 */
    'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcData_39 */
    '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcData_39 */
    '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcData_39 */
    '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcData_39 */
    'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcData_39 */
    '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcData_39 */
    'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcData_39 */
    'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcData_39 */
    '6', 0x00, '}', 0x00, 0x00, 0x00,                 /* wcData_39 */
};

#if DOUBLE_WINUSB == 1
static const uint8_t WINUSB_IF1_WCIDProperties[142] = {
    /*
     * WCID property descriptor
     */
    0x8e, 0x00, 0x00, 0x00,                           /* dwLength */
    0x00, 0x01,                                       /* bcdVersion */
    0x05, 0x00,                                       /* wIndex */
    0x01, 0x00,                                       /* wCount */

    /*
     * registry propter descriptor
     */
    0x84, 0x00, 0x00, 0x00,                           /* dwSize */
    0x01, 0x00, 0x00, 0x00,                           /* dwPropertyDataType */
    0x28, 0x00,                                       /* wPropertyNameLength */
    /* DeviceInterfaceGUID */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcName_20 */
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcName_20 */
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcName_20 */
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcName_20 */
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00,      /* wcName_20 */
    0x4e, 0x00, 0x00, 0x00,                           /* dwPropertyDataLength */
    /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
    '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcData_39 */
    'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcData_39 */
    '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcData_39 */
    '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcData_39 */
    '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcData_39 */
    'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcData_39 */
    '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcData_39 */
    'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcData_39 */
    'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcData_39 */
    '6', 0x00, '}', 0x00, 0x00, 0x00,                 /* wcData_39 */
};
#endif

const uint8_t *WINUSB_IFx_WCIDProperties[] = {
    WINUSB_IF0_WCIDProperties,
#if DOUBLE_WINUSB == 1
    WINUSB_IF1_WCIDProperties,
#endif
};

struct usb_msosv1_descriptor msosv1_desc = {
    .string = WCID_StringDescriptor_MSOS,
    .vendor_code = WCID_VENDOR_CODE,
    .compat_id = WINUSB_WCIDDescriptor,
    .comp_id_property = WINUSB_IFx_WCIDProperties,
};

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID_WINUSB10, 0x0001, 0x01),
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
    "2024051701",                 /* Serial Number */
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

const struct usb_descriptor winusb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
    .msosv1_descriptor = &msosv1_desc,
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

struct usbd_interface intf0;

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

struct usbd_interface intf1;

#endif

void winusb_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &winusb_descriptor);

    usbd_add_interface(busid, &intf0);
    usbd_add_endpoint(busid, &winusb_out_ep1);
    usbd_add_endpoint(busid, &winusb_in_ep1);
#if DOUBLE_WINUSB == 1
    usbd_add_interface(busid, &intf1);
    usbd_add_endpoint(busid, &winusb_out_ep2);
    usbd_add_endpoint(busid, &winusb_in_ep2);
#endif
    usbd_initialize(busid, reg_base, usbd_event_handler);
}
