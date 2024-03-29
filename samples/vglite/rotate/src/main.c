/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "board.h"
#include "vg_lite.h"
#include "vglite_dbuf.h"
#include "hpm_lcdc_drv.h"
#include <semphr.h>

static void gpu_vg_lite_startup(void)
{
    clock_add_to_group(clock_axiv, 0);
    clock_add_to_group(clock_gpu0, 0);
    clock_add_to_group(clock_axig, 0);

    vg_lite_error_t error = vg_lite_init(512, 512);
    if (error != VG_LITE_SUCCESS) {
        printf("Err(%d): vg_lite_init\n", error);
    }

    char name[64] = "gpu";
    vg_lite_uint32_t chip_id;
    vg_lite_uint32_t chip_rev;
    vg_lite_uint32_t chip_cid;

    vg_lite_get_register(0x30, &chip_cid);
    vg_lite_get_product_info(name, &chip_id, &chip_rev);
    printf("gpu: name: %s, id: 0x%08X, rev: 0x%08X, cid: 0x%08X\n", name, chip_id, chip_rev, chip_cid);
}

static void startup_task(void *pvParameters)
{
    gpu_vg_lite_startup();
    board_init_lcd();
    lcdc_all_layer_config();
    double_buffer_display();

    while (1) {
        vTaskDelay(2000);
    }
}

int main(void)
{
    board_init();

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
