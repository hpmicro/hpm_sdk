/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_DISK_H
#define HPM_SDMMC_DISK_H

#include "ff.h"
#include "diskio.h"
#include "hpm_sdmmc_sd.h"

extern sd_card_t g_sd;


#ifdef __cplusplus
extern "C" {
#endif

DSTATUS sd_disk_initialize(BYTE pdrv);

DSTATUS sd_disk_deinitialize(BYTE pdrv);

DSTATUS sd_disk_status(BYTE pdrv);

DSTATUS sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

DSTATUS sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

DRESULT sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);


#ifdef __cplusplus
}
#endif

#endif /* HPM_SDMMC_DISK_H */
