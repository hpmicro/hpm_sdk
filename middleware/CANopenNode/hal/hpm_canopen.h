/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_CANOPEN_H
#define HPM_CANOPEN_H

#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_MCAN
#include "hpm_mcan_drv.h"
#include "hpm_canopen_mcan.h"
#else
#include "hpm_can_drv.h"
#include "hpm_canopen_can.h"
#endif
#include "can.h"
#include "CANopen.h"
#include "CO_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

void canopen_controller_init(struct canopen_context *candriverstate, void *canptr, uint32_t baudrate, int index);

#ifdef __cplusplus
}
#endif

#endif /* HPM_CANOPEN_H */