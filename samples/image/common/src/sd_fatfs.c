/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*
 * Includes
 *
 */
#include "board.h"
#include "hpm_sdmmc_sd.h"
#include "ff.h"
#include "diskio.h"
#include "sd_fatfs.h"
/*
 * Define variables
 *
 */
static FATFS sd_disk;
static bool disk_mounted;
static const TCHAR driver_num_buf[4] = {DEV_SD + '0', ':', '/', '\0'};
/*
 * Define function
 *
 */

FRESULT sd_mount_fs(void);
/*
 * SD card task, reading external file data
 *
 */

void init_disk(void)
{
    sd_mount_fs();
}

bool check_disk(void)
{
    return disk_mounted || (sd_mount_fs() == FR_OK);
}

/*
 * SD FatFs module reads image file data
 *
 */
FRESULT sd_mount_fs(void)
{
    FRESULT fresult;

    fresult = f_mount(&sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
    } else {
        printf("Failed to mount SD card, error code:%d\n", fresult);
        disk_mounted = false;
        return fresult;
    }
    /* change to newly mounted drive */
    fresult = f_chdrive(driver_num_buf);
    /* set root directory as current directory */
    f_chdir("/");
    disk_mounted = true;
    return fresult;
}

bool disk_is_mounted(void)
{
    return disk_mounted;
}
