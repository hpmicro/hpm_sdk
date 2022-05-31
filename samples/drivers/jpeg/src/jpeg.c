/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_jpeg_drv.h"

#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif

#define LCD BOARD_LCD_BASE
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define RUNNING_CORE HPM_CORE0

#define _CONCAT3(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) _CONCAT3(x, y, z)

#define PIXEL_FORMAT display_pixel_format_rgb565
#define COLOR_SIZE 16 
#define LAYER_WIDTH 48
#define LAYER_HEIGHT 32 

typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

/*
 * the following tables are specific for the test samples used in this demo
 */
const uint8_t in_ecs[] = {
#include "ecs_lvzhu_565_48x32.cdat"
};
const uint8_t in_img[] = {
#include "lvzhu_in_rgb565_48x32.cdat"
};
const uint32_t huffmin[16]={
    0xf0e0c082,
    0xf6f6f6f4,
    0x2b5d78f8,
    0x00000001,

    0xf0e0c080,
    0xfefefcf8,
    0xbbdf7efe,
    0x00000000,

    0xf0e0c288,
    0xf4f6f6f4,
    0x2b1c78f6,
    0x00000001,

    0xfcf8f0e0,
    0xfefefefe,
    0xbbdf7efe,
    0x00000001
};
const uint16_t huffbase[64]={
#include "base.cdat"
};
const uint8_t huffsymb[336]={
#include "symb.cdat"
};
const uint16_t huffenc[384]={
#include "htable.cdat"
};
const uint16_t qetable[256]={
#include "qetable.cdat"
};
const uint16_t qdtable[256]={
#include "qdtable.cdat"
};

uint8_t out_ecs[1024];
uint8_t out_img[LAYER_HEIGHT*LAYER_WIDTH*2];

void init_lcd(void)
{
    uint8_t layer_index = 0;
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    lcdc_get_default_config(LCD, &config);

    config.resolution_x = LCD_WIDTH;
    config.resolution_y = LCD_HEIGHT;
    
    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT, layer_index);

    layer.position_x = LCD_WIDTH / 2 - LAYER_WIDTH / 2;
    layer.position_y = LCD_HEIGHT / 2 - LAYER_HEIGHT / 2;
    layer.width = LAYER_WIDTH;
    layer.height = LAYER_HEIGHT;

    layer.buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)out_img);
    layer.alphablend.src_alpha = 0xF4; /* src */
    layer.alphablend.dst_alpha = 0xF0; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_override;
    layer.alphablend.dst_alpha_op = display_alpha_op_override;
    layer.background.u= 0xFFFF0000;
    layer.alphablend.mode = display_alphablend_mode_xor;

    if (status_success != lcdc_config_layer(LCD, layer_index, &layer, true)) {
        printf("failed to configure layer\n");
        while(1);
    }
}

void init_jpeg(void)
{
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)huffmin, ARRAY_SIZE(huffmin));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)huffbase, ARRAY_SIZE(huffbase));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)huffsymb, ARRAY_SIZE(huffsymb));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)huffenc, ARRAY_SIZE(huffenc));
    jpeg_disable(TEST_JPEG);
}

void fill_jpeg_encode_table(void)
{
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qetable, ARRAY_SIZE(qetable));
    jpeg_disable(TEST_JPEG);
}

void fill_jpeg_decode_table(void)
{
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qdtable, ARRAY_SIZE(qdtable));
    jpeg_disable(TEST_JPEG);
}

bool wait_jpeg_finish(void)
{
    do {
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_OUT_DMA_FINISH) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_OUT_DMA_FINISH);
            return true;
        }
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_ERROR) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_ERROR);
            return false;
        }
        board_delay_ms(100);
    } while(1);
}


void encode_decode_jpeg(void)
{
    uint32_t encoded_length;
    jpeg_job_config_t config = {0};
    printf("start encoding and decoding\n");

    TEST_JPEG->CFG=0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_encode_table();
    
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_422H;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.byte_order = JPEG_BYTE_ORDER_2301;
    config.enable_csc = true;
    config.enable_ycbcr = true;
    config.width_in_pixel = LAYER_WIDTH;
    config.height_in_pixel = LAYER_HEIGHT;
    config.in_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)in_img);
    config.out_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)out_ecs);

    if (status_success != jpeg_start_encode(TEST_JPEG, &config)) {
        printf("failed to endcode\n");
        while(1);
    }
    if (!wait_jpeg_finish()) {
        printf("encoding failed\n");
        while(1);
    }
    encoded_length = jpeg_get_encoded_length(TEST_JPEG);
    printf("encoded %d\n", encoded_length);

    TEST_JPEG->CFG=0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_decode_table();
    
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_422H;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.byte_order = JPEG_BYTE_ORDER_2301;
    config.enable_csc = true;
    config.enable_ycbcr = false;
    config.width_in_pixel = LAYER_WIDTH;
    config.height_in_pixel = LAYER_HEIGHT;
    config.in_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)out_ecs);
    config.out_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)out_img);

    if (status_success != jpeg_start_decode(TEST_JPEG, &config, encoded_length)) {
        printf("failed to decode\n");
        while(1);
    }
    if (wait_jpeg_finish()) {
        printf("encoding and decoding done\n");
    } else {
        printf("encoding and decoding failed\n");
    }
}

void decode_jpeg(void)
{
    jpeg_job_config_t config = {0};

    TEST_JPEG->CFG=0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_decode_table();
    
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_422H;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.byte_order = JPEG_BYTE_ORDER_2301;
    config.enable_csc = true;
    config.enable_ycbcr = true;
    config.width_in_pixel = LAYER_WIDTH;
    config.height_in_pixel = LAYER_HEIGHT;
    config.in_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)in_ecs);
    config.out_buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)out_img);

    printf("start decoding\n");
    if (status_success != jpeg_start_decode(TEST_JPEG, &config, sizeof(in_ecs))) {
        printf("failed to decode\n");
        while(1);
    }
    if (wait_jpeg_finish()) {
        printf("decoding done\n");
    } else {
        printf("decoding failed\n");
    }
}

int main(void)
{
    board_init();
    board_init_lcd();

    printf("jpeg example\r\n");

    init_jpeg();
    init_lcd();
    lcdc_turn_on_display(LCD);

    decode_jpeg();
    board_delay_ms(1000);
    encode_decode_jpeg();

    while(1);
}
