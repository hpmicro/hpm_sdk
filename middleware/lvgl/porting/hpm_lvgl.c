/*
 * Copyright (c) 2023-2026 HPMicro
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

#endif

typedef struct hpm_lvgl_context {
    lv_display_t *disp;
    lv_indev_t *indev;
    lv_draw_buf_t buf1;
    lv_draw_buf_t buf2;
    lv_draw_buf_t buf3;
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    lv_draw_buf_t pdma_buf;
#else
    volatile lv_draw_buf_t * volatile wait_buf;    /* already rendered buffer */
    volatile lv_draw_buf_t * volatile online_buf;  /* currently being used by lcdc buffer */
    volatile lv_draw_buf_t * volatile ready_buf;   /* already set to lcdc but not yet used */
#endif

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
#if defined(LV_USE_HPM_3RD_BUFFER) && LV_USE_HPM_3RD_BUFFER
static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) hpm_lvgl_fb2[HPM_LVGL_FB_SIZE];
#endif

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

static bool hpm_lvgl_lcdc_shadow_load_done(void)
{
    return lcdc_layer_control_shadow_loaded(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX);
}

SDK_DECLARE_EXT_ISR_M(HPM_LVGL_LCDC_IRQ_NUM, hpm_lvgl_lcdc_vsync_isr)
void hpm_lvgl_lcdc_vsync_isr(void)
{
    hpm_lvgl_context_t *ctx = &hpm_lvgl_context;

    lcdc_clear_status(HPM_LVGL_LCDC_BASE, LCDC_ST_VS_BLANK_MASK);

#if !(defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH)
    if (hpm_lvgl_lcdc_shadow_load_done()) {
        if (ctx->ready_buf) {
            ctx->online_buf = ctx->ready_buf;
            ctx->ready_buf = NULL;
        }

        if (ctx->wait_buf) {
            lcdc_layer_set_next_buffer(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX, (uint32_t)ctx->wait_buf->data);
            ctx->ready_buf = ctx->wait_buf;
            ctx->wait_buf = NULL;
        }
    }
#endif

    hpm_lvgl_lcdc_vsync_flag_set(ctx, 1);
    hpm_lvgl_lcdc_vsync_signal(ctx);
}

static void hpm_lvgl_lcdc_enable(void *fb_buffer)
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
    layer.buffer = (uint32_t)fb_buffer;
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

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
SDK_DECLARE_EXT_ISR_M(HPM_LVGL_PDMA_IRQ_NUM, hpm_lvgl_pdma_isr)
void hpm_lvgl_pdma_isr(void)
{
    pdma_stop(HPM_LVGL_PDMA_BASE);
    lv_display_flush_ready(hpm_lvgl_context.disp);
}

static int hpm_lvgl_pdma_flush_start(lv_display_t *disp, uint8_t *px_map)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);
    lv_display_rotation_t lvgl_rotation = lv_display_get_rotation(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t status;
    display_buf_t src;
    display_buf_t dst;
    pdma_blit_option_t ops = {0};

    pdma_get_default_blit_option(&ops);

    src.buf = (uint8_t *)px_map;
    src.format = (lv_color_format_get_bpp(cf) == 32) ? display_buf_format_argb8888 : display_buf_format_rgb565;
    src.width = lv_display_get_horizontal_resolution(disp);
    src.height = lv_display_get_vertical_resolution(disp);
    src.stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
    src.alpha.op = display_alpha_op_invalid;

    dst.buf = (uint8_t *)ctx->pdma_buf.data;
    dst.format = src.format;
    dst.alpha.op = display_alpha_op_invalid;

    if (lvgl_rotation == LV_DISPLAY_ROTATION_90) {
        ops.rotate = pdma_rotate_270_degree;
        dst.width = lv_display_get_vertical_resolution(disp);
        dst.height = lv_display_get_horizontal_resolution(disp);
        dst.stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_270) {
        ops.rotate = pdma_rotate_90_degree;
        dst.width = lv_display_get_vertical_resolution(disp);
        dst.height = lv_display_get_horizontal_resolution(disp);
        dst.stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_180) {
        ops.rotate = pdma_rotate_180_degree;
        dst.width = src.width;
        dst.height = src.height;
        dst.stride = src.stride;
    } else {
        ops.rotate = pdma_rotate_0_degree;
        dst.width = src.width;
        dst.height = src.height;
        dst.stride = src.stride;
    }

    pdma_blit_ex(HPM_LVGL_PDMA_BASE, &dst, &src, &ops, false, &status);
    pdma_enable_irq(HPM_LVGL_PDMA_BASE, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, true);

    return 0;
}

static void hpm_lv_display_set_pdma_buffers(lv_display_t *disp, void *buf, uint32_t buf_size)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);

    uint32_t w = lv_display_get_original_horizontal_resolution(disp);
    uint32_t h = lv_display_get_original_vertical_resolution(disp);

    /* buf is not aligned according to LV_DRAW_BUF_ALIGN */
    LV_ASSERT_FORMAT_MSG(buf == lv_draw_buf_align(buf, cf), "buf is not aligned: %p", buf);
    uint32_t stride = lv_draw_buf_width_to_stride(w, cf);

    lv_draw_buf_init(&ctx->pdma_buf, w, h, cf, stride, buf, buf_size);
}

void hpm_lvgl_display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);

    if (ctx->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT && lv_display_flush_is_last(disp) == 0) {
        lv_display_flush_ready(disp);
        return;
    }
#if !(defined(LV_USE_DRAW_VG_LITE) && LV_USE_DRAW_VG_LITE)
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, HPM_LVGL_FB_SIZE);
    }
