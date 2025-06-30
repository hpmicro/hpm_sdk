/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"
#include "hpm_jpeg_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_clock_drv.h"
#include "file_op.h"

#ifndef JPEG_HW_MODE
#define JPEG_HW_MODE 1
#endif

#if !JPEG_HW_MODE
#include "jpeglib.h"
#include "format_convert.h"
#define JPEG_SW_LIBJPEG_TURBO 1
#endif

#ifdef JPEG_USE_SDCARD
#include "sd_fatfs.h"
#elif defined JPEG_USE_UDISK
#include "msc_app.h"
#endif

/* image resolution */
#define IMAGE_WIDTH      640
#define IMAGE_HEIGHT     480

/* jpeg related */
#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif

/* cam related */
#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif
#define CAM_I2C BOARD_CAM_I2C_BASE
#define CAMERA_INTERFACE camera_interface_dvp

#define LCD BOARD_LCD_BASE
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define LCD_LAYER_INDEX 0
#define LCD_LAYER_DONE_MASK (LCD_LAYER_INDEX + 1)

#define FILE_NAME_MAX 256
#define JPEG_COMP_QUAL 80

/* Supported RGB565 and Y8 (grayscale) */
#ifndef JPEG_USE_GRAYSCALE
#define JPEG_USE_GRAYSCALE 0
#endif
#if JPEG_USE_GRAYSCALE
#define PIXEL_FORMAT display_pixel_format_y8
#else
#define PIXEL_FORMAT display_pixel_format_rgb565
#endif

#define CAM_BUF_LEN HPM_L1C_CACHELINE_ALIGN_UP(IMAGE_HEIGHT * IMAGE_WIDTH * 3)

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint8_t file_buffer[CAM_BUF_LEN];
ATTR_PLACE_AT_NONCACHEABLE uint8_t decoding_buffer[CAM_BUF_LEN];
ATTR_PLACE_AT_WITH_ALIGNMENT(".framebuffer", HPM_L1C_CACHELINE_SIZE) uint8_t buffers[2][CAM_BUF_LEN];
ATTR_PLACE_AT(".framebuffer") uint8_t csc_buffer[CAM_BUF_LEN];

static volatile bool vsync = false;
#if JPEG_HW_MODE
static uint8_t jpeg_header[] = {
#if JPEG_USE_GRAYSCALE
#include "jpg_header_grayscale.cdat"
#else
#include "jpg_header_rgb.cdat"
#endif
};
#endif

/*
 * huffmin table
 */
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

/*
 * huffbase table
 */
const uint16_t huffbase[64] = {
#include "base.cdat"
};

/*
 * huffsymb table
 */
const uint8_t huffsymb[336] = {
#include "symb.cdat"
};

/*
 * huffenc table
 */
const uint16_t huffenc[384] = {
#include "htable.cdat"
};

const uint16_t qetable[256] = {
#include "qetable.cdat"
};

const uint16_t qdtable[256] = {
#include "qdtable.cdat"
};

/*
 * sensor configuration
 *
 */
void init_camera_device(void)
{
    camera_context_t camera_context = {0};
    camera_config_t camera_config = {0};

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

#if !JPEG_HW_MODE
uint32_t libjpeg_compress_image(uint8_t *src, uint32_t width, uint32_t height, uint8_t format, uint8_t *dst, uint32_t buffer_size)
{
    uint32_t act_len = 0;
    uint32_t row_stride;
    uint32_t components;
    uint8_t *tmp = 0;
    J_COLOR_SPACE color_space;

    struct jpeg_compress_struct jcs = {0};
    struct jpeg_error_mgr jem = {0};
    JSAMPROW row_pointer;

    /* color space coversion */
    if (format == display_pixel_format_rgb565) {
        tmp = csc_buffer;
        convert_rgb565_to_rgb888(src, width, height, tmp);
        color_space = JCS_RGB;
        components = 3;
    } else if (format == display_pixel_format_y8) {
        tmp = src;
        color_space = JCS_GRAYSCALE;
        components = 1;
    } else {
        printf("Unsupported color space\n");
        while (1) {
        }
    }

    act_len = buffer_size;
    /* initialize the JPEG compression object with default error handling. */
    jcs.err = jpeg_std_error(&jem);
    jpeg_create_compress(&jcs);

    jpeg_mem_dest(&jcs, &dst, (unsigned long int *)&act_len);

    /* set default compression parameters */
    jcs.image_width = width;
    jcs.image_height = height;
    jcs.input_components = components;
    jcs.in_color_space = color_space;
    jpeg_set_defaults(&jcs);
    jpeg_set_quality(&jcs, JPEG_COMP_QUAL, true);

    jpeg_start_compress(&jcs, true);
    row_stride = jcs.image_width * jcs.num_components;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer = &tmp[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, &row_pointer, 1);
    }

    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    return act_len;
}

