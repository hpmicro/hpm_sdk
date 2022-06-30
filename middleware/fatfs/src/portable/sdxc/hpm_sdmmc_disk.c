/*
 * Copyright (c) 2021 - 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_sdmmc_disk.h"
#include "hpm_l1c_drv.h"
#include "board.h"

#define SD_SECTOR_SIZE (512UL)

ATTR_PLACE_AT_NONCACHEABLE_BSS sd_card_t g_sd;

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint32_t g_aligned_buf[MAX_ALIGNED_BUF_SIZE / sizeof(uint32_t)];

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

    if (((uint32_t)buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&g_aligned_buf);
        uint32_t remaining_size = SD_SECTOR_SIZE * count;
        while(remaining_size > 0) {
            uint32_t write_size = MIN(sizeof(g_aligned_buf), remaining_size);

            memcpy(g_aligned_buf, buff, write_size);
            l1c_dc_flush(sys_aligned_buf_addr, write_size);
            uint32_t sector_count = (uint32_t) write_size / SD_SECTOR_SIZE;
            if (sd_write_blocks(&g_sd, (const uint8_t *) sys_aligned_buf_addr, (uint32_t) sector, sector_count) != status_success) {
                return RES_ERROR;
            }
            buff += write_size;
            sector += sector_count;
            remaining_size -= write_size;
        }
    } else {
        if (sd_write_blocks(&g_sd, (const uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

DSTATUS sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }

    if (((uint32_t)buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&g_aligned_buf);
        uint32_t remaining_size = SD_SECTOR_SIZE * count;
        while(remaining_size > 0) {
            uint32_t read_size = MIN(sizeof(g_aligned_buf), remaining_size);
            uint32_t sector_count = read_size / SD_SECTOR_SIZE;
            if (sd_read_blocks(&g_sd, (uint8_t *) sys_aligned_buf_addr, sector, sector_count) != status_success) {
                return RES_ERROR;
            }
            l1c_dc_invalidate(sys_aligned_buf_addr, read_size);
            memcpy(buff, g_aligned_buf, read_size);
            buff += read_size;
            sector += sector_count;
            remaining_size -= read_size;
        }
    } else {
      if (sd_read_blocks(&g_sd, (uint8_t *) buff, sector, count) != status_success) {
          while(1);
          return RES_ERROR;
      }
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
