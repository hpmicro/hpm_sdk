/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"

#define LED_FLASH_PERIOD_IN_MS 300

extern volatile uint8_t dtr_enable;
extern void cdc_acm_hid_msc_descriptor_init(void);
extern void hid_mouse_test(void);
extern void cdc_acm_data_send_with_dtr_test(void);

#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task2_PRIORITY    (configMAX_PRIORITIES - 4U)

static void task1(void *pvParameters)
{
    printf("[cherryusb hid mouse with freertos sample]: task started.\n");

    while (1) {
        hid_mouse_test();
    }
}

static void task2(void *pvParameters)
{
    printf("[cherryusb cdc acm with freertos sample]: task started.\n");

    cdc_acm_data_send_with_dtr_test();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    board_init_led_pins();
    board_init_usb_pins();
    board_init_gpio_pins();
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    printf("cherry usb composite cdc_acm_hid_msc freertos sample.\n");

    cdc_acm_hid_msc_descriptor_init();

    if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 256U, NULL, task1_PRIORITY, NULL) != pdPASS) {
        printf("Task1 creation failed!.\n");
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 256U, NULL, task2_PRIORITY, NULL) != pdPASS) {
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
