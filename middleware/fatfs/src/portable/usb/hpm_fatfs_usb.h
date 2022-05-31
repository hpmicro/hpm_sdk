/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_FATFS_USB_H
#define HPM_FATFS_USB_H

#include "tusb.h"
#include "diskio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void    USB_disk_deinitialize(void);
DSTATUS USB_disk_initialize(BYTE pdrv);
DSTATUS USB_disk_status(BYTE pdrv);
DRESULT USB_disk_read(BYTE pdrv, BYTE*buff, DWORD sector, BYTE count);
DRESULT USB_disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, BYTE count);
DRESULT USB_disk_ioctl(BYTE pdrv, BYTE cmd, void* buff);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_FATFS_USB_H */