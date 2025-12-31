/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e2p_device_i2c.h"

/**
 * @brief eeprom i2c supported device information list
 */
static const e2p_device_i2c_attribute_t supported_e2p_i2c_device_list[] = {
    /* Dev Addr, Addr Bytes, Page Size, Timeout, borrow_bits, comment */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 8, .timeout = 5, .borrow_bits = 0 },   /* AT24C01,AT24C02, 24xx01, 24xx02 */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 4, .borrow_bits = 0 },  /* NV24C02 */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 4, .borrow_bits = 1 },  /* NC24C04* */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 4, .borrow_bits = 2 },  /* NV24C08* */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 4, .borrow_bits = 3 },  /* NC24C16* */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 5, .borrow_bits = 0 },  /* M24C01, M24C02 */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 5, .borrow_bits = 1 },  /* M24C04*, 24xx04*, AT24C04* */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 5, .borrow_bits = 2 },  /* M24C08*, 24xx08*, AT24C08* */
    { .dev_addr = 0x50, .addr_bytes = 1, .page_size = 16, .timeout = 5, .borrow_bits = 3 },  /* M24C16*, 24xx16*, AT24C16* */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 32, .timeout = 4, .borrow_bits = 0 },  /* NV24C32 */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 32, .timeout = 10, .borrow_bits = 0 },  /* AT24C32,AT24C64 */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 32, .timeout = 5, .borrow_bits = 0 },  /* M24C32, 24xxC64, NV24C64 */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 64, .timeout = 5, .borrow_bits = 0 },  /* M24128, M24256, AT24C128, AT24C256, 24xx128, 24xx256, NV24C128, NV24C256 */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 128, .timeout = 5, .borrow_bits = 0 }, /* AT24C512, M24512, 24xx512 */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 256, .timeout = 5, .borrow_bits = 1 }, /* AT24CM01*, M24M01* */
    { .dev_addr = 0x50, .addr_bytes = 2, .page_size = 256, .timeout = 10, .borrow_bits = 2 }, /* AT24CM02*, M24M02* */
};

/* calcluate from addr_bytes
 * addr_bytes: 1 -> 256 bytes per page
 * addr_bytes: 2 -> 65536 bytes per page
 */
static const uint32_t eeprom_common_size[] = {0x100, 0x10000};

/**
 * @brief eeprom i2c device operations structure
 */
static e2p_device_ops_t hpm_e2p_i2c_device_ops = {
    .init = e2p_device_i2c_init,
    .deinit = e2p_device_i2c_deinit,
    .read = e2p_device_i2c_read,
    .write = e2p_device_i2c_write,
};

/**
 * @brief Get eeprom i2c device attribute
 */
e2p_device_i2c_attribute_t *e2p_device_i2c_get_attribute(hpm_eeprom_device_t *device)
{
    return (e2p_device_i2c_attribute_t *)(device->config);
}

/**
 * @brief Register eeprom i2c device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @param [in] host_info Pointer to eeprom i2c host configuration structure
 * @param [in] dev_id Eeprom device id
 * @retval status_success, or status_fail if dev_id is invalid
 */
hpm_stat_t e2p_device_i2c_register_param(hpm_eeprom_device_t *device, void *host_info, uint32_t dev_id)
{
    device->ops = &hpm_e2p_i2c_device_ops;
    device->host = host_info;
    e2p_device_i2c_attribute_t *config = device->config;

    uint16_t device_id = dev_id & 0xFFFF;

    if (device_id < sizeof(supported_e2p_i2c_device_list) / sizeof(e2p_device_i2c_attribute_t)) {
        memcpy(config, &supported_e2p_i2c_device_list[device_id], sizeof(e2p_device_i2c_attribute_t));
        config->dev_addr = HPM_E2P_I2C_DEV_ADDR(config->dev_addr, (dev_id >> 16) & 0xFFFF);
        return status_success;
    }  else {
        return status_fail;
    }
}

/**
 * @brief Initialize eeprom i2c device, hw init(clock and pin) and i2c master init
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success, or status_fail if initialization fails
 */
hpm_stat_t e2p_device_i2c_init(hpm_eeprom_device_t *device)
{
    hpm_stat_t stat;
    i2c_config_t config;
    uint32_t freq;
    e2p_device_i2c_host_config_t *host_info = device->host;

    freq = host_info->clock_init((I2C_Type *)host_info->base);
    host_info->pin_init();

    config.i2c_mode = i2c_mode_normal;
    config.is_10bit_addressing = false;
    stat = i2c_init_master(host_info->base, freq, &config);
    if (stat != status_success) {
        return status_fail;
    }
    return status_success;
}

/**
 * @brief Deinitialize eeprom i2c device
 */
hpm_stat_t e2p_device_i2c_deinit(hpm_eeprom_device_t *device)
{
    e2p_device_i2c_host_config_t *host_info = device->host;
    i2c_reset((I2C_Type *)host_info->base);

    return status_success;
}

