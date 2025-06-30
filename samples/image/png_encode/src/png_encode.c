/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"
#include "hpm_lcdc_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_l1c_drv.h"
#include "file_op.h"
#include "lodepng.h"
#include "format_convert.h"

#ifdef PNG_USE_SDCARD
#include "sd_fatfs.h"
#elif defined PNG_USE_UDISK
#include "msc_app.h"
#endif

/* image resolution */
#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480

/* cam related */
#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif
#define CAM_I2C          BOARD_CAM_I2C_BASE
#define CAMERA_INTERFACE camera_interface_dvp

#define LCD                 BOARD_LCD_BASE
#define LCD_WIDTH           BOARD_LCD_WIDTH
#define LCD_HEIGHT          BOARD_LCD_HEIGHT
#define LCD_LAYER_INDEX     0
#define LCD_LAYER_DONE_MASK (LCD_LAYER_INDEX + 1)

#define FILE_NAME_MAX 256

/* Supported RGB565 and Y8 (grayscale) */
#ifndef PNG_USE_GRAYSCALE
#define PNG_USE_GRAYSCALE 0
#endif
#if PNG_USE_GRAYSCALE
#define PIXEL_FORMAT display_pixel_format_y8
#else
#define PIXEL_FORMAT display_pixel_format_rgb565
#endif

#define CAM_BUF_LEN HPM_L1C_CACHELINE_ALIGN_UP(IMAGE_HEIGHT * IMAGE_WIDTH * 3)

ATTR_PLACE_AT_WITH_ALIGNMENT(".framebuffer", HPM_L1C_CACHELINE_SIZE) uint8_t buffers[2][CAM_BUF_LEN];
ATTR_PLACE_AT(".framebuffer") uint8_t convert_buffer[CAM_BUF_LEN];

static volatile bool vsync = false;
static uint8_t *image_out_ptr;

/*
 * sensor configuration
 *
 */
void init_camera_device(void)
{
    camera_context_t camera_context = { 0 };
    camera_config_t camera_config = { 0 };

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
    camera_config.interface = CAMERA_INTERFACE;
    camera_config.pixel_format = PIXEL_FORMAT;

    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while (1) {
        };
    }
}

/*
 *  configure CAM
 */
void init_cam(uint8_t *buffer)
{
    cam_config_t cam_config;

    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);

    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = true;
    cam_config.buffer1 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)buffer);
    cam_config.buffer2 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)buffer);

    if (PIXEL_FORMAT == display_pixel_format_rgb565) {
        cam_config.color_format = CAM_COLOR_FORMAT_RGB565;
    } else if (PIXEL_FORMAT == display_pixel_format_y8) {
        cam_config.color_format = CAM_COLOR_FORMAT_YCBCR422_YUV422;
        cam_config.data_store_mode = CAM_DATA_STORE_MODE_Y_ONLY;
    }
    cam_init(TEST_CAM, &cam_config);
}

uint32_t image_encode(uint8_t *src_buf, uint32_t width, uint32_t height)
{
    uint32_t file_size;
    uint32_t error;

    if (l1c_dc_is_enabled()) {
        l1c_dc_invalidate((uint32_t)src_buf, CAM_BUF_LEN);
    }

    /* color space coversion */
    if (PIXEL_FORMAT == display_pixel_format_rgb565) {
        convert_rgb565_to_bgr888(src_buf, width, height, convert_buffer);
        error = lodepng_encode24(&image_out_ptr, (size_t *)&file_size, convert_buffer, width, height);
    } else if (PIXEL_FORMAT == display_pixel_format_y8) {
        error = lodepng_encode_memory(&image_out_ptr, (size_t *)&file_size, src_buf, width, height, LCT_GREY, 8);
    } else {
        printf("Unsupported color space\n");
        while (1) {
        }
    }

    if (error != 0) {
        printf("sw encode failed, error = %d: %s\n", error, lodepng_error_text(error));
        file_size = 0;
    }

    return file_size;
}

/*
 * GPIO button setup
 */
void init_gpio_button(void)
{
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

bool gpio_button_is_pressed(void)
{
    return !gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

/*
 * LCD configuration
 */
SDK_DECLARE_EXT_ISR_M(BOARD_LCD_IRQ, isr_lcd_d0)
void isr_lcd_d0(void)
{
    volatile uint32_t s = lcdc_get_dma_status(LCD);
    lcdc_clear_dma_status(LCD, s);
    if ((s & (LCD_LAYER_DONE_MASK << LCDC_DMA_ST_DMA0_DONE_SHIFT))) {
        vsync = true;
    }
}

void init_lcd(uint32_t buffer)
{
    lcdc_config_t config = { 0 };
    lcdc_layer_config_t layer = { 0 };

    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    lcdc_init(LCD, &config);

    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT, LCD_LAYER_INDEX);

    layer.position_x = (LCD_WIDTH - IMAGE_WIDTH) / 2;
    layer.position_y = (LCD_HEIGHT - IMAGE_HEIGHT) / 2;
    layer.width = IMAGE_WIDTH;
    layer.height = IMAGE_HEIGHT;

    layer.buffer = core_local_mem_to_sys_address(HPM_CORE0, buffer);
    layer.alphablend.src_alpha = 0xFF; /* src */
    layer.alphablend.dst_alpha = 0xF0; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_override;
    layer.alphablend.dst_alpha_op = display_alpha_op_override;
    layer.background.u = 0xFFFF0000;
    layer.alphablend.mode = display_alphablend_mode_src_over;

    if (status_success != lcdc_config_layer(LCD, LCD_LAYER_INDEX, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        }
    }

    lcdc_enable_interrupt(LCD, LCD_LAYER_DONE_MASK << 16);
    intc_m_enable_irq_with_priority(BOARD_LCD_IRQ, 1);
    lcdc_turn_on_display(LCD);
}

static uint64_t clock_cpu_get_us(void)
{
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t result;
    uint32_t resultl_first = read_csr(CSR_MCYCLE);
    uint32_t resulth = read_csr(CSR_MCYCLEH);
    uint32_t resultl_second = read_csr(CSR_MCYCLE);

    if (resultl_first < resultl_second) {
        result = ((uint64_t)resulth << 32) | resultl_first;
    } else {
        resulth = read_csr(CSR_MCYCLEH);
        result = ((uint64_t)resulth << 32) | resultl_second;
    }

    return result / ticks_per_us;
}

uint32_t clock_cpu_elapsed_us(uint64_t start)
{
    return clock_cpu_get_us() - start;
}

static void wait_for_button_press(void)
{
    const uint32_t filter_us = 10000;
    uint64_t start;

    /*
     * wait for pressed
     */
    while (!gpio_button_is_pressed()) {
    }

    /*
     * debounce for pressed
     */
    start = clock_cpu_get_us();
    while (clock_cpu_elapsed_us(start) < filter_us) {
        if (!gpio_button_is_pressed())
            start = clock_cpu_get_us();
    }

    /*
     * wait for released
     */
    while (gpio_button_is_pressed()) {
    }

    /*
     * debounce for released
     */
    start = clock_cpu_get_us();
    while (clock_cpu_elapsed_us(start) < filter_us) {
        if (gpio_button_is_pressed())
            start = clock_cpu_get_us();
    }
}

static void update_lcd_buffer(uint32_t buffer)
{
    while (!vsync) {
    }
    lcdc_layer_set_next_buffer(LCD, LCD_LAYER_INDEX, buffer);
    vsync = false;
}

int main(void)
{
    uint32_t i = 0;
    uint32_t image_size;
    uint8_t *tmp;
    int retry;
    char fname[FILE_NAME_MAX];

    uint8_t *front_buffer, *back_buffer;
    front_buffer = buffers[0];
    back_buffer = buffers[1];

    board_init();
    board_init_lcd();
    board_lcd_backlight(false);
    board_init_gpio_pins();
    init_gpio_button();

    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_cam_pins();

    init_camera_device();
    init_cam(front_buffer);
    cam_start(TEST_CAM);
    board_delay_ms(100);
    init_lcd((uint32_t)front_buffer);
    board_lcd_backlight(true);
    init_disk();
    while (!check_disk()) {
        ;
    }

    while (1) {
        wait_for_button_press();
        printf("preview is captured\n");

        cam_stop_safely(TEST_CAM);
        tmp = front_buffer;
        front_buffer = back_buffer;
        back_buffer = tmp;
        cam_update_buffer(TEST_CAM, (uint32_t)front_buffer);

        /* show image */
        update_lcd_buffer((uint32_t)back_buffer);

        /* encoding captured image */
        image_size = image_encode(back_buffer, IMAGE_WIDTH, IMAGE_HEIGHT);
        if (image_size == 0) {
            return -1;
        }

        snprintf((char *)fname, sizeof(fname), "cam_png_encode_%02d.png", i);
        retry = 3;
        while (retry--) {
            if (check_disk() && file_store(fname, image_out_ptr, image_size)) {
                printf("captured image encoded to %s\n", fname);
                i++;
                break;
            }
        }

        free(image_out_ptr);

        wait_for_button_press();
        printf("preview is resumed\n");
        /* resume sensor preview */
        cam_start(TEST_CAM);
        update_lcd_buffer((uint32_t)front_buffer);
    }
    return 0;
}
