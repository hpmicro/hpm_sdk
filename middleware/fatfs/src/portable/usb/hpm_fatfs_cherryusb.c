/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "usbh_core.h"
#include "usbh_msc.h"
#include "hpm_fatfs_usb.h"

static USB_NOCACHE_RAM_SECTION struct usbh_msc *active_msc_class[CONFIG_USBHOST_MAX_MSC_CLASS];
static BYTE s_pdrv[CONFIG_USBHOST_MAX_MSC_CLASS];

void usb_disk_set_active_msc_class(BYTE pdrv, void *ptr)
{
    if (pdrv >= DEV_USB_MSC_0) {
        active_msc_class[pdrv - DEV_USB_MSC_0] = (struct usbh_msc *)ptr;
        s_pdrv[pdrv - DEV_USB_MSC_0] = pdrv;
    } else {
        active_msc_class[pdrv - DEV_USB] = (struct usbh_msc *)ptr;
        s_pdrv[pdrv - DEV_USB] = pdrv;
    }
}

uint8_t usb_disk_free_active_msc_class(void *ptr)
{
    uint8_t i;

    for (i = 0; i < CONFIG_USBHOST_MAX_MSC_CLASS; i++) {
        if (active_msc_class[i] == ptr) {
            active_msc_class[i] = NULL;
            break;
        }
    }

    return s_pdrv[i];
}

DSTATUS usb_disk_status(BYTE pdrv)
{
    struct usbh_msc *ptr;

    if (pdrv >= DEV_USB_MSC_0) {
        ptr = active_msc_class[pdrv - DEV_USB_MSC_0];
    } else {
        ptr = active_msc_class[pdrv - DEV_USB];
    }

    if (ptr == NULL) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DSTATUS usb_disk_initialize(BYTE pdrv)
{
    struct usbh_msc *ptr;

    if (pdrv >= DEV_USB_MSC_0) {
        ptr = active_msc_class[pdrv - DEV_USB_MSC_0];
    } else {
        ptr = active_msc_class[pdrv - DEV_USB];
    }

    if (ptr == NULL) {
        return STA_NOINIT;
    }

    if (usbh_msc_scsi_init(ptr) < 0) {
        return RES_NOTRDY;
    }

    return RES_OK;
}

DRESULT usb_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    struct usbh_msc *ptr;

    if (pdrv >= DEV_USB_MSC_0) {
        ptr = active_msc_class[pdrv - DEV_USB_MSC_0];
    } else {
        ptr = active_msc_class[pdrv - DEV_USB];
    }

    return usbh_msc_scsi_read10(ptr, sector, buff, count);
}

DRESULT usb_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    struct usbh_msc *ptr;

    if (pdrv >= DEV_USB_MSC_0) {
        ptr = active_msc_class[pdrv - DEV_USB_MSC_0];
    } else {
        ptr = active_msc_class[pdrv - DEV_USB];
    }

    return usbh_msc_scsi_write10(ptr, sector, buff, count);
}

DRESULT usb_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    int result = 0;
    struct usbh_msc *ptr;

    if (pdrv >= DEV_USB_MSC_0) {
        ptr = active_msc_class[pdrv - DEV_USB_MSC_0];
    } else {
        ptr = active_msc_class[pdrv - DEV_USB];
    }

    switch (cmd) {
    case CTRL_SYNC:
        result = RES_OK;
        break;

    case GET_SECTOR_SIZE:
        *(WORD *)buff = ptr->blocksize;
        result = RES_OK;
        break;

    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        result = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(LBA_t *)buff = ptr->blocknum;
        result = RES_OK;
        break;

    default:
        result = RES_PARERR;
        break;
    }

    return result;
}