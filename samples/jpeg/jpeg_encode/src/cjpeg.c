/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "board.h"
#include "hpm_cam_drv.h"
#include "hpm_camera.h"
#include "jpeglib.h"
#include "file_op.h"
#include "hpm_jpeg_drv.h"
/*---------------------------------------------------------------------*
 * Macro Definitions
 *---------------------------------------------------------------------
 */
#if defined  SD_FATFS_MODE
    #include "sd_fatfs.h"
#elif defined TINYUSB_HOST_MODE
    #include "msc_app.h"
#endif

/*Stored JPG data buff length*/
#define FILELENGTH 102400
/*rgb565 data buff length*/
#define RGBBUFFLENGTH 1253376
/*Get camera resolution*/
#define IMAGE_WIDTH      800
#define IMAGE_HEIGHT     480
#define IMAGE_COMPONENT  3

/*jpeg related*/
#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif

/*cam related*/
#define CAM_I2C BOARD_CAM_I2C_BASE
#define CAMERA_INTERFACE camera_interface_dvp
#define PIXEL_FORMAT display_pixel_format_rgb565
#ifndef TEST_CAM
#define TEST_CAM HPM_CAM0
#endif
/*libjpeg related*/
#define UPALIGN4(n) (((n) + 3) & ~3)
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
/*JPG file data size*/
int32_t jpg_size = 0;
/*JPG file data buff*/
uint8_t filebuff[FILELENGTH];

/*rgb565 data buff*/
__attribute__((section(".framebuffer"))) uint8_t rgb565buff[RGBBUFFLENGTH] = {0};

/*JPG file data header length*/
#define JPGHEADERLEN 623

/*rgb888 data buff*/
uint8_t convertbuff[RGBBUFFLENGTH] = {0};

/*---------------------------------------------------------------------*
 *JPG file data header data
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
*/
const uint8_t in_ecs[] = {
#include "jpg_header.cdat"
};

/*---------------------------------------------------------------------*
 *JPG file huffmin table
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
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
/*---------------------------------------------------------------------*
 *JPG file huffbase table
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
*/
const uint16_t huffbase[64]={
#include "base.cdat"
};
/*---------------------------------------------------------------------*
 *JPG file huffsymb table
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
*/
const uint8_t huffsymb[336]={
#include "symb.cdat"
};
/*---------------------------------------------------------------------*
 *JPG file huffenc table
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
*/
const uint16_t huffenc[384]={
#include "htable.cdat"
};
/*---------------------------------------------------------------------*
 *JPG file qetable table
 *the data is extracted from the reference_jpeg_from_design.jpg, it is provided by design as reference
 *---------------------------------------------------------------------*
*/
const uint16_t qetable[256]={
#include "qetable.cdat"
};

/*---------------------------------------------------------------------*
 * Camera device parameter initialization
 *---------------------------------------------------------------------
 */
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
        while (1) {
        };
    }
}

/*---------------------------------------------------------------------*
 * Camera output data parameter initialization
 *---------------------------------------------------------------------
 */
void init_cam(uint8_t *rgbbuff)
{
    cam_config_t cam_config;

    cam_get_default_config(TEST_CAM, &cam_config ,PIXEL_FORMAT);
    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = true;
    cam_config.buffer1 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rgbbuff);
    cam_config.color_format = CAM_COLOR_FORMAT_RGB565;
    cam_init(TEST_CAM, &cam_config);
}

/*---------------------------------------------------------------------*
 * -Convert rgb565 data to rgb888 data
 *---------------------------------------------------------------------
 */
void jpeg_rgb565_to_rgb888(uint8_t *psrc, int32_t width, int32_t height, uint8_t *pdst, int32_t *bsize)
{
    int32_t srclinesize = UPALIGN4(width * 2);
    int32_t dstlinesize = UPALIGN4(width * 3);

    uint8_t *psrcline;
    const uint16_t *psrcdot;
    uint8_t *pdstline;
    uint8_t *pdstdot;
    int32_t i =0;
    int32_t j =0;

    psrcline = psrc;
    pdstline = pdst;
    for (i = 0; i < height; i++) {
        psrcdot = (const uint16_t *)psrcline;
        pdstdot = pdstline;
        for (j = 0; j < width; j++) {
            /*RGB16 bit data is combined into 24 bit data*/
            *pdstdot++ = (uint8_t)(((*psrcdot) >> 0) << 3);
            *pdstdot++ = (uint8_t)(((*psrcdot) >> 5) << 2);
            *pdstdot++ = (uint8_t)(((*psrcdot) >> 11) << 3);
            psrcdot++;
        }
        psrcline += srclinesize;
        pdstline += dstlinesize;
    }
    *bsize = i * j * 3;
}

