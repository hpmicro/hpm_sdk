/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eeprom_hal.h"

/**
 * @brief eeprom hal context structure
 */
typedef struct {
    hpm_eeprom_device_t device[EEPROM_MAX_DEVICE_CNT]; /* eeprom device list */
    uint8_t device_count;                          /* registered device count */
    uint32_t device_bit;                       /* registered device bit map */
} eeprom_context_t;
/* eeprom hal device pool */
eeprom_context_t g_e2p_ctx;

/**
 * @brief Initialize eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success or status_fail if initialization fails
 */
hpm_stat_t hpm_eeprom_init(hpm_eeprom_device_t *device)
{
    e2p_device_ops_t *ops = device->ops;
    return ops->init(device);
}

/**
 * @brief Deinitialize eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @retval status_success
 */
hpm_stat_t hpm_eeprom_deinit(hpm_eeprom_device_t *device)
{
    e2p_device_ops_t *ops = device->ops;
    return ops->deinit(device);
}

/**
 * @brief Read data from eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to read from
 * @param [in] len Length of data to read
 * @param [in] data Pointer to buffer to store read data
 * @retval status_success if without any error
 */
hpm_stat_t hpm_eeprom_read_block(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    e2p_device_ops_t *ops = device->ops;
    return ops->read(device, data_addr, len, data);
}

/**
 * @brief Write data to eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] data_addr Data address to write to
 * @param [in] len Length of data to write
 * @param [in] data Pointer to buffer containing data to write
 * @retval status_success if without any error
 */
hpm_stat_t hpm_eeprom_write_block(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data)
{
    e2p_device_ops_t *ops = device->ops;
    return ops->write(device, data_addr, len, data);
}

/**
 * @brief Find empty device bit
 * @param [in] device_bit Device bit map
 * @retval empty device bit index
 */
static uint8_t find_empty_device_bit(uint32_t device_bit)
{
    for (uint8_t i = 0; i < 32; i++) {
        if ((device_bit & (1U << i)) == 0) {
            return i;
        }
    }
    return 0;
}

/**
 * @brief Register eeprom device
 * @param [in] device Pointer to eeprom device structure
 * @param [in] device_name Device name
 * @retval status_success
 */
hpm_stat_t hpm_eeprom_register(hpm_eeprom_device_t *device, char device_name[])
{
    uint8_t index = 0;
    if (g_e2p_ctx.device_count >= EEPROM_MAX_DEVICE_CNT) {
        return status_fail;
    } else {
        index = find_empty_device_bit(g_e2p_ctx.device_bit);
        g_e2p_ctx.device_bit |= (1 << index);
        memcpy(g_e2p_ctx.device[index].device_name, device_name, E2P_DEVICE_NAME_MAX_LEN);
    }

    g_e2p_ctx.device[index].host = device->host;
    g_e2p_ctx.device[index].flag = device->flag;
    g_e2p_ctx.device[index].config = device->config;
    g_e2p_ctx.device[index].ops = device->ops;

    g_e2p_ctx.device_count++;
    return status_success;
}

/**
 * @brief Unregister eeprom device
 * @param [in] device_name Device name
 * @retval status_success or status_fail if unregister fails
 */
hpm_stat_t hpm_eeprom_unregister(char device_name[])
{
    for (uint32_t i = 0; i < EEPROM_MAX_DEVICE_CNT; i++) {
        if (strcmp(g_e2p_ctx.device[i].device_name, device_name) == 0) {
            e2p_device_ops_t *ops = g_e2p_ctx.device[i].ops;
            if (ops->deinit != NULL) {
                ops->deinit(&g_e2p_ctx.device[i]);
            }
            g_e2p_ctx.device_bit &= ~(1 << i);
            memset(&g_e2p_ctx.device[i], 0, sizeof(hpm_eeprom_device_t));
            g_e2p_ctx.device_count--;
            return status_success;
        }
    }
    return status_fail;
}

/**
 * @brief Find eeprom device by device name
 * @param [in] device_name Device name
 * @retval Pointer to eeprom device structure
 */
hpm_eeprom_device_t *hpm_eeprom_find(char device_name[])
{
    for (uint32_t i = 0; i < EEPROM_MAX_DEVICE_CNT; i++) {
        if (strcmp(g_e2p_ctx.device[i].device_name, device_name) == 0) {
            return &g_e2p_ctx.device[i];
        }
    }
    return NULL;
}
