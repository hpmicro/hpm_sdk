/*
 * Copyright (c) 2023-2025 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_lvgl.h"
#include <hpm_touch.h>
#include <hpm_panel.h>
#include <board.h>
#include <hpm_soc.h>
#include <hpm_csr_regs.h>
#include <hpm_interrupt.h>
#include <hpm_lcdc_drv.h>
#include <hpm_l1c_drv.h>
#include <hpm_pdma_drv.h>
#include <hpm_mchtmr_drv.h>

#if (LV_USE_OS != LV_OS_NONE)
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#endif

#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /*Buffer size for invalid areas*/
#endif

#define HPM_LVGL_RUNNING_CORE HPM_CORE0
#define HPM_LVGL_LCDC_BASE BOARD_LCD_BASE
#define HPM_LVGL_LCDC_LAYER_INDEX (0)
#define HPM_LVGL_LCDC_IRQ_NUM  BOARD_LCD_IRQ
#define HPM_LVGL_LCDC_IRQ_PRIORITY  7

#define HPM_LVGL_PIXEL_SIZE (LV_COLOR_DEPTH / 8)
#define HPM_LVGL_LCD_WIDTH (BOARD_LCD_WIDTH)
#define HPM_LVGL_LCD_HEIGHT (BOARD_LCD_HEIGHT)
#define HPM_LVGL_LCD_STRIDE ((HPM_LVGL_LCD_WIDTH * HPM_LVGL_PIXEL_SIZE + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1))
#define HPM_LVGL_FB_SIZE HPM_L1C_CACHELINE_ALIGN_UP(HPM_LVGL_LCD_STRIDE * HPM_LVGL_LCD_HEIGHT)

#define HPM_LVGL_MCHTMR HPM_MCHTMR
#define HPM_LVGL_MCHTMR_CLK clock_mchtmr0

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH

#define HPM_LVGL_PDMA_IRQ_NUM IRQn_PDMA_D0
#define HPM_LVGL_PDMA_IRQ_PRIORITY 6
#define HPM_LVGL_PDMA_BASE HPM_PDMA
#define LVGL_PDMA_CLOCK clock_pdma

typedef void (*hpm_lvgl_pdma_finish_cb_t)(void *cb_data);

typedef struct hpm_lvgl_pdma_flush_context {
    struct {
        uint32_t pixel_size;
        uint32_t stride;
        pdma_plane_config_t plane_src;
        pdma_output_config_t output;
    } cfg;
    void *pdma_base;
    uint32_t dst_addr;
    uint32_t src_addr;
    lv_area_t areas[LV_INV_BUF_SIZE];
    volatile uint16_t area_num;
    volatile uint16_t area_num_cnt;
    hpm_lvgl_pdma_finish_cb_t finish_cb;
    void *finish_cb_data;
    void *user_data;
} hpm_lvgl_pdma_flush_context_t;
#endif

typedef struct hpm_lvgl_context {
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    hpm_lvgl_pdma_flush_context_t pdma_ctx;
#endif
    lv_display_t *disp;
    lv_indev_t *indev;
    volatile int lcdc_vsync_flag;
    int lcdc_is_enable;
    int render_mode;
#if (LV_USE_OS != LV_OS_NONE)
    SemaphoreHandle_t sync;
#endif
} hpm_lvgl_context_t;

hpm_lvgl_context_t hpm_lvgl_context;

#define FB_SECTION ".framebuffer"

static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) hpm_lvgl_fb0[HPM_LVGL_FB_SIZE];
static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) hpm_lvgl_fb1[HPM_LVGL_FB_SIZE];
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) hpm_lcdc_fb[HPM_LVGL_FB_SIZE];
#endif

