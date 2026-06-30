/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_hid.h"

/*!< hidraw in endpoint */
#define HIDRAW_IN_EP          0x81
#define HIDRAW_IN_EP_MPS_HS   1024
#define HIDRAW_IN_EP_MPS_FS   64
#define HIDRAW_IN_INTERVAL    10

/*!< hidraw out endpoint */
#define HIDRAW_OUT_EP          0x02
#define HIDRAW_OUT_EP_MPS_HS   1024
#define HIDRAW_OUT_EP_MPS_FS   64
#define HIDRAW_OUT_EP_INTERVAL 10

/*!< hid report counter */
#define HID_REPORT_CNT 64

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ (9 + 9 + 9 + 7 + 7)

/*!< custom hid report descriptor size */
#define HID_CUSTOM_REPORT_DESC_SIZE 38

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01)
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    HID_CUSTOM_INOUT_DESCRIPTOR_INIT(0x00, 0x01, HID_CUSTOM_REPORT_DESC_SIZE, HIDRAW_OUT_EP, HIDRAW_IN_EP, HIDRAW_IN_EP_MPS_HS, HIDRAW_IN_INTERVAL),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    HID_CUSTOM_INOUT_DESCRIPTOR_INIT(0x00, 0x01, HID_CUSTOM_REPORT_DESC_SIZE, HIDRAW_OUT_EP, HIDRAW_IN_EP, HIDRAW_IN_EP_MPS_FS, HIDRAW_IN_INTERVAL),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    HID_CUSTOM_INOUT_DESCRIPTOR_INIT(0x00, 0x01, HID_CUSTOM_REPORT_DESC_SIZE, HIDRAW_OUT_EP, HIDRAW_IN_EP, HIDRAW_IN_EP_MPS_FS, HIDRAW_IN_INTERVAL),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    HID_CUSTOM_INOUT_DESCRIPTOR_INIT(0x00, 0x01, HID_CUSTOM_REPORT_DESC_SIZE, HIDRAW_OUT_EP, HIDRAW_IN_EP, HIDRAW_IN_EP_MPS_HS, HIDRAW_IN_INTERVAL),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro HID DEMO",           /* Product */
    "2024051701",                 /* Serial Number */
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

const struct usb_descriptor hid_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

/*!< custom hid report descriptor */
static const uint8_t hid_custom_report_desc[HID_CUSTOM_REPORT_DESC_SIZE] = {
    /* USER CODE BEGIN 0 */
    0x06, 0x00, 0xff, /* USAGE_PAGE (Vendor Defined Page 1) */
    0x09, 0x01,       /* USAGE (Vendor Usage 1) */
    0xa1, 0x01,       /* COLLECTION (Application) */
    0x85, 0x02,       /*   REPORT ID (0x02) */
    0x09, 0x01,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x26, 0xff, 0x00, /*   LOGICAL_MAXIMUM (255) */
    0x95, HID_REPORT_CNT - 1,   /*   REPORT_COUNT (63) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x81, 0x02,       /*   INPUT (Data,Var,Abs) */
    /* <___________________________________________________> */
    0x85, 0x01,       /*   REPORT ID (0x01) */
    0x09, 0x01,       /*   USAGE (Vendor Usage 1) */
    0x15, 0x00,       /*   LOGICAL_MINIMUM (0) */
    0x26, 0xff, 0x00, /*   LOGICAL_MAXIMUM (255) */
    0x95, HID_REPORT_CNT - 1,   /*   REPORT_COUNT (63) */
    0x75, 0x08,       /*   REPORT_SIZE (8) */
    0x91, 0x02,       /*   OUTPUT (Data,Var,Abs) */
    /* USER CODE END 0 */
    0xC0 /*     END_COLLECTION	             */
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[HID_REPORT_CNT];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t send_buffer[HID_REPORT_CNT];

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

/*!< hid state ! Data can be sent only when state is idle  */
static volatile uint8_t custom_state;

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
        usbd_ep_start_read(busid, HIDRAW_OUT_EP, read_buffer, HID_REPORT_CNT);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

static void usbd_hid_custom_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;

    USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    custom_state = HID_STATE_IDLE;
}

static void usbd_hid_custom_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    usbd_ep_start_read(busid, ep, read_buffer, HID_REPORT_CNT);
    read_buffer[0] = 0x02; /* IN: report id */
    usbd_ep_start_write(busid, HIDRAW_IN_EP, read_buffer, nbytes);
}

static struct usbd_endpoint custom_in_ep = {
    .ep_cb = usbd_hid_custom_in_callback,
    .ep_addr = HIDRAW_IN_EP
};

static struct usbd_endpoint custom_out_ep = {
    .ep_cb = usbd_hid_custom_out_callback,
    .ep_addr = HIDRAW_OUT_EP
};

/* function ------------------------------------------------------------------*/
/**
 * @brief            hid custom init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
struct usbd_interface intf0;

void hid_custom_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &hid_descriptor);
    usbd_add_interface(busid, usbd_hid_init_intf(busid, &intf0, hid_custom_report_desc, HID_CUSTOM_REPORT_DESC_SIZE));
    usbd_add_endpoint(busid, &custom_in_ep);
    usbd_add_endpoint(busid, &custom_out_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
