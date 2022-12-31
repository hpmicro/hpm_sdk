/*
 * Copyright (c) 2021 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "board.h"
#include "hpm_l1c_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_dma_drv.h"

#include "lv_port_disp.h"
#include "hpm_dma_drv.h"

/*
 * resource definition
 */

#ifndef LVGL_COPY_FB_DATA_USE_MEMCPY

#ifndef LVGL_USE_DMA
#define LVGL_USE_DMA
#endif

#endif

#ifndef RUNNING_CORE_INDEX
#define RUNNING_CORE_INDEX HPM_CORE0
#endif

#define LCD_CONTROLLER BOARD_LCD_BASE
#define LCD_LAYER_INDEX (0)

#define LV_LCD_WIDTH BOARD_LCD_WIDTH
#define LV_LCD_HEIGHT BOARD_LCD_HEIGHT

static void init_lcd(void);
static void flush_display(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static lcdc_layer_config_t layer;
static lv_disp_drv_t g_disp_drv;
static lv_color_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) framebuffer[3][LV_LCD_WIDTH * LV_LCD_HEIGHT];
static uint32_t ms_per_frame = 0;

#define LV_LCDC_TRANS_BUFFER_ADDR        ((void *)(&framebuffer[2]))
/* DMA defines */
#ifdef LVGL_USE_DMA

#ifdef BOARD_APP_XDMA

/* XDMA */
#define LV_DMA_CONTROLLER BOARD_APP_XDMA
#define LV_DMA_BURST_SIZE    (1024)
#ifndef LV_DMA_IRQ
#define LV_DMA_IRQ BOARD_APP_XDMA_IRQ
#endif

#elif defined BOARD_APP_HDMA

/* HDMA */
#define LV_DMA_CONTROLLER BOARD_APP_HDMA
#define LV_DMA_BURST_SIZE    (512)
#ifndef LV_DMA_IRQ
#define LV_DMA_IRQ BOARD_APP_HDMA_IRQ
#endif

#else

#error "Both BOARD_APP_XDMA and BOARD_APP_HDMA not found. Define LVGL_COPY_FB_DATA_USE_MEMCPY to run without using DMA."

#endif

#ifndef LV_DMA_CHANNEL
#define LV_DMA_CHANNEL (0U)
#endif

#if LV_DMA_BURST_SIZE > (8 * SIZE_1KB)
#error LV_DMA_BURST_SIZE cannot > 8KB!!
#endif

void isr_lv_dma(void)
{
    volatile uint32_t stat = dma_get_irq_status(LV_DMA_CONTROLLER);
    dma_clear_irq_status(LV_DMA_CONTROLLER, stat);
    lv_disp_flush_ready(&g_disp_drv);
}

SDK_DECLARE_EXT_ISR_M(LV_DMA_IRQ, isr_lv_dma)

void lv_start_transfer_data_to_buffer(void* src, uint32_t len)
{
    volatile hpm_stat_t stat;
    stat = dma_start_memcpy(LV_DMA_CONTROLLER, LV_DMA_CHANNEL,
                (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)LV_LCDC_TRANS_BUFFER_ADDR),
                (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t)src),
                len, LV_DMA_BURST_SIZE);
    if (stat != status_success) {
        printf("failed to start dma transfer\n");
    }
}

#endif

__attribute__((weak)) void hpm_lv_monitor(lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{
    ms_per_frame = time;
}

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    init_lcd();
#ifdef LVGL_USE_DMA
    intc_m_enable_irq_with_priority(LV_DMA_IRQ, 1);
#endif
    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, framebuffer[0], framebuffer[1], LV_LCD_WIDTH * LV_LCD_HEIGHT);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&g_disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    g_disp_drv.hor_res = LV_LCD_WIDTH;
    g_disp_drv.ver_res = LV_LCD_HEIGHT;

    /*Used to copy the buffer's content to the display*/
    g_disp_drv.flush_cb = flush_display;
    g_disp_drv.monitor_cb = hpm_lv_monitor;

    /*Set a display buffer*/
    g_disp_drv.draw_buf = &draw_buf_dsc;
    g_disp_drv.full_refresh = 1;

    /*Finally register the driver*/
    lv_disp_drv_register(&g_disp_drv);
}

/* Initialize your display and the required peripherals. */
static void init_lcd(void)
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
    lcdc_get_default_layer_config(LCD_CONTROLLER, &layer, pixel_format, LCD_LAYER_INDEX);

    memset(framebuffer, 0, sizeof(framebuffer));

    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = config.resolution_x;
    layer.height = config.resolution_y;
    layer.buffer = (uint32_t) core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t) LV_LCDC_TRANS_BUFFER_ADDR);
    layer.background.u = 0;

    if (status_success != lcdc_config_layer(LCD_CONTROLLER, LCD_LAYER_INDEX, &layer, true)) {
        while (1) {
        }
    }

    lcdc_turn_on_display(LCD_CONTROLLER);
#ifdef LVGL_USE_DMA
    printf("lvgl using DMA transfer method.\r\n");
#else
    printf("lvgl using memcpy instead of DMA transfer method!!\r\n");

#endif
}

static void flush_display(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_color_t* cur_buffer = (lv_color_t *)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t) color_p);

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)cur_buffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)cur_buffer + LV_LCD_HEIGHT * LV_LCD_WIDTH * LV_COLOR_DEPTH / 8);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }

#ifdef LVGL_USE_DMA
    lv_start_transfer_data_to_buffer((void *)cur_buffer, sizeof(lv_color_t) * LV_LCD_WIDTH * LV_LCD_HEIGHT);
#else
    memcpy(LV_LCDC_TRANS_BUFFER_ADDR, (void *)cur_buffer, sizeof(lv_color_t) * LV_LCD_WIDTH * LV_LCD_HEIGHT);

    if (l1c_dc_is_enabled()) {
        lv_color_t* buf = (lv_color_t *)core_local_mem_to_sys_address(RUNNING_CORE_INDEX, (uint32_t) LV_LCDC_TRANS_BUFFER_ADDR);
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)buf + LV_LCD_HEIGHT * LV_LCD_WIDTH * LV_COLOR_DEPTH / 8);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }
    lv_disp_flush_ready(&g_disp_drv);
#endif

}