/**
 * @brief Calculate the max address of eeprom device, depending on borrow_bits and addr_bytes
 * @param [in] borrow_bits Number of borrow bits
 * @param [in] addr_bytes Number of address bytes
 * @retval max_addr Maximum address of eeprom device
 */
static uint32_t e2p_device_i2c_calculate_max_addr(uint8_t borrow_bits, uint32_t addr_bytes)
{
    if ((borrow_bits > 3) || (addr_bytes < 1) || (addr_bytes > 2)) {
        return 0;
    }

    return (1UL << (addr_bytes * 8 + borrow_bits)) - 1;
}

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
 */
hpm_stat_t e2p_device_i2c_page_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    hpm_stat_t status = status_success;
    uint8_t addr[2];
    e2p_device_i2c_host_config_t *host_info = device->host;
    e2p_device_i2c_attribute_t *config = device->config;

    uint32_t current_read_addr = data_addr;
    uint32_t remaining_len = len;
    uint8_t *current_data_ptr = data;

    while (remaining_len > 0) {
        uint32_t chunk_size = (remaining_len < I2C_SOC_TRANSFER_COUNT_MAX)
                              ? remaining_len
                              : I2C_SOC_TRANSFER_COUNT_MAX;

        if (config->addr_bytes == 1) {
            addr[0] = current_read_addr & 0xFF;
        } else {
            addr[0] = (current_read_addr >> 8) & 0xFF;
            addr[1] = current_read_addr & 0xFF;
        }

        status = i2c_master_address_read(host_info->base,
                                         config->dev_addr,
                                         (uint8_t *)&addr,
                                         config->addr_bytes,
                                         current_data_ptr,
                                         chunk_size);

        if (status != status_success) {
            return status;
        }

        remaining_len -= chunk_size;
        current_read_addr += chunk_size;
        current_data_ptr += chunk_size;
    }

    return status_success;
}

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
hpm_stat_t e2p_device_i2c_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    hpm_stat_t status = status_success;
    e2p_device_i2c_attribute_t *config = device->config;

    uint32_t max_addr = e2p_device_i2c_calculate_max_addr(config->borrow_bits, config->addr_bytes);
    if ((max_addr == 0) || (len == 0) || (data_addr > max_addr) || (len > (max_addr - data_addr + 1))) {
        return status_invalid_argument;
    }

    if (config->borrow_bits == 0) {
        return e2p_device_i2c_page_read(device, data_addr, len, data);
    }

    uint32_t current_addr = data_addr;
    uint32_t remaining_len = len;
    uint8_t *current_data = data;

    uint32_t page_size = eeprom_common_size[config->addr_bytes - 1];

    uint8_t dev_addr_start = config->dev_addr;

    while (remaining_len > 0) {
        uint8_t page_index = (uint8_t)(current_addr / page_size);

        uint8_t temp_dev_addr = dev_addr_start | page_index;

        uint32_t offset_in_page = current_addr % page_size;
        uint32_t bytes_left_in_page = page_size - offset_in_page;

        uint32_t chunk_size = (remaining_len < bytes_left_in_page) ? remaining_len : bytes_left_in_page;

        if (chunk_size == 0) {
            break;
        }

        config->dev_addr = temp_dev_addr;

        status = e2p_device_i2c_page_read(device, current_addr, chunk_size, current_data);

        config->dev_addr = dev_addr_start;

        if (status != status_success) {
            return status;
        }

        remaining_len -= chunk_size;
        current_addr += chunk_size;
        current_data += chunk_size;
    }

    return status_success;
}

/**
 * @brief Check if eeprom i2c device write cycle is in progress
 * @param [in] device Pointer to eeprom device structure
 * @retval true if write cycle is in progress, false otherwise
 * @note Once the internally-timed write cycle has started and the EEPROM inputs are disabled, acknowledge polling can be initiated.
 * This involves sending a start condition followed by the device addressword.
 * The read/write bit is representative of the operation desired. Only if the internal
 * write cycle has completed will the EEPROM respond with a zero, allowing the read or write sequence to continue.
 */
