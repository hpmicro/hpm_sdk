/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _E2P_DEVICE_I2C_H
#define _E2P_DEVICE_I2C_H

#include "board.h"
#include "hpm_i2c_drv.h"
#include "eeprom_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief eeprom i2c device address definition macro
 */
#define HPM_E2P_I2C_DEV_ADDR(family_id, custom_id) ((uint8_t)(family_id) | (uint8_t)(custom_id))
#define HPM_E2P_I2C_ADD_CUSTOM_ID_TO_DEV_ID(dev_id, custom_id) ((((uint32_t)(custom_id)) << 16) | ((uint32_t)(dev_id)))

/**
 * @brief eeprom i2c device board configuration structure
 */
typedef struct {
    I2C_Type *base;                     /* I2C base address */
    uint32_t (*clock_init)(I2C_Type *ptr); /* Function to initialize I2C clock */
    void (*pin_init)(void);                 /* Function to initialize I2C pins */
} e2p_device_i2c_host_config_t;

/**
 * @brief eeprom i2c device configuration structure
 */
typedef struct {
    uint32_t dev_addr;      /* Device i2c address */
    uint32_t addr_bytes;   /* Number of address bytes */
    uint32_t page_size;   /* Page size in bytes */
    uint32_t timeout;     /* Write cycle timeout in milliseconds */
    uint8_t borrow_bits; /* Number of borrow bits */
} e2p_device_i2c_attribute_t;


/**
 * @brief Register eeprom i2c device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @param [in] host_info Pointer to eeprom i2c host configuration structure
 * @param [in] dev_id Eeprom device id
 * @retval status_success, or status_fail if dev_id is invalid
 */
hpm_stat_t e2p_device_i2c_register_param(hpm_eeprom_device_t *device, void *host_info, uint32_t dev_id);

/**
 * @brief Get eeprom i2c device attribute
 * @param [in] device Pointer to eeprom device structure
 * @retval Pointer to eeprom i2c device attribute structure
 */
e2p_device_i2c_attribute_t *e2p_device_i2c_get_attribute(hpm_eeprom_device_t *device);

/**
 * @brief Initialize eeprom i2c device, hw init(clock and pin) and i2c master init
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success, or status_fail if initialization fails
 */
hpm_stat_t e2p_device_i2c_init(hpm_eeprom_device_t *device);

/**
 * @brief De-initialize eeprom i2c device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t e2p_device_i2c_deinit(hpm_eeprom_device_t *device);

/**
 * @brief Read data from eeprom i2c device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [out] data Pointer to buffer to store read data
 * @retval status_success
 * or status_fail if reading fails
 * or status_invalid_argument if invalid argument is detected
 * or status_timeout if timeout occurs during reading
 * or status_i2c_no_addr_hit if no acknowledge from slave device
 * @note This function will handle page splitting and borrow bits internally, and call e2p_device_i2c_page_read; so the user does not need to worry about these details.
 */
hpm_stat_t e2p_device_i2c_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

/**
 * @brief Write data to eeprom i2c device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success
 * or status_fail if writing fails
 * or status_timeout if timeout occurs during writing
 * or status_i2c_no_addr_hit if no acknowledge from slave device
 * or status_invalid_argument if invalid argument is detected
 * @note This function will handle page splitting and borrow bits internally, and call e2p_device_i2c_page_write; so the user does not need to worry about these details.
 */
hpm_stat_t e2p_device_i2c_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif