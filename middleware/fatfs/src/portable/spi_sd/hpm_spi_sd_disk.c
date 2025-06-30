/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_spi_sd_disk.h"

static spi_sdcard_info_t sd_info;

DSTATUS spi_sd_disk_initialize(BYTE pdrv)
{
    /* the spi hardware has been initialized before the sd disk init*/

    hpm_stat_t sta;
    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }
    sta = sdcard_spi_get_card_info(&sd_info);

    return (sta == status_success) ? RES_OK : RES_ERROR;
}

DSTATUS spi_sd_disk_deinitialize(BYTE pdrv)
{
    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }
    /* currently there is no need to operate */
    return RES_OK;
}

DSTATUS spi_sd_disk_status(BYTE pdrv)
{
    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }
    return (sdcard_spi_status() == status_success) ? RES_OK : RES_NOTRDY;
}

DSTATUS spi_sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }

    if (sdcard_spi_read_multi_block((uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

DSTATUS spi_sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }

    if (sdcard_spi_write_multi_block((uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT spi_sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_SD) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(LBA_t *) buff = sd_info.csd.device_size;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *) buff = sd_info.block_size;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *) buff = sd_info.block_size;
            break;
        case CTRL_SYNC:
            result = RES_OK;
            break;
        default:
            result = RES_PARERR;
            break;
        }

    } while (false);

    return result;
}
