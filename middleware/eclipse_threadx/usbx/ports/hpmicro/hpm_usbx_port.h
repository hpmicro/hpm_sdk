/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _HPM_USBX_PORT_H_
#define _HPM_USBX_PORT_H_
#include <stdint.h>
#include "hpm_usb_device.h"

/*! @brief Available common EVENT types in device callback */
typedef enum _usb_device_event
{
    USBD_EVENT_BUS_RESET = 1U, /*!< USB bus reset signal detected */
    USBD_EVENT_SUSPEND,       /*!< USB bus suspend signal detected */
    USBD_EVENT_RESUME,    /*!< USB bus resume signal detected. The resume signal is driven by itself or a host */
    USBD_EVENT_SLEEP,

    USBD_EVENT_ERROR,     /*!< An error is happened in the bus. */
    USBD_EVENT_DETACH,    /*!< USB device is disconnected from a host. */
    USBD_EVENT_ATTACH,    /*!< USB device is connected to a host. */
    USBD_EVENT_SET_CONFIG, /*!< Set configuration. */
    USBD_EVENT_SET_INTERFACE,     /*!< Set interface. */

} usb_device_event_t;

void hpm_usbd_send(usb_device_handle_t *handle, uint8_t endpointAddress, uint8_t *buffer, uint32_t length);

void hpm_usbd_recv(usb_device_handle_t *handle, uint8_t endpointAddress, uint8_t *buffer, uint32_t length);

#endif