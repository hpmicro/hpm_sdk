/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "usb_osal.h"

#include "usbh_core.h"
#include "usbh_serial.h"

#include "board.h"
#include "hpm_gpio_drv.h"

usb_osal_thread_t serial_tx_thread;
usb_osal_thread_t serial_rx_thread;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t serial_tx_buffer[512];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t serial_rx_buffer[512];

static struct usbh_serial *s_serial;

static void usbh_serial_tx_thread(void *argument)
{
    int ret;
    (void)argument;

    while (1) {
        if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == BOARD_BUTTON_PRESSED_VALUE) {
            serial_tx_buffer[0] = (uint8_t)'H';
            serial_tx_buffer[1] = (uint8_t)'P';
            serial_tx_buffer[2] = (uint8_t)'M';
            serial_tx_buffer[3] = (uint8_t)'i';
            serial_tx_buffer[4] = (uint8_t)'c';
            serial_tx_buffer[5] = (uint8_t)'r';
            serial_tx_buffer[6] = (uint8_t)'o';
            serial_tx_buffer[7] = (uint8_t)'\n';

            ret = usbh_serial_write(s_serial, serial_tx_buffer, 8);
            if (ret < 0) {
                printf("serial write error, ret:%d\n", ret);
                break;
            } else {
                printf("send length:%d\n", ret);
            }
        }

        usb_osal_msleep(200);
    }

    serial_tx_thread = NULL;
    usb_osal_thread_delete(NULL);
}

static void usbh_serial_rx_thread(void *argument)
{
    int ret;
    (void)argument;

    while (1) {
        ret = usbh_serial_read(s_serial, serial_rx_buffer, sizeof(serial_rx_buffer));
        if (ret < 0) {
            printf("serial read error, ret:%d\n", ret);
            break;
        } else {
            printf("receive length:%d\n", ret);
            printf("receive data: ");
            for (uint32_t i = 0; i < (uint32_t)ret; i++) {
                printf("%c", serial_rx_buffer[i]);
            }
            printf("\n");
        }
    }

    serial_rx_thread = NULL;
    usb_osal_thread_delete(NULL);
}

void usbh_serial_run(struct usbh_serial *serial)
{
    s_serial = usbh_serial_open(serial->hport->config.intf[serial->intf].devname, USBH_SERIAL_O_RDWR);
    if (s_serial == NULL) {
        printf("serial open failed!\n");
        return;
    }

    struct usbh_serial_termios termios;

    memset(&termios, 0, sizeof(termios));
    termios.baudrate = 115200;
    termios.stopbits = USBH_SERIAL_STOPBITS_1;
    termios.parity = USBH_SERIAL_PARITY_NONE;
    termios.databits = USBH_SERIAL_DATABITS_8;
    termios.rtscts = false;
    termios.rx_timeout = 0;
    usbh_serial_control(s_serial, USBH_SERIAL_CMD_SET_ATTR, &termios);

    if (serial_tx_thread == NULL) {
        serial_tx_thread = usb_osal_thread_create("usbh_serial_tx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_serial_tx_thread, NULL);
        if (serial_tx_thread == NULL) {
            printf("serial_tx_thread creation failed!\n");
            for (;;) {
                ;
            }
        }
    }

    if (serial_rx_thread == NULL) {
        serial_rx_thread = usb_osal_thread_create("usbh_serial_rx", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_serial_rx_thread, NULL);
        if (serial_rx_thread == NULL) {
            printf("serial_rx_thread creation failed!\n");
            for (;;) {
                ;
            }
        }
    }
}
