/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_mcl_hw_loop.h"
#if defined(HPMSOC_HAS_HPMSDK_VSC)
#include "hpm_vsc_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_CLC)
#include "hpm_clc_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_QEOV2)
#include "hpm_qeov2_drv.h"
#endif

hpm_mcl_stat_t hpm_mcl_hw_loop_init(mcl_hw_loop_t *loop, mcl_hw_loop_cfg_t *cfg)
{
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    loop->cfg = cfg;

    /* Initialize VSC (Vector Signal Controller) if callback is provided */
    if (loop->cfg->callback.vsc_init != NULL) {
        MCL_ASSERT(loop->cfg->vsc_cfg.base != NULL, mcl_invalid_pointer);
        #if defined(HPMSOC_HAS_HPMSDK_VSC)
        loop->cfg->callback.vsc_init(loop->cfg->vsc_cfg.base);
        #else
        MCL_ASSERT(false, mcl_not_supported);
        #endif
    }

    /* Initialize CLC (Current Loop Controller) if callback is provided */
    if (loop->cfg->callback.clc_init != NULL) {
        MCL_ASSERT(loop->cfg->clc_cfg.base != NULL, mcl_invalid_pointer);
        #if defined(HPMSOC_HAS_HPMSDK_CLC)
        loop->cfg->callback.clc_init(loop->cfg->clc_cfg.base);
        #else
        MCL_ASSERT(false, mcl_not_supported);
        #endif
    }

    /* Initialize QEO (Quadrature Encoder Output) if callback is provided */
    if (loop->cfg->callback.qeo_init != NULL) {
        MCL_ASSERT(loop->cfg->qeo_cfg.base != NULL, mcl_invalid_pointer);
        #if defined(HPMSOC_HAS_HPMSDK_QEOV2)
        loop->cfg->callback.qeo_init(loop->cfg->qeo_cfg.base);
        #else
        MCL_ASSERT(false, mcl_not_supported);
        #endif
    }
    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_vsc_run(mcl_hw_loop_t *loop)
{
#if defined(HPMSOC_HAS_HPMSDK_VSC)
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop->cfg->vsc_cfg.base != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop->cfg->callback.vsc_convert_input != NULL, mcl_invalid_pointer);

    /* Convert software values to hardware format for VSC input */
    loop->cfg->callback.vsc_convert_input(loop->hw_loop_data.software.ia, loop->hw_loop_data.software.ib, loop->hw_loop_data.software.ic,
        loop->hw_loop_data.software.position, &loop->hw_loop_data.hardware.ia, &loop->hw_loop_data.hardware.ib, &loop->hw_loop_data.hardware.ic,
        &loop->hw_loop_data.hardware.position);

    /* Clear previous conversion done flag */
    vsc_clear_irq_status(loop->cfg->vsc_cfg.base, vsc_irq_convert_done);

    /* Inject position and current values into VSC */
    vsc_sw_inject_pos_value(loop->cfg->vsc_cfg.base, loop->hw_loop_data.hardware.position);
    vsc_sw_inject_abc_value(loop->cfg->vsc_cfg.base, loop->hw_loop_data.hardware.ia, loop->hw_loop_data.hardware.ib, loop->hw_loop_data.hardware.ic);

    /* Wait for VSC conversion to complete */
    while (!vsc_get_irq_flag(loop->cfg->vsc_cfg.base, vsc_irq_convert_done)) {
        ;
    }

    /* Read transformed values from VSC */
    loop->hw_loop_data.hardware.alpha = vsc_get_alpha_axis_value(loop->cfg->vsc_cfg.base, true);
    loop->hw_loop_data.hardware.beta = vsc_get_beta_axis_value(loop->cfg->vsc_cfg.base, true);
    loop->hw_loop_data.hardware.d = vsc_get_d_axis_value(loop->cfg->vsc_cfg.base, true);
    loop->hw_loop_data.hardware.q = vsc_get_q_axis_value(loop->cfg->vsc_cfg.base, true);

    /* Convert hardware values back to software format if CLC is not enabled */
    if (!loop->hw_loop_status.clc_enabled) {
        MCL_ASSERT(loop->cfg->callback.vsc_convert_output != NULL, mcl_invalid_pointer);
        loop->cfg->callback.vsc_convert_output(loop->hw_loop_data.hardware.alpha, loop->hw_loop_data.hardware.beta, loop->hw_loop_data.hardware.d, loop->hw_loop_data.hardware.q,
            &loop->hw_loop_data.software.alpha, &loop->hw_loop_data.software.beta, &loop->hw_loop_data.software.d, &loop->hw_loop_data.software.q);
    }
    return mcl_success;
#else
    (void)loop;
    MCL_ASSERT(false, mcl_not_supported);
#endif
}

hpm_mcl_stat_t hpm_mcl_clc_run(mcl_hw_loop_t *loop, float d_expect, float q_expect)
{
#if defined(HPMSOC_HAS_HPMSDK_CLC)
    uint32_t d_expect_hardware, q_expect_hardware;
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop->cfg->clc_cfg.base != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop->cfg->callback.clc_convert_input != NULL, mcl_invalid_pointer);

    /* Convert software d-q values to hardware format if VSC is not enabled */
    if (!loop->hw_loop_status.vsc_enabled) {
        loop->cfg->callback.clc_convert_input(loop->hw_loop_data.software.d, loop->hw_loop_data.software.q,
            &loop->hw_loop_data.hardware.d, &loop->hw_loop_data.hardware.q);
    }

    /* Convert expected d-q values to hardware format */
    loop->cfg->callback.clc_convert_input(d_expect, q_expect, &d_expect_hardware, &q_expect_hardware);

    /* Set expected values for CLC controller */
    clc_set_expect_adc_value(loop->cfg->clc_cfg.base, clc_vd_chn, d_expect_hardware);
    clc_set_expect_adc_value(loop->cfg->clc_cfg.base, clc_vq_chn, q_expect_hardware);

    /* Inject actual d-q current values into CLC */
    clc_sw_inject_dq_adc_value(loop->cfg->clc_cfg.base, loop->hw_loop_data.hardware.d, loop->hw_loop_data.hardware.q);

    /* Wait for CLC calculation to complete for both d and q channels */
    while (!clc_get_irq_flag(loop->cfg->clc_cfg.base, clc_vd_chn, clc_irq_calc_done) || !clc_get_irq_flag(loop->cfg->clc_cfg.base, clc_vq_chn, clc_irq_calc_done)) {
        ;
    }

    /* Read calculated voltage outputs from CLC */
    loop->hw_loop_data.hardware.ud = clc_get_output_value(loop->cfg->clc_cfg.base, clc_vd_chn);
    loop->hw_loop_data.hardware.uq = clc_get_output_value(loop->cfg->clc_cfg.base, clc_vq_chn);

    /* Convert hardware values back to software format if QEO is not enabled */
    if (!loop->hw_loop_status.qeo_enabled) {
        MCL_ASSERT(loop->cfg->callback.clc_convert_output != NULL, mcl_invalid_pointer);
        loop->cfg->callback.clc_convert_output(loop->hw_loop_data.hardware.ud, loop->hw_loop_data.hardware.uq,
            &loop->hw_loop_data.software.ud, &loop->hw_loop_data.software.uq);
    }
    return mcl_success;
#else
    (void)loop;
    (void)d_expect;
    (void)q_expect;
    MCL_ASSERT(false, mcl_not_supported);
#endif
}

hpm_mcl_stat_t hpm_mcl_qeo_run(mcl_hw_loop_t *loop)
{
#if defined(HPMSOC_HAS_HPMSDK_QEOV2)
    MCL_ASSERT(loop != NULL, mcl_invalid_pointer);
    MCL_ASSERT(loop->cfg->qeo_cfg.base != NULL, mcl_invalid_pointer);
    /* Convert software value to QEO hardware value is must be called */
    MCL_ASSERT(loop->cfg->callback.qeo_convert_input != NULL, mcl_invalid_pointer);

    /* Convert software values to hardware format for QEO input */
    loop->cfg->callback.qeo_convert_input(loop->hw_loop_data.software.ud, loop->hw_loop_data.software.uq,
        loop->hw_loop_data.software.predicted_position, &loop->hw_loop_data.hardware.ud, &loop->hw_loop_data.hardware.uq,
        &loop->hw_loop_data.hardware.predicted_position);

    /* Configure d-q voltage values in QEO */
    qeo_wave_config_vd_vq_value(loop->cfg->qeo_cfg.base, loop->hw_loop_data.hardware.ud, loop->hw_loop_data.hardware.uq);

    /* Load d-q values into QEO for processing */
    qeo_wave_load_vd_vq(loop->cfg->qeo_cfg.base);

    /* Inject predicted position for transformation */
    qeo_software_position_inject(loop->cfg->qeo_cfg.base, loop->hw_loop_data.hardware.predicted_position);

    /* Wait for QEO calculation to finish */
    while (!qeo_check_calculate_finish(loop->cfg->qeo_cfg.base)) {
        ;
    }

    /* Read three-phase duty cycle outputs from QEO */
    loop->hw_loop_data.hardware.duty_u = qeo_get_wave_output_val(loop->cfg->qeo_cfg.base, 0);
    loop->hw_loop_data.hardware.duty_v = qeo_get_wave_output_val(loop->cfg->qeo_cfg.base, 1);
    loop->hw_loop_data.hardware.duty_w = qeo_get_wave_output_val(loop->cfg->qeo_cfg.base, 2);

    /* Convert hardware duty values back to software format if callback is provided */
    if (loop->cfg->callback.qeo_convert_output != NULL) {
        loop->cfg->callback.qeo_convert_output(loop->hw_loop_data.hardware.duty_u, loop->hw_loop_data.hardware.duty_v, loop->hw_loop_data.hardware.duty_w,
            &loop->hw_loop_data.software.duty_u, &loop->hw_loop_data.software.duty_v, &loop->hw_loop_data.software.duty_w);
    }
    return mcl_success;
#else
    (void)loop;
    MCL_ASSERT(false, mcl_not_supported);
#endif
}

