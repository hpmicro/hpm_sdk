/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_fatfs_usb.h"

static DSTATUS usb_disk_stat[CFG_TUSB_HOST_DEVICE_MAX];

static bool USB_disk_wait_for_complete(uint8_t usb_addr)
{
    #if CFG_TUSB_OS != OPT_OS_NONE
    int32_t retry_cnt = 200;
    #else
    int32_t retry_cnt = 5000000;
    #endif

    while (!tuh_msc_idle(usb_addr) && retry_cnt--)
    {
        #if CFG_TUSB_OS != OPT_OS_NONE
        osal_task_delay(10);
        #else
        tuh_task();
        #endif
    }

    return retry_cnt > 0 ? true : false;
}

DSTATUS USB_disk_status(BYTE pdrv)
{
    uint8_t usb_addr = pdrv + 1;

    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, CFG_TUSB_HOST_DEVICE_MAX);

    if (tuh_msc_mounted(usb_addr) == true) {
        usb_disk_stat[pdrv] &= ~STA_NOINIT;
    } else {
        usb_disk_stat[pdrv] |= STA_NODISK;
    }

    return usb_disk_stat[pdrv];
}

void USB_disk_deinitialize(void)
{
    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, sizeof(DSTATUS));
}

DSTATUS USB_disk_initialize(BYTE pdrv)
{
    uint8_t usb_addr = pdrv + 1;

    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, CFG_TUSB_HOST_DEVICE_MAX);

    if (tuh_msc_mounted(usb_addr) == true) {
        usb_disk_stat[pdrv] &= ~STA_NOINIT;
    } else {
        usb_disk_stat[pdrv] |= STA_NODISK;
    }

    return usb_disk_stat[pdrv];
}

DRESULT USB_disk_read(BYTE pdrv, BYTE*buff, DWORD sector, BYTE count)
{
    uint8_t usb_addr = pdrv + 1;
    bool result;

    result = tuh_msc_read10(usb_addr, LUN_USB, buff, sector, count, NULL);

    if (result) {
        result =  USB_disk_wait_for_complete(usb_addr);
    }

    return result ? RES_OK : RES_ERROR;
}

DRESULT USB_disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, BYTE count)
{
    uint8_t usb_addr = pdrv + 1;
    bool result;

    result = tuh_msc_write10(usb_addr, LUN_USB, buff, sector, count, NULL);

    if (result) {
        result = USB_disk_wait_for_complete(usb_addr);
    }

    return result ? RES_OK : RES_ERROR;
}

DRESULT USB_disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    if (cmd != CTRL_SYNC) {
        return RES_ERROR;
    } else {
        return RES_OK;
    }
}