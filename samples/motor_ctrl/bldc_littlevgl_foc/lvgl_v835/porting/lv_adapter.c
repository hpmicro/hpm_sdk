/*
 * Copyright (c) 2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#if defined(CONFIG_LV_TOUCH) && CONFIG_LV_TOUCH
#include "hpm_touch.h"
#endif
#include "hpm_l1c_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_pdma_drv.h"
#include "lvgl.h"

#define LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE 1
#define LVGL_CONFIG_DIRECT_MODE_VSYNC_ENABLE 1

#ifndef RUNNING_CORE_INDEX
#define RUNNING_CORE_INDEX HPM_CORE0
#endif

#define LCD_CONTROLLER BOARD_LCD_BASE
#define LCD_LAYER_INDEX (0)
#define LCD_LAYER_DONE_MASK (1U<<LCD_LAYER_INDEX)
#define LCD_IRQ_NUM  BOARD_LCD_IRQ

#ifndef HPM_LCD_IRQ_PRIORITY
#define HPM_LCD_IRQ_PRIORITY  7
#endif

#define LVGL_PDMA_IRQ_NUM IRQn_PDMA_D0
#define LVGL_PDMA_IRQ_PRIORITY 6
#define LVGL_PDMA_BASE HPM_PDMA
#define LVGL_PDMA_CLOCK clock_pdma

#define LV_LCD_WIDTH (BOARD_LCD_WIDTH)
#define LV_LCD_HEIGHT (BOARD_LCD_HEIGHT)

#define LV_FB_SIZE_ALIGNED HPM_L1C_CACHELINE_ALIGN_UP(LV_LCD_WIDTH * LV_LCD_HEIGHT)

#if LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE

#ifndef HPM_LVGL_FRAMEBUFFER_NONCACHEABLE
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer0[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer1[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) lcdc_framebuffer[LV_FB_SIZE_ALIGNED];
#else
static lv_color_t __attribute__((section(".noncacheable.non_init"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer0[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".noncacheable.non_init"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer1[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".noncacheable.non_init"), aligned(HPM_L1C_CACHELINE_SIZE))) lcdc_framebuffer[LV_FB_SIZE_ALIGNED];
#endif

#else

#ifndef HPM_LVGL_FRAMEBUFFER_NONCACHEABLE
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer0[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer1[LV_FB_SIZE_ALIGNED];
#else
static lv_color_t __attribute__((section(".noncacheable.non_init"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer0[LV_FB_SIZE_ALIGNED];
static lv_color_t __attribute__((section(".noncacheable.non_init"), aligned(HPM_L1C_CACHELINE_SIZE))) lv_framebuffer1[LV_FB_SIZE_ALIGNED];
#endif
static lv_color_t *lcdc_framebuffer = lv_framebuffer1;

#endif/*LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE*/

#if LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE
struct pdma_ctx {
    struct {
        pdma_plane_config_t plane_src_cfg;
        pdma_output_config_t output_cfg;
        display_yuv2rgb_coef_t yuv2rgb_coef;
    } cfg;
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    volatile uint16_t inv_p;
};
#endif

struct lv_adapter {
    lv_disp_draw_buf_t draw_buf;
    lv_disp_drv_t disp_drv;
#if defined(CONFIG_LV_TOUCH)
    lv_indev_drv_t indev_touch_drv;
#endif
    volatile uint32_t wait_flush_buffer;
    volatile uint32_t lcdc_buffer;
#if LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE
    struct pdma_ctx pdma_ctx;
    volatile uint32_t direct_vsync;
#endif
};

static struct lv_adapter lv_adapter_ctx;

