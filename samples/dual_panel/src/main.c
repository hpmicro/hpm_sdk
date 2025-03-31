/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "board.h"
#include <hpm_lcdc_drv.h>
#include "hpm_sysctl_drv.h"
#include "hpm_clock_drv.h"
#include <hpm_soc.h>
#include <hpm_panel.h>

typedef struct ibuf {
    uint32_t stride;
    uint32_t height;
    void *buf;
} ibuf_t;

static uint8_t __attribute__((section(".noncacheable"), aligned(64))) panel_buffer0[8 * 1024 * 1024];

static void image_set_color32(ibuf_t *ibuf, uint32_t color)
{
    uint32_t *buf = ibuf->buf;
    uint32_t width = ibuf->stride / 4;
    for (uint32_t y = 0; y < ibuf->height; y++)
        for (uint32_t x = 0; x < width; x++) {
            buf[y * width + x] = color;
        }
}

static void lvds_init_and_enable(ibuf_t *ibuf)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer_config = {0};
    int layer_index;
    display_pixel_format_t layer_format;

    hpm_panel_t *panel = hpm_panel_find_device(BOARD_MULTI_PANEL_LVDS_NAME);
    const hpm_panel_timing_t *timing = hpm_panel_get_timing(panel);

    ibuf->stride = timing->hactive * 4;
    ibuf->height = timing->vactive;
    ibuf->buf = hpm_panel_memory_alloc(HPM_PANEL_MEM_TYPE_NONCACHEABLE, ibuf->stride * ibuf->height);

    image_set_color32(ibuf, 0xffff0000);
    lcdc_get_default_config(BOARD_MULTI_PANEL_LVDS_LCDC_BASE, &config);
    board_panel_para_to_lcdc_by_name(BOARD_MULTI_PANEL_LVDS_NAME, &config);

    config.background.u = 0x00;
    lcdc_init(BOARD_MULTI_PANEL_LVDS_LCDC_BASE, &config);

    layer_index = 0;
    layer_format = display_pixel_format_argb8888;
    lcdc_get_default_layer_config(BOARD_MULTI_PANEL_LVDS_LCDC_BASE, &layer_config, layer_format, layer_index);
    layer_config.buffer = (uint32_t)ibuf->buf;
    layer_config.position_x = 0;
    layer_config.position_y = 0;
    layer_config.width = timing->hactive;
    layer_config.height = timing->vactive;
    layer_config.background.u = 0x00;

    /*
     * blender of layer0 isn't exist.
     * layer0 is dst of layer1. layer0 and layer1 are using blender of layer1.
     */
    layer_config.alphablend.mode = display_alphablend_mode_clear;
    layer_config.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.src_alpha = 0x00;
    layer_config.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.dst_alpha = 0x00;
    layer_config.byteorder = display_byteorder_a3a2a1a0;
    layer_config.max_bytes = lcdc_layer_max_bytes_256;
    layer_config.max_ot = 0;
    layer_config.csc_config.enable = false;
    layer_config.csc_config.ycbcr_mode = false;
    layer_config.yuv = display_yuv_mode_422_u1y1v1y2;
    lcdc_config_layer(BOARD_MULTI_PANEL_LVDS_LCDC_BASE, layer_index, &layer_config, true);

    lcdc_turn_on_display(BOARD_MULTI_PANEL_LVDS_LCDC_BASE);
}

static void mipi_init_and_enable(ibuf_t *ibuf)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer_config = {0};
    int layer_index;
    display_pixel_format_t layer_format;

    hpm_panel_t *panel = hpm_panel_find_device(BOARD_MULTI_PANEL_MIPI_NAME);
    const hpm_panel_timing_t *timing = hpm_panel_get_timing(panel);

    ibuf->stride = timing->hactive * 4;
    ibuf->height = timing->vactive;
    ibuf->buf = hpm_panel_memory_alloc(HPM_PANEL_MEM_TYPE_NONCACHEABLE, ibuf->stride * ibuf->height);

    image_set_color32(ibuf, 0xff00ff00);
    lcdc_get_default_config(BOARD_MULTI_PANEL_MIPI_LCDC_BASE, &config);
    board_panel_para_to_lcdc_by_name(BOARD_MULTI_PANEL_MIPI_NAME, &config);

    config.background.u = 0x00;
    lcdc_init(BOARD_MULTI_PANEL_MIPI_LCDC_BASE, &config);

    layer_index = 0;
    layer_format = display_pixel_format_argb8888;
    lcdc_get_default_layer_config(BOARD_MULTI_PANEL_MIPI_LCDC_BASE, &layer_config, layer_format, layer_index);
    layer_config.buffer = (uint32_t)ibuf->buf;
    layer_config.position_x = 0;
    layer_config.position_y = 0;
    layer_config.width = timing->hactive;
    layer_config.height = timing->vactive;
    layer_config.background.u = 0x00;

    /*
     * blender of layer0 isn't exist.
     * layer0 is dst of layer1. layer0 and layer1 are using blender of layer1.
     */
    layer_config.alphablend.mode = display_alphablend_mode_clear;
    layer_config.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.src_alpha = 0x00;
    layer_config.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.dst_alpha = 0x00;
    layer_config.byteorder = display_byteorder_a3a2a1a0;
    layer_config.max_bytes = lcdc_layer_max_bytes_256;
    layer_config.max_ot = 0;
    layer_config.csc_config.enable = false;
    layer_config.csc_config.ycbcr_mode = false;
    layer_config.yuv = display_yuv_mode_422_u1y1v1y2;
    lcdc_config_layer(BOARD_MULTI_PANEL_MIPI_LCDC_BASE, layer_index, &layer_config, true);

    lcdc_turn_on_display(BOARD_MULTI_PANEL_MIPI_LCDC_BASE);
}

int main(void)
{
    ibuf_t lvds;
    ibuf_t mipi;
    const uint32_t colors[] = {0xff00ff00, 0xffff0000, 0xff0000ff};
    int num = ARRAY_SIZE(colors);

    board_init();
    board_init_lcd();
    hpm_panel_memory_register(HPM_PANEL_MEM_TYPE_NONCACHEABLE, panel_buffer0, sizeof(panel_buffer0));
    lvds_init_and_enable(&lvds);
    mipi_init_and_enable(&mipi);

    while (1) {
        for (int i = 0; i < num; i++) {
            image_set_color32(&lvds, colors[i]);
            image_set_color32(&mipi, colors[(i + 1) % num]);
            board_delay_ms(1000);
        }
    }
    return 0;
}
