/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_sdmmc_disk.h"
#include "board.h"
#include "hpm_interrupt.h"

#include "hpm_sdmmc_sd.h"
#include "hpm_sdmmc_emmc.h"

typedef hpm_stat_t (*sdmmc_write_op_t)(void *card, const uint8_t *buffer, uint32_t start_block, uint32_t block_count);
typedef hpm_stat_t (*sdmmc_read_op_t)(void *card, uint8_t *buffer, uint32_t start_block, uint32_t block_count);

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
ATTR_PLACE_AT_NONCACHEABLE_BSS static sdmmc_host_t s_sd_host;
static sd_card_t s_sd = {.host = &s_sd_host };

#if defined(BOARD_APP_SDCARD_HOST_USING_IRQ) && (BOARD_APP_SDCARD_HOST_USING_IRQ == 1)
#if defined(BOARD_APP_SDCARD_SDXC_IRQ_PRIORITY)
#define SDCARD_SDXC_IRQ_PRIORITY BOARD_APP_SDCARD_SDXC_IRQ_PRIORITY
#else
#define SDCARD_SDXC_IRQ_PRIORITY 1
#endif
SDK_DECLARE_EXT_ISR_M(BOARD_APP_SDCARD_SDXC_IRQ, sdcard_isr)
void sdcard_isr(void)
{
    sdmmchost_irq_handler(&s_sd_host);
}
#endif

#endif

#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
ATTR_PLACE_AT_NONCACHEABLE_BSS static sdmmc_host_t s_emmc_host;
static emmc_card_t s_emmc = {.host = &s_emmc_host};

#if defined(BOARD_APP_EMMC_SDXC_IRQ) && (BOARD_APP_EMMC_SDXC_IRQ == 1)
#if defined(BOARD_APP_EMMC_SDXC_IRQ_PRIORITY)
#define MMC_SDXC_IRQ_PRIORITY BOARD_APP_EMMC_SDXC_IRQ_PRIORITY
#else
#define MMC_SDXC_IRQ_PRIORITY 1
#endif
SDK_DECLARE_EXT_ISR_M(BOARD_APP_EMMC_SDXC_IRQ, mmc_isr)
void mmc_isr(void)
{
    sdmmchost_irq_handler(&s_emmc_host);
}
#endif

#endif


#if defined(SD_FATFS_ENABLE) || defined(MMC_FATFS_ENABLE)

static DSTATUS sdmmc_card_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    sdmmc_write_op_t card_write;
    void *card;

#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    if (pdrv == DEV_SD) {
        card = &s_sd;
        card_write = (sdmmc_write_op_t)sd_write_blocks;
    }
#endif
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
    if (pdrv == DEV_MMC) {
        card = &s_emmc;
        card_write = (sdmmc_write_op_t)emmc_write_blocks;
    }
#endif
    if (card_write(card, (const uint8_t *) buff, (uint32_t) sector, (uint32_t) count) != status_success) {
        return RES_ERROR;
    }

    return RES_OK;
}

static DSTATUS sdmmc_card_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    sdmmc_read_op_t card_read;
    void *card;
#if defined(SD_FATFS_ENABLE) && SD_FATFS_ENABLE
    if (pdrv == DEV_SD) {
        card = &s_sd;
        card_read = (sdmmc_read_op_t)sd_read_blocks;
    }
#endif
#if defined(MMC_FATFS_ENABLE) && MMC_FATFS_ENABLE
    if (pdrv == DEV_MMC) {
        card = &s_emmc;
        card_read = (sdmmc_read_op_t)emmc_read_blocks;
    }
#endif
    if (card_read(card, (uint8_t *) buff, sector, count) != status_success) {
        return RES_ERROR;
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
        status = sd_read_status(&s_sd);
        if (status == status_success) {
            return RES_OK;
        }
    }
    has_card_initialized = false;

    status = board_init_sd_host_params(&s_sd_host, BOARD_APP_SDCARD_SDXC_BASE);
    if (status != status_success) {
        return STA_NOINIT;
    }
#if defined(BOARD_APP_SDCARD_HOST_USING_IRQ) && (BOARD_APP_SDCARD_HOST_USING_IRQ == 1)
    intc_m_enable_irq_with_priority(BOARD_APP_SDCARD_SDXC_IRQ, SDCARD_SDXC_IRQ_PRIORITY);
#endif
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
            *(LBA_t *) buff = s_sd.block_count;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *) buff = s_sd.block_size;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *) buff = s_sd.csd.erase_sector_size;
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

    if (!sdmmchost_is_card_detected(&s_sd_host)) {
        return STA_NODISK;
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
#if defined(BOARD_APP_EMMC_HOST_USING_IRQ) && (BOARD_APP_EMMC_HOST_USING_IRQ == 1)
    intc_m_enable_irq_with_priority(BOARD_APP_EMMC_SDXC_IRQ, MMC_SDXC_IRQ_PRIORITY);
#endif
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
            *(LBA_t *) buff = s_emmc.device_attribute.sector_count;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *) buff = s_emmc.device_attribute.sector_size;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *) buff = s_emmc.device_attribute.sector_size;
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
