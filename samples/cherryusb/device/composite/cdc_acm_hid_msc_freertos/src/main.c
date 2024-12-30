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

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "usb_config.h"

#define LED_FLASH_PERIOD_IN_MS 300
#define USB_BUS_ID 0

extern void cdc_acm_hid_msc_descriptor_init(uint8_t busid, uint32_t reg_base);
extern void hid_mouse_test(uint8_t busid);
extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);

#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task2_PRIORITY    (configMAX_PRIORITIES - 4U)

static void task1(void *pvParameters)
{
    (void)pvParameters;
    printf("[cherryusb hid mouse with freertos sample]: task started.\n");

    while (1) {
        hid_mouse_test(USB_BUS_ID);
    }
}

static void task2(void *pvParameters)
{
    (void)pvParameters;
    printf("[cherryusb cdc acm with freertos sample]: task started.\n");

    cdc_acm_data_send_with_dtr_test(USB_BUS_ID);

    usb_osal_thread_delete(NULL);
}

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 2);
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb composite cdc_acm_hid_msc freertos sample.\n");

    cdc_acm_hid_msc_descriptor_init(USB_BUS_ID, CONFIG_HPM_USBD_BASE);
    if (usb_osal_thread_create("task1", 8192U, task1_PRIORITY, task1, NULL) == NULL) {
        printf("Task1 creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (usb_osal_thread_create("task2", 8192U, task2_PRIORITY, task2, NULL) == NULL) {
        printf("Task2 creation failed!.\n");
        for (;;) {
            ;
        }
    }
    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
