/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "file_op.h"
/*--------------------------------------------------------------------*
 * Macro Definitions
 *---------------------------------------------------------------------
 */
#if defined  SD_FATFS_MODE
    #include "sd_fatfs.h"
#elif defined TINYUSB_HOST_MODE
    #include "msc_app.h"
#endif

#if defined JPEG_HARDWARE_MODE
    #include "jpeg_hw.h"
#elif defined JPEG_TURBO_MODE
    #include "jpeg_turbo.h"
#endif

/*LCD Definitions*/
#ifndef LCD
#define LCD BOARD_LCD_BASE
#endif

/*Pixel format of LCD display*/
#define PIXEL_FORMAT display_pixel_format_rgb565
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
/*rgb565 data buff*/
ATTR_PLACE_AT_WITH_ALIGNMENT(".framebuffer", HPM_L1C_CACHELINE_SIZE) uint8_t rgb565buff[RGBBUFFLEN] = {0};

/*JPG file data buff*/
uint8_t filebuff[FILEBUFFLEN];
/*---------------------------------------------------------------------*
 * Display picture data on LCD
 *---------------------------------------------------------------------
 */
void lcdc_display_picture(uint8_t *rgbbuff, int32_t rgb_width, int32_t rgb_heihgt)
{
    uint8_t layer_index = 0;
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    /*LCD default parameter settings*/
    lcdc_get_default_config(LCD, &config);
    config.resolution_x = BOARD_LCD_WIDTH;
    config.resolution_y = BOARD_LCD_HEIGHT;
    lcdc_init(LCD, &config);
    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT, layer_index);

    /*LCD layer parameter configuration*/
    layer.position_x = 0;
    layer.position_y = 0;
    layer.pixel_format = PIXEL_FORMAT;
    layer.width = rgb_width;
    layer.height = rgb_heihgt;
    layer.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rgbbuff);
    layer.alphablend.src_alpha = 0xFF;/* src */
    layer.alphablend.dst_alpha = 0xFF;/* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.background.u = 0;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    layer.yuv = 0;
    layer.csc_config.enable = false;
    if (status_success != lcdc_config_layer(LCD, layer_index, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        };
    }

    /*LCD display function enable*/
    lcdc_turn_on_display(LCD);
}

/*---------------------------------------------------------------------*
 * Storage device initialization
 *---------------------------------------------------------------------
 */
void store_device_init(void)
{
#if defined SD_FATFS_MODE
    /*Read picture data by SD card*/
    printf("Read picture data by SD card\n");
    sdfatfs_task();
#elif defined TINYUSB_HOST_MODE
    /*Reading picture data in tinyusb mode*/
    printf("Read picture data by usb-mode\n");
    tinyusb_task();
#endif
}

/*---------------------------------------------------------------------*
 * MAIN.C
 *---------------------------------------------------------------------
 */
int main(void)
{
    /*Width and height of RGB data*/
    int32_t rgbwidth, rgbheight;
    /*Get the number of files*/
    int32_t filenum;
    /*JPG data buff size*/
    int32_t jpg_size;
    /*Get file list name*/
    file_name_list_t filelist = {0};
    /*Function return status*/
    uint8_t status;

    /*System initialization*/
    board_init();
    /*LCD initialization*/
    board_init_lcd();
    /*Storage device initialization*/
    store_device_init();

    /*Get a list of specific file types under the specified directory*/
    status = file_scan("/", ".jpg", &filelist);
    if (!status) {
        printf("Failed to scan files\n");
    } else {
        /*Get JPG files circularly and convert them into RGB data*/
        for (filenum = 0; filenum < filelist.fillnum; filenum++) {
            /*Get a JPG file data*/
            status = file_get(filenum, &filelist, filebuff, &jpg_size);
            if (!status) {
                printf("file %s is too big, Please store pictures smaller than %d byte.\n", filelist.filename[filenum], FILEBUFFLEN);
                break;
            }    
#if defined JPEG_TURBO_MODE
            /*Libjpeg-turbo conversion to convert JPG data into rgb565 data*/
            jpeg_convert_data(filebuff, jpg_size, &rgbwidth, &rgbheight, rgb565buff);
            printf("Libjpeg-turbo decode completed\n");
#elif defined JPEG_HARDWARE_MODE
            /*jpeg-hardware conversion to convert JPG data into rgb565 data*/
            jpeg_convert_hw(filebuff, jpg_size, &rgbwidth, &rgbheight, rgb565buff);
            printf("jpeg-hardware decode completed\n");
#endif
            /*Display picture data on LCD*/
            lcdc_display_picture(rgb565buff, rgbwidth, rgbheight);
            board_delay_ms(2000);
        }
    }
}