#if LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE
static void lvgl_pdma_init(struct lv_adapter *ctx)
{
    struct pdma_ctx *pdma_ctx = &ctx->pdma_ctx;
    pdma_config_t config;

#if LV_COLOR_DEPTH == 32
    display_pixel_format_t pixel_format = display_pixel_format_argb8888;
#else
    display_pixel_format_t pixel_format = display_pixel_format_rgb565;
#endif
    clock_add_to_group(LVGL_PDMA_CLOCK, RUNNING_CORE_INDEX);
    pdma_get_default_config(LVGL_PDMA_BASE, &config, pixel_format);

    config.enable_plane = pdma_plane_src;
    config.block_size = pdma_blocksize_8x8;

    pdma_init(LVGL_PDMA_BASE, &config);
    pdma_plane_config_t *plane_src_cfg = &pdma_ctx->cfg.plane_src_cfg;
    pdma_output_config_t *output_cfg = &pdma_ctx->cfg.output_cfg;
    display_yuv2rgb_coef_t *yuv2rgb_coef = &pdma_ctx->cfg.yuv2rgb_coef;

    pdma_get_default_plane_config(LVGL_PDMA_BASE, plane_src_cfg, pixel_format);
    pdma_get_default_yuv2rgb_coef_config(LVGL_PDMA_BASE, yuv2rgb_coef, pixel_format);
    pdma_get_default_output_config(LVGL_PDMA_BASE, output_cfg, pixel_format);

    intc_m_enable_irq_with_priority(LVGL_PDMA_IRQ_NUM, LVGL_PDMA_IRQ_PRIORITY);
}

static void lvgl_pdma_blit(struct lv_adapter *ctx, void *dst, uint16_t dst_stride, void *src, uint16_t src_stride,
                    uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint32_t pixel_size = sizeof(lv_color_t);
    struct pdma_ctx *pdma_ctx = &ctx->pdma_ctx;
    pdma_plane_config_t *plane_src_cfg = &pdma_ctx->cfg.plane_src_cfg;
    pdma_output_config_t *output_cfg = &pdma_ctx->cfg.output_cfg;
    pdma_stop(LVGL_PDMA_BASE);

    plane_src_cfg->buffer = (uint32_t)src + (y * src_stride + x) * pixel_size;
    plane_src_cfg->width = width;
    plane_src_cfg->height = height;
    plane_src_cfg->pitch = src_stride * pixel_size;
    plane_src_cfg->background = 0xFFFFFFFF;
    pdma_config_planes(LVGL_PDMA_BASE, plane_src_cfg, NULL, NULL);

    output_cfg->plane[pdma_plane_src].x = 0;
    output_cfg->plane[pdma_plane_src].y = 0;
    output_cfg->plane[pdma_plane_src].width = width;
    output_cfg->plane[pdma_plane_src].height = height;

    output_cfg->alphablend.src_alpha = 0xFF;
    output_cfg->alphablend.src_alpha_op = display_alpha_op_override;
    output_cfg->alphablend.mode = display_alphablend_mode_clear;

    output_cfg->width = width;
    output_cfg->height = height;
    output_cfg->buffer = (uint32_t)dst + (y * dst_stride + x) * pixel_size;
    output_cfg->pitch = dst_stride * pixel_size;

    pdma_config_output(LVGL_PDMA_BASE, output_cfg);
    pdma_start(LVGL_PDMA_BASE);
    pdma_enable_irq(LVGL_PDMA_BASE, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, true);
}

static void lvgl_pdma_done(struct lv_adapter *ctx)
{
    struct pdma_ctx *pdma_ctx = &ctx->pdma_ctx;
    if (pdma_ctx->inv_p == 0) {
        lv_disp_flush_ready(&ctx->disp_drv);
        pdma_stop(LVGL_PDMA_BASE);
        return;
    }

    while (pdma_ctx->inv_p > 0) {
        pdma_ctx->inv_p--;
        /*area should be ignored*/
        if (pdma_ctx->inv_area_joined[pdma_ctx->inv_p]) {
            if (pdma_ctx->inv_p == 0) {
                lv_disp_flush_ready(&ctx->disp_drv);
                pdma_stop(LVGL_PDMA_BASE);
            }
            continue;
        }

        lv_area_t *area = &pdma_ctx->inv_areas[pdma_ctx->inv_p];
        lvgl_pdma_blit(ctx, (void *)ctx->lcdc_buffer, LV_LCD_WIDTH,
                        (void *)ctx->wait_flush_buffer,  LV_LCD_WIDTH,
                    area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area));
        break;
    }
}

void lvgl_pdma_start(struct lv_adapter *ctx)
{
    struct pdma_ctx *pdma_ctx = &ctx->pdma_ctx;

    if (pdma_ctx->inv_p == 0) {
        /*Never to be run*/
        lv_disp_flush_ready(&ctx->disp_drv);
        return;
    }

    while (pdma_ctx->inv_p > 0) {
        pdma_ctx->inv_p--;
        /*area should be ignored*/
        if (pdma_ctx->inv_area_joined[pdma_ctx->inv_p]) {
            if (pdma_ctx->inv_p == 0) {
                /*Never to be run*/
                lv_disp_flush_ready(&ctx->disp_drv);
                pdma_stop(LVGL_PDMA_BASE);
            }
            continue;
        }

        lv_area_t *area = &pdma_ctx->inv_areas[pdma_ctx->inv_p];
        lvgl_pdma_blit(ctx, (void *)ctx->lcdc_buffer, LV_LCD_WIDTH,
                        (void *)ctx->wait_flush_buffer,  LV_LCD_WIDTH,
                    area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area));
        break;
    }
}

