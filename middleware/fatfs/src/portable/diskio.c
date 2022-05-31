/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "diskio.h"          /* Declarations of disk functions */

#ifdef USB_FATFS_ENABLE
#include "./usb/hpm_fatfs_usb.h"
#endif

#ifdef SD_FATFS_ENABLE
#include "./sdxc/hpm_sdmmc_disk.h"
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB :
        stat = USB_disk_status(pdrv);
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        stat = sd_disk_status(pdrv);
        break;
#endif

    default:
        break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
void disk_deinitialize(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB:
        USB_disk_deinitialize();
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        sd_disk_deinitialize(pdrv);
        break;
#endif
    default:
        break;
    }
}

DSTATUS disk_initialize(
        BYTE pdrv   /* Physical drive number to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB :
        stat = USB_disk_initialize(pdrv);
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        stat = sd_disk_initialize(pdrv);
        break;
#endif
    default:
        break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
        BYTE pdrv,      /* Physical drive number to identify the drive */
        BYTE *buff,     /* Data buffer to store read data */
        LBA_t sector,   /* Start sector in LBA */
        UINT count      /* Number of sectors to read */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB :
        res = USB_disk_read(pdrv, buff, sector, count);
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        res = sd_disk_read(pdrv, buff, sector, count);
        break;
#endif
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
        BYTE pdrv,          /* Physical drive number to identify the drive */
        const BYTE *buff,   /* Data to be written */
        LBA_t sector,       /* Start sector in LBA */
        UINT count          /* Number of sectors to write */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB :
        res = USB_disk_write(pdrv, buff, sector, count);
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        res = sd_disk_write(pdrv, buff, sector, count);
        break;
#endif
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(
        BYTE pdrv,      /* Physical drive number (0..) */
        BYTE cmd,       /* Control code */
        void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM :
        break;

    case DEV_MMC :
        break;

#if USB_FATFS_ENABLE
    case DEV_USB :
        res = USB_disk_ioctl(pdrv, cmd ,buff);
        break;
#endif

#ifdef SD_FATFS_ENABLE
    case DEV_SD:
        res = sd_disk_ioctl(pdrv, cmd, buff);
        break;
#endif

    default:
        res = RES_ERROR;
        break;
    }

    return res;
}
