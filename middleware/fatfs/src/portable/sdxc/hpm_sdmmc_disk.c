/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_sdmmc_disk.h"
#include "hpm_l1c_drv.h"
#include "board.h"

#include "hpm_sdmmc_sd.h"
#include "hpm_sdmmc_emmc.h"

#define SD_SECTOR_SIZE (512UL)

typedef hpm_stat_t (*sdmmc_write_op_t)(void *card, const uint8_t *buffer, uint32_t start_block, uint32_t block_count);
typedef hpm_stat_t (*sdmmc_read_op_t)(void *card, uint8_t *buffer, uint32_t start_block, uint32_t block_count);

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
ATTR_PLACE_AT_NONCACHEABLE_BSS static sdmmc_host_t s_sd_host;
static sd_card_t s_sd = {.host = &s_sd_host };
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint32_t s_sd_aligned_buf[MAX_ALIGNED_BUF_SIZE / sizeof(uint32_t)];
#endif

#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
ATTR_PLACE_AT_NONCACHEABLE_BSS static sdmmc_host_t s_emmc_host;
static emmc_card_t s_emmc = {.host = &s_emmc_host};
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint32_t s_emmc_aligned_buf[MAX_ALIGNED_BUF_SIZE / sizeof(uint32_t)];
#endif


#if defined(SD_FATFS_ENABLE) || defined(MMC_FATFS_ENABLE)
static void sdmmc_get_card_and_aligned_buf_info(BYTE pdrv, void **card, uint32_t **buf, uint32_t *buf_size)
{
#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    if (pdrv == DEV_SD) {
        *card = &s_sd;
        *buf =  s_sd_aligned_buf;
        *buf_size = sizeof(s_sd_aligned_buf);
    }
#endif
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
    if (pdrv == DEV_MMC) {
        *card = &s_emmc;
        *buf =  s_emmc_aligned_buf;
        *buf_size = sizeof(s_emmc_aligned_buf);
    }
#endif
}

static DSTATUS sdmmc_card_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    sdmmc_write_op_t card_write;
    void *card;
    uint32_t *aligned_buf;
    uint32_t aligned_buf_size;
    sdmmc_get_card_and_aligned_buf_info(pdrv, &card, &aligned_buf, &aligned_buf_size);
#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    if (pdrv == DEV_SD) {
        card_write = (sdmmc_write_op_t)sd_write_blocks;
    }
#endif
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
    if (pdrv == DEV_MMC) {
        card_write = (sdmmc_write_op_t)emmc_write_blocks;
    }
