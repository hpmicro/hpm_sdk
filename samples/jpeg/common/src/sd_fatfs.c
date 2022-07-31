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
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
FATFS s_sd_disk;

const TCHAR driver_num_buf[3] = {DEV_SD + '0', ':', '/'};
/*---------------------------------------------------------------------*
 * Define function
 *---------------------------------------------------------------------
 */
FRESULT sd_mount_fs(void);
/*---------------------------------------------------------------------*
 * SD card task, reading external file data
 *---------------------------------------------------------------------
 */
void sdfatfs_task(void)
{
    FRESULT filefresult;
    do {
        /*SD FatFs module reads image file data*/
        filefresult = sd_mount_fs();
    } while (filefresult != FR_OK);
}
/*---------------------------------------------------------------------*
 * SD FatFs module reads image file data
 *---------------------------------------------------------------------
 */
FRESULT sd_mount_fs(void)
{
    FRESULT fresult;

    fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
    } else {
        printf("Failed to mount SD card, error code:%d\n", fresult);
        return fresult;
    }
    /* change to newly mounted drive */
    fresult = f_chdrive(driver_num_buf);
    /* set root directory as current directory */
    f_chdir("/");
    return fresult;
}