/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _E2P_EMULATION_H
#define _E2P_EMULATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "eeprom_emulation.h"
#include "eeprom_device.h"

/**
 * @brief eeprom emulation xpi board configuration structure
 */
typedef struct {
    XPI_Type *base; /* xpi base */
} e2p_emu_xpi_host_config_t;

/**
 * @brief eeprom emulation xpi flash configuration structure
 */
typedef struct {
    uint32_t flash_base_addr; /* flash base address */
    uint32_t version;        /* version 'E''S' */
    uint32_t opt_header;    /* option header */
    uint32_t opt0;       /* option0 */
    uint32_t opt1;      /* option1 */
    uint32_t start_addr;    /* eeprom emulation start address */
    uint32_t erase_size;    /* erase size */
    uint32_t sector_cnt;    /* sector count */
} e2p_emu_xpi_attribute_config_t;

/**
 * @brief Register eeprom xpi emulation device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t e2p_emulation_xpi_register_param(hpm_eeprom_device_t *device);

/**
 * @brief Initialize eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success or status_fail if initialization fails
 */
hpm_stat_t e2p_emulation_xpi_init(hpm_eeprom_device_t *device);

/**
 * @brief Read data from eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @retval status_success or status_fail if read fails
 */
hpm_stat_t e2p_emulation_xpi_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

/**
 * @brief Write data to eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success or status_fail if write fails
 */
hpm_stat_t e2p_emulation_xpi_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif