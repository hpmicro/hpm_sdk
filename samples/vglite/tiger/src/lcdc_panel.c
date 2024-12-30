/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_gpio_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_pdma_drv.h"
#include "hpm_l1c_drv.h"
#include "lcdc_panel.h"
#include "vg_lite.h"
#include "tiger_paths.h"

#define LAYER_NUM 8
#define LAYER_WIDTH_GAP 40
#define LAYER_HEIGHT_GAP 10
#define LCDC_FB_SIZE_ALIGNED HPM_L1C_CACHELINE_ALIGN_UP(BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT)
static uint32_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) fb_buffer[LAYER_NUM][LCDC_FB_SIZE_ALIGNED];
struct lcdc_context lcdc_ctx;

SDK_DECLARE_EXT_ISR_M(BOARD_LCD_IRQ, isr_lcd)
void isr_lcd(void)
{
    volatile uint32_t s = lcdc_get_dma_status(BOARD_LCD_BASE);
    lcdc_clear_dma_status(BOARD_LCD_BASE, s);
    lcdc_ctx.status = s;
}

static void fill_layer_argb888(void *fb, uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t stride, uint32_t color)
{
    volatile uint32_t *fb32 = (uint32_t *)fb;

    for (int y = y0; y < y0 + height; y++) {
        for (int x = x0; x < x0 + width; x++) {
            fb32[y * stride + x] = color;
        }
    }

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)fb);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)fb + LCDC_FB_SIZE_ALIGNED * sizeof(uint32_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

void lcdc_all_layer_config(void)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t *layer;
    struct lcdc_layer_info *info;
    uint16_t index = 0;
    uint32_t dma_done_irq = 0;

    lcdc_get_default_config(BOARD_LCD_BASE, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(BOARD_LCD_BASE, &config);

    /*
     * layer0
     */
    index = 0;
    info = &lcdc_ctx.info[index];
    info->index = index;
    info->enable = true;
    if (info->enable) {
        layer = &lcdc_ctx.info[info->index].layer;
        info->fb = &fb_buffer[info->index];
        info->irq_enable = true;
        info->format = display_pixel_format_argb8888;
        lcdc_get_default_layer_config(BOARD_LCD_BASE, layer, info->format, info->index);
        layer->buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)info->fb);
        layer->position_x = 0;
        layer->position_y = 0;
        layer->width = BOARD_LCD_WIDTH;
        layer->height = BOARD_LCD_HEIGHT;
        layer->alphablend.mode = display_alphablend_mode_clear;
        layer->background.u = 0;
        layer->max_bytes = lcdc_layer_max_bytes_1024;
        lcdc_config_layer(BOARD_LCD_BASE, info->index, layer, info->enable);
        if (info->irq_enable) {
            dma_done_irq |= LCDC_INT_EN_DMA_DONE_SET(1ul<<info->index);
        }
        fill_layer_argb888(info->fb, layer->position_x, layer->position_y, layer->width, layer->height, layer->width, 0xFFFF0000);
    }

    /*
     * layer1
     */
    index = 1;
    info = &lcdc_ctx.info[index];
    info->index = index;
    info->enable = false;

    if (info->enable) {
        layer = &lcdc_ctx.info[info->index].layer;
        info->fb = &fb_buffer[info->index];
        info->irq_enable = true;
        info->format = display_pixel_format_argb8888;
        lcdc_get_default_layer_config(BOARD_LCD_BASE, layer, info->format, info->index);
        layer->buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)info->fb);
        layer->position_x = 0;
        layer->position_y = 0;
        layer->width = BOARD_LCD_WIDTH - LAYER_WIDTH_GAP * info->index;
        layer->height = BOARD_LCD_HEIGHT - LAYER_HEIGHT_GAP * info->index;
        layer->alphablend.mode = display_alphablend_mode_src_over;
        layer->max_bytes = lcdc_layer_max_bytes_1024;
        lcdc_config_layer(BOARD_LCD_BASE, info->index, layer, info->enable);
        if (info->irq_enable) {
            dma_done_irq |= LCDC_INT_EN_DMA_DONE_SET(1ul<<info->index);
        }
        fill_layer_argb888(info->fb, layer->position_x, layer->position_y, layer->width, layer->height, layer->width, 0xFF00FF00);
    }

    lcdc_turn_on_display(BOARD_LCD_BASE);
    lcdc_enable_interrupt(BOARD_LCD_BASE, dma_done_irq);
    intc_m_enable_irq_with_priority(BOARD_LCD_IRQ, 4);
}

static void gpu_vg_lite_startup(void)
{
     clock_add_to_group(clock_gpu0, 0);
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

#define TIGER_CENTRE_X 20
#define TIGER_CENTRE_Y 25
#define TIGER_SIZE 160.0f

void tiger_path_display(void)
{
    vg_lite_buffer_t vg_buffer;
    vg_lite_matrix_t matrix;
    float scale0;
    float scale1;
    float scale;

    gpu_vg_lite_startup();

    /*vg_buffer init value must be zreo*/
    memset(&vg_buffer, 0x00, sizeof(vg_buffer));

    vg_buffer.width  = BOARD_LCD_WIDTH;
    vg_buffer.height = BOARD_LCD_HEIGHT;
    vg_buffer.format = VG_LITE_BGRA8888;
    vg_lite_allocate(&vg_buffer);
    vg_lite_clear(&vg_buffer, NULL, 0xFF00FF00);
    vg_lite_finish();

    scale0 = BOARD_LCD_WIDTH / TIGER_SIZE;
    scale1 = BOARD_LCD_HEIGHT / TIGER_SIZE;
    scale = scale0 > scale1 ? scale1 : scale0;

    vg_lite_identity(&matrix);
    /* Move tiger centre to screen centre */
    vg_lite_translate(BOARD_LCD_WIDTH / 2 - TIGER_CENTRE_X, BOARD_LCD_HEIGHT / 2 - TIGER_CENTRE_Y, &matrix);
    vg_lite_translate(TIGER_CENTRE_X, TIGER_CENTRE_Y, &matrix);
    vg_lite_scale(scale, scale, &matrix);
    vg_lite_translate(-TIGER_CENTRE_X, -TIGER_CENTRE_Y, &matrix);

    /* Draw the path using the matrix. */
    for (int i = 0; i < pathCount; i++) {
        vg_lite_draw(&vg_buffer, &path[i], VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, color_data[i]);
    }

    vg_lite_finish();

    uint8_t *src, *dst;
    dst = lcdc_ctx.info[0].fb;
    src = vg_buffer.memory;
    uint32_t dst_stride = BOARD_LCD_WIDTH * 4;
    uint32_t src_stride = vg_buffer.stride;

    for (int i = 0; i < BOARD_LCD_HEIGHT; i++) {
        memcpy(dst + i * dst_stride,  src + i * src_stride, dst_stride);
    }

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + LCDC_FB_SIZE_ALIGNED * sizeof(uint32_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }

    vg_lite_free(&vg_buffer);
}
