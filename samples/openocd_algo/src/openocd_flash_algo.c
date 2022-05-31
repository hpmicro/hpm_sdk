/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_common.h"
#include "hpm_l1c_drv.h"
#include "hpm_romapi.h"

#define XPI0_MEM_START (0x80000000UL)
#define XPI1_MEM_START (0x90000000UL)
#define XPI_USE_PORT_B_MASK (0x100)
#define XPI_USE_PORT_A_MASK (0)
#define XPI_USE_PORT_SHIFT (0x8)

typedef struct {
    uint32_t total_sz_in_bytes;
    uint32_t sector_sz_in_bytes;
} hpm_flash_info_t;

__attribute__ ((section(".flash_algo.data"))) xpi_nor_config_t nor_config;
__attribute__ ((section(".flash_algo.data"))) bool xpi_inited = false;
__attribute__ ((section(".flash_algo.data"))) uint32_t channel = xpi_channel_a1;

const uint8_t flash_config_dummy[256] = {
	0x58, 0x4E, 0x4F, 0x52, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x04, 0x00, 0x40, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xEB, 0x04, 0x18, 0x0A, 0x00, 0x1E, 0x04, 0x32,
	0x04, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x18, 0x08,
	0x04, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x04, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x04, 0x18, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x60, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_init(uint32_t flash_base, uint32_t header, uint32_t opt0, uint32_t opt1)
{
    uint32_t i = 0;
    XPI_Type *xpi_base;
    xpi_nor_config_option_t cfg_option;
    hpm_stat_t stat = status_success;

    if (flash_base == XPI0_MEM_START) {
        xpi_base = HPM_XPI0;
    } else if (flash_base == XPI1_MEM_START) {
        xpi_base = HPM_XPI1;
    } else {
        return status_invalid_argument;
    }

    if (xpi_inited) {
        return stat;
    }

    for (i = 0; i < sizeof(cfg_option); i++) {
        *((uint8_t *)&cfg_option + i) = 0;
    }
    for (i = 0; i < sizeof(nor_config); i++) {
        *((uint8_t *)&nor_config + i) = 0;
    }

    /* dummy config needs to be done before actual configuration */
    ROM_API_TABLE_ROOT->xpi_nor_driver_if->init(xpi_base, (xpi_nor_config_t *)flash_config_dummy);

    cfg_option.header.U = header;
    cfg_option.option0.U = opt0;
    cfg_option.option1.U = opt1;

    if (opt1 & XPI_USE_PORT_B_MASK) {
        channel = xpi_channel_b1;
    } else {
        channel = xpi_channel_a1;
    }

    stat = ROM_API_TABLE_ROOT->xpi_nor_driver_if->auto_config(xpi_base, &nor_config, &cfg_option);
    if (stat) {
        return stat;
    }
    nor_config.device_info.clk_freq_for_non_read_cmd = 0;
    if (!xpi_inited) {
        xpi_inited = true;
    }
    return stat;
}

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_erase(uint32_t flash_base, uint32_t address, uint32_t size)
{
    XPI_Type *xpi_base;
    hpm_stat_t stat = status_success;
    uint32_t left, start, block_size, align;
    if (flash_base == XPI0_MEM_START) {
        xpi_base = HPM_XPI0;
    } else if (flash_base == XPI1_MEM_START) {
        xpi_base = HPM_XPI1;
    } else {
        return status_invalid_argument;
    }

    left = size;
    start = address;
    block_size = nor_config.device_info.block_size_kbytes * 1024;
    if (left >= block_size) {
        align = block_size - (start % block_size);
        if (align != block_size) {
            stat = ROM_API_TABLE_ROOT->xpi_nor_driver_if->erase(xpi_base, channel, &nor_config, start, align);
            if (stat != status_success) {
                return stat;
            }
            left -= align;
            start += align;
        }
        while (left > block_size) {
            stat = ROM_API_TABLE_ROOT->xpi_nor_driver_if->erase_block(xpi_base, channel, &nor_config, start);
            if (stat != status_success) {
                break;
            }
            left -= block_size;
            start += block_size;
        }
    }
    if ((stat == status_success) && left) {
        stat = ROM_API_TABLE_ROOT->xpi_nor_driver_if->erase(xpi_base, channel, &nor_config, start, left);
    }
    return stat;
}

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_program(uint32_t flash_base, uint32_t address, uint32_t *buf, uint32_t size)
{
    XPI_Type *xpi_base;
    hpm_stat_t stat;

    if (flash_base == XPI0_MEM_START) {
        xpi_base = HPM_XPI0;
    } else if (flash_base == XPI1_MEM_START) {
        xpi_base = HPM_XPI1;
    } else {
        return status_invalid_argument;
    }

    stat = ROM_API_TABLE_ROOT->xpi_nor_driver_if->program(xpi_base, channel, &nor_config, buf, address, size);
    return stat;
}

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_read(uint32_t flash_base, uint32_t *buf, uint32_t address, uint32_t size)
{
    XPI_Type *xpi_base;
    hpm_stat_t stat;
    if (flash_base == XPI0_MEM_START) {
        xpi_base = HPM_XPI0;
    } else if (flash_base == XPI1_MEM_START) {
        xpi_base = HPM_XPI1;
    } else {
        return status_invalid_argument;
    }

    stat = rom_xpi_nor_read(xpi_base, channel, &nor_config, buf, address, size);
    return stat;
}

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_get_info(uint32_t flash_base, hpm_flash_info_t *flash_info)
{
    if (!flash_info) {
        return status_invalid_argument;
    }

    flash_info->total_sz_in_bytes = nor_config.device_info.size_in_kbytes << 10;
    flash_info->sector_sz_in_bytes = nor_config.device_info.sector_size_kbytes << 10;
    return status_success;
}

__attribute__ ((section(".flash_algo.text"))) uint32_t flash_erase_chip(uint32_t flash_base)
{
    XPI_Type *xpi_base;
    if (flash_base == XPI0_MEM_START) {
        xpi_base = HPM_XPI0;
    } else if (flash_base == XPI1_MEM_START) {
        xpi_base = HPM_XPI1;
    } else {
        return status_invalid_argument;
    }

    return rom_xpi_nor_erase_chip(xpi_base, channel, &nor_config);
}

__attribute__ ((section(".flash_algo.text"))) void flash_deinit(void)
{
    return;
}
