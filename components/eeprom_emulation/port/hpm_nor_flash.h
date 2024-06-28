/*
 * Copyright (c) 2023 HPMicro
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
#endif /* __cplusplus  */

/**
 * @brief eeprom emulation default in ram, avoid flash crash
 */
#ifndef E2P_PLACE_FLASH
#define E2P_ATTR ATTR_RAMFUNC
#else
#define E2P_ATTR
#endif

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
 * @brief hpm nor-flash initialization
 * 
 * @param[in] cfg config_context
 * @return hpm_stat_t
 */
hpm_stat_t nor_flash_init(nor_flash_config_t *cfg);

/**
 * @brief hpm nor-flash read function
 * 
 * @param[in] cfg config_context
 * @param[out] buf store read data
 * @param[in] addr read physical start addr
 * @param[in] size read bytes size
 * @return hpm_stat_t 
 */
hpm_stat_t nor_flash_read(nor_flash_config_t *cfg, uint8_t *buf, uint32_t addr, uint32_t size);

/**
 * @brief hpm nor-flash write function
 * 
 * @param[in] cfg config_context
 * @param[in] buf data to be written
 * @param[in] addr write physical start addr
 * @param[in] size write bytes size
 * @return hpm_stat_t 
 */
hpm_stat_t nor_flash_write(nor_flash_config_t *cfg, uint8_t *buf, uint32_t addr, uint32_t size);

/**
 * @brief hpm nor-flash erase function
 * 
 * @param[in] cfg config_context
 * @param[in] start_addr erase physical start addr
 * @param[in] size erase bytes size
 */
void nor_flash_erase(nor_flash_config_t *cfg, uint32_t start_addr, uint32_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif