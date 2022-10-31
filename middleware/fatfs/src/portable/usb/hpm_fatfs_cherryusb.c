/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "usbh_core.h"
#include "usbh_msc.h"
#include "hpm_fatfs_usb.h"

static USB_NOCACHE_RAM_SECTION struct usbh_msc *active_msc_class;

void usb_disk_set_active_msc_class(void *ptr)
{
    active_msc_class = (struct usbh_msc *)ptr;
}

DSTATUS usb_disk_status(BYTE pdrv)
{
    if (active_msc_class == NULL) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DSTATUS usb_disk_initialize(BYTE pdrv)
{
    if (active_msc_class == NULL) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DRESULT usb_disk_read(BYTE pdrv, BYTE *buff, DWORD sector, BYTE count)
{
    return usbh_msc_scsi_read10(active_msc_class, sector, buff, count);
}

DRESULT usb_disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, BYTE count)
{
    return usbh_msc_scsi_write10(active_msc_class, sector, buff, count);
}

DRESULT usb_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
    case CTRL_SYNC:
        result = RES_OK;
        break;

    case GET_SECTOR_SIZE:
        *(WORD *)buff = active_msc_class->blocksize;
        result = RES_OK;
        break;

    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        result = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD *)buff = active_msc_class->blocknum;
        result = RES_OK;
        break;

    default:
        result = RES_PARERR;
        break;
    }

    return result;
}