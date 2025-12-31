/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e2p_device_spi.h"

/**
 * @brief eeprom spi supported device information list
 */
static const e2p_device_spi_attribute_t supported_e2p_spi_device_list[] = {
    /* Addr Bytes, Page Size, Timeout, Max Freq (Hz), borrow_bits, comment */
    { .addr_bytes = 1, .page_size = 8, .timeout = 5, .max_freq = 5000000, .borrow_bits = 0 }, /* AT25010B, AT25020B */
    { .addr_bytes = 1, .page_size = 8, .timeout = 5, .max_freq = 5000000, .borrow_bits = 1 }, /* AT25040B* */
    { .addr_bytes = 1, .page_size = 16, .timeout = 5, .max_freq = 20000000, .borrow_bits = 0 }, /* M95010, M95020 */
    { .addr_bytes = 1, .page_size = 16, .timeout = 5, .max_freq = 20000000, .borrow_bits = 1 }, /* M95040* */
    { .addr_bytes = 1, .page_size = 16, .timeout = 4, .max_freq = 5000000, .borrow_bits = 0 }, /* S-25C010A, S-25C020A */
    { .addr_bytes = 1, .page_size = 16, .timeout = 4, .max_freq = 5000000, .borrow_bits = 1 }, /* S-25C040A* */
    { .addr_bytes = 1, .page_size = 16, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* 25xx010A, 25xx020A, NV25010, NV25020 */
    { .addr_bytes = 1, .page_size = 16, .timeout = 5, .max_freq = 10000000, .borrow_bits = 1 }, /* 25xx040A*, NV25040* */
    { .addr_bytes = 2, .page_size = 16, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* 25xx080A, 25xx160A */
    { .addr_bytes = 2, .page_size = 32, .timeout = 5, .max_freq = 20000000, .borrow_bits = 0 }, /* M95080, M95160, M95320, M95640, AT25080B, AT25160B, AT25320B, AT25640B */
    { .addr_bytes = 2, .page_size = 32, .timeout = 4, .max_freq = 5000000, .borrow_bits = 0 }, /* S-25C080A */
    { .addr_bytes = 2, .page_size = 32, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* 25xx080B, 25xx160B, 25xx320A, 25xx640A, NV25080, NV25160, NV25320, NV25640 */
    { .addr_bytes = 2, .page_size = 32, .timeout = 5, .max_freq = 5000000, .borrow_bits = 0 }, /* S-25C160A, S-25C320A, S-25C640A */
    { .addr_bytes = 2, .page_size = 64, .timeout = 5, .max_freq = 5000000, .borrow_bits = 0 }, /* S-25C128A */
    { .addr_bytes = 2, .page_size = 64, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* 25xx128, 25xx256, NV25128, NV25256, S-25C256A */
    { .addr_bytes = 2, .page_size = 64, .timeout = 5, .max_freq = 20000000, .borrow_bits = 0 }, /* AT25128B, AT25256B, M95128, M95256 */
    { .addr_bytes = 2, .page_size = 128, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* S-25C512A, NV25512 */
    { .addr_bytes = 2, .page_size = 128, .timeout = 5, .max_freq = 16000000, .borrow_bits = 0 }, /* M95512 */
    { .addr_bytes = 2, .page_size = 128, .timeout = 5, .max_freq = 20000000, .borrow_bits = 0 }, /* 25xx512 */
    { .addr_bytes = 3, .page_size = 256, .timeout = 5, .max_freq = 16000000, .borrow_bits = 0 }, /* M95M01 */
    { .addr_bytes = 3, .page_size = 256, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* NV25M01, S-25CM01A */
    { .addr_bytes = 3, .page_size = 256, .timeout = 10, .max_freq = 5000000, .borrow_bits = 0 }, /* M95M02 */
    { .addr_bytes = 3, .page_size = 256, .timeout = 6, .max_freq = 20000000, .borrow_bits = 0 }, /* 25xx1024 */
    { .addr_bytes = 3, .page_size = 512, .timeout = 5, .max_freq = 10000000, .borrow_bits = 0 }, /* M95M04 */
};

/**
 * @brief eeprom spi device operations structure
 */
static e2p_device_ops_t hpm_e2p_spi_device_ops = {
    .init = e2p_device_spi_init,
    .deinit = e2p_device_spi_deinit,
    .read = e2p_device_spi_read,
    .write = e2p_device_spi_write,
};

/**
 * @brief Get eeprom spi device attribute
 * @param [in] device Pointer to eeprom device structure
 * @retval Pointer to eeprom spi device attribute structure
 */
e2p_device_spi_attribute_t *e2p_device_spi_get_attribute(hpm_eeprom_device_t *device)
{
    return (e2p_device_spi_attribute_t *)(device->config);
}

/**
 * @brief Register eeprom spi device parameter to device structure
 * @param [in] device Pointer to eeprom device structure
 * @param [in] host_info Pointer to eeprom spi host configuration structure
 * @param [in] dev_id Eeprom device id
 * @retval status_success or status_fail if dev_id is invalid
 */
hpm_stat_t e2p_device_spi_register_param(hpm_eeprom_device_t *device, void *host_info, uint32_t dev_id)
{
    device->ops = &hpm_e2p_spi_device_ops;
    device->host = host_info;

    if (dev_id < sizeof(supported_e2p_spi_device_list) / sizeof(e2p_device_spi_attribute_t)) {
        memcpy(device->config, &supported_e2p_spi_device_list[dev_id], sizeof(e2p_device_spi_attribute_t));
        return status_success;
    } else {
        return status_fail;
    }
}

/**
 * @brief Initialize eeprom spi device, hw init(clock and pin) and spi master init
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 * or status_fail if initialization fails
 * or status_invalid_argument if configuration is invalid
 */
hpm_stat_t e2p_device_spi_init(hpm_eeprom_device_t *device)
{
    hpm_stat_t status = status_success;
    uint32_t spi_clock;
    e2p_device_spi_host_config_t *host_info = device->host;
    e2p_device_spi_attribute_t *dev_config = device->config;

    spi_timing_config_t timing_config = { 0 };
    spi_format_config_t format_config = { 0 };

    spi_clock = host_info->clock_init(host_info->base);
    host_info->pin_init();

    spi_master_get_default_timing_config(&timing_config);
    /* clk_src_freq_in_hz -> src default clock, sclk_freq_in_hz -> device max clock */
    timing_config.master_config.clk_src_freq_in_hz = spi_clock;
    timing_config.master_config.sclk_freq_in_hz = dev_config->max_freq;
    status = spi_master_timing_init(host_info->base, &timing_config);
    if (status != status_success) {
        return status;
    }

    spi_master_get_default_format_config(&format_config);
    format_config.master_config.addr_len_in_bytes = dev_config->addr_bytes;
    format_config.common_config.data_len_in_bits = BOARD_APP_SPI_DATA_LEN_IN_BITS;
    format_config.common_config.mode = spi_master_mode;
    /* default mode 0 */
    format_config.common_config.cpol = spi_sclk_low_idle;
    format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges;
    spi_format_init(host_info->base, &format_config);

    return status;
}

/**
 * @brief De-initialize eeprom spi device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t e2p_device_spi_deinit(hpm_eeprom_device_t *device)
{
    e2p_device_spi_host_config_t *host_info = device->host;
    spi_reset(host_info->base);

    return status_success;
}

/**
 * @brief Read data from eeprom spi device from one page
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @param [in] is_cross_page Indicate whether the read operation crosses a page boundary
 * @retval status_success
 * or status_fail if read operation fails
 * or status_invalid_argument if configuration is invalid
 * or status_spi_master_busy if spi bus is busy
 * or status_timeout if spi transfer timeout
 * @note if is_cross_page is true, the MSB address bit will be set in the instruction;
 * @note Reading the device via the SO pin requires the following sequence. After the C̅S̅ line is pulled low to select a device,
 * the Read op-code is transmitted via the SI line followed by the byte address to be read.
 * Upon completion, any data on the SI line will be ignored. The data (D7 - D0) at the specified address is then shifted out onto the SO line.
 * If only one byte is to be read, the C̅S̅ line should be driven high after the data comes out.
 * The read sequence can be continued since the byte address is automatically incremented and data will continue to be shifted out. When the highest address is reached,
 * the address counter will roll over to the lowest address allowing the entire memory to be read in one continuous read cycle.
 */
static hpm_stat_t e2p_device_spi_page_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data, bool is_cross_page)
{
    uint8_t cmd;
    e2p_device_spi_host_config_t *host_info = device->host;
    spi_control_config_t control_config = { 0 };

    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = true;
    control_config.master_config.addr_enable = true;
    control_config.common_config.trans_mode = spi_trans_read_only;

    /* some spi eeprom MSB address bit will contained on instruction, more details on list information */
    if (is_cross_page) {
        cmd = E2P_SPI_INSTRUCTION_READ | 0x08;
    } else {
        cmd = E2P_SPI_INSTRUCTION_READ;
    }

    host_info->cs_control(host_info->cs_pin, false);
    hpm_stat_t status = spi_transfer(host_info->base, &control_config, &cmd, &data_addr, NULL, 0, (uint8_t *)data, len);
    host_info->cs_control(host_info->cs_pin, true);

    return status;
}

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
hpm_stat_t e2p_device_spi_read(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    e2p_device_spi_attribute_t *dev_config = device->config;
    uint8_t borrow_bits = dev_config->borrow_bits;

    if (borrow_bits == 0 || (borrow_bits == 1 && data_addr < 0xFF && (data_addr + len - 1) < 0xFF)) {
        return e2p_device_spi_page_read(device, data_addr, len, data, false);
    } else if (borrow_bits == 1 && data_addr > 0xFF && (data_addr + len - 1) > 0xFF) {
        return e2p_device_spi_page_read(device, data_addr, len, data, true);
    } else if (borrow_bits == 1 && data_addr < 0xFF && (data_addr + len - 1) > 0xFF) {
        uint32_t first_page_len = 0x100 - data_addr;
        hpm_stat_t status;

        status = e2p_device_spi_page_read(device, data_addr, first_page_len, data, false);
        if (status != status_success) {
            return status;
        }

        data_addr += first_page_len;
        data += first_page_len;
        len -= first_page_len;

        status = e2p_device_spi_page_read(device, data_addr, len, data, true);
        return status;
    } else {
        return status_invalid_argument;
    }
}

/**
 * @brief Write Enable instruction to set the write enable latch (WEL) bit in the status register to a logic 1.
 * @param [in] device Pointer to eeprom device structure
 * @param [in] control_config Pointer to spi control configuration structure
 * @retval status_success
 * or status_fail if read operation fails
 * or status_invalid_argument if configuration is invalid
 * or status_spi_master_busy if spi bus is busy
 * or status_timeout if spi transfer timeout
 * @note The device will power-up in the write disable state when VCC is applied.
 * All programming instructions must therefore be preceded by a Write Enable instruction.
 * After proceed a write enable instruction, ready and write enable status of the device
 * can be read by the RDSR instruction.
 */
static hpm_stat_t e2p_device_spi_write_enable(hpm_eeprom_device_t *device, spi_control_config_t *control_config)
{
    e2p_device_spi_host_config_t *host_info = device->host;

    control_config->master_config.cmd_enable = true;
    control_config->master_config.addr_enable = false;

    uint8_t cmd = E2P_SPI_INSTRUCTION_WREN;
    control_config->common_config.trans_mode = spi_trans_no_data;

    host_info->cs_control(host_info->cs_pin, false);
    if (spi_transfer(host_info->base, control_config, &cmd, NULL, NULL, 0, NULL, 0) != status_success) {
        return status_fail;
    }
    host_info->cs_control(host_info->cs_pin, true);

    cmd = E2P_SPI_INSTRUCTION_RDSR;
    control_config->common_config.trans_mode = spi_trans_write_read_together;
    uint8_t wr[1] = { 0x0 };
    uint8_t rd[1] = { 0x0 };
    host_info->cs_control(host_info->cs_pin, false);
    if (spi_transfer(host_info->base, control_config, &cmd, NULL, (uint8_t *)wr, sizeof(wr), (uint8_t *)rd, sizeof(rd) != status_success)) {
        return status_fail;
    }
    host_info->cs_control(host_info->cs_pin, true);

    if (rd[0] & E2P_SPI_INSTRUCTION_STATUS_WEL_MASK) {
        return status_success;
    } else {
        return status_fail;
    }
}

/**
 * @brief Read data from eeprom spi device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] control_config Pointer to spi control configuration structure
 * @retval true Write in progress; false Write cycle completed
 * @note After Write instruction, the Ready/Busy status of the device can be determined by initiating a Read Status Register (RDSR) instruction.
 * If Bit 0 = 1, the Write cycle is still in progress. If Bit 0 = 0, the Write cycle has ended.
 */
static bool e2p_device_spi_write_in_progress(hpm_eeprom_device_t *device, spi_control_config_t *control_config)
{
    e2p_device_spi_host_config_t *host_info = device->host;

    uint8_t cmd = E2P_SPI_INSTRUCTION_RDSR;
    control_config->master_config.addr_enable = false;
    control_config->common_config.trans_mode = spi_trans_write_read_together;
    uint8_t wr[1] = { 0x0 };
    uint8_t rd[1] = { 0x0 };
    host_info->cs_control(host_info->cs_pin, false);
    if (spi_transfer(host_info->base, control_config, &cmd, NULL, (uint8_t *)wr, sizeof(wr), (uint8_t *)rd, sizeof(rd) != status_success)) {
        return status_fail;
    }
    host_info->cs_control(host_info->cs_pin, true);

    if ((rd[0] & E2P_SPI_INSTRUCTION_STATUS_WIP_MASK) == 0x00) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief Write data to eeprom spi device from one page
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer to store write data
 * @param [in] is_cross_page Indicate whether the write operation crosses a page boundary
 * @retval status_success
 * or status_fail if read operation fails
 * or status_invalid_argument if configuration is invalid
 * or status_spi_master_busy if spi bus is busy
 * or status_timeout if spi transfer timeout
 * @note if is_cross_page is true, the MSB address bit will be set in the instruction;
 * @note In order to program the device, two separate instructions must be executed.
 * First, the device must be write enabled via the Write Enable (WREN) instruction.
 * Then a Write instruction may be executed. During an internal write cycle, all commands will be ignored except the RDSR instruction.
 * A Write Instruction requires the following sequence. After the C̅S̅ line is pulled low to select the device,
 * the Write op-code is transmitted via the SI line followed by the byte address and the data (D7 - D0) to be programmed.
 * Programming will start after the C̅S̅ pin is brought high.
 */
static hpm_stat_t e2p_device_spi_page_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data, bool is_cross_page)
{
    hpm_stat_t status;
    uint32_t dummy_len = 0;
    uint32_t dummy_addr = 0;
    uint8_t *dummy_data;
    uint8_t cmd;
    spi_control_config_t control_config = { 0 };
    e2p_device_spi_host_config_t *host_info = device->host;
    e2p_device_spi_attribute_t *dev_config = device->config;
    uint32_t max_write_size = dev_config->page_size;
    uint32_t timeout = dev_config->timeout;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks;

    spi_master_get_default_control_config(&control_config);

    /* some spi eeprom MSB address bit will contained on instruction, more details on list information */
    if (is_cross_page) {
        cmd = E2P_SPI_INSTRUCTION_WRITE | 0x08;
    } else {
        cmd = E2P_SPI_INSTRUCTION_WRITE;
    }

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

        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_spi_write_enable(device, &control_config) != status_success) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }

        control_config.master_config.cmd_enable = true;
        control_config.master_config.addr_enable = true;
        control_config.common_config.trans_mode = spi_trans_write_only;

        host_info->cs_control(host_info->cs_pin, false);
        status = spi_transfer(host_info->base, &control_config, &cmd, &dummy_addr, (uint8_t *)dummy_data, dummy_len, NULL, 0);
        host_info->cs_control(host_info->cs_pin, true);

        if (status != status_success) {
            return status;
        }

        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_spi_write_in_progress(device, &control_config)) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }
    }

    if (len == 0) {
        return status_success;
    }

    for (uint32_t i = 0; i < len; i += max_write_size) {
        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_spi_write_enable(device, &control_config) != status_success) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }

        control_config.master_config.cmd_enable = true;
        control_config.master_config.addr_enable = true;
        control_config.common_config.trans_mode = spi_trans_write_only;

        host_info->cs_control(host_info->cs_pin, false);
        if (i + max_write_size > len) {
            status = spi_transfer(host_info->base, &control_config, &cmd, &data_addr, (uint8_t *)data + i, len - i, NULL, 0);
        } else {
            status = spi_transfer(host_info->base, &control_config, &cmd, &data_addr, (uint8_t *)data + i, max_write_size, NULL, 0);
            data_addr += max_write_size;
        }
        host_info->cs_control(host_info->cs_pin, true);

        if (status != status_success) {
            return status;
        }

        expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
        while (e2p_device_spi_write_in_progress(device, &control_config)) {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }
    }

    return status;
}

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
hpm_stat_t e2p_device_spi_write(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    e2p_device_spi_attribute_t *dev_config = device->config;
    uint8_t borrow_bits = dev_config->borrow_bits;

    /* no cross page and all data write on one page */
    if (borrow_bits == 0 || (borrow_bits == 1 && data_addr < 0xFF && (data_addr + len - 1) < 0xFF)) {
        return e2p_device_spi_page_write(device, data_addr, len, data, false);
    /* has cross page but data all write on seconnd page */
    } else if (borrow_bits == 1 && data_addr > 0xFF && (data_addr + len - 1) > 0xFF) {
        return e2p_device_spi_page_write(device, data_addr, len, data, true);
    /* has cross page and data write on first and seconnd page, so write them separately */
    } else if (borrow_bits == 1 && data_addr < 0xFF && (data_addr + len - 1) > 0xFF) {
        uint32_t first_page_len = 0x100 - data_addr;
        hpm_stat_t status;

        status = e2p_device_spi_page_write(device, data_addr, first_page_len, data, false);
        if (status != status_success) {
            return status;
        }

        data_addr += first_page_len;
        data += first_page_len;
        len -= first_page_len;

        status = e2p_device_spi_page_write(device, data_addr, len, data, true);
        return status;
    } else {
        return status_invalid_argument;
    }
}
