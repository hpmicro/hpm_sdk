/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PING_H
#define PING_H

#include "hpm_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

hpm_stat_t ping(char *target_name, uint32_t times, size_t size);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* PING_H */

