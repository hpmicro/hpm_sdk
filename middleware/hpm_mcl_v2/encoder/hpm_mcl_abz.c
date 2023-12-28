/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_abz.h"
#ifdef HPMSOC_HAS_HPMSDK_QEI
#include "hpm_qei_drv.h"
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
#include "hpm_qeiv2_drv.h"
#endif

hpm_mcl_stat_t hpm_mcl_abz_get_theta(void *qei_base, uint32_t phase_count, float deta_theta, float *theta)
{
    uint32_t  pos;
    float angle = 0.0;

    MCL_ASSERT_OPT(theta != NULL, mcl_invalid_pointer);
#ifdef HPMSOC_HAS_HPMSDK_QEI
    pos = qei_get_current_phase_phcnt((QEI_Type *)qei_base);
#endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    pos = qeiv2_get_current_phase_phcnt((QEIV2_Type *)qei_base);
#endif
    angle = pos % phase_count;
    angle *= deta_theta;
    *theta = angle;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_abz_get_abs_theta(void *qei_base, uint32_t phase_count, float deta_theta, float offset, float *theta)
{
    int32_t ph, z;
    float theta1;

    MCL_ASSERT_OPT(theta != NULL, mcl_invalid_pointer);
#ifdef HPMSOC_HAS_HPMSDK_QEI
    ph = qei_get_current_phase_phcnt((QEI_Type *)qei_base);
    z = qei_get_current_count((QEI_Type *)qei_base, qei_counter_type_z);
 #endif
#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    ph = qeiv2_get_current_phase_phcnt((QEIV2_Type *)qei_base);
    z = qeiv2_get_current_count((QEIV2_Type *)qei_base, qeiv2_counter_type_z);
#endif
    theta1 = (z * (int32_t)(phase_count)) + ph;
    *theta = -theta1 * deta_theta + offset;

    return mcl_success;
}
