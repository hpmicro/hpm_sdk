/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_DEBUG_H
#define HPM_MCL_DEBUG_H

#include "hpm_mcl_common.h"

#define MCL_DEBUG_FIFO  MCL_USER_DEFINED_DEBUG_FIFO

/**
 * @brief callback function
 *
 */
typedef struct {
    void (*write_buf)(uint8_t *byte, uint16_t num);
} mcl_debug_callback_t;

#define MCL_DEBUG_DATA_TYPE_SET(num, size) (((num) << 8) | (size))
#define MCL_DEBUG_DATA_TYPE_GET_SIZE(type) ((type) & 0xff)

/**
 * @brief Types of debug data
 *
 */
typedef enum {
    mcl_debug_data_i8 = MCL_DEBUG_DATA_TYPE_SET(0, 1),
    mcl_debug_data_i16 = MCL_DEBUG_DATA_TYPE_SET(1, 2),
    mcl_debug_data_i32 = MCL_DEBUG_DATA_TYPE_SET(2, 4),
    mcl_debug_data_f32 = MCL_DEBUG_DATA_TYPE_SET(3, 4)
} mcl_debug_data_type_t;

/**
 * @brief debug config
 *
 */
typedef struct {
    mcl_debug_callback_t callback;
    union {
        float *f32;
        int16_t *i16;
        int32_t *i32;
        int8_t *i8;
        uint8_t *addr;
    } data;
    uint8_t cnt;
    mcl_debug_data_type_t data_type;
} mcl_debug_cfg_t;

/**
 * @brief debug data
 *
 */
typedef struct {
    mcl_debug_cfg_t cfg;
} mcl_debug_t;

/**
 * @brief init debug data struct
 *
 * @param debug @ref mcl_debug_t
 */
void hpm_mcl_debug_init(mcl_debug_t *debug);

/**
 * @brief Sends data in the buffer to the outside, Depending on how the callback function works,
 *        this function could potentially hang the application
 *
 * @param debug @ref mcl_debug_t
 */
void hpm_mcl_debug_send_scope(mcl_debug_t *debug);

/**
 * @brief Update debug data
 *
 * @param debug @ref mcl_debug_t
 * @param chn channel number
 * @param data input data
 * @param type @ref mcl_debug_data_type_t
 */
void hpm_mcl_debug_update_data(mcl_debug_t *debug, uint8_t chn, void *data, mcl_debug_data_type_t type);

#endif
