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
/*---------------------------------------------------------------------*
 * Macro Definitions
 *---------------------------------------------------------------------
 */
#if defined  SD_FATFS_MODE
    #include "sd_fatfs.h"
#elif defined TINYUSB_HOST_MODE
    #include "msc_app.h"
#endif

/*jpg-file data buff length*/
#define FILEBUFFLEN 10000
/*rgb565 data buff length*/
#define RGBBUFFLEN 100000
/*Get camera resolution*/
#define IMAGE_WIDTH      227
#define IMAGE_HEIGHT     149
#define IMAGE_COMPONENT  3

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
uint8_t filebuff[FILEBUFFLEN];
/*rgb565 data buff*/
__attribute__((section(".framebuffer"))) uint8_t rgb565buff[RGBBUFFLEN] = {0};

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
void init_cam(void)
{
    cam_config_t cam_config;

    cam_get_default_config(TEST_CAM, &cam_config ,PIXEL_FORMAT);
    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = true;
    cam_config.buffer1 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rgb565buff);
    cam_config.color_format = PIXEL_FORMAT;
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
    for (i = 0; i < height; i++){
        psrcdot = (const uint16_t *)psrcline;
        pdstdot = pdstline;
        for (j = 0; j < width; j++){
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
    for (i = 0; i < fsize; i += bit / 8){
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
    while (jcs.next_scanline < jcs.image_height){
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
void jpeg_convert_data(int32_t datawidth, int32_t dataheight, int32_t *datasize)
{
    uint8_t convertbuff[RGBBUFFLEN *2] = {0};

    /*Convert rgb565 data to rgb888 data*/
    jpeg_rgb565_to_rgb888(rgb565buff, datawidth, dataheight, convertbuff, datasize);
    printf("jpeg_rgb565_to_rgb888 --> datasize= %d   \n",*datasize);
    /*Convert rgb888 data to bgr888 data*/
    jpeg_rgb888_to_bgr888(convertbuff, datasize, 24);
    printf("jpeg_rgb888_to_bgr888 --> datasize= %d   \n",*datasize);
    uint8_t *jpgbuff = (uint8_t *)malloc(FILEBUFFLEN);
    /*Convert bgr888 data to jpg data*/
    jpeg_bgr888_to_jpgmem(convertbuff, datawidth, dataheight, IMAGE_COMPONENT, &jpgbuff, datasize);
    printf("jpeg_bgr888_to_jpgmem --> datasize= %d   \n",*datasize);
    /*Store data to buff*/
    memcpy(filebuff, &jpgbuff[0], (*datasize));
    /*Free memory*/
    free(jpgbuff);
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
    /*Camera parameter initialization*/
    init_camera_device();
    init_cam(); 
    /*Camera module enable*/
    cam_start(TEST_CAM);
    board_delay_ms(2000);

    /*Libjpeg conversion to convert rgb565 data into JPG data*/
    jpeg_convert_data(IMAGE_WIDTH,IMAGE_HEIGHT,&jpg_size);
    printf("jpeg_convert_data --> jpg_size= %d   \n",jpg_size);

    /* Store camera pictures */
    do{
#if defined SD_FATFS_MODE
        /*storage picture data by SD card*/
        printf("storage picture data by SD card\n");        
        sdfatfs_task();
#elif defined TINYUSB_HOST_MODE
        /*storage picture data by tinyusb mode*/
        printf("storage picture data by tinyusb mode\n");
        tinyusb_task();
#endif        
    }while (jpg_size);
    printf("Libjpeg-turbo encode completed\n");
    while (1) {
    };
    return 0;
}
