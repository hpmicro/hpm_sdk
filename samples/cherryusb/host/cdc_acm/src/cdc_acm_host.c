/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "usb_osal.h"

#include "usbh_core.h"
#include "usbh_cdc_acm.h"

#include "board.h"
#include "hpm_gpio_drv.h"

usb_osal_thread_t cdc_tx_thread;
usb_osal_thread_t cdc_rx_thread;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_tx_buffer[512];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_rx_buffer[512];

static void usbh_cdc_acm_tx_thread(void *argument)
{
    int ret;
    struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)argument;

    while (1) {
        if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == BOARD_BUTTON_PRESSED_VALUE) {
            cdc_tx_buffer[0] = (uint8_t)'H';
            cdc_tx_buffer[1] = (uint8_t)'P';
            cdc_tx_buffer[2] = (uint8_t)'M';
            cdc_tx_buffer[3] = (uint8_t)'i';
            cdc_tx_buffer[4] = (uint8_t)'c';
            cdc_tx_buffer[5] = (uint8_t)'r';
            cdc_tx_buffer[6] = (uint8_t)'o';
            cdc_tx_buffer[7] = (uint8_t)'\n';

            ret = usbh_cdc_acm_bulk_out_transfer(cdc_acm_class, cdc_tx_buffer, 8, USB_OSAL_WAITING_FOREVER);
            if (ret < 0) {
                printf("bulk out error,ret:%d\n", ret);
                break;
            } else {
                printf("send length:%d\n", ret);
            }
        }

        usb_osal_msleep(200);
    }

    cdc_tx_thread = NULL;
    usb_osal_thread_delete(NULL);
}

static void usbh_cdc_acm_rx_thread(void *argument)
{
    int ret;
    struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)argument;

    while (1) {
        ret = usbh_cdc_acm_bulk_in_transfer(cdc_acm_class, cdc_rx_buffer, 512, USB_OSAL_WAITING_FOREVER);
        if (ret < 0) {
            printf("bulk in error,ret:%d\n", ret);
            break;
        } else {
            printf("receive length:%d\n", ret);
            printf("receive data: ");
            for (uint32_t i = 0; i < (uint32_t)ret; i++) {
                printf("%c", cdc_rx_buffer[i]);
            }
            printf("\n");
        }
    }

    cdc_rx_thread = NULL;
    usb_osal_thread_delete(NULL);
}

void usbh_cdc_acm_run(struct usbh_cdc_acm *cdc_acm_class)
{
    if (cdc_tx_thread == NULL) {
        cdc_tx_thread = usb_osal_thread_create("usbh_cdc_tx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_cdc_acm_tx_thread, cdc_acm_class);
        if (cdc_tx_thread == NULL) {
            printf("cdc_tx_thread creation failed!.\n");
            for (;;) {
                ;
            }
        }
    }

    if (cdc_rx_thread == NULL) {
        cdc_rx_thread = usb_osal_thread_create("usbh_cdc_rx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_cdc_acm_rx_thread, cdc_acm_class);
        if (cdc_rx_thread == NULL) {
            printf("cdc_rx_thread creation failed!.\n");
            for (;;) {
                ;
            }
        }
    }
}
