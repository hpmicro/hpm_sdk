/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "usb_osal.h"

#include "usbh_core.h"
#include "usbh_hid.h"

#define task_PRIORITY (configMAX_PRIORITIES - 5U)

usb_osal_thread_t hid_mouse_thread;
usb_osal_thread_t hid_keyboard_thread;
usb_osal_sem_t semaphore_hid_mouse_event;
usb_osal_sem_t semaphore_hid_keyboard_event;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_keyboard_buffer[64];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_mouse_buffer[64];
static volatile uint32_t keyboard_msg_bytes;
static volatile uint32_t mouse_msg_bytes;

void usbh_hid_keyboard_callback(void *arg, int nbytes)
{
    (void)arg;

    if (nbytes > 0) {
        keyboard_msg_bytes = nbytes;
        if (semaphore_hid_keyboard_event != NULL) {
            if (usb_osal_sem_give(semaphore_hid_keyboard_event) != 0) {
                printf("%s usb_osal_sem_give error\n", __func__);
            }
        }
    }
}

void usbh_hid_mouse_callback(void *arg, int nbytes)
{
    (void)arg;

    if (nbytes > 0) {
        mouse_msg_bytes = nbytes;
        if (semaphore_hid_mouse_event != NULL) {
            if (usb_osal_sem_give(semaphore_hid_mouse_event) != 0) {
                printf("%s usb_osal_sem_give error\n", __func__);
            }
        }
    }
}

static void task_keyboard(void *pvParameters)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)pvParameters;

    while (1) {
        if (usb_osal_sem_take(semaphore_hid_keyboard_event, portMAX_DELAY) != 0) {
            USB_LOG_ERR("%s usb_osal_sem_take error\r\n", __func__);
        }
        vTaskSuspendAll();
        USB_LOG_RAW("Keyboard Data:");
        for (uint32_t i = 0; i < keyboard_msg_bytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_keyboard_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", keyboard_msg_bytes);
        usbh_submit_urb(&hid_class->intin_urb);
        (void)xTaskResumeAll();
    }
}

static void task_mouse(void *pvParameters)
{
    struct usbh_hid *hid_class = (struct usbh_hid *)pvParameters;

    while (1) {
        if (usb_osal_sem_take(semaphore_hid_mouse_event, portMAX_DELAY) != 0) {
            USB_LOG_ERR("%s usb_osal_sem_take error\r\n", __func__);
        }
        vTaskSuspendAll();
        USB_LOG_RAW("Mouse Data:");
        for (uint32_t i = 0; i < mouse_msg_bytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_mouse_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", mouse_msg_bytes);
        usbh_submit_urb(&hid_class->intin_urb);
        (void)xTaskResumeAll();
    }
}

void usbh_hid_run(struct usbh_hid *hid_class)
{
    if (hid_class != NULL) {
        if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_KEYBOARD) {
            if (semaphore_hid_keyboard_event == NULL) {
                semaphore_hid_keyboard_event = usb_osal_sem_create(0);
            }
            if (hid_keyboard_thread == NULL) {
                hid_keyboard_thread = usb_osal_thread_create("task_keyboard", 2048U, task_PRIORITY, task_keyboard, hid_class);
            }
            if ((hid_keyboard_thread == NULL) || (semaphore_hid_keyboard_event == NULL)) {
                printf("Task_keyboard creation failed!.\n");
                for (;;) {
                    ;
                }
            }
            usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, \
                              hid_keyboard_buffer, hid_class->intin->wMaxPacketSize, 0, \
                              usbh_hid_keyboard_callback, hid_class);
            usbh_submit_urb(&hid_class->intin_urb);
            USB_LOG_RAW("mount a keyboard\r\n");
        } else if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_MOUSE) {
            if (semaphore_hid_mouse_event == NULL) {
                semaphore_hid_mouse_event = usb_osal_sem_create(0);
            }
            if (hid_mouse_thread == NULL) {
                hid_mouse_thread = usb_osal_thread_create("task_mouse", 2048U, task_PRIORITY, task_mouse, hid_class);
            }
            if ((hid_mouse_thread == NULL) || (semaphore_hid_mouse_event == NULL)) {
                printf("Task_mouse creation failed!.\n");
                for (;;) {
                    ;
                }
            }
            usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, \
                              hid_mouse_buffer, hid_class->intin->wMaxPacketSize, 0, \
                              usbh_hid_mouse_callback, hid_class);
            usbh_submit_urb(&hid_class->intin_urb);
            USB_LOG_RAW("mount a mouse\r\n");
        } else {
            ;
        }
    }
}

void usbh_hid_stop(struct usbh_hid *hid_class)
{
    if (hid_class != NULL) {
        if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_KEYBOARD) {
            if (hid_keyboard_thread != NULL) {
                usb_osal_thread_delete(hid_keyboard_thread);
                hid_keyboard_thread = NULL;
            }
            if (semaphore_hid_keyboard_event != NULL) {
                usb_osal_sem_delete(semaphore_hid_keyboard_event);
                semaphore_hid_keyboard_event = NULL;
            }
        } else if (hid_class->hport->config.intf[hid_class->intf].altsetting[0].intf_desc.bInterfaceProtocol == HID_PROTOCOL_MOUSE) {
            if (hid_mouse_thread != NULL) {
                usb_osal_thread_delete(hid_mouse_thread);
                hid_mouse_thread = NULL;
            }
            if (semaphore_hid_mouse_event != NULL) {
                usb_osal_sem_delete(semaphore_hid_mouse_event);
                semaphore_hid_mouse_event = NULL;
            }
        } else {
            ;
        }
    }
}

void usbh_class_test(void)
{
    printf("hid host test\r\n");
}
