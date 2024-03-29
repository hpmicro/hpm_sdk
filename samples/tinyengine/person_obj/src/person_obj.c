/*
 * Copyright (c) 2024 HPMicro
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
#include "hpm_gptmr_drv.h"
#include "hpm_pdma_drv.h"
#include "genNN.h"
#include "detectionUtility.h"

#define IMAGE_PUT_RGB565_PIXEL(image, x, y, v) \
({ \
    image[(IMAGE_WIDTH * y) + x] = v; \
})

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

#define IMAGE_INFER_H 480
#define IMAGE_INFER_W 800

color_t buffer_cam[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section(".framebuffer")));
color_t buffer_inf[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section(".framebuffer")));
color_t buffer_lcd[IMAGE_WIDTH * IMAGE_HEIGHT] __attribute__((section(".framebuffer")));
color_t ATTR_RAMFUNC img_ai[160*128];
camera_context_t camera_context = {0};
camera_config_t camera_config;

uint32_t run_times;
uint64_t delta_time;

#define NUM_CLASS 1
int cnt[NUM_CLASS];
det_box *boxes[NUM_CLASS];

void start_time(void)
{
    delta_time = hpm_csr_get_core_mcycle();
}

uint32_t get_end_time(void)
{
    delta_time = hpm_csr_get_core_mcycle() - delta_time;
    return delta_time;
}


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

    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer_lcd);
    layer.alphablend.src_alpha = 0xF4; /* src */
    layer.alphablend.dst_alpha = 0xF0; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_override;
    layer.alphablend.dst_alpha_op = display_alpha_op_override;
    layer.background.u = 0xFFFF0000;
    layer.alphablend.mode = display_alphablend_mode_xor;

    if (status_success != lcdc_config_layer(LCD, layer_index, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        }
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
        while (1) {
        }
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
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buffer_cam);
    cam_config.color_format = cam_get_pixel_format(PIXEL_FORMAT);
    if (CAM_COLOR_FORMAT_UNSUPPORTED == cam_config.color_format) {
        printf("cam does not support this pixel format\n");
        return;
    }

    cam_init(TEST_CAM, &cam_config);
}

void rgb565_img_get(int32_t w, int32_t h, color_t *p, int8_t *output)
{
    int r, g, b;
    int i, j;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            int index = i + w * j;
            uint16_t color = p[index];
            r = ((color & 0xF800) >> 11) * 8;
            g = ((color & 0x07E0) >> 5) * 4;
            b = ((color & 0x001F) >> 0) * 8;
            output[index * 3] = (int8_t) (r - 128);
            output[index * 3 + 1] = (int8_t) (g - 128);
            output[index * 3 + 2] = (int8_t) (b - 128);
        }
    }
}

void camera_img_resize(int32_t w, int32_t h, color_t *p, color_t *output)
{
    int i, j;
    int index;
    index = 0;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            int index_1 = 5 * i + (int)((float)j * 3.75) * IMAGE_WIDTH;
            output[index] = p[index_1];
            index++;
        }
    }
}

void rgb565_camera_img_get(int32_t w, int32_t h, color_t *p, int8_t *output)
{
    int r, g, b;
    int i, j;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            int index = i + w * j;
            uint16_t color = p[index];
            r = ((color & 0xF800) >> 11) * 8;
            g = ((color & 0x07E0) >> 5) * 4;
            b = ((color & 0x001F) >> 0) * 8;
            output[index * 3] = (int8_t) (r - 128);
            output[index * 3 + 1] = (int8_t) (g - 128);
            output[index * 3 + 2] = (int8_t) (b - 128);
        }
    }
}

void image_draw_square(uint32_t cnt, det_box *box)
{
    uint32_t bcnt;

    for (bcnt = 0; bcnt < cnt; bcnt++) {
        uint16_t color = 2016;

        int y1 = MIN((int)(box[bcnt].y1 * 3.75), IMAGE_INFER_H - 1);
        int y0 = MAX((int)(box[bcnt].y0 * 3.75), 0);
        int x1 = MIN((int)(box[bcnt].x1 * 5), IMAGE_INFER_W - 1);
        int x0 = MAX((int)(box[bcnt].x0 * 5), 0);
        printf("(%d, %d, %d, %d)\n", x0, x1, y0, y1);
        int i;
        for (i = x0; i < x1; i++) {
            IMAGE_PUT_RGB565_PIXEL(buffer_inf, i, y0, color);
        }
        for (i = x0; i < x1; i++) {
            IMAGE_PUT_RGB565_PIXEL(buffer_inf, i, y1, color);
        }
        for (i = y0; i < y1; i++) {
            IMAGE_PUT_RGB565_PIXEL(buffer_inf, x0, i, color);
        }
        for (i = y0; i < y1; i++) {
            IMAGE_PUT_RGB565_PIXEL(buffer_inf, x1, i, color);
        }
    }
}

int main(void)
{
    float tf_run_inference;

    board_init();
    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_lcd();
    board_init_cam_pins();
    printf("person object example\r\n");
    init_camera_device();
    init_lcd();

    init_cam();
    cam_start(TEST_CAM);

    lcdc_turn_on_display(LCD);

    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            buffer_lcd[y*IMAGE_WIDTH+x] = 0;
            buffer_inf[y*IMAGE_WIDTH+x] = 0;
        }
    }

    while (1) {
        start_time();
        camera_img_resize(160, 128, buffer_cam, img_ai);
        rgb565_img_get(160, 128, img_ai, getInput());
        for (int x = 0; x < (IMAGE_HEIGHT * IMAGE_WIDTH); x++) {
            buffer_inf[x] = buffer_cam[x];
        }
        invoke(NULL);
        run_times = get_end_time();
        det_post_procesing(cnt, boxes, 0.15);
        printf("Get: class 0: %d\n", cnt[0]);
        image_draw_square(cnt[0], boxes[0]);
        for (int x = 0; x < (IMAGE_HEIGHT * IMAGE_WIDTH); x++) {
            buffer_lcd[x] = buffer_inf[x];
        }
        tf_run_inference = (float)clock_get_frequency(clock_cpu0) / run_times;
        printf("fps:%f.\r\n", tf_run_inference);
    }
}
