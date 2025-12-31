/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "agile_modbus.h"
#include "agile_modbus_slave_util.h"
#include "FreeRTOS.h"
#include "semphr.h"

static uint16_t _tab_registers[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static SemaphoreHandle_t register_mutex = NULL;

static int get_map_buf(void *buf, int bufsz)
{
    (void)bufsz;
    uint16_t *ptr = (uint16_t *)buf;

    if (register_mutex != NULL) {
        if (xSemaphoreTake(register_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            for (uint32_t i = 0; i < sizeof(_tab_registers) / sizeof(_tab_registers[0]); i++) {
                ptr[i] = _tab_registers[i];
            }
            xSemaphoreGive(register_mutex);
        } else {
            /* Timeout occurred */
            return -1;
        }
    } else {
        /* Fallback for non-FreeRTOS environment */
        for (uint32_t i = 0; i < sizeof(_tab_registers) / sizeof(_tab_registers[0]); i++) {
            ptr[i] = _tab_registers[i];
        }
    }
    return 0;
}

static int set_map_buf(int index, int len, void *buf, int bufsz)
{
    (void)bufsz;
    uint16_t *ptr = (uint16_t *)buf;

    if (register_mutex != NULL) {
        if (xSemaphoreTake(register_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            for (int i = 0; i < len; i++) {
                _tab_registers[index + i] = ptr[index + i];
            }
            xSemaphoreGive(register_mutex);
        } else {
            /* Timeout occurred */
            return -1;
        }
    } else {
        /* Fallback for non-FreeRTOS environment */
        for (int i = 0; i < len; i++) {
            _tab_registers[index + i] = ptr[index + i];
        }
    }
    return 0;
}

const agile_modbus_slave_util_map_t hold_register_maps[1] = { { 0x00, 0x09, get_map_buf, set_map_buf } };

/**
 * @brief Initialize register mutex for thread safety
 */
void modbus_mem_init(void)
{
    if (register_mutex == NULL) {
        register_mutex = xSemaphoreCreateMutex();
        if (register_mutex == NULL) {
            /* Failed to create mutex, but we can still work without it */
            printf("Warning: Failed to create register mutex\n");
        }
    }
}

/**
 * @brief Deinitialize register mutex
 */
void modbus_mem_deinit(void)
{
    if (register_mutex != NULL) {
        vSemaphoreDelete(register_mutex);
        register_mutex = NULL;
    }
}
