/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_ABZ_H
#define HPM_MCL_ABZ_H
#include "hpm_common.h"
#include "hpm_mcl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the angle of the abz encoder
 *
 * @param qei_base qei peripheral base address
 * @param phase_count Number of pulses per revolution
 * @param deta_theta Mechanical angle for each pulse
 * @param theta rad
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_abz_get_theta(void *qei_base, uint32_t phase_count, float deta_theta, float *theta);

/**
 * @brief Get absolute angle value
 *
 * @param qei_base qei peripheral base address
 * @param phase_count Number of pulses per revolution
 * @param deta_theta Mechanical angle for each pulse
 * @param theta0 Initial angle calibration value
 * @param theta rad
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_abz_get_abs_theta(void *qei_base, uint32_t phase_count, float deta_theta, float theta0, float *theta);

#ifdef __cplusplus
}
#endif

#endif