#endif

#if defined(LV_USE_HPM_PDMA_WAIT_VSYNC) && LV_USE_HPM_PDMA_WAIT_VSYNC
    hpm_lvgl_wait_lcdc_vsync(ctx);
#endif
    hpm_lvgl_pdma_flush_start(disp, px_map);
}

#else

void hpm_lvgl_display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    (void) area;
    uint32_t irq_state;
    lv_draw_buf_t *draw_buf;
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);

    if (ctx->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT && lv_display_flush_is_last(disp) == 0) {
        lv_display_flush_ready(disp);
        return;
    }

#if !(defined(LV_USE_DRAW_VG_LITE) && LV_USE_DRAW_VG_LITE)
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, HPM_LVGL_FB_SIZE);
    }
#endif

    lv_display_flush_ready(disp);
    draw_buf = lv_display_get_buf_active(disp);

    irq_state = hpm_lvgl_irq_lock();
    if (hpm_lvgl_lcdc_shadow_load_done()) {
        lcdc_layer_set_next_buffer(HPM_LVGL_LCDC_BASE, HPM_LVGL_LCDC_LAYER_INDEX, (uint32_t)draw_buf->data);
        ctx->ready_buf = draw_buf;
    } else {
        ctx->wait_buf = draw_buf;
    }
    hpm_lvgl_irq_unlock(irq_state);
}

static void hpm_lv_event_render_start_cb(lv_event_t * e)
{
    lv_display_t *disp = lv_event_get_target(e);
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);
    lv_draw_buf_t *draw_buf = lv_display_get_buf_active(disp);

    while (draw_buf == ctx->online_buf) {
        /* wait for the current buffer to be consumed by lcdc */
        hpm_lvgl_wait_lcdc_vsync(ctx);
    }
}
#endif

static void hpm_lv_display_set_buffers(lv_display_t *disp, void *buf1, void *buf2, void *buf3, uint32_t buf_size,
                            lv_display_render_mode_t render_mode)
{
    hpm_lvgl_context_t *ctx = (hpm_lvgl_context_t *)lv_display_get_user_data(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t w = lv_display_get_horizontal_resolution(disp);
    uint32_t h = lv_display_get_vertical_resolution(disp);

    /* buf1 or buf2 or buf3 is not aligned according to LV_DRAW_BUF_ALIGN */
    LV_ASSERT_FORMAT_MSG(buf1 == lv_draw_buf_align(buf1, cf), "buf1 is not aligned: %p", buf1);
    LV_ASSERT_FORMAT_MSG(buf2 == lv_draw_buf_align(buf2, cf), "buf2 is not aligned: %p", buf2);
    LV_ASSERT_FORMAT_MSG(buf3 == NULL || buf3 == lv_draw_buf_align(buf3, cf), "buf3 is not aligned: %p", buf3);

    uint32_t stride = lv_draw_buf_width_to_stride(w, cf);

    lv_draw_buf_init(&ctx->buf1, w, h, cf, stride, buf1, buf_size);
    lv_draw_buf_init(&ctx->buf2, w, h, cf, stride, buf2, buf_size);
    lv_display_set_draw_buffers(disp, &ctx->buf1, &ctx->buf2);

    if (buf3) {
        lv_draw_buf_init(&ctx->buf3, w, h, cf, stride, buf3, buf_size);
        lv_display_set_3rd_draw_buffer(disp, &ctx->buf3);
    }
    lv_display_set_render_mode(disp, render_mode);
}

void hpm_lvgl_display_init(void)
{
    lv_display_render_mode_t render_mode;
    lv_display_t *disp;
    void *lcdc_fb_buffer;
    void *buf_3rd = NULL;
    hpm_lvgl_context_t *ctx = &hpm_lvgl_context;

#if defined(LV_USE_HPM_MODE_DIRECT) && LV_USE_HPM_MODE_DIRECT
    render_mode = LV_DISPLAY_RENDER_MODE_DIRECT;
#else
    render_mode = LV_DISPLAY_RENDER_MODE_FULL;
#endif

    lcdc_fb_buffer = hpm_lvgl_fb1;
    disp = lv_display_create(HPM_LVGL_LCD_WIDTH, HPM_LVGL_LCD_HEIGHT);
    lv_display_set_user_data(disp, ctx);

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
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    hpm_lv_display_set_pdma_buffers(disp, hpm_lcdc_fb, HPM_LVGL_FB_SIZE);
#endif

#if defined(LV_USE_HPM_3RD_BUFFER) && LV_USE_HPM_3RD_BUFFER
    buf_3rd = hpm_lvgl_fb2;
#endif

    hpm_lv_display_set_buffers(disp, hpm_lvgl_fb0, hpm_lvgl_fb1, buf_3rd, HPM_LVGL_FB_SIZE, render_mode);
    lv_display_set_flush_cb(disp, hpm_lvgl_display_flush_cb);

    ctx->render_mode = render_mode;
    ctx->disp = disp;

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    clock_add_to_group(LVGL_PDMA_CLOCK, HPM_LVGL_RUNNING_CORE);
    intc_m_enable_irq_with_priority(HPM_LVGL_PDMA_IRQ_NUM, HPM_LVGL_PDMA_IRQ_PRIORITY);

    lcdc_fb_buffer = hpm_lcdc_fb;
#else
    lv_display_add_event_cb(disp, hpm_lv_event_render_start_cb, LV_EVENT_RENDER_START, NULL);
#endif

    memset(lcdc_fb_buffer, 0x00, HPM_LVGL_FB_SIZE);
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
    (void) indev;
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
