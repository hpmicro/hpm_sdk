
/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef VGLITE_DBUF_H
#define VGLITE_DBUF_H

#include <hpm_lcdc_drv.h>
#include "vg_lite.h"
#include "FreeRTOS.h"
#include "task.h"
#include <semphr.h>

struct lcdc_layer_info {
    bool enable;
    bool irq_enable;
    bool dma_done;
    uint32_t index;
    void *fb;
    void (*dma_done_cb)(struct lcdc_layer_info *info);
    display_pixel_format_t format;
    lcdc_layer_config_t layer;
    vg_lite_buffer_t vg_buffer[2];
    vg_lite_buffer_t *free_buffer;
    vg_lite_buffer_t *busy_buffer;
    vg_lite_buffer_t *ready_buffer;
    SemaphoreHandle_t buf_semaphore;
};

struct lcdc_context {
    struct lcdc_layer_info info[8];
    uint32_t status;
};

extern struct lcdc_context lcdc_ctx;

void lcdc_all_layer_config(void);
void double_buffer_display(void);
#endif
