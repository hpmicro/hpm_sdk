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
#include <vg_lite.h>

void hpm_lvgl_demos(void);

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

static void lvgl_task(void *pvParameters)
{
    (void) pvParameters;
    uint32_t delay;

    board_init_cap_touch();
    board_init_lcd();
    gpu_vg_lite_startup();

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
