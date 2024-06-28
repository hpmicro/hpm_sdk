/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_pwm.h"
#ifdef HPMSOC_HAS_HPMSDK_PWM
#include "hpm_pwm_drv.h"
#endif
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
#include "hpm_pwmv2_drv.h"
#endif

typedef struct {
    uint8_t cmp_start;
    uint8_t cmp_end;
    uint8_t trig_cmp_index;
} hpm_mcl_central_aligned_pair_para_t;


void hpm_mcl_pwm_central_aligned_pair_init(hpm_mcl_central_aligned_pair_para_t para)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)

#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)

#endif
}
