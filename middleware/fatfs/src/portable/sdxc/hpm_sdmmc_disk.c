/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_sdmmc_disk.h"

ATTR_PLACE_AT_NONCACHEABLE_BSS sd_card_t g_sd;

DSTATUS sd_disk_initialize(BYTE pdrv)
{
    static bool has_card_initialized = false;

    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }

    if (has_card_initialized) {
        return RES_OK;
    }

    if (sd_init(&g_sd) != status_success) {
        sd_deinit(&g_sd);
        memset(&g_sd, 0, sizeof(g_sd));
        return STA_NODISK;
    }
    has_card_initialized = true;

    return RES_OK;
}

DSTATUS sd_disk_deinitialize(BYTE pdrv)
{
    sd_deinit(&g_sd);

    return RES_OK;
}

DSTATUS sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }
    if (sd_write_blocks(&g_sd, (const uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

DSTATUS sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }
    if (sd_read_blocks(&g_sd, (uint8_t *) buff, sector, count) != status_success) {
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_SD) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(uint32_t *) buff = g_sd.block_count;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *) buff = g_sd.block_size;
            break;
        case GET_BLOCK_SIZE:
            *(uint32_t *) buff = g_sd.csd.erase_sector_size;
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

DSTATUS sd_disk_status(BYTE pdrv)
{
    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }

    return RES_OK;
}
