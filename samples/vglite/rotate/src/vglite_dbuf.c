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
#include "vglite_dbuf.h"
#include "vg_lite.h"
#include "tiger_paths.h"

struct lcdc_context lcdc_ctx;
static void layer_dma_done(void);

SDK_DECLARE_EXT_ISR_M(BOARD_LCD_IRQ, isr_lcd)
void isr_lcd(void)
{
    volatile uint32_t s = lcdc_get_dma_status(BOARD_LCD_BASE);
    lcdc_clear_dma_status(BOARD_LCD_BASE, s);
    lcdc_ctx.status = s;
    layer_dma_done();
}

void layer_vg_buffer_init(struct lcdc_layer_info *info)
{
    vg_lite_error_t error;

    info->busy_buffer = &info->vg_buffer[0];
    info->free_buffer = &info->vg_buffer[1];
    info->ready_buffer = NULL;

    memset(info->busy_buffer, 0x00, sizeof(vg_lite_buffer_t));
    info->busy_buffer->width  = BOARD_LCD_WIDTH;
    info->busy_buffer->height = BOARD_LCD_HEIGHT;
    info->busy_buffer->format = VG_LITE_BGRA8888;
    error = vg_lite_allocate(info->busy_buffer);
    if (error != VG_LITE_SUCCESS)
        printf("[%s]: busy_buffer allocate failed-%d\n", error);

    memset(info->free_buffer, 0x00, sizeof(vg_lite_buffer_t));
    info->free_buffer->width  = BOARD_LCD_WIDTH;
    info->free_buffer->height = BOARD_LCD_HEIGHT;
    info->free_buffer->format = VG_LITE_BGRA8888;
    error = vg_lite_allocate(info->free_buffer);
    if (error != VG_LITE_SUCCESS)
        printf("[%s]: free_buffer allocate failed-%d\n", error);

    vg_lite_clear(info->busy_buffer, NULL, 0x00FF0000);
    vg_lite_finish();
}

void layer_vg_buffer_deinit(struct lcdc_layer_info *info)
{
    vg_lite_free(&info->vg_buffer[0]);
    vg_lite_free(&info->vg_buffer[1]);
}

vg_lite_buffer_t *layer_get_free_vg_buffer(struct lcdc_layer_info *info)
{
    vg_lite_buffer_t *free_buffer = NULL;
    if (info->free_buffer) {
        free_buffer = info->free_buffer;
        info->free_buffer = NULL;
    }

    return free_buffer;
}

vg_lite_buffer_t *layer_wait_free_vg_buffer(struct lcdc_layer_info *info)
{
    vg_lite_buffer_t *free_buffer;
    while (!(free_buffer = layer_get_free_vg_buffer(info))) {
        xSemaphoreTake(info->buf_semaphore, portMAX_DELAY);
    }

    return free_buffer;
}

ATTR_RAMFUNC void layer_set_vg_buffer_ready(struct lcdc_layer_info *info, vg_lite_buffer_t *buf)
{
    info->ready_buffer = buf;
}

ATTR_RAMFUNC void layer_dma_done(void)
{
    for (int i = 0; i < 8; i++) {
        if ((lcdc_ctx.status & (1<<(i + LCDC_DMA_ST_DMA0_DONE_SHIFT))) &&
            lcdc_ctx.info[i].dma_done_cb)
            lcdc_ctx.info[i].dma_done_cb(&lcdc_ctx.info[i]);
    }
}

ATTR_RAMFUNC void layer0_dma_done(struct lcdc_layer_info *info)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    if (info->ready_buffer) {
        lcdc_layer_set_next_buffer(BOARD_LCD_BASE, 0, (uint32_t)info->ready_buffer->memory);
        info->free_buffer = info->busy_buffer;
        info->busy_buffer = info->ready_buffer;
        info->ready_buffer = NULL;
        xSemaphoreGiveFromISR(info->buf_semaphore, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken != pdFALSE)
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
        layer_vg_buffer_init(info);
        info->buf_semaphore = xSemaphoreCreateBinary();
        layer = &lcdc_ctx.info[info->index].layer;
        info->fb = info->busy_buffer->memory;
        info->dma_done_cb = layer0_dma_done;
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
    }

    lcdc_turn_on_display(BOARD_LCD_BASE);
    lcdc_enable_interrupt(BOARD_LCD_BASE, dma_done_irq);
    intc_m_enable_irq_with_priority(BOARD_LCD_IRQ, 4);
}

#define TIGER_CENTRE_X 20
#define TIGER_CENTRE_Y 25
#define TIGER_SIZE 160.0f

void double_buffer_display(void)
{
    struct lcdc_layer_info *layer_info = &lcdc_ctx.info[0];
    vg_lite_matrix_t matrix;
    vg_lite_float_t degrees = 0;
    float scale0;
    float scale1;
    float scale;
    vg_lite_buffer_t *draw_buf;

    scale0 = BOARD_LCD_WIDTH / TIGER_SIZE;
    scale1 = BOARD_LCD_HEIGHT / TIGER_SIZE;
    scale = scale0 > scale1 ? scale1 : scale0;

    while (1) {
        draw_buf = layer_wait_free_vg_buffer(layer_info);
        vg_lite_clear(draw_buf, NULL, 0x00FF0000);

        degrees += 3.3;
        if (degrees > 360)
            degrees = 0;

        vg_lite_identity(&matrix);
        /* Move tiger centre to screen centre */
        vg_lite_translate(BOARD_LCD_WIDTH / 2 - TIGER_CENTRE_X, BOARD_LCD_HEIGHT / 2 - TIGER_CENTRE_Y, &matrix);
        vg_lite_translate(TIGER_CENTRE_X, TIGER_CENTRE_Y, &matrix);
        vg_lite_rotate(degrees, &matrix);
        vg_lite_scale(scale, scale, &matrix);
        vg_lite_translate(-TIGER_CENTRE_X, -TIGER_CENTRE_Y, &matrix);

        /* Draw the path using the matrix. */
        for (int i = 0; i < pathCount; i++) {
            vg_lite_draw(draw_buf, &path[i], VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, color_data[i]);
        }
        vg_lite_finish();

        layer_set_vg_buffer_ready(layer_info, draw_buf);
    }
}
