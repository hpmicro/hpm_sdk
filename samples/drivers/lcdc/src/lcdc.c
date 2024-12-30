/*
 * Copyright (c) 2021 HPMicro
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

#include "logo.h"

#ifndef TIMER_CLOCK_NAME
#define TIMER_CLOCK_NAME clock_mchtmr0
#endif

#ifndef LCD
#define LCD BOARD_LCD_BASE
#define LCD_IRQ BOARD_LCD_IRQ
#endif

#define TEST_LAYER_COUNT 8
#define TEST_LAYER_DONE_MASK (1)

#define _CONCAT3(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) _CONCAT3(x, y, z)

#define PIXEL_FORMAT display_pixel_format_argb8888
#define COLOR_SIZE 32

#define LAYER_WIDTH 64
#define LAYER_HEIGHT 32

typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

color_t logobuffer[LOGO_HEIGHT * LOGO_WIDTH] __attribute__((section(".framebuffer")));

color_t buffer[TEST_LAYER_COUNT][LAYER_WIDTH * LAYER_HEIGHT];
volatile bool vsync = false;

uint32_t timer_freq_in_hz = 0;

typedef struct {
    uint32_t u;
    uint8_t format;
} color;

color c[TEST_LAYER_COUNT];

struct {
    lcdc_layer_config_t layer;
    bool left_to_right;
    bool up_to_down;
} layer_info[TEST_LAYER_COUNT];

SDK_DECLARE_EXT_ISR_M(BOARD_LCD_IRQ, isr_lcd_d0)
void isr_lcd_d0(void)
{
    volatile uint32_t s = lcdc_get_dma_status(LCD);
    lcdc_clear_dma_status(LCD, s);
    if (s & (TEST_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT)) {
        if (lcdc_layer_control_shadow_loaded(LCD, 0)) {
            vsync = true;
        }
    }
}

uint32_t get_rgb_color(color *c)
{
    switch (c->format)
    {
        case display_pixel_format_rgb565:
            return  ((((c->u & 0xFF0000) >> 16) * 0x1F / 0xFF) << 11)
                          | (((((c->u & 0xFF00)) >> 8) * 0x3F / 0xFF) << 6)
                          | (((c->u & 0xFF) * 0x1F / 0xFF) << 0);
        case display_pixel_format_argb8888:
            return c->u;
        default:
            return 0;
    }
}

void prepare_rgb_data(uint8_t *buf, uint32_t size, color *c)
{
    uint32_t i = 0, j = 0;
    uint8_t pixel_width = display_get_pixel_size_in_byte(c->format);
    uint32_t color = get_rgb_color(c);

    for (i = 0; i < size; i+=pixel_width) {
        for (j = 0; j < pixel_width; j++) {
            *(buf + i + j) = (color >> (j * 8)) & 0xFF;
        }
    }

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)buf + size);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

void run_test_mode(void)
{
    uint8_t layer_index = 0;
    lcdc_layer_config_t dummy_layer = {0};
    lcdc_config_t config = {0};

    printf("run test mode\n");
    printf("Background\n");

    /*
     * lcdc needs to be initialized with one layer at least for test mode
     */
    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &dummy_layer, PIXEL_FORMAT, layer_index);
    dummy_layer.width = 2;
    dummy_layer.height = 2;
    dummy_layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer[0]);
    if (status_success != lcdc_config_layer(LCD, layer_index, &dummy_layer, true)) {
        printf("failed to configure dummy layer for test mode\n");
        return;
    }

    display_color_32b_t color;
    color.u = 0xFF0FF0;
    lcdc_set_background(LCD, color);
    lcdc_set_testmode(LCD, LCDC_TEST_MODE_BACKGROUND);
    board_delay_ms(2000);
    printf("Color bar: column\n");
    lcdc_set_testmode(LCD, LCDC_TEST_MODE_COLOR_BAR_COL);
    board_delay_ms(2000);
    printf("Color bar: row\n");
    lcdc_set_testmode(LCD, LCDC_TEST_MODE_COLOR_BAR_ROW);
    board_delay_ms(2000);
    lcdc_set_testmode(LCD, LCDC_TEST_MODE_DISABLE);
    lcdc_turn_off_display(LCD);
    board_delay_ms(100);
}

void generate_color_table(void)
{
    uint8_t i;
    for (i = 0; i < TEST_LAYER_COUNT; i++) {
        c[i].format = PIXEL_FORMAT;
        c[i].u = 0xFF000000 | ((rand() % 0xFF + i) << 16)
                  | ((rand() % 0xFF + i) << 8)
                  | ((rand() % 0xFF + i));
    }
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)&c);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)&c + sizeof(c));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

void move_layer(uint8_t index, uint32_t width, uint32_t height)
{
    lcdc_layer_config_t *layer = &layer_info[index].layer;

    if (layer_info[index].left_to_right) {
        layer->position_x++;
    } else {
        layer->position_x--;
    }

    if (layer_info[index].up_to_down) {
        layer->position_y++;
    } else {
        layer->position_y--;
    }
    if ((layer->position_x >= (width - layer->width)) || (layer->position_x == 0)) {
        layer_info[index].left_to_right ^= true;
        layer->position_x = layer->position_x ? width - layer->width : 0;
    }
    if ((layer->position_y == 0) || ((layer->position_y >= height - layer->height))) {
        layer_info[index].up_to_down ^= true;
        layer->position_y = layer->position_y ? height - layer->height : 0;
    }

    lcdc_layer_update_position(LCD, index, layer->position_x, layer->position_y);
}

void run_layer_change(void)
{
    uint32_t i = 0;
    lcdc_config_t config = {0};

    printf("%d rectangles will be drawn on the screen with alpha blending enabled\n", TEST_LAYER_COUNT);

    generate_color_table();
    lcdc_layer_config_t *layer;
    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD, &config);
    lcdc_enable_interrupt(LCD, LCDC_INT_EN_DMA_DONE_SET(TEST_LAYER_DONE_MASK));

    memcpy(logobuffer, &logo[0], sizeof(logo));
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)logobuffer);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)logobuffer + sizeof(logo));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
    }
    layer_info[TEST_LAYER_COUNT - 1].layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)logobuffer);
    layer_info[TEST_LAYER_COUNT - 1].layer.width = LOGO_WIDTH;
    layer_info[TEST_LAYER_COUNT - 1].layer.height = LOGO_HEIGHT;
    for (i = 0; i < TEST_LAYER_COUNT; i++) {
        layer = &layer_info[i].layer;
        lcdc_get_default_layer_config(LCD, layer, PIXEL_FORMAT, i);
        prepare_rgb_data((uint8_t *)&buffer[i], sizeof(buffer[i]), &c[i]);

        if (i < (TEST_LAYER_COUNT - 1)) {
            layer->buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer[i]);
            layer->width = LAYER_WIDTH;
            layer->height = LAYER_HEIGHT;
        }
        if (i == 0) {
            layer->background.u = 0;
        } else if (i == (TEST_LAYER_COUNT - 1)) {
            layer->alphablend.src_alpha = 0xFF; /* src */
            layer->alphablend.dst_alpha = 0xFF; /* dst */
            layer->alphablend.src_alpha_op = display_alpha_op_invalid;
            layer->alphablend.dst_alpha_op = display_alpha_op_invalid;
            layer->background.u = 0;
            layer->alphablend.mode = display_alphablend_mode_src_over;
        } else {
            layer->alphablend.src_alpha = 0xF4; /* src */
            layer->alphablend.dst_alpha = 0xF0; /* dst */
            layer->alphablend.src_alpha_op = display_alpha_op_override;
            layer->alphablend.dst_alpha_op = display_alpha_op_override;
            layer->background.u = 0;
            layer->alphablend.mode = display_alphablend_mode_src_over;
        }

        /*
         * Note:
         * layer->position_x + layer->width <= config.resolution_x
         * layer->position_y + layer->width <= config.resolution_y
         */
        layer->position_x = (rand() + layer->width) % (config.resolution_x - layer->width);
        layer->position_y = (rand() + layer->height) % (config.resolution_y - layer->height);

        if (status_success != lcdc_config_layer(LCD, i, layer, true)) {
            printf("failed to configure layer %d\n", i);
            return;
        }
    }

    lcdc_turn_on_display(LCD);
    intc_m_enable_irq_with_priority(BOARD_LCD_IRQ, 1);

    while(1) {
        vsync = false;
        while(!vsync) {
            __asm("nop");
        }
        for (i = 0; i < TEST_LAYER_COUNT; i++) {
            move_layer(i, config.resolution_x, config.resolution_y);
        }
    }
}

int main(void)
{
    board_init();
    board_init_lcd();

    timer_freq_in_hz = clock_get_frequency(TIMER_CLOCK_NAME);
    printf("lcdc example\r\n");

    run_test_mode();
    run_layer_change();
    while(1);
}
