/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_CODEC_CONTROL_H
#define HPM_CODEC_CONTROL_H

#include "hpm_common.h"
#include "hpm_i2c_drv.h"

/**
 * @brief Codec control handle structure
 *
 * This structure contains the common control interface for all codec drivers.
 */
typedef struct {
    I2C_Type *ptr;              /**< I2C instance pointer */
    uint8_t slave_address;      /**< Device I2C slave address */
} codec_control_t;

#endif /* HPM_CODEC_CONTROL_H */

