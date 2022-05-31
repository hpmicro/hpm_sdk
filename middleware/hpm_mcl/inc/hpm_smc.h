/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_SMC_H
#define HPM_SMC_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
void bldc_smc_pos_cal(BLDC_CONTROL_SMC_PARA *par);
void bldc_smc_const_cal(BLDC_MOTOR_PARA *par);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif