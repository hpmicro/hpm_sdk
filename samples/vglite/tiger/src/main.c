/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "board.h"
#include "vg_lite.h"
#include "lcdc_panel.h"
#include "hpm_lcdc_drv.h"
#include <semphr.h>

static void startup_task(void *pvParameters)
{
    lcdc_all_layer_config();
    tiger_path_display();

    while (1) {
        vTaskDelay(2000);
    }
}

int main(void)
{
    board_init();
    board_init_lcd();

    if (xTaskCreate(startup_task, "startup", 1024, NULL, 5, NULL) != pdPASS) {
        printf("Task1 creation failed!.\n");
        for (;;) {
        }
    }

    vTaskStartScheduler();
    for (;;) {

    }
    return 0;
}
