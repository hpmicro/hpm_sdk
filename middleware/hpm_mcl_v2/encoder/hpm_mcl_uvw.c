/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_uvw.h"

hpm_mcl_stat_t hpm_mcl_uvw_get_theta(void *encoder_base, uint8_t *last_pos, float deta_theta, float *theta)
{
#ifdef HPMSOC_HAS_HPMSDK_HALL
    hall_rotate_direction_t dir;
    float encoder_theta;
    (void)last_pos;

    encoder_theta = *theta;
    dir = hall_get_rotate_direction((HALL_Type *)encoder_base);
    if (dir == hall_rotate_direction_forward) {
        encoder_theta += deta_theta;
    } else {
        encoder_theta -= deta_theta;
    }
    if (encoder_theta > MCL_2PI) {
        encoder_theta -= MCL_2PI;
    } else if (encoder_theta < 0) {
        encoder_theta += MCL_2PI;
    }
    *theta = encoder_theta;
#endif

#ifdef HPMSOC_HAS_HPMSDK_QEIV2
    uint32_t pos;
    float encoder_theta;
    (void)deta_theta;

    encoder_theta = *theta;
    pos = qeiv2_get_postion((QEIV2_Type *)encoder_base);
    pos /= QEIV2_ANGLE_PRE;
    pos = (pos + 1) / 2;
    if ((pos > *last_pos) || (*last_pos - pos > 3)) {
        encoder_theta -= deta_theta;
    }
    if ((pos < *last_pos) || (pos - *last_pos > 3)) {
        encoder_theta += deta_theta;
    }
    *last_pos = pos;
    *theta = encoder_theta;
#endif

    return mcl_success;
}

