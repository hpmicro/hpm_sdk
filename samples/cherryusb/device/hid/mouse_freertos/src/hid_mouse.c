/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "usb_osal.h"
#include "usbd_core.h"
#include "usbd_hid.h"
#include "board.h"
#include "hpm_gpio_drv.h"

/*!< endpoint address */
#define HID_INT_EP          0x81
#define HID_INT_EP_SIZE     4
#define HID_INT_EP_INTERVAL 10

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ 34
/*!< report descriptor size */
#define HID_MOUSE_REPORT_DESC_SIZE 74

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                       /* bCountryCode: Hardware target country */
    0x01,                       /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                       /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL,          /* bInterval: Polling Interval */
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                       /* bCountryCode: Hardware target country */
    0x01,                       /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                       /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL,          /* bInterval: Polling Interval */
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro HID DEMO",           /* Product */
    "2024051705",                 /* Serial Number */
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

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return other_speed_config_descriptor;
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

/*!< hid mouse report descriptor */
static const uint8_t hid_mouse_report_desc[HID_MOUSE_REPORT_DESC_SIZE] = {
    0x05, 0x01, /*     USAGE_PAGE (Generic Desktop) */
    0x09, 0x02, /*     USAGE (Mouse) */
    0xA1, 0x01, /*     COLLECTION (Application) */
    0x09, 0x01, /*     USAGE (Pointer) */

    0xA1, 0x00, /*     COLLECTION (Physical) */
    0x05, 0x09, /*     USAGE_PAGE (Button) */
    0x19, 0x01, /*     USAGE_MINIMUM (Button 1) */
    0x29, 0x03, /*     USAGE_MAXIMUM (Button 3) */

    0x15, 0x00, /*     LOGICAL_MINIMUM (0) */
    0x25, 0x01, /*     LOGICAL_MAXIMUM (1) */
    0x95, 0x03, /*     REPORT_COUNT (3) */
    0x75, 0x01, /*     REPORT_SIZE (1) */

    0x81, 0x02, /*     INPUT (Data,Var,Abs) */
    0x95, 0x01, /*     REPORT_COUNT (1) */
    0x75, 0x05, /*     REPORT_SIZE (5) */
    0x81, 0x01, /*     INPUT (Cnst,Var,Abs) */

    0x05, 0x01, /*     USAGE_PAGE (Generic Desktop) */
    0x09, 0x30, /*     USAGE (X) */
    0x09, 0x31, /*     USAGE (Y) */
    0x09, 0x38,

    0x15, 0x81, /*     LOGICAL_MINIMUM (-127) */
    0x25, 0x7F, /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08, /*     REPORT_SIZE (8) */
    0x95, 0x03, /*     REPORT_COUNT (2) */

    0x81, 0x06, /*     INPUT (Data,Var,Rel) */
    0xC0, 0x09,
    0x3c, 0x05,
    0xff, 0x09,

    0x01, 0x15,
    0x00, 0x25,
    0x01, 0x75,
    0x01, 0x95,

    0x02, 0xb1,
    0x22, 0x75,
    0x06, 0x95,
    0x01, 0xb1,

    0x01, 0xc0 /*   END_COLLECTION */
};

/*!< mouse report struct */
struct hid_mouse {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel;
};

/*!< mouse report */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX struct hid_mouse mouse_cfg;
usb_osal_sem_t semaphore_tx_done;

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;

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
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

/* function ------------------------------------------------------------------*/
static void usbd_hid_int_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    if (semaphore_tx_done != NULL) {
        if (usb_osal_sem_give(semaphore_tx_done) != 0) {
            USB_LOG_ERR("%s usb_osal_sem_give error\n", __func__);
        }
    }
}

/*!< endpoint call back */
static struct usbd_endpoint hid_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = HID_INT_EP
};

/* function ------------------------------------------------------------------*/
/**
 * @brief            hid mouse init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
struct usbd_interface intf0;

void hid_mouse_init(uint8_t busid, uint32_t reg_base)
{
    semaphore_tx_done = usb_osal_sem_create(0);
    if (semaphore_tx_done == NULL) {
        USB_LOG_ERR("usb_osal_sem_create creation failed!.\n");
        for (;;) {
            usb_osal_msleep(10);
        }
    }
    usbd_desc_register(busid, &hid_descriptor);
    usbd_add_interface(busid, usbd_hid_init_intf(busid, &intf0, hid_mouse_report_desc, HID_MOUSE_REPORT_DESC_SIZE));
    usbd_add_endpoint(busid, &hid_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);

    /*!< init mouse report data */
    mouse_cfg.buttons = 0;
    mouse_cfg.wheel = 0;
    mouse_cfg.x = 0;
    mouse_cfg.y = 0;
}

/**
 * @brief            hid mouse test
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
void hid_mouse_test(uint8_t busid)
{
    if (usb_device_is_configured(busid) == false) {
        return;
    }

    if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == BOARD_BUTTON_PRESSED_VALUE) {
        /*!< move mouse pointer */
        mouse_cfg.x = 5;
        int ret = usbd_ep_start_write(busid, HID_INT_EP, (uint8_t *)&mouse_cfg, 4);
        if (ret < 0) {
            return;
        }
        if (usb_osal_sem_take(semaphore_tx_done, portMAX_DELAY) != 0) {
            USB_LOG_ERR("%s usb_osal_sem_take error\n", __func__);
        }
    }
    vTaskDelay(10);
}
