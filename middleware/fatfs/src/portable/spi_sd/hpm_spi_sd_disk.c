/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_spi_sd_disk.h"
#include "hpm_l1c_drv.h"
#include "board.h"

#define MAX_ALIGNED_BUF_SIZE (2048U)

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) uint32_t g_aligned_buf[MAX_ALIGNED_BUF_SIZE / sizeof(uint32_t)];
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

    if (((uint32_t)buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&g_aligned_buf);
        uint32_t remaining_size = SPI_SD_BLOCK_SIZE * count;
        while (remaining_size > 0) {
            uint32_t read_size = MIN(sizeof(g_aligned_buf), remaining_size);
            uint32_t sector_count = read_size / SPI_SD_BLOCK_SIZE;
            if (sdcard_spi_read_multi_block((uint8_t *) sys_aligned_buf_addr, sector, sector_count) != status_success) {
                return RES_ERROR;
            }
            l1c_dc_invalidate(sys_aligned_buf_addr, read_size);
            memcpy(buff, g_aligned_buf, read_size);
            buff += read_size;
            sector += sector_count;
            remaining_size -= read_size;
        }
    } else {
        if (sdcard_spi_read_multi_block((uint8_t *) buff, sector, count) != status_success) {
            return RES_ERROR;
      }
    }
    return RES_OK;
}

DSTATUS spi_sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }

    if (((uint32_t)buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&g_aligned_buf);
        uint32_t remaining_size = SPI_SD_BLOCK_SIZE * count;
        while (remaining_size > 0) {
            uint32_t write_size = MIN(sizeof(g_aligned_buf), remaining_size);

            memcpy(g_aligned_buf, buff, write_size);
            l1c_dc_flush(sys_aligned_buf_addr, write_size);
            uint32_t sector_count = (uint32_t) write_size / SPI_SD_BLOCK_SIZE;
            if (sdcard_spi_write_multi_block((uint8_t *) sys_aligned_buf_addr, (uint32_t) sector, sector_count) != status_success) {
                return RES_ERROR;
            }
            buff += write_size;
            sector += sector_count;
            remaining_size -= write_size;
        }
    } else {
        if (sdcard_spi_write_multi_block((uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
            return RES_ERROR;
        }
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
            *(uint32_t *) buff = sd_info.csd.device_size;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *) buff = sd_info.block_size;
            break;
        case GET_BLOCK_SIZE:
            *(uint32_t *) buff = sd_info.block_size;
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
