/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_UVW_H
#define HPM_MCL_UVW_H
#include "hpm_common.h"
#include "hpm_mcl_common.h"

#ifdef HPMSOC_HAS_HPMSDK_QEIV2
#include "hpm_qeiv2_drv.h"
#define QEIV2_ANGLE_PRE             (0x15555555)
#endif

#ifdef HPMSOC_HAS_HPMSDK_HALL
#include "hpm_hall_drv.h"
#endif

/**
 * @brief Getting the angle in uvw mode
 *
 * @param encoder_base HALL or QEIV2 Base
 * @param last_pos Global variables with initial value zero
 * @param deta_theta Angle value per call
 * @param theta Current angle value
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_uvw_get_theta(void *encoder_base, uint8_t *last_pos, float deta_theta, float *theta);

#endif
