/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_crc_drv.h"
#include "hpm_gwc_drv.h"
#include "hpm_pixelmux_drv.h"
#include <stdlib.h>

#define GWC_CFG_NUM 8
#define LAYER_INDEX 0
#define PIXEL_FORMAT display_pixel_format_argb8888
#define PIXEL_COLOR_SIZE 4
#define GWC_FB_SIZE_ALIGNED HPM_L1C_CACHELINE_ALIGN_UP(BOARD_GWC_PIXEL_WIDTH * BOARD_GWC_PIXEL_HEIGHT)
static uint32_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) fb_buffer[GWC_FB_SIZE_ALIGNED];

struct gwc_test_config {
    bool enable;
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
};

struct gwc_test_ctx {
    struct gwc_test_config cfgs[GWC_CFG_NUM];
    volatile bool lcdc_sync;
    volatile bool gwc_func;
    volatile uint32_t status;
    volatile uint32_t refcrc;
};

static struct gwc_test_ctx gwc_ctx = {
    .cfgs = {
        {/* one ponit in upper left */
            .enable = true,
            .x0 = 0,
            .y0 = 0,
            .x1 = 0,
            .y1 = 0
        }, {/* one ponit in upper right */
            .enable = true,
            .x0 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y0 = 0,
            .x1 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y1 = 0,
        }, {/* one ponit in lower left */
            .enable = true,
            .x0 = 0,
            .y0 = BOARD_GWC_PIXEL_HEIGHT - 1,
            .x1 = 0,
            .y1 = BOARD_GWC_PIXEL_HEIGHT - 1,
        }, {/* one ponit in lower right */
            .enable = true,
            .x0 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y0 = BOARD_GWC_PIXEL_HEIGHT - 1,
            .x1 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y1 = BOARD_GWC_PIXEL_HEIGHT - 1,
        }, {/*first row*/
            .enable = true,
            .x0 = 0,
            .y0 = 0,
            .x1 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y1 = 0,
        }, {/*first col*/
            .enable = true,
            .x0 = 0,
            .y0 = 0,
            .x1 = 0,
            .y1 = BOARD_GWC_PIXEL_HEIGHT - 1,
        }, { /*a part of screen */
            .enable = true,
            .x0 = BOARD_GWC_PIXEL_WIDTH / 4,
            .y0 = BOARD_GWC_PIXEL_HEIGHT / 4,
            .x1 = BOARD_GWC_PIXEL_WIDTH * 3 / 4,
            .y1 = BOARD_GWC_PIXEL_HEIGHT *3 / 4,
        }, { /*full screen*/
            .enable = true,
            .x0 = 0,
            .y0 = 0,
            .x1 = BOARD_GWC_PIXEL_WIDTH - 1,
            .y1 = BOARD_GWC_PIXEL_HEIGHT - 1,
        }
    }
};

SDK_DECLARE_EXT_ISR_M(BOARD_LCD_IRQ, isr_lcd)
void isr_lcd(void)
{
    volatile uint32_t s = lcdc_get_dma_status(BOARD_LCD_BASE);
    lcdc_clear_dma_status(BOARD_LCD_BASE, s);
    if (s & (1ul<<(LAYER_INDEX + LCDC_DMA_ST_DMA0_DONE_SHIFT))) {
        gwc_ctx.lcdc_sync = true;
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_GWC_FUNC_IRQ, isr_gwc_func)
void isr_gwc_func(void)
{
    gwc_clear_status(BOARD_GWC_BASE, GWC_IRQ_STS_FUNC_STS_MASK);
    gwc_ctx.gwc_func = true;
}

SDK_DECLARE_EXT_ISR_M(BOARD_GWC_ERR_IRQ, isr_gwc_err)
void isr_gwc_err(void)
{
    gwc_ctx.status = gwc_get_status(BOARD_GWC_BASE);
    gwc_clear_status(BOARD_GWC_BASE, GWC_IRQ_STS_GWC_FAIL_STS_MASK);
}

static uint32_t interface_random32(void)
{
    return (uint32_t)rand();
}

static void fill_layer(void)
{
    for (int y = 0; y < BOARD_GWC_PIXEL_HEIGHT; y++) {
        for (int x = 0; x < BOARD_GWC_PIXEL_WIDTH; x++) {
            fb_buffer[y * BOARD_GWC_PIXEL_WIDTH + x] = interface_random32();
        }
    }

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)fb_buffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)fb_buffer + sizeof(fb_buffer));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

static uint32_t calc_crc_color32(void *buf, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t stride)
{
    int crc_ch = 5;
    uint32_t *buf32 = (uint32_t *)buf;
    uint32_t index;
    crc_channel_config_t cfg = {
        .preset = crc_preset_none,
        .in_byte_order = crc_in_byte_order_lsb,
        .init = 0x0,
        .poly = 0x04C11DB7,
        .poly_width = CRC_POLY_WIDTH_32,
        .refout = false,
        .refin = false,
        .xorout = 0x0,
    };

    crc_setup_channel_config(HPM_CRC, crc_ch, &cfg);

    for (uint32_t y = y0; y <= y1; y++) {
        for (uint32_t x = x0; x <= x1; x++) {
            index = (y * stride + x);
            uint32_t p = (buf32[index] & 0Xff);
            p <<= 8;
            p |= (buf32[index]>>8 & 0Xff);
            p <<= 8;
            p |= (buf32[index]>>16 & 0Xff);
            crc_calc_block_bytes(HPM_CRC, crc_ch, (uint8_t *)&p, 3);
        }
    }

    return crc_get_result(HPM_CRC, crc_ch);
}

static void lcdc_config(void)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer;

    lcdc_get_default_config(BOARD_LCD_BASE, &config);
    config.resolution_x = BOARD_GWC_PIXEL_WIDTH;
    config.resolution_y = BOARD_GWC_PIXEL_HEIGHT;
    lcdc_init(BOARD_LCD_BASE, &config);

    lcdc_get_default_layer_config(BOARD_LCD_BASE, &layer, PIXEL_FORMAT, LAYER_INDEX);
    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)fb_buffer);
    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = BOARD_GWC_PIXEL_WIDTH;
    layer.height = BOARD_GWC_PIXEL_HEIGHT;
    lcdc_config_layer(BOARD_LCD_BASE, LAYER_INDEX, &layer, true);

    fill_layer();
    lcdc_turn_on_display(BOARD_LCD_BASE);
    lcdc_enable_interrupt(BOARD_LCD_BASE, LCDC_INT_EN_DMA_DONE_SET(1ul<<LAYER_INDEX));
    intc_m_enable_irq_with_priority(BOARD_LCD_IRQ, 1);
    gwc_ctx.lcdc_sync = false;
    while (gwc_ctx.lcdc_sync == false) {
    }
}

static void gwc_check(struct gwc_test_ctx *ctx, uint8_t channel)
{
    uint32_t ref_crc;
    uint32_t cal_crc;
    gwc_ch_config_t ch_cfg = {0};
    struct gwc_test_config *test_cfg;
    uint32_t cfg_num = GWC_CFG_NUM;

    for (uint32_t i = 0; i < cfg_num; i++) {
        test_cfg = &ctx->cfgs[i];
        if (test_cfg->enable == false) {
            continue;
        }

        ref_crc = calc_crc_color32(fb_buffer, test_cfg->x0, test_cfg->y0, test_cfg->x1, test_cfg->y1, BOARD_GWC_PIXEL_WIDTH);
        ch_cfg.freeze = false;
        ch_cfg.ref_crc = ref_crc;
        ch_cfg.start_col = test_cfg->x0;
        ch_cfg.start_row = test_cfg->y0;
        ch_cfg.end_col = test_cfg->x1;
        ch_cfg.end_row = test_cfg->y1;

        gwc_ch_disable(BOARD_GWC_BASE, channel);
        gwc_ch_init(BOARD_GWC_BASE, channel, &ch_cfg);
        ctx->gwc_func = false;
        ctx->status = 0;
        gwc_ch_enable(BOARD_GWC_BASE, channel);
        while (ctx->gwc_func == false) {
        }

        ctx->gwc_func = false;
        ctx->status = 0;
        while (ctx->gwc_func == false) {
        }

        cal_crc = gwc_ch_get_crc(BOARD_GWC_BASE, channel);
        if (ctx->status & GWC_IRQ_STS_ERR_STS_MASK) {
            printf("gwc test ch[%d] ref:[0x%08X] cal:[0x%08X] failed\n", channel, ref_crc, cal_crc);
            while (1) {
            };
        }

        gwc_ch_disable(BOARD_GWC_BASE, channel);
        ch_cfg.ref_crc = ~ref_crc;
        gwc_ch_init(BOARD_GWC_BASE, channel, &ch_cfg);

        ctx->gwc_func = false;
        ctx->status = 0;
        gwc_ch_enable(BOARD_GWC_BASE, channel);
        while (ctx->gwc_func == false) {
        }

        ctx->gwc_func = false;
        ctx->status = 0;
        while (ctx->gwc_func == false) {
        }

        cal_crc = gwc_ch_get_crc(BOARD_GWC_BASE, channel);
        if (!(ctx->status & GWC_IRQ_STS_ERR_STS_MASK)) {
            printf("gwc err test ch[%d] ref:[0x%08X] cal:[0x%08X] failed\n", channel, ref_crc, cal_crc);
            while (1) {
            };
        }
        gwc_ch_disable(BOARD_GWC_BASE, channel);
    }
    printf("gwc ch[%d] passed\n", channel);
}

static void gwc_config_init(void)
{
    gwc_config_t cfg;
#if defined(IRQn_GWCK0_ERR) && (BOARD_GWC_ERR_IRQ == IRQn_GWCK0_ERR)
    pixelmux_gwc0_data_source_enable(pixelmux_gwc0_sel_lcdc0);
#elif defined(IRQn_GWCK1_ERR) && (BOARD_GWC_ERR_IRQ == IRQn_GWCK1_ERR)
    pixelmux_gwc1_data_source_enable(pixelmux_gwc1_sel_lcdc0);
#endif
    gwc_get_default_config(&cfg);
    gwc_init(BOARD_GWC_BASE, &cfg);
    gwc_enable(BOARD_GWC_BASE);
    gwc_enable_interrupt(BOARD_GWC_BASE, GWC_IRQ_MASK_ERR_MASK_MASK | GWC_IRQ_MASK_FUNC_MASK_MASK);
    intc_m_enable_irq_with_priority(BOARD_GWC_FUNC_IRQ, 1);
    intc_m_enable_irq_with_priority(BOARD_GWC_ERR_IRQ, 1);
}

int main(void)
{
    board_init();
    board_init_gwc();
    lcdc_config();
    gwc_config_init();

    printf("gwc start..\n");
    for (uint32_t i = 0; i <= GWC_CHANNEL_CH15; i++) {
        gwc_check(&gwc_ctx, i);
    }
    printf("gwc all passed\n");

    while (1) {
    }
}
