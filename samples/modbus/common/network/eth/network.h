/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MODBUS_NETWORK
#define MODBUS_NETWORK
#include "stdint.h"
#include "hpm_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

hpm_stat_t network_init(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif