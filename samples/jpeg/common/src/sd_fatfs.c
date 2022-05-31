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
#include "hpm_sdmmc_sd.h"
#include "ff.h"
#include "diskio.h"
#include "sd_fatfs.h"
#include "file_op.h"
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
/*JPG data buff size*/
extern int32_t jpg_size;
/*jpg file data buff*/
extern uint8_t filebuff[];

extern sd_card_t g_sd;
FATFS s_sd_disk;
FIL s_file;
FRESULT fresult;
/*---------------------------------------------------------------------*
 * Define function
 *---------------------------------------------------------------------
 */
static FRESULT sd_mount_fs(void);

/*---------------------------------------------------------------------*
 * SD card task, reading external file data
 *---------------------------------------------------------------------
 */
void sdfatfs_task(void)
{
    /*SD FatFs module reads image file data*/
    sd_mount_fs();
}

/*---------------------------------------------------------------------*
 * SD FatFs module reads image file data
 *---------------------------------------------------------------------
 */
static FRESULT sd_mount_fs(void)
{
    TCHAR driver_num_buf[3] = { DEV_SD + '0', ':', '/' };
    char *filename;
    UINT cnt = 0;

    fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
    } else {
        printf("Failed to mount SD card, error code:%d\n", fresult);
    }
    /* change to newly mounted drive */
    fresult = f_chdrive(driver_num_buf);
    /* set root directory as current directory */
    f_chdir("/");
#if defined DECOMPRESS_MODE 
    /* scan files in the specified directory of an SD card */
    filename = f_scan("/");

    fresult = f_open(&s_file,filename, FA_READ);
    if (fresult == FR_OK) {
        printf("Open file successfully, status=%d\n", fresult);
        /* read the specified file from U disk */
        f_read(&s_file, filebuff, FILEBUFFLEN, &cnt);
        f_close(&s_file);
        jpg_size = cnt;
    } 
#elif defined COMPRESS_MODE
    /* scan files in the specified directory of an SD card */
    f_scan("/");
    filename = "Camera.jpg";
    fresult = f_open(&s_file,filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult == FR_OK) {
        printf("Open file successfully, status=%d\n", fresult);
        /* Write the specified file via sd card */
        for (int32_t fl = 0; fl < jpg_size; fl++){
            f_write(&s_file, (uint8_t *)(filebuff + fl), 1, &cnt);
        }
        f_scan("/");
        f_close(&s_file);
        jpg_size = 0;
    }
#endif
    else {
        printf("Open file failed, status=%d\n", fresult);
        return fresult;
    }
    return fresult;
}