static inline uint32_t hpm_lvgl_irq_lock(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

static inline void hpm_lvgl_irq_unlock(uint32_t state)
{
    restore_global_irq(state);
}

#if (LV_USE_OS != LV_OS_NONE)
int hpm_lvgl_lcdc_vsync_init(hpm_lvgl_context_t *ctx)
{
    ctx->sync = xSemaphoreCreateBinary();
    return 0;
}

ATTR_RAMFUNC int hpm_lvgl_lcdc_vsync_wait(hpm_lvgl_context_t *ctx)
{
    xSemaphoreTake(ctx->sync, portMAX_DELAY);
    return 0;
}

ATTR_RAMFUNC int hpm_lvgl_lcdc_vsync_signal(hpm_lvgl_context_t *ctx)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(ctx->sync, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdFALSE )
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return 0;
}

ATTR_RAMFUNC uint32_t hpm_lvgl_tick_get_cb(void)
{
    return xTaskGetTickCount();
}

ATTR_RAMFUNC void hpm_lvgl_delay_cb(uint32_t ms)
{
    vTaskDelay(ms);
}

void hpm_lvgl_tick_init(void)
{
    lv_tick_set_cb(hpm_lvgl_tick_get_cb);
    lv_delay_set_cb(hpm_lvgl_delay_cb);
}
#else
int hpm_lvgl_lcdc_vsync_wait(hpm_lvgl_context_t *ctx)
{
    (void) ctx;
    return 0;
}

ATTR_RAMFUNC int hpm_lvgl_lcdc_vsync_signal(hpm_lvgl_context_t *ctx)
{
    (void) ctx;
    return 0;
}

ATTR_RAMFUNC int hpm_lvgl_lcdc_vsync_init(hpm_lvgl_context_t *ctx)
{
    (void) ctx;
    return 0;
}

ATTR_RAMFUNC uint32_t hpm_lvgl_tick_get_cb(void)
{
    static uint32_t mchtmr_freq_in_khz = 0;

    if (!mchtmr_freq_in_khz) {
        clock_add_to_group(HPM_LVGL_MCHTMR_CLK, 0);
        mchtmr_freq_in_khz = clock_get_frequency(HPM_LVGL_MCHTMR_CLK) / 1000;
    }

    return mchtmr_get_count(HPM_LVGL_MCHTMR) / mchtmr_freq_in_khz;
}

void hpm_lvgl_tick_init(void)
{
    lv_tick_set_cb(hpm_lvgl_tick_get_cb);
}
#endif /* LV_USE_OS != LV_OS_NONE */

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
static void hpm_lvgl_pdma_init(hpm_lvgl_pdma_flush_context_t *ctx, void *pdma_base, uint32_t pixel_size)
{
    pdma_config_t config;
    display_pixel_format_t pixel_format;
    pdma_plane_config_t *plane_src_cfg = &ctx->cfg.plane_src;
    pdma_output_config_t *output_cfg = &ctx->cfg.output;
    PDMA_Type *pdma_ptr = (PDMA_Type *)pdma_base;
    ctx->pdma_base = pdma_base;
    ctx->cfg.pixel_size = pixel_size;
    clock_add_to_group(LVGL_PDMA_CLOCK, HPM_LVGL_RUNNING_CORE);
    pixel_format = (pixel_size == 4) ? display_pixel_format_argb8888 : display_pixel_format_rgb565;
    pdma_get_default_config(pdma_ptr, &config, pixel_format);

    config.enable_plane = pdma_plane_src;
    config.block_size = pdma_blocksize_16x16;
    pdma_init(pdma_ptr, &config);

    pdma_get_default_plane_config(pdma_ptr, plane_src_cfg, pixel_format);
    pdma_get_default_output_config(pdma_ptr, output_cfg, pixel_format);
    intc_m_enable_irq_with_priority(HPM_LVGL_PDMA_IRQ_NUM, HPM_LVGL_PDMA_IRQ_PRIORITY);
}

