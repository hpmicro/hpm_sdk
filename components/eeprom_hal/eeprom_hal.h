/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _EEPROM_HAL_H
#define _EEPROM_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "eeprom_device.h"

/** @brief eeprom support max device count */
#define EEPROM_MAX_DEVICE_CNT 32

/**
 * @brief Register eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] device_name Device name
 * @retval status_success
 */
hpm_stat_t hpm_eeprom_register(hpm_eeprom_device_t *device, char device_name[]);

/**
 * @brief Unregister eeprom device
 * @param [in] device_name Device name
 * @retval status_success or status_fail if unregister fails
 */
hpm_stat_t hpm_eeprom_unregister(char device_name[]);

/**
 * @brief Find eeprom device by device name
 * @param [in] device_name Device name
 * @retval Pointer to eeprom device structure
 */
hpm_eeprom_device_t *hpm_eeprom_find(char device_name[]);

/**
 * @brief Initialize eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success or status_fail if initialization fails
 */
hpm_stat_t hpm_eeprom_init(hpm_eeprom_device_t *device);

/**
 * @brief Deinitialize eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t hpm_eeprom_deinit(hpm_eeprom_device_t *device);

/**
 * @brief Read data from eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @retval status_success if without any error
 */
hpm_stat_t hpm_eeprom_read_block(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

/**
 * @brief Write data to eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success if without any error
 */
hpm_stat_t hpm_eeprom_write_block(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif