/*
 * Copyright (c) 2025-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* this file for canopen user config */

#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#include "CO_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CANOPEN_DEVICE 1
#define CO_CONFIG_SDO_CLI CO_CONFIG_SDO_CLI_ENABLE | CO_CONFIG_SDO_CLI_SEGMENTED
#define CO_CONFIG_HB_CONS                                                                                              \
    (CO_CONFIG_HB_CONS_ENABLE | CO_CONFIG_GLOBAL_FLAG_CALLBACK_PRE | CO_CONFIG_GLOBAL_FLAG_TIMERNEXT                   \
    | CO_CONFIG_GLOBAL_FLAG_OD_DYNAMIC | CO_CONFIG_HB_CONS_QUERY_FUNCT)
#define CO_CONFIG_NMT (CO_CONFIG_GLOBAL_FLAG_CALLBACK_PRE | CO_CONFIG_GLOBAL_FLAG_TIMERNEXT | CO_CONFIG_NMT_MASTER)
#define CO_CONFIG_FIFO 1

#ifdef __cplusplus
}
#endif

#endif