ATTR_RAMFUNC static int hpm_lvgl_pdma_copy_start(hpm_lvgl_pdma_flush_context_t *ctx, uint32_t dst, uint32_t src)
{
    PDMA_Type *pdma_ptr;
    lv_area_t *area;
    pdma_plane_config_t *plane_src_cfg;
    pdma_output_config_t *output_cfg;
    hpm_lvgl_context_t *lvgl_ctx = ctx->user_data;

    if (ctx->area_num < 1)
        return -1;

    ctx->area_num_cnt = 0;
    ctx->dst_addr = dst;
    ctx->src_addr = src;
    pdma_ptr = (PDMA_Type *)ctx->pdma_base;
    area = &ctx->areas[ctx->area_num_cnt];
    plane_src_cfg = &ctx->cfg.plane_src;
    output_cfg = &ctx->cfg.output;
    pdma_stop(pdma_ptr);

    lv_display_t *disp = lvgl_ctx->disp;
    lv_display_rotation_t lvgl_rotation = lv_display_get_rotation(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t src_stride;
    uint32_t dst_stride;
    lv_area_t dst_area;

    if (lvgl_rotation == LV_DISPLAY_ROTATION_90) {
        plane_src_cfg->rotate = pdma_rotate_270_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_270) {
        plane_src_cfg->rotate = pdma_rotate_90_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_180) {
        plane_src_cfg->rotate = pdma_rotate_180_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    } else {
        plane_src_cfg->rotate = pdma_rotate_0_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    }

    lv_area_copy(&dst_area, area);
    lv_display_rotate_area(disp, &dst_area);

    plane_src_cfg->buffer = (uint32_t)src + (area->y1 * src_stride + area->x1 * ctx->cfg.pixel_size);
    plane_src_cfg->width = lv_area_get_width(area);
    plane_src_cfg->height = lv_area_get_height(area);
    plane_src_cfg->pitch = src_stride;
    plane_src_cfg->background = 0x0;
    pdma_config_planes(pdma_ptr, plane_src_cfg, NULL, NULL);

    output_cfg->plane[pdma_plane_src].x = 0;
    output_cfg->plane[pdma_plane_src].y = 0;
    output_cfg->plane[pdma_plane_src].width = lv_area_get_width(&dst_area);
    output_cfg->plane[pdma_plane_src].height = lv_area_get_height(&dst_area);

    output_cfg->alphablend.src_alpha = 0xFF;
    output_cfg->alphablend.src_alpha_op = display_alpha_op_override;
    output_cfg->alphablend.mode = display_alphablend_mode_clear;

    output_cfg->buffer = (uint32_t)dst + (dst_area.y1 * dst_stride + dst_area.x1 * ctx->cfg.pixel_size);
    output_cfg->width = lv_area_get_width(&dst_area);
    output_cfg->height = lv_area_get_height(&dst_area);
    output_cfg->pitch = dst_stride;
    pdma_config_output(pdma_ptr, output_cfg);

    pdma_start(pdma_ptr);
    pdma_enable_irq(pdma_ptr, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, true);

    return 0;
}

ATTR_RAMFUNC static void hpm_lvgl_pdma_copy_done(hpm_lvgl_pdma_flush_context_t *ctx)
{
    PDMA_Type *pdma_ptr;
    lv_area_t *area;;
    pdma_plane_config_t *plane_src_cfg;
    pdma_output_config_t *output_cfg;
    hpm_lvgl_context_t *lvgl_ctx = ctx->user_data;
    uint32_t dst, src;

    if (ctx->area_num < 1)
        return;

    ctx->area_num_cnt++;
    if (ctx->area_num == ctx->area_num_cnt) {
        ctx->area_num = 0;
        if (ctx->finish_cb)
            ctx->finish_cb(ctx->finish_cb_data);
        return;
    }

    dst = ctx->dst_addr;
    src = ctx->src_addr;
    pdma_ptr = (PDMA_Type *)ctx->pdma_base;
    area = &ctx->areas[ctx->area_num_cnt];
    plane_src_cfg = &ctx->cfg.plane_src;
    output_cfg = &ctx->cfg.output;

    lv_display_t *disp = lvgl_ctx->disp;
    lv_display_rotation_t lvgl_rotation = lv_display_get_rotation(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t src_stride;
    uint32_t dst_stride;
    lv_area_t dst_area;

    if (lvgl_rotation == LV_DISPLAY_ROTATION_90) {
        plane_src_cfg->rotate = pdma_rotate_270_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_270) {
        plane_src_cfg->rotate = pdma_rotate_90_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_180) {
        plane_src_cfg->rotate = pdma_rotate_180_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    } else {
        plane_src_cfg->rotate = pdma_rotate_0_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    }

    lv_area_copy(&dst_area, area);
    lv_display_rotate_area(disp, &dst_area);

    plane_src_cfg->buffer = (uint32_t)src + (area->y1 * src_stride + area->x1 * ctx->cfg.pixel_size);
    plane_src_cfg->width = lv_area_get_width(area);
    plane_src_cfg->height = lv_area_get_height(area);
    plane_src_cfg->pitch = src_stride;
    plane_src_cfg->background = 0x0;
    pdma_config_planes(pdma_ptr, plane_src_cfg, NULL, NULL);

    output_cfg->plane[pdma_plane_src].x = 0;
    output_cfg->plane[pdma_plane_src].y = 0;
    output_cfg->plane[pdma_plane_src].width = lv_area_get_width(&dst_area);
    output_cfg->plane[pdma_plane_src].height = lv_area_get_height(&dst_area);

    output_cfg->alphablend.src_alpha = 0xFF;
    output_cfg->alphablend.src_alpha_op = display_alpha_op_override;
    output_cfg->alphablend.mode = display_alphablend_mode_clear;

    output_cfg->buffer = (uint32_t)dst + (dst_area.y1 * dst_stride + dst_area.x1 * ctx->cfg.pixel_size);
    output_cfg->width = lv_area_get_width(&dst_area);
    output_cfg->height = lv_area_get_height(&dst_area);
    output_cfg->pitch = dst_stride;
    pdma_config_output(pdma_ptr, output_cfg);

    pdma_start(pdma_ptr);
    pdma_enable_irq(pdma_ptr, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, true);
}

ATTR_RAMFUNC static void hpm_lvgl_pdma_copy_register_finish_cb(hpm_lvgl_pdma_flush_context_t *ctx, hpm_lvgl_pdma_finish_cb_t cb, void *cb_data)
{
    ctx->finish_cb = cb;
    ctx->finish_cb_data = cb_data;
}

ATTR_RAMFUNC static int hpm_lvgl_pdma_add_area(hpm_lvgl_pdma_flush_context_t *ctx, const lv_area_t *area)
{
    lv_area_copy(&ctx->areas[ctx->area_num], area);
    ctx->area_num++;
    return 0;
}

SDK_DECLARE_EXT_ISR_M(HPM_LVGL_PDMA_IRQ_NUM, hpm_lvgl_pdma_isr)
void hpm_lvgl_pdma_isr(void)
{
    PDMA_Type *pdma_ptr;
    hpm_lvgl_pdma_flush_context_t *ctx;

    ctx = &hpm_lvgl_context.pdma_ctx;
    pdma_ptr = (PDMA_Type *)ctx->pdma_base;
    pdma_ptr->STAT = PDMA_STAT_PDMA_DONE_MASK;

    pdma_stop(pdma_ptr);
    pdma_enable_irq(pdma_ptr, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, false);
    hpm_lvgl_pdma_copy_done(ctx);
}

#endif /* defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH */

ATTR_RAMFUNC void hpm_lvgl_lcdc_vsync_flag_set(hpm_lvgl_context_t *ctx, int val)
{
    uint32_t state;

    state = hpm_lvgl_irq_lock();
    ctx->lcdc_vsync_flag = val;
    hpm_lvgl_irq_unlock(state);
}

ATTR_RAMFUNC int hpm_lvgl_lcdc_vsync_flag_get(hpm_lvgl_context_t *ctx)
{
    return ctx->lcdc_vsync_flag;
}

void hpm_lvgl_wait_lcdc_vsync(hpm_lvgl_context_t *ctx)
{
    hpm_lvgl_lcdc_vsync_flag_set(ctx, 0);
    while (!hpm_lvgl_lcdc_vsync_flag_get(ctx))
        hpm_lvgl_lcdc_vsync_wait(ctx);
}

SDK_DECLARE_EXT_ISR_M(HPM_LVGL_LCDC_IRQ_NUM, hpm_lvgl_lcdc_vsync_isr)
void hpm_lvgl_lcdc_vsync_isr(void)
{
    hpm_lvgl_context_t *ctx = &hpm_lvgl_context;

    lcdc_clear_status(HPM_LVGL_LCDC_BASE, LCDC_ST_VS_BLANK_MASK);
    hpm_lvgl_lcdc_vsync_flag_set(ctx, 1);
    hpm_lvgl_lcdc_vsync_signal(ctx);
}

static void hpm_lvgl_lcdc_enable(uint32_t fb_buffer)
{
    display_pixel_format_t pixel_format;
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer;

    lcdc_get_default_config(HPM_LVGL_LCDC_BASE, &config);
    board_panel_para_to_lcdc(&config);

#if LV_COLOR_DEPTH == 32
    pixel_format = display_pixel_format_argb8888;
#elif LV_COLOR_DEPTH == 16
    pixel_format = display_pixel_format_rgb565;
#else
#error only support 16 or 32 color depth
#endif

    lcdc_init(HPM_LVGL_LCDC_BASE, &config);
    lcdc_get_default_layer_config(HPM_LVGL_LCDC_BASE, &layer, pixel_format, HPM_LVGL_LCDC_LAYER_INDEX);
    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = HPM_LVGL_LCD_WIDTH;
    layer.height = HPM_LVGL_LCD_HEIGHT;
    layer.buffer = fb_buffer;
    layer.stride = HPM_LVGL_LCD_STRIDE;
    layer.background.u = 0;
    layer.max_bytes = lcdc_layer_max_bytes_512;

    if (status_success != lcdc_config_layer(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        }
    }

    lcdc_turn_on_display(HPM_LVGL_LCDC_BASE);
    lcdc_enable_interrupt(HPM_LVGL_LCDC_BASE, LCDC_INT_EN_VS_BLANK_MASK);
    intc_m_enable_irq_with_priority(HPM_LVGL_LCDC_IRQ_NUM, HPM_LVGL_LCDC_IRQ_PRIORITY);
}

void hpm_lvgl_display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    hpm_lvgl_pdma_add_area(&ctx->pdma_ctx, area);
#endif

    if (ctx->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT && lv_display_flush_is_last(disp) == 0) {
        lv_display_flush_ready(disp);
        return;
    }

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, HPM_LVGL_FB_SIZE);
    }

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
#if defined(LV_USE_HPM_PDMA_WAIT_VSYNC) && LV_USE_HPM_PDMA_WAIT_VSYNC
    hpm_lvgl_wait_lcdc_vsync(ctx);