void jpeg_sw_decode(const uint8_t *src_buf, uint32_t width, uint32_t height, uint32_t jpg_size, uint8_t *buffer)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer;

    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* Specify data source for decompression */
    jpeg_mem_src(&cinfo, src_buf, jpg_size);
    /* Read file header, set default decompression parameters */
    jpeg_read_header(&cinfo, TRUE);

    cinfo.scale_num = 1;
    cinfo.scale_denom = 1;

    /* Start decompressor */
    jpeg_start_decompress(&cinfo);

    /* Process data */
    while (cinfo.output_scanline < cinfo.output_height) {
#if JPEG_USE_GRAYSCALE
        row_pointer = &buffer[cinfo.output_scanline * cinfo.output_width * cinfo.num_components];
#else
        row_pointer = &csc_buffer[cinfo.output_scanline * cinfo.output_width * cinfo.num_components];
#endif
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
    }
    /* Finish decompression*/
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

#if !JPEG_USE_GRAYSCALE
    convert_rgb888_to_rgb565(csc_buffer, width, height, buffer);
#endif
}

uint32_t jpeg_sw_encode(uint8_t *src, uint32_t width, uint32_t height, uint8_t format, uint8_t *file_buf, uint32_t buffer_size)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_invalidate((uint32_t)src, CAM_BUF_LEN);
    }

#ifdef JPEG_SW_LIBJPEG_TURBO
    return libjpeg_compress_image(src, width, height, format, file_buf, buffer_size);
#endif
    /* unsupported SW mode */
    return 0;
}
#else

/*
 * JPEG configuration
 */
void reset_jpeg(bool encoding)
{
    jpeg_init(TEST_JPEG);
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)huffmin, ARRAY_SIZE(huffmin));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)huffbase, ARRAY_SIZE(huffbase));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)huffsymb, ARRAY_SIZE(huffsymb));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)huffenc, ARRAY_SIZE(huffenc));
    if (encoding) {
        jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qetable, ARRAY_SIZE(qetable));
    } else {
        jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qdtable, ARRAY_SIZE(qdtable));
    }
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
    } while (1);
}

void jpeg_hw_decode(uint8_t *src_buf, uint32_t width, uint32_t height, uint32_t jpg_size, uint8_t format, uint8_t *buffer)
{
    jpeg_job_config_t config = {0};
    reset_jpeg(false);

    if (format == display_pixel_format_y8) {
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_400;
        config.in_pixel_format = jpeg_pixel_format_y8;
        config.out_pixel_format = jpeg_pixel_format_y8;
        config.enable_ycbcr = false;
    } else if (format == display_pixel_format_rgb565) {
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
        config.in_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.out_pixel_format = jpeg_pixel_format_rgb565;
        config.enable_ycbcr = true;
        config.out_byte_order = JPEG_BYTE_ORDER_2301;
    } else {
        printf("unsupported pixel format 0x%x\n", format);
        while (1) {
        }
    }

    config.width_in_pixel = width;
    config.height_in_pixel = height;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) src_buf);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) buffer);

    if (status_success != jpeg_start_decode(TEST_JPEG, &config, jpg_size)) {
        printf("failed to decode\n");
        while (1) {
        }
    }

    if (!wait_jpeg_finish()) {
        printf("decoding failed\n");
    }
}

uint32_t jpeg_hw_encode(uint8_t *src_buf, uint32_t width, uint32_t height, uint8_t format, uint8_t *buffer)
{
    uint32_t jpg_size;
    jpeg_job_config_t config = {0};

    reset_jpeg(true);

    /* jpeg encode parameter configuration */
    if (format == display_pixel_format_y8) {
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_400;
        config.in_pixel_format = jpeg_pixel_format_y8;
        config.out_pixel_format = jpeg_pixel_format_y8;
        config.enable_ycbcr = false;
    } else if (format == display_pixel_format_rgb565) {
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
        config.in_pixel_format = jpeg_pixel_format_rgb565;
        config.out_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.enable_ycbcr = true;
    } else {
        printf("unsupported pixel format 0x%x\n", format);
        while (1) {
        }
    }
    config.width_in_pixel = width;
    config.height_in_pixel = height;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) src_buf);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) buffer);

    if (status_success != jpeg_start_encode(TEST_JPEG, &config)) {
        printf("failed to endcode\n");
        return 0;
    }
    if (!wait_jpeg_finish()) {
        printf("encoding failed\n");
        return 0;
    }
    jpg_size = jpeg_get_encoded_length(TEST_JPEG);
    printf("complete encoding length %d bytes\n", jpg_size);

    return jpg_size;
}

void jpeg_header_update_resolution(uint8_t *header, uint32_t header_size, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < header_size; i++) {
        /* find SOF0 */
        if ((header[i] == 0xFF) && (header[i+1] == 0xC0)) {
            /* frame resolution offset 5 bytes after SOF0 signature */
            header[i+5] = height >> 8;
            header[i+6] = height & 0xFF;
            header[i+7] = width >> 8;
            header[i+8] = width & 0xFF;
            break;
        }
    }
}

void jpeg_add_header(uint8_t *file_buf, uint32_t width, uint32_t height)
{
    jpeg_header_update_resolution(jpeg_header, sizeof(jpeg_header), width, height);
    memcpy(file_buf, &jpeg_header[0], sizeof(jpeg_header));
}
#endif

/*
 * GPIO button setup
 */
void init_gpio_button(void)
{
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN);
}

bool gpio_button_is_pressed(void)
{
    return !gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

uint32_t jpeg_encode(uint8_t *src_buf, uint32_t width, uint32_t height, uint8_t *file_buf, uint32_t buffer_size)
{
    uint32_t size = 0;
#if !JPEG_HW_MODE
    size = jpeg_sw_encode(src_buf, width, height, PIXEL_FORMAT, file_buf, buffer_size);
    jpeg_sw_decode(file_buf, width, height, size, decoding_buffer);
#else
    (void)buffer_size;
    jpeg_add_header(file_buf, width, height);
    if (l1c_dc_is_enabled()) {
        l1c_dc_flush((uint32_t)file_buf, HPM_L1C_CACHELINE_ALIGN_UP(buffer_size));
    }

    size = jpeg_hw_encode(src_buf, width, height, PIXEL_FORMAT, file_buf + sizeof(jpeg_header));
    jpeg_hw_decode(file_buf + sizeof(jpeg_header), width, height, size, PIXEL_FORMAT, decoding_buffer);
    size += sizeof(jpeg_header);

#endif
    return size;
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
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

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
    uint32_t i = 0, jpg_size;
    uint8_t *tmp;
    int retry;
    char fname[FILE_NAME_MAX];

    uint8_t *front_buffer, *back_buffer;
    front_buffer = buffers[0];
    back_buffer = buffers[1];

    board_init();

#if defined(JPEG_HW_MODE) && JPEG_HW_MODE
    clock_add_to_group(clock_jpeg, 0);
#endif

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
    printf("JPEG %s mode\n", JPEG_HW_MODE ? "hardware" : "software");
    while (1) {
        wait_for_button_press();
        printf("preview is captured\n");

        cam_stop_safely(TEST_CAM);
        tmp = front_buffer;
        front_buffer = back_buffer;
        back_buffer = tmp;
        cam_update_buffer(TEST_CAM, (uint32_t)front_buffer);

        /* encoding captured image */
        jpg_size = jpeg_encode(back_buffer, IMAGE_WIDTH, IMAGE_HEIGHT, file_buffer, sizeof(file_buffer));

        update_lcd_buffer((uint32_t)decoding_buffer);

        snprintf((char *)fname, sizeof(fname), "cam_jpg_%s_encode_%02d.jpg", JPEG_HW_MODE ? "hw" : "sw", i);

        retry = 3;
        while (retry--) {
            if (check_disk() && file_store(fname, file_buffer, jpg_size)) {
                printf("captured image encoded to %s\n", fname);
                i++;
                break;
            }
        }

        wait_for_button_press();
        printf("preview is resumed\n");
        /* resume sensor preview */
        cam_start(TEST_CAM);
        update_lcd_buffer((uint32_t)front_buffer);
    }
    return 0;
}
