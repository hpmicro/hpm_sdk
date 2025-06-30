/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tusb.h"
#include "hpm_fatfs_usb.h"

static DSTATUS usb_disk_stat[CFG_TUH_DEVICE_MAX];
static volatile bool usb_xfer_busy[CFG_TUH_DEVICE_MAX];

static bool usb_disk_io_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const *cb_data)
{
    (void)dev_addr;
    *(bool *)cb_data->user_arg = false;

    return true;
}

static void usb_wait_for_disk_io(BYTE pdrv)
{
    while (usb_xfer_busy[pdrv]) {
#if CFG_TUSB_OS == OPT_OS_NONE
        tuh_task();
#else
        osal_task_delay(10);
#endif
    }
}

DSTATUS usb_disk_status(BYTE pdrv)
{
    uint8_t usb_addr = pdrv + 1;

    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, CFG_TUH_DEVICE_MAX);

    if (tuh_msc_mounted(usb_addr) == true) {
        usb_disk_stat[pdrv] &= ~STA_NOINIT;
    } else {
        usb_disk_stat[pdrv] |= STA_NODISK;
    }

    return usb_disk_stat[pdrv];
}

void usb_disk_deinitialize(void)
{
    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, sizeof(DSTATUS));
}

DSTATUS usb_disk_initialize(BYTE pdrv)
{
    uint8_t usb_addr = pdrv + 1;

    /* set the default status as STA_NOINIT */
    memset(usb_disk_stat, STA_NOINIT, CFG_TUH_DEVICE_MAX);

    if (tuh_msc_mounted(usb_addr) == true) {
        usb_disk_stat[pdrv] &= ~STA_NOINIT;
    } else {
        usb_disk_stat[pdrv] |= STA_NODISK;
    }

    return usb_disk_stat[pdrv];
}

DRESULT usb_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    uint8_t usb_addr = pdrv + 1;
    bool result;

    usb_xfer_busy[pdrv] = true;
    result = tuh_msc_read10(usb_addr, LUN_USB, buff, sector, count, usb_disk_io_complete_cb, (uintptr_t)&usb_xfer_busy[pdrv]);

    if (result) {
        usb_wait_for_disk_io(pdrv);
    }

    return result ? RES_OK : RES_ERROR;
}

DRESULT usb_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    uint8_t usb_addr = pdrv + 1;
    bool result;

    usb_xfer_busy[pdrv] = true;
    result = tuh_msc_write10(usb_addr, LUN_USB, buff, sector, count, usb_disk_io_complete_cb, (uintptr_t)&usb_xfer_busy[pdrv]);

    if (result) {
        usb_wait_for_disk_io(pdrv);
    }

    return result ? RES_OK : RES_ERROR;
}

DRESULT usb_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    uint8_t usb_addr = pdrv + 1;
    int result = 0;

    switch (cmd) {
    case CTRL_SYNC:
        result = RES_OK;
        break;

    case GET_SECTOR_SIZE:
        *(WORD *)buff = tuh_msc_get_block_size(usb_addr, LUN_USB);
        result = RES_OK;
        break;

    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        result = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(LBA_t *)buff = tuh_msc_get_block_count(usb_addr, LUN_USB);
        result = RES_OK;
        break;

    default:
        result = RES_PARERR;
        break;
    }

    return result;
}