static bool e2p_device_i2c_write_in_progress(hpm_eeprom_device_t *device)
{
    hpm_stat_t status;
    e2p_device_i2c_host_config_t *host_info = device->host;
    e2p_device_i2c_attribute_t *dev_config = device->config;

    status = i2c_master_write(host_info->base, dev_config->dev_addr, NULL, 0);
    if (status != status_success) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Write data to eeprom i2c device, page write, not consider borrow_bits
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success
 * or status_fail if writing fails
 * or status_timeout if timeout occurs during writing
 * or status_i2c_no_addr_hit if no acknowledge from slave device
 * or status_invalid_argument if invalid argument is detected
 */
static hpm_stat_t e2p_device_i2c_page_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    uint8_t addr[2] = { 0x0 };
    uint32_t dummy_len = 0;
    uint32_t dummy_addr = 0;
    uint8_t *dummy_data;
    e2p_device_i2c_host_config_t *host_info = device->host;
    e2p_device_i2c_attribute_t *config = device->config;
    hpm_stat_t status = status_success;
    uint32_t max_write_size = (config->page_size < I2C_SOC_TRANSFER_COUNT_MAX)
                          ? config->page_size
                          : I2C_SOC_TRANSFER_COUNT_MAX/2;
    uint32_t timeout = config->timeout;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks;

    /* calculate write size to ensure will not occur cross page */
    if (data_addr % max_write_size != 0) {
        if (data_addr / max_write_size > 0) {
            if (data_addr % max_write_size + len < max_write_size) {
                dummy_len = len;
                dummy_addr = data_addr;
                dummy_data = data;
                len = 0;
            } else {
                dummy_len = max_write_size - data_addr % max_write_size;
                dummy_addr = data_addr;
                data_addr = data_addr + dummy_len;
                dummy_data = data;
                data += dummy_len;
                len -= dummy_len;
            }
        } else {
            if (data_addr + len < max_write_size) {
                dummy_len = len;
                dummy_addr = data_addr;
                dummy_data = data;
                len = 0;
            } else {
                dummy_len = max_write_size - data_addr;
                len = len - dummy_len;
                dummy_addr = data_addr;
                dummy_data = data;
                data += dummy_len;
                data_addr = data_addr + dummy_len;
            }
        }

        if (config->addr_bytes == 1) {
            addr[0] = dummy_addr & 0xFF;
        } else {
            addr[0] = dummy_addr >> 8;
            addr[1] = dummy_addr & 0xFF;
        }

        status = i2c_master_address_write(host_info->base, config->dev_addr, (uint8_t *)&addr, config->addr_bytes, dummy_data, dummy_len);
        if (status != status_success) {
            return status;
        }

        /* calculate timeout ticks */
        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_i2c_write_in_progress(device)) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }
    }

    if (len == 0) {
        return status_success;
    }

    for (uint16_t i = 0; i < len; i += max_write_size) {
        if (config->addr_bytes == 1) {
            addr[0] = (data_addr + i) & 0xFF;
        } else {
            addr[0] = (data_addr + i) >> 8;
            addr[1] = (data_addr + i) & 0xFF;
        }

        if (i + max_write_size > len) {
            status = i2c_master_address_write(host_info->base, config->dev_addr, (uint8_t *)&addr, config->addr_bytes, data + i, len - i);
        } else {
            status = i2c_master_address_write(host_info->base, config->dev_addr, (uint8_t *)&addr, config->addr_bytes, data + i, max_write_size);
        }
        if (status != status_success) {
            return status;
        }

        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_i2c_write_in_progress(device)) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }
    }
    return status;
}

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
hpm_stat_t e2p_device_i2c_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    hpm_stat_t status = status_success;
    e2p_device_i2c_attribute_t *config = device->config;

    uint32_t max_addr = e2p_device_i2c_calculate_max_addr(config->borrow_bits, config->addr_bytes);
    if ((max_addr == 0) || (len == 0) || (data_addr > max_addr) || (len > (max_addr - data_addr + 1))) {
        return status_invalid_argument;
    }

    /* if no borrow_bits，means only one page, so no need to consider address change and can write directly. */
    if (config->borrow_bits == 0) {
        return e2p_device_i2c_page_write(device, data_addr, len, data);
    }

    uint32_t current_addr = data_addr;
    uint8_t *current_data = data;
    uint32_t remaining_len = len;

    /* the size of a full page */
    uint32_t whole_page_size = eeprom_common_size[config->addr_bytes - 1];

    /* mark the default address */
    uint8_t dev_addr_start = config->dev_addr;

    /* manually page-splitting ensures that writing occurs on the same page and does not cross pages. */
    while (remaining_len > 0) {
        /* calculation determines which page the current address is located on */
        uint8_t page_index = (uint8_t)(current_addr / whole_page_size);
        /* calculate the address of the currently written page */
        uint8_t temp_dev_addr = dev_addr_start | page_index;
        /* calculate how much free space is left on the current page */
        uint32_t bytes_left_in_page = whole_page_size - (current_addr % whole_page_size);
        /* ensure will not occur cross page */
        uint32_t chunk_size = (remaining_len < bytes_left_in_page) ? remaining_len : bytes_left_in_page;

        /* ensure has effective data wait to write, if not, stop write*/
        if (chunk_size == 0) {
            break;
        }

        config->dev_addr = temp_dev_addr;

        status = e2p_device_i2c_page_write(device, current_addr, chunk_size, current_data);

        /* initialize default address */
        config->dev_addr = dev_addr_start;

        if (status != status_success) {
            return status;
        }

        /* calculate the remaining data length, data pointer, and address. */
        remaining_len -= chunk_size;
        current_addr += chunk_size;
        current_data += chunk_size;
    }

    return status_success;

}