#endif
    if (((uint32_t) buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) aligned_buf);
        uint32_t remaining_size = SD_SECTOR_SIZE * count;
        while (remaining_size > 0) {
            uint32_t write_size = MIN(aligned_buf_size, remaining_size);
            memcpy(aligned_buf, buff, write_size);
            l1c_dc_flush(sys_aligned_buf_addr, write_size);
            uint32_t sector_count = (uint32_t) write_size / SD_SECTOR_SIZE;
            if (card_write(card, (const uint8_t *) sys_aligned_buf_addr, (uint32_t) sector, sector_count) !=
                status_success) {
                return RES_ERROR;
            }
            buff += write_size;
            sector += sector_count;
            remaining_size -= write_size;
        }
    } else {
        if (card_write(card, (const uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

static DSTATUS sdmmc_card_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    sdmmc_read_op_t card_read;
    void *card;
    uint32_t *aligned_buf;
    uint32_t aligned_buf_size;
    sdmmc_get_card_and_aligned_buf_info(pdrv, &card, &aligned_buf, &aligned_buf_size);
#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    if (pdrv == DEV_SD) {
        card_read = (sdmmc_read_op_t)sd_read_blocks;
    }
#endif
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
    if (pdrv == DEV_MMC) {
        card_read = (sdmmc_read_op_t)emmc_read_blocks;
    }
#endif
    if (((uint32_t) buff % 4) != 0) {
        uint32_t sys_aligned_buf_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t) aligned_buf);
        uint32_t remaining_size = SD_SECTOR_SIZE * count;
        while (remaining_size > 0) {
            uint32_t read_size = MIN(aligned_buf_size, remaining_size);
            uint32_t sector_count = read_size / SD_SECTOR_SIZE;
            if (card_read(card, (uint8_t *) sys_aligned_buf_addr, sector, sector_count) != status_success) {
                return RES_ERROR;
            }
            l1c_dc_invalidate(sys_aligned_buf_addr, read_size);
            memcpy(buff, aligned_buf, read_size);
            buff += read_size;
            sector += sector_count;
            remaining_size -= read_size;
        }
    } else {
        if (card_read(card, (uint8_t *) buff, sector, count) != status_success) {
            return RES_ERROR;
        }
    }
    return RES_OK;
}
#endif

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
DSTATUS sd_disk_initialize(BYTE pdrv)
{
    static bool has_card_initialized = false;
    hpm_stat_t status;

    if (pdrv != DEV_SD) {
        return STA_NOINIT;
    }

    if (has_card_initialized) {
        return RES_OK;
    }

    status = board_init_sd_host_params(&s_sd_host, BOARD_APP_SDCARD_SDXC_BASE);
    if (status != status_success) {
        return STA_NOINIT;
    }
    status = sd_init(&s_sd);
    if (status != status_success) {
        sd_deinit(&s_sd);
        return STA_NODISK;
    }
    has_card_initialized = true;

    return RES_OK;
}

DSTATUS sd_disk_deinitialize(BYTE pdrv)
{
    if (pdrv == DEV_SD) {
        sd_deinit(&s_sd);
        return RES_OK;
    }
    return STA_NOINIT;
}

DSTATUS sd_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }
    return sdmmc_card_write(pdrv, buff, sector, count);
}

DSTATUS sd_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SD) {
        return RES_PARERR;
    }
    return sdmmc_card_read(pdrv, buff, sector, count);
}

DRESULT sd_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_SD) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(uint32_t *) buff = s_sd.block_count;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *) buff = s_sd.block_size;
            break;
        case GET_BLOCK_SIZE:
            *(uint32_t *) buff = s_sd.csd.erase_sector_size;
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
#endif


#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
DSTATUS emmc_disk_initialize(BYTE pdrv)
{
    static bool has_card_initialized = false;
    hpm_stat_t status;

    if (pdrv != DEV_MMC) {
        return STA_NOINIT;
    }

    if (has_card_initialized) {
        return RES_OK;
    }

    status = board_init_emmc_host_params(&s_emmc_host, BOARD_APP_EMMC_SDXC_BASE);
    if (status != status_success) {
        return STA_NOINIT;
    }
    status = emmc_init(&s_emmc);
    if (status != status_success) {
        emmc_deinit(&s_emmc);
        return STA_NODISK;
    }
    has_card_initialized = true;

    return RES_OK;
}

DSTATUS emmc_disk_deinitialize(BYTE pdrv)
{
    if (pdrv == DEV_MMC) {
        emmc_deinit(&s_emmc);
        return RES_OK;
    }
    return STA_NOINIT;
}

DSTATUS emmc_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_MMC) {
        return RES_PARERR;
    }
    return sdmmc_card_write(pdrv, buff, sector, count);
}

DSTATUS emmc_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_MMC) {
        return RES_PARERR;
    }
    return sdmmc_card_read(pdrv, buff, sector, count);
}

DRESULT emmc_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_MMC) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(uint32_t *) buff = s_emmc.device_attribute.sector_count;
            break;
        case GET_SECTOR_SIZE:
            *(uint32_t *) buff = s_emmc.device_attribute.sector_size;
            break;
        case GET_BLOCK_SIZE:
            *(uint32_t *) buff = s_emmc.device_attribute.sector_size;
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

DSTATUS emmc_disk_status(BYTE pdrv)
{
    if (pdrv != DEV_MMC) {
        return STA_NOINIT;
    }

    return RES_OK;
}
#endif
