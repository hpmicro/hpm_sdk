/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_nor_flash.h"
#include "hpm_l1c_drv.h"
#include "board.h"

hpm_stat_t nor_flash_init(nor_flash_config_t *cfg)
{
    xpi_nor_config_option_t option;

    option.header.U = cfg->opt_header;
    option.option0.U = cfg->opt0;
    option.option1.U = cfg->opt1;
    hpm_stat_t status = rom_xpi_nor_auto_config(cfg->xpi_base, &cfg->nor_config, &option);
    if (status != status_success) {
        return status;
    }

    rom_xpi_nor_get_property(cfg->xpi_base, &cfg->nor_config, xpi_nor_property_sector_size, &cfg->sector_size);

    return status_success;
}

hpm_stat_t nor_flash_read(nor_flash_config_t *cfg, uint8_t *buf, uint32_t addr, uint32_t size)
{
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + size);
    uint32_t aligned_size = aligned_end - aligned_start;

    (void)cfg;
    l1c_dc_invalidate(aligned_start, aligned_size);

    memcpy(buf, (void *)addr, size);
    return status_success;
}

hpm_stat_t nor_flash_write(nor_flash_config_t *cfg, uint8_t *buf, uint32_t addr, uint32_t size)
{
    addr > cfg->base_addr ? (addr -= cfg->base_addr) : addr;
    hpm_stat_t status = rom_xpi_nor_program(cfg->xpi_base, xpi_xfer_channel_auto, \
                                            &cfg->nor_config, (const uint32_t *)buf, addr, size);

    return status;
}

static hpm_stat_t nor_flash_erase_sector(nor_flash_config_t *cfg, uint32_t start_addr)
{
    start_addr > cfg->base_addr ? (start_addr -= cfg->base_addr) : start_addr;
    hpm_stat_t status = rom_xpi_nor_erase_sector(cfg->xpi_base, xpi_xfer_channel_auto, &cfg->nor_config, start_addr);

    return status;
}

void nor_flash_erase(nor_flash_config_t *cfg, uint32_t start_addr, uint32_t size)
{
    uint32_t sector_size = cfg->sector_size;
    for (uint32_t i = 0; i < size / sector_size; i++) {
        nor_flash_erase_sector(cfg, start_addr + i * sector_size);
    }
}
