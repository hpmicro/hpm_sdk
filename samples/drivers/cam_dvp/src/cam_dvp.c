/*
 * Copyright (c) 2021 HPMicro
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

#if (CAMERA_MAX_IMAGE_OUTPUT_WIDTH > 800)
#define IMAGE_WIDTH  800
#else
#define IMAGE_WIDTH  CAMERA_MAX_IMAGE_OUTPUT_WIDTH
#endif

#if (CAMERA_MAX_IMAGE_OUTPUT_HEIGHT > 480)
#define IMAGE_HEIGHT 480
#else
#define IMAGE_HEIGHT CAMERA_MAX_IMAGE_OUTPUT_HEIGHT
#endif

#define PIXEL_FORMAT display_pixel_format_rgb565
#define CAMERA_INTERFACE camera_interface_dvp

#define COLOR_SIZE 16
typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

color_t buffer[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section (".framebuffer")));
camera_context_t camera_context = {0};
camera_config_t camera_config;

void init_lcd(void)
{
    uint8_t layer_index = 0;
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT, layer_index);

    layer.position_x = (BOARD_LCD_WIDTH - IMAGE_WIDTH) / 2;
    layer.position_y = (BOARD_LCD_HEIGHT - IMAGE_HEIGHT) / 2;
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
    camera_context.i2c_device_addr = CAMERA_DEVICE_ADDR;
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

    /* get dvp interface parameters */
    if (CAMERA_INTERFACE == camera_interface_dvp) {
        camera_device_get_dvp_param(&camera_context, &camera_config);
    }

    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while(1);
    }
}

void init_cam(void)
{
    cam_config_t cam_config;
    camera_param_dvp_t *dvp;
    assert((camera_config.interface == camera_interface_dvp) && (camera_config.interface_param != NULL));
    dvp = (camera_param_dvp_t *)camera_config.interface_param;

    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);

    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low  = dvp->hsync_active_low;
    cam_config.vsync_active_low  = dvp->vsync_active_low;
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer);
    cam_config.color_format = cam_get_pixel_format(PIXEL_FORMAT);
    if (CAM_COLOR_FORMAT_UNSUPPORTED == cam_config.color_format) {
        printf("cam does not support this pixel format\n");
        return;
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
