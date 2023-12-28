
/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LCDC_PANEL_H
#define LCDC_PANEL_H

#include <hpm_lcdc_drv.h>


struct lcdc_layer_info {
    bool enable;
    bool irq_enable;
    bool dma_done;
    uint32_t index;
    void *fb;
    display_pixel_format_t format;
    lcdc_layer_config_t layer;
};

struct lcdc_context {
    struct lcdc_layer_info info[8];
    uint32_t status;
};

extern struct lcdc_context lcdc_ctx;

void lcdc_all_layer_config(void);
void tiger_path_display(void);
#endif