#endif
    hpm_lvgl_pdma_copy_start(&ctx->pdma_ctx, (uint32_t)hpm_lcdc_fb, (uint32_t)px_map);
#else
    do {
        lcdc_layer_set_next_buffer(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX, (uint32_t)px_map);
        hpm_lvgl_wait_lcdc_vsync(ctx);
    } while (!lcdc_layer_control_shadow_loaded(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX));
        
    lv_display_flush_ready(disp);
#endif
}

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
ATTR_RAMFUNC void hpm_lvgl_pdma_finish_callback(void *cb_data)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)cb_data;

    lv_display_flush_ready(ctx->disp);
}
#endif


void hpm_lvgl_display_init(void)
{
    lv_display_render_mode_t render_mode;
    lv_display_t *disp;
    uint32_t lcdc_fb_buffer;
    hpm_lvgl_context_t *ctx = &hpm_lvgl_context;

#if defined(LV_USE_HPM_MODE_DIRECT) && LV_USE_HPM_MODE_DIRECT
    render_mode = LV_DISPLAY_RENDER_MODE_DIRECT;
#else
    render_mode = LV_DISPLAY_RENDER_MODE_FULL;
#endif

    lcdc_fb_buffer = (uint32_t)hpm_lvgl_fb1;
    disp = lv_display_create(HPM_LVGL_LCD_WIDTH, HPM_LVGL_LCD_HEIGHT);

    /*
     * lvgl support rotation when LV_USE_HPM_PDMA_FLUSH is enabled.
     *
     * You can set rotation to:
     * - LV_DISPLAY_ROTATION_0 (default)
     * - LV_DISPLAY_ROTATION_90
     * - LV_DISPLAY_ROTATION_180
     * - LV_DISPLAY_ROTATION_270
     */
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
#endif
    lv_display_set_buffers(disp, hpm_lvgl_fb0, hpm_lvgl_fb1, HPM_LVGL_FB_SIZE, render_mode);
    lv_display_set_flush_cb(disp, hpm_lvgl_display_flush_cb);
    lv_display_set_user_data(disp, ctx);

    ctx->render_mode = render_mode;
    ctx->disp = disp;

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(HPM_LVGL_LCD_WIDTH, cf);
    ctx->pdma_ctx.cfg.stride = stride;
    lcdc_fb_buffer = (uint32_t)hpm_lcdc_fb;

    hpm_lvgl_pdma_init(&ctx->pdma_ctx, HPM_LVGL_PDMA_BASE, LV_COLOR_DEPTH / 8);
    hpm_lvgl_pdma_copy_register_finish_cb(&ctx->pdma_ctx, hpm_lvgl_pdma_finish_callback, ctx);
    ctx->pdma_ctx.user_data = ctx;
#endif

    memset((void *)lcdc_fb_buffer, 0x00, HPM_LVGL_FB_SIZE);
    hpm_lvgl_lcdc_vsync_init(ctx);
    hpm_lvgl_lcdc_enable(lcdc_fb_buffer);
}

