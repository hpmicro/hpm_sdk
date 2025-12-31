/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _E2P_DEVICE_SPI_H
#define _E2P_DEVICE_SPI_H

#include "board.h"
#include "hpm_spi_drv.h"
#include "eeprom_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief eeprom spi device support register instruction
 */
#define E2P_SPI_INSTRUCTION_WREN 0x06
#define E2P_SPI_INSTRUCTION_RDSR 0x05
#define E2P_SPI_INSTRUCTION_WRDI 0x04
#define E2P_SPI_INSTRUCTION_READ 0x03
#define E2P_SPI_INSTRUCTION_WRITE 0x02
#define E2P_SPI_INSTRUCTION_WRSR 0x01

/**
 * @brief eeprom spi device status register bit mask
 */
#define E2P_SPI_INSTRUCTION_STATUS_WIP_MASK 0x01
#define E2P_SPI_INSTRUCTION_STATUS_WEL_MASK 0x02

/**
 * @brief eeprom spi device board configuration structure
 */
typedef struct {
    SPI_Type *base;                    /* SPI base address */
    uint32_t cs_pin;                  /* Chip select pin */
    uint32_t (*clock_init)(SPI_Type *ptr); /* Function to initialize SPI clock */
    void (*pin_init)(void);                /* Function to initialize SPI pins */
    void (*cs_control)(uint32_t pin, uint8_t state); /* Function to control chip select pin */
} e2p_device_spi_host_config_t;

/**
 * @brief eeprom spi device attribute structure
 */
typedef struct {
    uint32_t addr_bytes;  /* Number of address bytes */
    uint32_t page_size;  /* Page size in bytes */
    uint32_t timeout;   /* Write cycle timeout in milliseconds */
    uint32_t max_freq; /* Maximum supported frequency */
    uint8_t borrow_bits; /* Number of borrow bits */
} e2p_device_spi_attribute_t;

/**
 * @brief Register eeprom spi device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @param [in] host_info Pointer to eeprom spi host configuration structure
 * @param [in] dev_id Eeprom device id
 * @retval status_success or status_fail if dev_id is invalid
 */
hpm_stat_t e2p_device_spi_register_param(hpm_eeprom_device_t *device, void *host_info, uint32_t dev_id);

/**
 * @brief Get eeprom spi device attribute
 * @param [in] device Pointer to eeprom device structure
 * @retval Pointer to eeprom spi device attribute structure
 */
e2p_device_spi_attribute_t *e2p_device_spi_get_attribute(hpm_eeprom_device_t *device);

/**
 * @brief Initialize eeprom spi device, hw init(clock and pin) and spi master init
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 * or status_fail if initialization fails
 * or status_invalid_argument if configuration is invalid
 */
hpm_stat_t e2p_device_spi_init(hpm_eeprom_device_t *device);

/**
 * @brief De-initialize eeprom spi device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t e2p_device_spi_deinit(hpm_eeprom_device_t *device);

/**
 * @brief Read data from eeprom spi device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @retval status_success
 * or status_fail if read operation fails
 * or status_invalid_argument if configuration is invalid
 * or status_spi_master_busy if spi bus is busy
 * or status_timeout if spi transfer timeout
 */
hpm_stat_t e2p_device_spi_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

/**
 * @brief Write data to eeprom spi device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success
 * or status_fail if read operation fails
 * or status_invalid_argument if configuration is invalid
 * or status_spi_master_busy if spi bus is busy
 * or status_timeout if spi transfer timeout
 * @note This function will handle page splitting and borrow bits internally, and call e2p_device_spi_page_write; so the user does not need to worry about these details.
 */
hpm_stat_t e2p_device_spi_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif