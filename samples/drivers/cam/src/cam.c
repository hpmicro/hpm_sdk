/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"

#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif

#define LCD BOARD_LCD_BASE
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define CAM_I2C BOARD_CAM_I2C_BASE

#define _CONCAT3(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) _CONCAT3(x, y, z)

#if (CAMERA_MAX_IMAGE_OUTPUT_WIDTH > BOARD_LCD_WIDTH)
#define IMAGE_WIDTH  BOARD_LCD_WIDTH
#else
#define IMAGE_WIDTH  CAMERA_MAX_IMAGE_OUTPUT_WIDTH
#endif

#if (CAMERA_MAX_IMAGE_OUTPUT_HEIGHT > BOARD_LCD_HEIGHT)
#define IMAGE_HEIGHT BOARD_LCD_HEIGHT
#else
#define IMAGE_HEIGHT CAMERA_MAX_IMAGE_OUTPUT_HEIGHT
#endif

#define PIXEL_FORMAT display_pixel_format_rgb565
#define CAMERA_INTERFACE camera_interface_dvp

#define COLOR_SIZE 16 
typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

color_t buffer[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section (".framebuffer")));

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

    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = IMAGE_WIDTH;
    layer.height = IMAGE_HEIGHT;

    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
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

void init_camera_device(void)
{
    camera_context_t camera_context = {NULL, NULL, NULL, NULL};
    camera_config_t camera_config;

    camera_context.ptr = CAM_I2C;
    camera_context.delay_ms = board_delay_ms;
#ifdef BOARD_SUPPORT_CAM_RESET
    camera_context.write_rst = board_write_cam_rst;
#endif
#ifdef BOARD_SUPPORT_CAM_PWDN
    camera_context.write_pwdn = board_write_cam_pwdn;
#endif

    camera_config.width = IMAGE_WIDTH;
    camera_config.height = IMAGE_HEIGHT;
    camera_config.pixel_format = PIXEL_FORMAT;
    camera_config.interface = CAMERA_INTERFACE;

    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while(1);
    }
}

void init_cam(void)
{
    cam_config_t cam_config;

    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);

    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = true;
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    if (PIXEL_FORMAT == display_pixel_format_rgb565) {
        cam_config.color_format = CAM_COLOR_FORMAT_RGB565;
    }
    cam_init(TEST_CAM, &cam_config);
}

int main(void)
{
    board_init();
    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_lcd();
    board_init_cam_pins();

    printf("cam example\r\n");

    init_camera_device();
    init_lcd();

    init_cam();
    cam_start(TEST_CAM);

    lcdc_turn_on_display(LCD);
    while(1);
}
