/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_pdma_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"

#ifndef BLIT_RESULT_VERIFICATION
#define BLIT_RESULT_VERIFICATION 0
#endif

#define LCD BOARD_LCD_BASE
#define LCD_IRQ BOARD_LCD_IRQ
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define PDMA BOARD_PDMA_BASE
#define RUNNING_CORE HPM_CORE0

#define MCHTMR_COUNT_PER_MS (BOARD_MCHTMR_FREQ_IN_HZ/1000)
#define TEST_LAYER_COUNT 2
#define COUNT_DOWN 60

#define _CONCAT3(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) _CONCAT3(x, y, z)

#define PIXEL_FORMAT display_pixel_format_argb8888
#define COLOR_SIZE 32

#define LAYER_WIDTH 128
#define LAYER_HEIGHT 96

typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

color_t __attribute__((section (".framebuffer"))) buffer[TEST_LAYER_COUNT][LAYER_WIDTH * LAYER_HEIGHT];
volatile bool vsync = false;

typedef struct {
    uint32_t u;
    display_pixel_format_t format;
} color;

color c[TEST_LAYER_COUNT];

struct {
    lcdc_layer_config_t layer;
    bool left_to_right;
    bool up_to_down;
} layer_info[TEST_LAYER_COUNT];

void isr_lcd_d0(void)
{
    if (!vsync) {
        vsync = true;
    }
    LCD->ST |= LCDC_ST_VSYNC_MASK;
}
SDK_DECLARE_EXT_ISR_M(LCD_IRQ, isr_lcd_d0)

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

void flip_rotate(color_t *dst, uint32_t dst_width,
                const color_t *src, uint32_t src_width, uint32_t x, uint32_t y,
                uint32_t width, uint32_t height,
                pdma_flip_t flip, pdma_rotate_t rotate, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_flip_rotate(PDMA,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
            dst_width,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
            src_width, x, y,
            width, height, flip, rotate, alpha, PIXEL_FORMAT, true, &status);

    if (stat != status_success) {
        printf("pdma flip rotate failed: 0x%x\n", status);
    }
}

void blit(color_t *dst, uint32_t dst_width,
        const color_t *src, uint32_t src_width,
        uint32_t x, uint32_t y,
        uint32_t width, uint32_t height, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_blit(PDMA,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
            dst_width,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
            src_width,
            x, y, width, height, alpha, PIXEL_FORMAT, true, &status);

    if (stat != status_success) {
        printf("pdma blit failed: 0x%x\n", status);
    }

#if BLIT_RESULT_VERIFICATION == 1
    uint32_t i, j, source, *p_dest;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            p_dest = (uint32_t *)((uint32_t)dst
                    + ((i + y) * dst_width + x + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT));
            source = *(uint32_t *)((uint32_t)src
                    + (i * width + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT));
            if (((*p_dest) & 0xFFFFFF) != (source & 0xFFFFFF)) {
                printf("!color mismatch: src: 0x%x, 0x%x; dst: 0x%x, 0x%x\n",
                        (uint32_t)src + (i * width + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT), source,
                        p_dest, *p_dest);
            }
        }
    }
#endif
}

void fill_area(color_t *buf, uint32_t width, uint32_t height, color *c, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)buf + width * height * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_fill_color(PDMA,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)buf), width,
            width, height,
            c->u, alpha, c->format, true, &status);

    if (stat != status_success) {
        printf("pdma fill color failed: 0x%x\n", status);
    }
}


void scale(color_t *dst, uint32_t dst_width,
        const color_t *src, uint32_t src_width,
        uint32_t x, uint32_t y,
        uint32_t width, uint32_t height,
        uint32_t target_width, uint32_t target_height,
        uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_scale(PDMA,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
            dst_width,
            (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
            src_width,
            x, y, width, height,
            target_width, target_height,
            alpha, PIXEL_FORMAT, true, &status);

    if (stat != status_success) {
        printf("pdma scale failed: 0x%x\n", status);
    }

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

void move_layer(uint8_t index)
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
    if ((layer->position_x >= (LCD_WIDTH - layer->width)) || (layer->position_x == 0)) {
        layer_info[index].left_to_right ^= true;
        layer->position_x = layer->position_x ? LCD_WIDTH - layer->width : 0;
    }
    if ((layer->position_y == 0) || ((layer->position_y >= LCD_HEIGHT - layer->height))) {
        layer_info[index].up_to_down ^= true;
        layer->position_y = layer->position_y ? LCD_HEIGHT- layer->height : 0;
    }

    lcdc_config_layer(LCD, index, layer, true);
}

bool initialize_lcd(void)
{
    uint32_t i = 0;
    lcdc_config_t config = {0};
    lcdc_layer_config_t *layer;
    lcdc_get_default_config(LCD, &config);

    config.resolution_x = LCD_WIDTH;
    config.resolution_y = LCD_HEIGHT;

    lcdc_init(LCD, &config);
    lcdc_enable_interrupt(LCD, LCDC_INT_EN_VSYNC_MASK);

    for (i = 0; i < TEST_LAYER_COUNT; i++) {
        layer = &layer_info[i].layer;
        lcdc_get_default_layer_config(LCD, layer, PIXEL_FORMAT, i);
        fill_area(buffer[i], LAYER_WIDTH, LAYER_HEIGHT, &c[i], 0xFF);

        layer->position_x = i * (config.resolution_x - LAYER_WIDTH) / TEST_LAYER_COUNT;
        layer->position_y = (rand() + LAYER_HEIGHT) % config.resolution_y;
        layer->width = LAYER_WIDTH;
        layer->height = LAYER_HEIGHT;
        layer->buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)buffer[i]);
        if (i == 0) {
            layer->background.u = 0;
        } else {
            layer->alphablend.src_alpha = 0xFF; /* src */
            layer->alphablend.dst_alpha = 0xFF; /* dst */
            layer->alphablend.src_alpha_op = display_alpha_op_invalid;
            layer->alphablend.dst_alpha_op = display_alpha_op_invalid;
            layer->background.u = 0;
            layer->alphablend.mode = display_alphablend_mode_src_over;
        }

        if (status_success != lcdc_config_layer(LCD, i, layer, true)) {
            printf("failed to configure layer %d\n", i);
            return false;
        }
    }

    lcdc_turn_on_display(LCD);
    intc_m_enable_irq_with_priority(LCD_IRQ, 1);
    return true;
}

void run(void)
{
    uint32_t i;
    uint32_t count_down;
    uint32_t alpha_count;
    pdma_rotate_t rotate;

    generate_color_table();
    if (!initialize_lcd()) {
        printf("initialize LCD failed\n");
        while(1);
    }
    printf("%d rectangles will be drawn on the screen with alpha blending enabled\n", TEST_LAYER_COUNT);

    count_down = COUNT_DOWN;
    alpha_count = 0;
    rotate = pdma_rotate_90_degree;
    while(1) {
        if(!vsync) {
            __asm("wfi");
        }
        for (i = 0; i < TEST_LAYER_COUNT; i++) {
            move_layer(i);
        }
        if (count_down == 0) {
            alpha_count++;
            if (alpha_count == COUNT_DOWN) {
                alpha_count = 0;
            }
            generate_color_table();
            for (i = 0; i < TEST_LAYER_COUNT; i++) {
                fill_area(buffer[i], LAYER_WIDTH, LAYER_HEIGHT,
                        &c[i], 0xFF - (alpha_count << 1));
            }
            count_down = COUNT_DOWN;
#if TEST_LAYER_COUNT > 1
        } else if (count_down == (COUNT_DOWN >> 1)) {
            i = rand() % (TEST_LAYER_COUNT - 1);
            /* copy image from buffer[i] to buffer[TEST_LAYER_COUNT -1] */
            blit(buffer[TEST_LAYER_COUNT - 1], LAYER_WIDTH,     /* dest buffer, dest buffer width */
                 buffer[i], LAYER_WIDTH,                        /* source buffer, source buffer width */
                 0, LAYER_HEIGHT / 2,                           /* x, y: coordinates in the dest area */
                 LAYER_WIDTH / 2, LAYER_HEIGHT / 2, 0xFF);      /* width, height, alpha */
            /* rotate image from buffer[TEST_LAYER_COUNT - 1] to buffer[1] */
            flip_rotate(buffer[i], LAYER_WIDTH,                 /* dest buffer, dest buffer width  */
                    buffer[TEST_LAYER_COUNT - 1], LAYER_WIDTH,  /* source buffer, source buffer width */
                    LAYER_WIDTH / 2, 0,                         /* x, y: coordinates in the dest area */
                    LAYER_WIDTH / 2, LAYER_HEIGHT / 4,          /* width, height */
                    pdma_flip_none, rotate, 0xFF);
            rotate++;
            rotate &= pdma_rotate_270_degree;

            scale(buffer[i], LAYER_WIDTH,                      /* dest buffer, dest buffer width  */
                  buffer[TEST_LAYER_COUNT - 1], LAYER_WIDTH,   /* source buffer, source buffer width */
                  0, LAYER_HEIGHT / 2,                         /* x, y: coordinates in the dest area */
                  LAYER_WIDTH / 2, LAYER_HEIGHT / 4,           /* width, height */
                  LAYER_WIDTH / 2, LAYER_HEIGHT / 6,           /* target_width, target_height */
                  0xFF);
#endif
        }
        count_down--;
        vsync = false;
    }
}

int main(void)
{
    board_init();
    board_init_lcd();

    printf("pdma example\r\n");

    run();
    while(1);
}
