/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <board.h>
#include <hpm_lvgl.h>

void hpm_lvgl_demos(void);

static void lvgl_task(void *pvParameters)
{
    (void) pvParameters;
    uint32_t delay;

    board_init_cap_touch();
    board_init_lcd();

    hpm_lvgl_init();
    hpm_lvgl_demos();

    while (1) {
        delay = lv_timer_handler();
        vTaskDelay(delay);
    }
}

int main(void)
{
    board_init();

    if (xTaskCreate(lvgl_task, "lvgl", 2048, NULL, 5, NULL) != pdPASS) {
        printf("Task creation failed!.\n");
        for (;;) {
        }
    }

    vTaskStartScheduler();
    for (;;) {

    }
    return 0;
}
