/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e2p_emulation.h"

/* eeprom emulation handle */
e2p_t hpm_e2p_emulation;

/**
 * @brief eeprom xpi emulation device operations structure
 */
static e2p_device_ops_t hpm_e2p_xpi_device_ops = {
    .init = e2p_emulation_xpi_init,
    .deinit = NULL,
    .read = e2p_emulation_xpi_read,
    .write = e2p_emulation_xpi_write,
};

/**
 * @brief Flash read function for eeprom emulation
 * @param [in] buf Pointer to buffer to store read data
 * @param [in] addr Address to read from
 * @param [in] size Size of data to read
 * @retval status_success
 */
static uint32_t flash_read(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_read(&hpm_e2p_emulation.nor_config, buf, addr, size);
}

/**
 * @brief Flash write function for eeprom emulation
 * @param [in] buf Pointer to buffer containing data to write
 * @param [in] addr Address to write to
 * @param [in] size Size of data to write
 * @retval status_success
 */
static uint32_t flash_write(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_write(&hpm_e2p_emulation.nor_config, buf, addr, size);
}

/**
 * @brief Flash erase function for eeprom emulation
 * @param [in] start_addr Start address to erase
 * @param [in] size Size of data to erase
 */
static void flash_erase(uint32_t start_addr, uint32_t size)
{
    nor_flash_erase(&hpm_e2p_emulation.nor_config, start_addr, size);
}

/**
 * @brief Enter critical section by disabling global interrupts
 */
static uint32_t eeprom_enter_critical(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

/**
 * @brief Exit critical section by restoring global interrupts
 */
static void eeprom_exit_critical(uint32_t level)
{
    restore_global_irq(level);
}

/**
 * @brief Register eeprom xpi emulation device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t e2p_emulation_xpi_register_param(hpm_eeprom_device_t *device)
{
    e2p_emu_xpi_host_config_t *host_config = device->host;
    e2p_emu_xpi_attribute_config_t *config = device->config;
    hpm_e2p_emulation.nor_config.xpi_base = host_config->base;
    hpm_e2p_emulation.nor_config.base_addr = config->flash_base_addr;
    hpm_e2p_emulation.config.start_addr = hpm_e2p_emulation.nor_config.base_addr + config->start_addr;
    hpm_e2p_emulation.config.erase_size = config->erase_size;
    hpm_e2p_emulation.config.sector_cnt = config->sector_cnt;
    hpm_e2p_emulation.config.version = config->version; /* 'E' 'S' */
    hpm_e2p_emulation.nor_config.opt_header = config->opt_header;
    hpm_e2p_emulation.nor_config.opt0 = config->opt0;
    hpm_e2p_emulation.nor_config.opt1 = config->opt1;
    hpm_e2p_emulation.config.flash_read = flash_read;
    hpm_e2p_emulation.config.flash_write = flash_write;
    hpm_e2p_emulation.config.flash_erase = flash_erase;

    device->ops = &hpm_e2p_xpi_device_ops;

    return status_success;
}

/**
 * @brief Initialize eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success or status_fail if initialization fails
 */
hpm_stat_t e2p_emulation_xpi_init(hpm_eeprom_device_t *device)
{
    ARG_UNUSED(device);
    hpm_stat_t stat;

    uint32_t level = eeprom_enter_critical();
    stat = nor_flash_init(&hpm_e2p_emulation.nor_config);
    eeprom_exit_critical(level);
    if (status_success != stat) {
        return status_fail;
    }

    level = eeprom_enter_critical();
    stat = e2p_config(&hpm_e2p_emulation);
    eeprom_exit_critical(level);
    if (E2P_STATUS_OK != stat) {
        return status_fail;
    }

    return status_success;
}

/**
 * @brief Read data from eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @retval status_success or status_fail if read fails
 */
hpm_stat_t e2p_emulation_xpi_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    ARG_UNUSED(device);
    if (e2p_read(data_addr, len, data) != E2P_STATUS_OK) {
        return status_fail;
    } else {
        return status_success;
    }
}

/**
 * @brief Write data to eeprom xpi emulation device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success or status_fail if write fails
 */
hpm_stat_t e2p_emulation_xpi_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    ARG_UNUSED(device);
    if (e2p_write(data_addr, len, data) != E2P_STATUS_OK) {
        return status_fail;
    } else {
        return status_success;
    }
}

