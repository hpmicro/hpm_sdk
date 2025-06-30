/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_qei_drv.h"

void qei_config_mode(QEI_Type *qei_x, qei_mode_config_t *config)
{
    qei_counter_reset_assert(qei_x);
    qei_set_work_mode(qei_x, config->work_mode);
    qei_phase_config(qei_x, config->phcnt_max, config->z_count_inc_mode, config->z_cali_enable);
    qei_phase_set_index(qei_x, config->phcnt_idx);
    qei_counter_reset_release(qei_x);
}

void qei_config_h_phase(QEI_Type *qei_x, qei_h_phase_config_t *config)
{
    uint32_t tmp = (qei_x->CR & ~(QEI_CR_HFDIR0_MASK | QEI_CR_HFDIR1_MASK | QEI_CR_HRDIR0_MASK | QEI_CR_HRDIR1_MASK
                                | QEI_CR_HRSTSPD_MASK | QEI_CR_HRSTPH_MASK | QEI_CR_HRSTZ_MASK));

    if (config->h_fall_dir_forward) {
        tmp |= QEI_CR_HFDIR0_MASK;
    }
    if (config->h_fall_dir_reverse) {
        tmp |= QEI_CR_HFDIR1_MASK;
    }
    if (config->h_rise_dir_forward) {
        tmp |= QEI_CR_HRDIR0_MASK;
    }
    if (config->h_rise_dir_reverse) {
        tmp |= QEI_CR_HRDIR1_MASK;
    }
    if (config->h_valid_reset_spdcnt) {
        tmp |= QEI_CR_HRSTSPD_MASK;
    }
    if (config->h_valid_reset_phcnt) {
        tmp |= QEI_CR_HRSTPH_MASK;
    }
    if (config->h_valid_reset_zcnt) {
        tmp |= QEI_CR_HRSTZ_MASK;
    }

    qei_x->CR = tmp;
}

void qei_config_pause(QEI_Type *qei_x, qei_pause_config_t *config)
{
    uint32_t tmp = (qei_x->CR & ~(QEI_CR_PAUSESPD_MASK | QEI_CR_PAUSEPH_MASK | QEI_CR_PAUSEZ_MASK));

    if (config->pause_valid_pause_spdcnt) {
        tmp |= QEI_CR_PAUSESPD_MASK;
    }
    if (config->pause_valid_pause_phcnt) {
        tmp |= QEI_CR_PAUSEPH_MASK;
    }
    if (config->pause_valid_pause_zcnt) {
        tmp |= QEI_CR_PAUSEZ_MASK;
    }

    qei_x->CR = tmp;
}
