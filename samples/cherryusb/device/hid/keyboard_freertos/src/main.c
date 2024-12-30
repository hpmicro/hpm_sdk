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

extern void hid_keyboard_init(uint8_t busid, uint32_t reg_base);
extern void hid_keyboard_test(uint8_t busid);
#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)

static void task1(void *pvParameters)
{
    (void)pvParameters;
    printf("[cherryusb hid keyboard with freertos sample]: task started.\n");

    while (1) {
        hid_keyboard_test(0);
    }
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

    printf("cherry usb hid_keyboard device sample.\n");

    hid_keyboard_init(0, CONFIG_HPM_USBD_BASE);
    if (usb_osal_thread_create("task1", 8192U, task1_PRIORITY, task1, NULL) == NULL) {
        perror("Task1 creation failed!.\n");
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
