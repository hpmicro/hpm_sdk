/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MODBUS_MEM_H
#define MODBUS_MEM_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Initialize register mutex for thread safety
 */
void modbus_mem_init(void);

/**
 * @brief Deinitialize register mutex
 */
void modbus_mem_deinit(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MODBUS_MEM_H */
