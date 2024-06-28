/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pcfg_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_gpiom_drv.h"

#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task2_PRIORITY    (configMAX_PRIORITIES - 4U)

static void task1(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        printf("task1 is running.\n");

        vTaskDelay(1000);
    }
}

static void task2(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        printf("task2 is running.\n");

        vTaskDelay(433);
    }
}

int main(void)
{
    board_init();
    board_init_gpio_pins();
#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, BOARD_LED_ON_LEVEL);
#endif

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
