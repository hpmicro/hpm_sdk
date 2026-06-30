/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffconf.h"
#include "hpm_spi_nor_disk.h"
#include "hpm_serial_nor.h"
#include "hpm_serial_nor_host_port.h"
#include "board.h"

#if defined(USE_FREERTOS) && USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

static hpm_serial_nor_t nor_flash_dev = {0};
static hpm_serial_nor_info_t flash_info;
static hpm_stat_t nor_init_sta;

DSTATUS spi_nor_disk_initialize(BYTE pdrv)
{
    if (pdrv != DEV_SPI_NOR) {
        return STA_NOINIT;
    }
    serial_nor_get_board_host(&nor_flash_dev.host);
    board_init_spi_clock(nor_flash_dev.host.host_param.param.host_base);
    serial_nor_spi_pins_init(nor_flash_dev.host.host_param.param.host_base);
    nor_init_sta = hpm_serial_nor_init(&nor_flash_dev, &flash_info);
    return (nor_init_sta == status_success) ? RES_OK : RES_ERROR;
}

DSTATUS spi_nor_disk_deinitialize(BYTE pdrv)
{
    if (pdrv != DEV_SPI_NOR) {
        return STA_NOINIT;
    }
    /* currently there is no need to operate */
    return RES_OK;
}

DSTATUS spi_nor_disk_status(BYTE pdrv)
{
    if (pdrv != DEV_SPI_NOR) {
        return STA_NOINIT;
    }
    return (nor_init_sta == status_success) ? RES_OK : RES_NOTRDY;
}

DSTATUS spi_nor_disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SPI_NOR) {
        return RES_PARERR;
    }
    uint32_t sector_size = FF_MAX_SS;
    uint32_t remaining_size = sector_size * count;
    uint32_t start_addr = sector_size * sector;
    hpm_stat_t stat = hpm_serial_nor_read(&nor_flash_dev, (uint8_t *)buff, remaining_size, start_addr);
    if (stat != status_success) {
        return RES_ERROR;
    }
    return RES_OK;
}

DSTATUS spi_nor_disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != DEV_SPI_NOR) {
        return RES_PARERR;
    }
    uint32_t sector_size = FF_MAX_SS;
    uint32_t remaining_size = sector_size * count;
    uint32_t start_addr = sector_size * sector;
    hpm_stat_t stat = hpm_serial_nor_erase_blocking(&nor_flash_dev, start_addr, remaining_size);
    if (stat != status_success) {
        return RES_ERROR;
    }
    stat = hpm_serial_nor_program_blocking(&nor_flash_dev, (uint8_t *)buff, remaining_size, start_addr);
    if (stat != status_success) {
        return RES_ERROR;
    }
    return RES_OK;
}

DRESULT spi_nor_disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_PARERR;
    do {
        HPM_BREAK_IF((pdrv != DEV_SPI_NOR) || ((cmd != CTRL_SYNC) && (buff == NULL)));
        result = RES_OK;
        switch (cmd) {
        case GET_SECTOR_COUNT:
            *(LBA_t *) buff = (flash_info.size_in_kbytes / flash_info.sector_size_kbytes) * 1024;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *) buff = (WORD)FF_MAX_SS;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *) buff = (flash_info.sector_size_kbytes * 1024);
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
