/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_NOR_FLASH_H
#define _HPM_NOR_FLASH_H

#include <stdint.h>
#include "hpm_common.h"
#include "hpm_romapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NOR flash port configuration for eeprom emulation
 */
typedef struct {
    XPI_Type *xpi_base;
    uint32_t base_addr;
    uint32_t sector_size;
    uint32_t opt_header;
    uint32_t opt0;
    uint32_t opt1;
    xpi_nor_config_t nor_config;
} nor_flash_config_t;

/**
 * @brief Initialize NOR flash via ROM API
 *
 * @param[in] cfg  port configuration
 * @return hpm_stat_t
 */
hpm_stat_t nor_flash_init(nor_flash_config_t *cfg);

/**
 * @brief Read data from NOR flash (memory-mapped XIP read)
 *
 * @param[in]  cfg  port configuration
 * @param[out] buf  destination buffer
 * @param[in]  addr physical address
 * @param[in]  size bytes to read
 * @return hpm_stat_t
 */
hpm_stat_t nor_flash_read(nor_flash_config_t *cfg,
                          uint8_t *buf, uint32_t addr, uint32_t size);

/**
 * @brief Program data to NOR flash via ROM API
 *
 * @param[in] cfg  port configuration
 * @param[in] buf  source buffer
 * @param[in] addr physical address
 * @param[in] size bytes to write
 * @return hpm_stat_t
 */
hpm_stat_t nor_flash_write(nor_flash_config_t *cfg,
                           const uint8_t *buf, uint32_t addr, uint32_t size);

/**
 * @brief Erase NOR flash sectors covering the given range
 *
 * @param[in] cfg        port configuration
 * @param[in] start_addr physical start address (sector-aligned)
 * @param[in] size       bytes to erase (multiple of sector_size)
 * @return hpm_stat_t
 */
hpm_stat_t nor_flash_erase(nor_flash_config_t *cfg,
                           uint32_t start_addr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _HPM_NOR_FLASH_H */
