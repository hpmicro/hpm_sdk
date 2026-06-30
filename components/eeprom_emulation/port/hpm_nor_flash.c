/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "eeprom_emulation.h"
#include "hpm_nor_flash.h"
#include "hpm_l1c_drv.h"
#include "board.h"

#ifndef E2P_ATTR
#ifndef E2P_PLACE_FLASH
#define E2P_ATTR ATTR_RAMFUNC
#else
#define E2P_ATTR
#endif
#endif

/* ------------------------------------------------------------------ */
/*  Critical section — default implementation (save / restore level)    */
/* ------------------------------------------------------------------ */

E2P_ATTR
uint32_t e2p_enter_critical(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

E2P_ATTR
void e2p_exit_critical(uint32_t level)
{
    restore_global_irq(level);
}

/* ------------------------------------------------------------------ */
/*  NOR flash port operations                                           */
/* ------------------------------------------------------------------ */

E2P_ATTR
hpm_stat_t nor_flash_init(nor_flash_config_t *cfg)
{
    xpi_nor_config_option_t option;

    option.header.U = cfg->opt_header;
    option.option0.U = cfg->opt0;
    option.option1.U = cfg->opt1;

    hpm_stat_t status = rom_xpi_nor_auto_config(cfg->xpi_base,
                                                &cfg->nor_config,
                                                &option);
    if (status != status_success) {
        return status;
    }

    rom_xpi_nor_get_property(cfg->xpi_base, &cfg->nor_config,
                             xpi_nor_property_sector_size,
                             &cfg->sector_size);
    return status_success;
}

E2P_ATTR
hpm_stat_t nor_flash_read(nor_flash_config_t *cfg,
                          uint8_t *buf, uint32_t addr, uint32_t size)
{
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + size);
    uint32_t aligned_size = aligned_end - aligned_start;

    (void)cfg;
    l1c_dc_invalidate(aligned_start, aligned_size);

    memcpy(buf, (void *)addr, size);
    return status_success;
}

E2P_ATTR
hpm_stat_t nor_flash_write(nor_flash_config_t *cfg,
                           const uint8_t *buf, uint32_t addr, uint32_t size)
{
    uint32_t offset = (addr > cfg->base_addr)
                      ? (addr - cfg->base_addr) : addr;

    return rom_xpi_nor_program(cfg->xpi_base, xpi_xfer_channel_auto,
                               &cfg->nor_config,
                               (const uint32_t *)buf, offset, size);
}

E2P_ATTR
static hpm_stat_t nor_flash_erase_sector(nor_flash_config_t *cfg,
                                         uint32_t start_addr)
{
    uint32_t offset = (start_addr > cfg->base_addr)
                      ? (start_addr - cfg->base_addr) : start_addr;

    return rom_xpi_nor_erase_sector(cfg->xpi_base, xpi_xfer_channel_auto,
                                    &cfg->nor_config, offset);
}

E2P_ATTR
hpm_stat_t nor_flash_erase(nor_flash_config_t *cfg,
                           uint32_t start_addr, uint32_t size)
{
    uint32_t sector_size = cfg->sector_size;
    hpm_stat_t status = status_success;

    for (uint32_t i = 0; i < size / sector_size; i++) {
        status = nor_flash_erase_sector(cfg,
                                        start_addr + i * sector_size);
        if (status != status_success) {
            return status;
        }
    }
    return status_success;
}