static hpm_stat_t hpm_lvgl_touchpad_init(void)
{
    hpm_stat_t stat;
    hpm_panel_t *panel;
    const char *name;

    panel = hpm_panel_find_device_default();
    name = hpm_panel_get_name(panel);
    if (!strcmp(name, "mc10128007_31b")) {
        touch_config(1, 0, 1);
    }

    stat = touch_init(BOARD_CAP_I2C_BASE);
    if (stat != status_success) {
        printf("Warning: touch init failed\n");
    }
    return stat;
}

static void hpm_lvgl_indev_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    hpm_stat_t stat;
    touch_point_t touch_points[HPM_TOUCH_MAX_POINTS];
    uint8_t num_of_points;
    static lv_indev_data_t pre_data;

    stat = touch_get_data(touch_points, &num_of_points);
    if (stat == status_success) {
        if ((num_of_points < HPM_TOUCH_MAX_POINTS) && num_of_points) {
            pre_data.state = LV_INDEV_STATE_PRESSED;
            pre_data.point.x = touch_points[0].x;
            pre_data.point.y = touch_points[0].y;
        }
    } else if (stat == status_touch_points_over_number) {
        pre_data.state = LV_INDEV_STATE_RELEASED;
    }

    data->state = pre_data.state;
    data->point.x = pre_data.point.x;
    data->point.y = pre_data.point.y;
}

void hpm_lvgl_indev_init(void)
{
    lv_indev_t *indev;

    if (hpm_lvgl_touchpad_init() == status_success) {
        indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev,  hpm_lvgl_indev_read_cb);
        lv_indev_set_user_data(indev, &hpm_lvgl_context);
        hpm_lvgl_context.indev = indev;
    }
}

void hpm_lvgl_init(void)
{
    lv_init();
    hpm_lvgl_tick_init();
    hpm_lvgl_display_init();
    hpm_lvgl_indev_init();
}