/*---------------------------------------------------------------------*
 * Convert rgb888 data to bgr888 data
 *---------------------------------------------------------------------
 */
void jpeg_rgb888_to_bgr888(uint8_t *bgr, int32_t *bsize, int32_t bit)
{
    int32_t i = 0;
    int32_t fsize = (*bsize);

    /*Blue data bytes and red data bytes are exchanged*/
    for (i = 0; i < fsize; i += bit / 8) {
        uint8_t b = bgr[i];
        bgr[i] = bgr[i + 2];
        bgr[i + 2] = b;
    }
}

/*---------------------------------------------------------------------*
 *libjpeg-turbo   Convert bgr888 data to jpg data
 *---------------------------------------------------------------------
 */
void jpeg_bgr888_to_jpgmem(uint8_t *src, int32_t width, int32_t height, int32_t depth, uint8_t **dst, int32_t *dstLen)
{
    unsigned long act_len = 0;

    struct jpeg_compress_struct jcs;
    struct jpeg_error_mgr jem;

    /* Initialize the JPEG compression object with default error handling. */
    jcs.err = jpeg_std_error(&jem);
    jpeg_create_compress(&jcs);

    /* Specify data source for compression */
    jpeg_mem_dest(&jcs, dst, &act_len);

    /* set default compression parameters */
    jcs.image_width = width;
    jcs.image_height = height;
    jcs.input_components = depth;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    jpeg_set_quality(&jcs, 80, true);

    /* Start compressor */
    jpeg_start_compress(&jcs, TRUE);
    JSAMPROW row_pointer[1];
    int32_t row_stride = jcs.image_width * jcs.num_components;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &src[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    /* Finish compression*/
    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    *dstLen = act_len;
}

/*---------------------------------------------------------------------*
 *Libjpeg conversion to convert rgb565 data into JPG data
 *---------------------------------------------------------------------
 */
void jpeg_convert_data(uint8_t *rgbbuff, int32_t datawidth, int32_t dataheight, int32_t *datasize, uint8_t *filesbuff)
{
    /*Convert rgb565 data to rgb888 data*/
    jpeg_rgb565_to_rgb888(rgbbuff, datawidth, dataheight, convertbuff, datasize);
    printf("jpeg_rgb565_to_rgb888 --> datasize= %d   \n",*datasize);
    /*Convert rgb888 data to bgr888 data*/
    jpeg_rgb888_to_bgr888(convertbuff, datasize, 24);
    printf("jpeg_rgb888_to_bgr888 --> datasize= %d   \n",*datasize);
    uint8_t *jpgbuff = (uint8_t *)malloc(FILELENGTH);
    /*Convert bgr888 data to jpg data*/
    jpeg_bgr888_to_jpgmem(convertbuff, datawidth, dataheight, IMAGE_COMPONENT, &jpgbuff, datasize);
    printf("jpeg_bgr888_to_jpgmem --> datasize= %d   \n",*datasize);
    /*Store data to buff*/
    memcpy(filesbuff, &jpgbuff[0], (*datasize));
    /*Free memory*/
    free(jpgbuff);
}

/*---------------------------------------------------------------------*
 *jpeg hardware file encode table
 *---------------------------------------------------------------------
 */
void fill_jpeg_encode_table(void)
{
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qetable, ARRAY_SIZE(qetable));
    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 *jpeg hardware wait finish
 *---------------------------------------------------------------------
 */
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
    } while(1);
}

/*---------------------------------------------------------------------*
 *jpeg hardware convert
 *---------------------------------------------------------------------
 */
void jpeg_encode(uint8_t *rgbbuff, int32_t width, int32_t height, int32_t *dasize, uint8_t *filesbuff)
{

    jpeg_job_config_t config = {0};

    /*JPEG default parameter table settings*/
    jpeg_init(TEST_JPEG);
    fill_jpeg_encode_table();

    /*jpeg encode parameter configuration*/
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.byte_order = JPEG_BYTE_ORDER_3210;
    config.enable_csc = true;
    config.enable_ycbcr = true;
    config.width_in_pixel = width;
    config.height_in_pixel = height;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rgbbuff);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)(filesbuff + JPGHEADERLEN));

    /* Start compressor */
    if (status_success != jpeg_start_encode(TEST_JPEG, &config)) {
        printf("failed to endcode\n");
        while (1) {
        };
    }
    if (!wait_jpeg_finish()) {
        printf("encoding failed\n");
        while (1) {
        };
    }
    *dasize = jpeg_get_encoded_length(TEST_JPEG);
    printf("complete encoding length %d bytes\n", *dasize);
}

/*---------------------------------------------------------------------*
 *Hardware-jpeg-init
 *---------------------------------------------------------------------
 */
void init_jpeg(void)
{
    jpeg_enable(TEST_JPEG);
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)huffmin, ARRAY_SIZE(huffmin));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)huffbase, ARRAY_SIZE(huffbase));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)huffsymb, ARRAY_SIZE(huffsymb));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)huffenc, ARRAY_SIZE(huffenc));
    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 *jpeg file build
 *---------------------------------------------------------------------
 */
void jpeg_build_file(int32_t *dbsize, uint8_t *filesbuff)
{
    memcpy(filesbuff, &in_ecs[0], JPGHEADERLEN);
    *dbsize = *dbsize + JPGHEADERLEN;
}

/*---------------------------------------------------------------------*
 *Hardware-jpeg   Hardware to convert rgb565 data into JPG data
 *---------------------------------------------------------------------
 */
void jpeg_convert_hw(uint8_t *rgbbuff, int32_t datawidth, int32_t dataheight, int32_t *datasize, uint8_t *filesbuff)
{
    init_jpeg();
    /*jpeg hardware convert*/
    jpeg_encode(rgbbuff, datawidth, dataheight, datasize, filesbuff);
    /*jpeg file build*/
    jpeg_build_file(datasize, filesbuff);
}

/*---------------------------------------------------------------------*
 * Storage device initialization
 *---------------------------------------------------------------------
 */
void store_device_init(void)
{
#if defined SD_FATFS_MODE
    /*storage picture data by SD card*/
    printf("storage picture data by SD card\n");        
    sdfatfs_task();
#elif defined TINYUSB_HOST_MODE
    /*storage picture data by tinyusb mode*/
    printf("storage picture data by tinyusb mode\n");
    tinyusb_task();
#endif  
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------
 */
int main(void)
{
    /* System initialization */
    board_init();

    /*Camera module initialization*/
    board_init_cam_clock(TEST_CAM);
    board_init_i2c(CAM_I2C);
    board_init_cam_pins();

    /*Storage device initialization*/
    store_device_init();

    /*Camera parameter initialization*/
    init_camera_device();
    init_cam(rgb565buff); 
    /*Camera module enable*/
    cam_start(TEST_CAM);
    board_delay_ms(2000);

#if defined JPEG_TURBO_MODE
    /*Libjpeg conversion to convert rgb565 data into JPG data*/
    jpeg_convert_data(rgb565buff, IMAGE_WIDTH, IMAGE_HEIGHT, &jpg_size, filebuff);
    printf("jpeg_convert_data --> jpg_size= %d   \n",jpg_size);
#elif defined JPEG_HARDWARE_MODE
    /*Hardware to convert rgb565 data into JPG data*/
    jpeg_convert_hw(rgb565buff, IMAGE_WIDTH, IMAGE_HEIGHT, &jpg_size, filebuff);
    printf("jpeg_convert_hw --> jpg_size= %d   \n",jpg_size);
#endif

    /* Store camera pictures */
    file_store(filebuff, &jpg_size, "Camera.jpg");

    printf("Libjpeg-turbo encode completed\n");
    while (1) {
    };
    return 0;
}
