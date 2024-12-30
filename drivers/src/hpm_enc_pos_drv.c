/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_enc_pos_drv.h"


float encoder_position_to_deg(uint32_t pos)
{
    double tmp;

    tmp = ((double)pos / (double)0xFFFFFFFF) * 360.0;

    return (float)tmp;
}

uint32_t encoder_deg_to_position(float deg)
{
    double tmp = (double)deg;

    while (tmp < 0) {
        tmp += 360.0;
    }
    while (tmp > 360.0) {
        tmp -= 360.0;
    }

    return (uint32_t)((tmp / 360.0) * (double)0xFFFFFFFF);
}

float encoder_position_to_rad(uint32_t pos)
{
    double tmp;

    tmp = ((double)pos / (double)0xFFFFFFFF) * HPM_2_PI;

    return (float)tmp;
}

uint32_t encoder_rad_to_position(float rad)
{
    double tmp = (double)rad;

    while (tmp < 0) {
        tmp += HPM_2_PI;
    }
    while (tmp > HPM_2_PI) {
        tmp -= HPM_2_PI;
    }

    return (uint32_t)((tmp / HPM_2_PI) * (double)0xFFFFFFFF);
}