static void lv_flush_display_direct(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    struct lv_adapter *ctx = disp_drv->user_data;
    uint32_t wait_flush_buffer = core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)color_p);

    if (!lv_disp_flush_is_last(disp_drv)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
#ifndef HPM_LVGL_FRAMEBUFFER_NONCACHEABLE
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(wait_flush_buffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(wait_flush_buffer + disp_drv->draw_buf->size * sizeof(lv_color_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }
#endif
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    struct pdma_ctx *pdma_ctx = &ctx->pdma_ctx;
    ctx->wait_flush_buffer = wait_flush_buffer;
    pdma_ctx->inv_p = disp->inv_p;
    memcpy(pdma_ctx->inv_area_joined, disp->inv_area_joined, sizeof(disp->inv_area_joined));
    memcpy(pdma_ctx->inv_areas, disp->inv_areas, sizeof(disp->inv_areas));
#if LVGL_CONFIG_DIRECT_MODE_VSYNC_ENABLE
    while (!ctx->direct_vsync) {
    }
    ctx->direct_vsync = 0;
#endif

    lvgl_pdma_start(ctx);
}

SDK_DECLARE_EXT_ISR_M(LVGL_PDMA_IRQ_NUM, lvgl_pdma_isr)
void lvgl_pdma_isr(void)
{
    pdma_enable_irq(LVGL_PDMA_BASE, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, false);
    pdma_stop(LVGL_PDMA_BASE);
    lvgl_pdma_done(&lv_adapter_ctx);
}

SDK_DECLARE_EXT_ISR_M(LCD_IRQ_NUM, hpm_lcdc_isr)
void hpm_lcdc_isr(void)
{
    volatile uint32_t s = lcdc_get_dma_status(LCD_CONTROLLER);
    lcdc_clear_dma_status(LCD_CONTROLLER, s);

    if (s & (LCD_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT)) {
        lv_adapter_ctx.direct_vsync = 1;
    }
}

#else

static void lv_flush_display_full(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    struct lv_adapter *ctx = disp_drv->user_data;
    uint32_t wait_flush_buffer = core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)color_p);

    /* Never to be run */
    if (ctx->wait_flush_buffer) {
        printf("Warning: discard %p lvgl buffer\n", color_p);
        return;
    }

#ifndef HPM_LVGL_FRAMEBUFFER_NONCACHEABLE
    if (l1c_dc_is_enabled()) {
       uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(wait_flush_buffer);
       uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(wait_flush_buffer + disp_drv->draw_buf->size * sizeof(lv_color_t));
       uint32_t aligned_size = aligned_end - aligned_start;
       l1c_dc_writeback(aligned_start, aligned_size);
    }
#endif

    ctx->wait_flush_buffer = wait_flush_buffer;
}

SDK_DECLARE_EXT_ISR_M(LCD_IRQ_NUM, hpm_lcdc_isr)
void hpm_lcdc_isr(void)
{
    volatile uint32_t s = lcdc_get_dma_status(LCD_CONTROLLER);
    lcdc_clear_dma_status(LCD_CONTROLLER, s);

    if ((lv_adapter_ctx.wait_flush_buffer) &&\
        lcdc_layer_control_shadow_loaded(LCD_CONTROLLER, LCD_LAYER_INDEX) &&\
        (s & (LCD_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT))) {
        lcdc_layer_set_next_buffer(LCD_CONTROLLER, LCD_LAYER_INDEX, (uint32_t)lv_adapter_ctx.wait_flush_buffer);
        lv_adapter_ctx.wait_flush_buffer = 0;
        lv_disp_flush_ready(&lv_adapter_ctx.disp_drv);
    }
}

#endif/*LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE*/

static void hpm_lcdc_init(void)
{
    display_pixel_format_t pixel_format;
    lcdc_config_t config = {0};
    lcdc_get_default_config(LCD_CONTROLLER, &config);
    board_panel_para_to_lcdc(&config);

#if LV_COLOR_DEPTH == 32
    pixel_format = display_pixel_format_argb8888;
#elif LV_COLOR_DEPTH == 16
    pixel_format = display_pixel_format_rgb565;
#else
#error only support 16 or 32 color depth
#endif

    lcdc_init(LCD_CONTROLLER, &config);

    memset(lcdc_framebuffer, 0, LV_LCD_WIDTH * LV_LCD_HEIGHT * sizeof(lv_color_t));
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(LCD_CONTROLLER, &layer, pixel_format, LCD_LAYER_INDEX);

    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = LV_LCD_WIDTH;
    layer.height = LV_LCD_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)lcdc_framebuffer);
    layer.background.u = 0;

    if (status_success != lcdc_config_layer(LCD_CONTROLLER, LCD_LAYER_INDEX, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        }
    }

    lcdc_turn_on_display(LCD_CONTROLLER);
    lcdc_enable_interrupt(LCD_CONTROLLER, LCD_LAYER_DONE_MASK << 16);
    intc_m_enable_irq_with_priority(LCD_IRQ_NUM, HPM_LCD_IRQ_PRIORITY);
    lv_adapter_ctx.lcdc_buffer = layer.buffer;
}

static void lv_disp_init(void)
{
    lv_disp_draw_buf_t *draw_buf = &lv_adapter_ctx.draw_buf;
    lv_disp_drv_t *disp_drv = &lv_adapter_ctx.disp_drv;

    hpm_lcdc_init();
    lv_disp_draw_buf_init(draw_buf, lv_framebuffer0, lv_framebuffer1, LV_LCD_WIDTH * LV_LCD_HEIGHT);
    lv_disp_drv_init(disp_drv);

    disp_drv->hor_res = LV_LCD_WIDTH;
    disp_drv->ver_res = LV_LCD_HEIGHT;
    disp_drv->draw_buf = draw_buf;

#if LVGL_CONFIG_FLUSH_DIRECT_MODE_ENABLE
     disp_drv->direct_mode = 1;
    disp_drv->flush_cb = lv_flush_display_direct;
    lvgl_pdma_init(&lv_adapter_ctx);
#else
    disp_drv->full_refresh = 1;
    disp_drv->flush_cb = lv_flush_display_full;
#endif
    disp_drv->user_data = &lv_adapter_ctx;

    lv_disp_drv_register(disp_drv);
}

#if defined(CONFIG_LV_TOUCH) && CONFIG_LV_TOUCH
static hpm_stat_t hpm_touchpad_init(void)
{
    hpm_stat_t stat;
    stat = touch_init(BOARD_CAP_I2C_BASE);
    if (stat != status_success) {
        printf("Warning: touch init failed\n");
    }
    return stat;
}

static void hpm_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    hpm_stat_t stat;
    touch_point_t touch_points[HPM_TOUCH_MAX_POINTS];
    uint8_t num_of_points;
    stat = touch_get_data(touch_points, &num_of_points);
    if ((stat == status_success) &&\
        (num_of_points < HPM_TOUCH_MAX_POINTS) &&\
        num_of_points) {
        data->state = LV_INDEV_STATE_PR;
#if defined(CONFIG_TOUCH_GT9XX) &&  CONFIG_TOUCH_GT9XX
        data->point.x = touch_points[0].x;
        data->point.y = touch_points[0].y;
#elif defined(CONFIG_TOUCH_FT5406) && CONFIG_TOUCH_FT5406
        data->point.x = touch_points[0].y;
        data->point.y = touch_points[0].x;
#else
    #error unsupported touch
#endif
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
#endif

static void lv_indev_init(void)
{
#if defined(CONFIG_LV_TOUCH) && CONFIG_LV_TOUCH
    if (hpm_touchpad_init() == status_success) {
        lv_indev_drv_init(&lv_adapter_ctx.indev_touch_drv);
        lv_adapter_ctx.indev_touch_drv.type = LV_INDEV_TYPE_POINTER;
        lv_adapter_ctx.indev_touch_drv.read_cb = hpm_touchpad_read;
        lv_indev_drv_register(&lv_adapter_ctx.indev_touch_drv);
    }
#endif

    /* Other indev can be added */
}

void lv_adapter_init(void)
{
    lv_disp_init();
    lv_indev_init();
}
