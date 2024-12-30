/*
 * Copyright (c) 2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "guix_display_driver_565rgb.h"
#include "hpm_touch.h"
#include "hpm_l1c_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_soc.h"
#include "stdio.h"

/* guix rgb565 driver config */
#define GUIX_PDMA_ENABLE
#define GUIX_TOGGLE_STRATEGY_SINGLE

#ifdef GUIX_PDMA_ENABLE
#include "hpm_pdma_drv.h"
#endif

#ifndef GUIX_RUNNING_CORE_INDEX
#define GUIX_RUNNING_CORE_INDEX HPM_CORE0
#endif

#ifndef HPM_LCD_IRQ_PRIORITY
#define HPM_LCD_IRQ_PRIORITY  7
#endif

#define TOGGLE_THREAD_SIZE 4096

#define PDMA BOARD_PDMA_BASE
#define PDMA_CLOCK clock_pdma
#define LCD_CONTROLLER BOARD_LCD_BASE
#define LCD_LAYER_INDEX (0)
#define LCD_LAYER_DONE_MASK (LCD_LAYER_INDEX + 1)
#define LCD_IRQ_NUM BOARD_LCD_IRQ

#define GUIX_LCD_WIDTH (BOARD_LCD_WIDTH)
#define GUIX_LCD_HEIGHT (BOARD_LCD_HEIGHT)
#define GUIX_PIXEL_FORMAT display_pixel_format_rgb565
#define RGB565_BYTE_PER_PIXEL 2

#define GUIX_FB_SIZE (((GUIX_LCD_WIDTH * GUIX_LCD_HEIGHT * RGB565_BYTE_PER_PIXEL) + HPM_L1C_CACHELINE_SIZE - 1) & (0xFFFFFFFF << 6))

static uint8_t ATTR_PLACE_AT_WITH_ALIGNMENT(".framebuffer", HPM_L1C_CACHELINE_SIZE) canvas_buffer[GUIX_FB_SIZE];
static uint8_t ATTR_PLACE_AT_WITH_ALIGNMENT(".framebuffer", HPM_L1C_CACHELINE_SIZE) framebuffer[GUIX_FB_SIZE];

#ifdef GUIX_PDMA_ENABLE
static void blit(uint32_t *dst, uint32_t dst_width,
        const uint32_t *src, uint32_t src_width,
        uint32_t x, uint32_t y,
        uint32_t width, uint32_t height, uint32_t alpha)
{
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(GUIX_PIXEL_FORMAT));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(GUIX_PIXEL_FORMAT));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    pdma_blit(PDMA,
            (uint32_t)core_local_mem_to_sys_address(GUIX_RUNNING_CORE_INDEX, (uint32_t)dst),
            dst_width,
            (uint32_t)core_local_mem_to_sys_address(GUIX_RUNNING_CORE_INDEX, (uint32_t)src),
            src_width,
            x, y, width, height, alpha, GUIX_PIXEL_FORMAT, true, &status);
}

#endif

#if defined (GUIX_TOGGLE_STRATEGY_SINGLE)

struct guix_rgb565_single_ctx {
    volatile bool is_display_finished;
    GX_CANVAS dummy_canvas;
};

static struct guix_rgb565_single_ctx single_ctx;

SDK_DECLARE_EXT_ISR_M(LCD_IRQ_NUM, hpm_lcdc_isr)
void hpm_lcdc_isr(void)
{
    volatile uint32_t s = lcdc_get_dma_status(LCD_CONTROLLER);
    lcdc_clear_dma_status(LCD_CONTROLLER, s);

    if ((!single_ctx.is_display_finished) &&\
        (s & (LCD_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT))) {
        single_ctx.is_display_finished = true;
    }
}

static void guix_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{   
    if (canvas == NULL || dirty == NULL)
        return;
                       
    struct guix_rgb565_single_ctx *ctx = &single_ctx;

    ctx->is_display_finished = false;
    while (!ctx->is_display_finished);

    _gx_canvas_dirty_mark(&ctx->dummy_canvas, dirty);

#ifdef GUIX_PDMA_ENABLE
    GX_RECTANGLE limit;
    GX_RECTANGLE overlap;

    gx_utility_rectangle_define(&limit, 0, 0, canvas->gx_canvas_x_resolution - 1,
                               canvas->gx_canvas_y_resolution - 1);
    gx_utility_rectangle_shift(&limit, canvas->gx_canvas_display_offset_x,
                               canvas->gx_canvas_display_offset_y);

    if (gx_utility_rectangle_overlap_detect(&limit, &(ctx->dummy_canvas.gx_canvas_dirty_area), &overlap)) 
        blit((uint32_t*)ctx->dummy_canvas.gx_canvas_memory, GUIX_LCD_WIDTH, (uint32_t *)canvas->gx_canvas_memory, GUIX_LCD_WIDTH, 
            0, 0, overlap.gx_rectangle_right + 1, overlap.gx_rectangle_bottom + 1, 0xFF);         
#else    
    GX_DISPLAY *display = canvas->gx_canvas_display;
    display->gx_display_driver_canvas_copy(canvas, &ctx->dummy_canvas);

#endif   
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(framebuffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(framebuffer + GUIX_FB_SIZE * sizeof(uint8_t));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }
}


#elif defined (GUIX_TOGGLE_STRATEGY_DOUBLE)
#define CONDITION(n) ((1 << n) - 1)
#define CONDITION_DONE CONDITION(2)

static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) frame_double[GUIX_FB_SIZE];

struct guix_rgb565_double_ctx {
    uint32_t display_buffer;
    uint32_t transfer_buffer;
    uint32_t condition_mask;
    
    TX_THREAD toggle_thread;
    UCHAR toggle_thread_stack[TOGGLE_THREAD_SIZE];
    TX_SEMAPHORE semaphore;
};

static struct guix_rgb565_double_ctx double_ctx;

static void guix_driver_semaphore_create(void)
{
    struct guix_rgb565_double_ctx *ctx = &double_ctx;

    tx_semaphore_create(&ctx->semaphore, "driver_semaphore", 1);
}

static void guix_display_put(struct guix_rgb565_double_ctx* ctx)
{
    ctx->condition_mask |= 1 << 0;
    if (ctx->condition_mask == CONDITION_DONE)
        tx_semaphore_put(&ctx->semaphore);
}

static void guix_draw_put(struct guix_rgb565_double_ctx* ctx)
{
    ctx->condition_mask |= 1 << 1;
    if (ctx->condition_mask == CONDITION_DONE)
        tx_semaphore_put(&ctx->semaphore);
}

static void guix_wait_semaphore(struct guix_rgb565_double_ctx* ctx)
{
    do {
        tx_semaphore_get(&ctx->semaphore, TX_WAIT_FOREVER); 
    } while(ctx->condition_mask != CONDITION_DONE);  
    
    ctx->condition_mask = 0;  
}


SDK_DECLARE_EXT_ISR_M(LCD_IRQ_NUM, hpm_lcdc_isr)
void hpm_lcdc_isr(void)
{
    struct guix_rgb565_double_ctx *ctx = &double_ctx; 
    volatile uint32_t s = lcdc_get_dma_status(LCD_CONTROLLER);
    lcdc_clear_dma_status(LCD_CONTROLLER, s);

    if ((ctx->display_buffer) &&\
        lcdc_layer_control_shadow_loaded(LCD_CONTROLLER, LCD_LAYER_INDEX) &&\
        (s & (LCD_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT))) {
        lcdc_layer_set_next_buffer(LCD_CONTROLLER, LCD_LAYER_INDEX, (uint32_t)ctx->display_buffer);
        ctx->display_buffer = 0;
            
        guix_display_put(ctx); 
    }
}

static void guix_double_buffer_toggle(struct guix_rgb565_double_ctx *ctx)
{
    guix_wait_semaphore(ctx);

    if (ctx->transfer_buffer == (uint32_t)framebuffer) {
        ctx->transfer_buffer = (uint32_t)frame_double;
    } else {
        ctx->transfer_buffer = (uint32_t)framebuffer;
    }

#ifdef  GUIX_PDMA_ENABLE
    blit((uint32_t *)ctx->transfer_buffer, GUIX_LCD_WIDTH, (uint32_t *)canvas_buffer, GUIX_LCD_WIDTH, 
        0, 0, GUIX_LCD_WIDTH, GUIX_LCD_HEIGHT, 0xFF);
#else
    memcpy((uint8_t *)ctx->transfer_buffer, canvas_buffer, GUIX_FB_SIZE * sizeof(uint8_t));
#endif    
    ctx->display_buffer = ctx->transfer_buffer;
}

static void guix_double_buffer_task(ULONG unused)
{
    struct guix_rgb565_double_ctx *ctx = (struct guix_rgb565_double_ctx *)unused;

    for (; ;) {
        guix_double_buffer_toggle(ctx);
    }
}

static void guix_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    struct guix_rgb565_double_ctx *ctx = &double_ctx;

    guix_draw_put(ctx);
}

static void start_toggle_thread(void)
{
    struct guix_rgb565_double_ctx *ctx = &double_ctx;
    ctx->transfer_buffer = core_local_mem_to_sys_address(GUIX_RUNNING_CORE_INDEX, (uint32_t)framebuffer);
    ctx->display_buffer = ctx->transfer_buffer;
    guix_driver_semaphore_create();
    tx_thread_create(&ctx->toggle_thread, "GUIX TOGGLE Thread", guix_double_buffer_task, 
                     (ULONG)ctx,  ctx->toggle_thread_stack, sizeof(ctx->toggle_thread_stack), 
                     GX_SYSTEM_THREAD_PRIORITY - 1,GX_SYSTEM_THREAD_PRIORITY - 1,
                     TX_NO_TIME_SLICE, TX_AUTO_START);
}
#endif

static void hpm_lcdc_init(void) 
{
    lcdc_config_t config = {0};

    lcdc_get_default_config(LCD_CONTROLLER, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD_CONTROLLER, &config);
}

static UINT guix_display_layer_hpm_initialize(INT layer, GX_CANVAS *canvas)
{
    if (layer == 0) {       
        /* change main_canvas buffer to "canvas_buffer" */
        canvas->gx_canvas_memory = (ULONG *)canvas_buffer;
        canvas->gx_canvas_memory_size = sizeof(canvas_buffer);
        
        display_pixel_format_t pixel_format;
        pixel_format = display_pixel_format_rgb565;
  
        memset(&framebuffer, 0, sizeof(framebuffer));
        lcdc_layer_config_t dummy_layer;
        lcdc_get_default_layer_config(LCD_CONTROLLER, &dummy_layer, pixel_format, layer);

        dummy_layer.position_x = 0;
        dummy_layer.position_y = 0;
        dummy_layer.width = GUIX_LCD_WIDTH;
        dummy_layer.height = GUIX_LCD_HEIGHT;
        dummy_layer.buffer = (uint32_t)core_local_mem_to_sys_address(GUIX_RUNNING_CORE_INDEX, (uint32_t)framebuffer);
        dummy_layer.background.u = 0;

        if (status_success != lcdc_config_layer(LCD_CONTROLLER, layer, &dummy_layer, true))
          while (1);

        lcdc_turn_on_display(LCD_CONTROLLER);
        lcdc_enable_interrupt(LCD_CONTROLLER, LCDC_INT_EN_DMA_DONE_SET(LCD_LAYER_DONE_MASK));
        intc_m_enable_irq_with_priority(LCD_IRQ_NUM, HPM_LCD_IRQ_PRIORITY);

        return GX_SUCCESS;
    }

    return GX_FAILURE;
}

static GX_DISPLAY_LAYER_SERVICES gx_hpm_layer_services = {
    .gx_display_layer_initialize = guix_display_layer_hpm_initialize,
};

UINT guix_display_driver_hpm6750_565rgb_setup(GX_DISPLAY *display)
{
#ifdef GUIX_PDMA_ENABLE
    clock_add_to_group(PDMA_CLOCK, GUIX_RUNNING_CORE_INDEX);
#endif
    hpm_lcdc_init();
    _gx_display_driver_565rgb_setup(display, GX_NULL, guix_buffer_toggle);
    display->gx_display_layer_services = &gx_hpm_layer_services;

#if defined(GUIX_TOGGLE_STRATEGY_SINGLE)
    gx_canvas_create(&single_ctx.dummy_canvas, "display_buffer", display, GX_CANVAS_SIMPLE, 
                GUIX_LCD_WIDTH, GUIX_LCD_HEIGHT, (GX_COLOR *)&framebuffer, GUIX_FB_SIZE);
#elif defined(GUIX_TOGGLE_STRATEGY_DOUBLE)
    start_toggle_thread();
#endif
    
    return GX_SUCCESS;
}
