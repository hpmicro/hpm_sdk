/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_HW_LOOP_H
#define HPM_MCL_HW_LOOP_H
#include "hpm_common.h"
#include "hpm_mcl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hardware loop callback
 */
typedef struct {
    void (*vsc_init)(void *data);  /*< Initialize VSC hardware */
    void (*clc_init)(void *data);  /*< Initialize CLC hardware */
    void (*qeo_init)(void *data);  /*< Initialize QEO hardware */
    /* Pre-processing callbacks */
    void (*vsc_pre_process)(void *loop);  /*< Pre-processing callback before VSC hardware loop */
    void (*clc_pre_process)(void *loop);  /*< Pre-processing callback before CLC hardware loop */
    void (*qeo_pre_process)(void *loop);  /*< Pre-processing callback before QEO hardware loop */
    /* Post-processing callbacks */
    void (*vsc_post_process)(void *loop);  /*< Post-processing callback after VSC hardware loop */
    void (*clc_post_process)(void *loop);  /*< Post-processing callback after CLC hardware loop */
    void (*qeo_post_process)(void *loop);  /*< Post-processing callback after QEO hardware loop */
    /* Input conversion callbacks - convert software values to hardware values */
    void (*vsc_convert_input)(float ia, float ib, float ic, float position, uint32_t *ia_hardware, uint32_t *ib_hardware, uint32_t *ic_hardware, uint32_t *position_hardware);    /*< Convert software value to VSC hardware value */
    void (*clc_convert_input)(float d, float q, uint32_t *d_hardware, uint32_t *q_hardware);    /*< Convert software value to CLC hardware value */
    void (*qeo_convert_input)(float ud, float uq, float predicted_position, uint32_t *ud_hardware, uint32_t *uq_hardware, uint32_t *predicted_position_hardware);    /*< Convert software value to QEO hardware value */
    /* Output conversion callbacks - convert hardware values to software values */
    void (*vsc_convert_output)(uint32_t alpha_hardware, uint32_t beta_hardware, uint32_t d_hardware, uint32_t q_hardware, float *alpha, float *beta, float *d, float *q);   /*< Convert VSC hardware value to software value */
    void (*clc_convert_output)(uint32_t ud_hardware, uint32_t uq_hardware, float *ud, float *uq);   /*< Convert CLC hardware value to software value */
    void (*qeo_convert_output)(uint32_t duty_u_hardware, uint32_t duty_v_hardware, uint32_t duty_w_hardware, float *duty_u, float *duty_v, float *duty_w);   /*< Convert QEO hardware value to software value */
} mcl_hw_loop_callback_t;

/**
 * @brief Hardware loop configuration
 */
typedef struct {
    /* VSC configuration */
    struct {
        void *base;
    } vsc_cfg;
    /* CLC configuration */
    struct {
        void *base;
    } clc_cfg;
    /* QEO configuration */
    struct {
        void *base;
    } qeo_cfg;
    mcl_hw_loop_callback_t callback;
} mcl_hw_loop_cfg_t;

typedef struct {
    mcl_hw_loop_cfg_t *cfg;
    /* Hardware loop status */
    struct {
        bool vsc_enabled;  /*< VSC hardware loop enabled */
        bool clc_enabled;  /*< CLC hardware loop enabled */
        bool qeo_enabled;  /*< QEO hardware loop enabled */
    } hw_loop_status;
    /* Hardware loop running data */
    struct {
        struct {
            uint32_t ia;  /*< phase current a */
            uint32_t ib;  /*< phase current b */
            uint32_t ic;  /*< phase current c */
            uint32_t position;  /*< position*/
            uint32_t alpha;  /*< alpha component */
            uint32_t beta;  /*< beta component */
            uint32_t d;  /*< d component */
            uint32_t q;  /*< q component */
            uint32_t ud;  /*< d component of voltage */
            uint32_t uq;  /*< q component of voltage */
            uint32_t duty_u;  /*< duty of phase u */
            uint32_t duty_v;  /*< duty of phase v */
            uint32_t duty_w;  /*< duty of phase w */
            uint32_t predicted_position;  /*< predicted position, if not needed, predicted_position = position*/
        } hardware; /*< VSC raw data */
        struct {
            float ia;  /*< phase current a */
            float ib;  /*< phase current b */
            float ic;  /*< phase current c */
            float position;  /*< position*/
            float alpha;  /*< alpha component */
            float beta;  /*< beta component */
            float d;  /*< d component */
            float q;  /*< q component */
            float ud;  /*< d component of voltage */
            float uq;  /*< q component of voltage */
            float duty_u;  /*< duty of phase u */
            float duty_v;  /*< duty of phase v */
            float duty_w;  /*< duty of phase w */
            float predicted_position;  /*< predicted position, if not needed, predicted_position = position*/
        } software; /*< VSC converted data */
    } hw_loop_data;
} mcl_hw_loop_t;


/**
 * @brief Enable VSC hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_enable_vsc_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.vsc_enabled = true;

    return mcl_success;
}

/**
 * @brief Disable VSC hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_disable_vsc_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.vsc_enabled = false;

    return mcl_success;
}

/**
 * @brief Enable CLC hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_enable_clc_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.clc_enabled = true;

    return mcl_success;
}

/**
 * @brief Disable CLC hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_disable_clc_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.clc_enabled = false;

    return mcl_success;
}

/**
 * @brief Enable QEO hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_enable_qeo_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.qeo_enabled = true;

    return mcl_success;
}

/**
 * @brief Disable QEO hardware loop
 *
 * @param loop @ref mcl_hw_loop_t
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_disable_qeo_hardware_loop(mcl_hw_loop_t *loop)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    loop->hw_loop_status.qeo_enabled = false;

    return mcl_success;
}

/**
 * @brief Get VSC hardware loop status
 *
 * @param loop @ref mcl_hw_loop_t
 * @param status @ref bool
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_get_vsc_hardware_loop_status(mcl_hw_loop_t *loop, bool *status)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    *status = loop->hw_loop_status.vsc_enabled;

    return mcl_success;
}

/**
 * @brief Get CLC hardware loop status
 *
 * @param loop @ref mcl_hw_loop_t
 * @return true if CLC hardware loop is enabled, false otherwise
 */
static inline hpm_mcl_stat_t hpm_mcl_get_clc_hardware_loop_status(mcl_hw_loop_t *loop, bool *status)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    *status = loop->hw_loop_status.clc_enabled;

    return mcl_success;
}

/**
 * @brief Get QEO hardware loop status
 *
 * @param loop @ref mcl_hw_loop_t
 * @param status @ref bool
 * @return @ref hpm_mcl_stat_t
 */
static inline hpm_mcl_stat_t hpm_mcl_get_qeo_hardware_loop_status(mcl_hw_loop_t *loop, bool *status)
{
    MCL_ASSERT_OPT(loop != NULL, mcl_invalid_pointer);
    *status = loop->hw_loop_status.qeo_enabled;

    return mcl_success;
}

/**
 * @brief Initialize hardware loop components
 *
 * This function initializes the hardware loop components including VSC, CLC, and QEO modules.
 * Each component is initialized through callback functions if they are configured.
 *
 * @param loop Pointer to hardware loop structure
 * @param cfg Pointer to hardware loop configuration
 * @return mcl_success if initialization successful, error code otherwise
 */
hpm_mcl_stat_t hpm_mcl_hw_loop_init(mcl_hw_loop_t *loop, mcl_hw_loop_cfg_t *cfg);

/**
 * @brief Run VSC (Vector Signal Controller) hardware acceleration
 *
 * This function performs Clarke and Park transformations using hardware VSC module.
 * It converts three-phase currents (ia, ib, ic) to alpha-beta and d-q coordinates.
 *
 * @param loop Pointer to hardware loop structure
 * @return mcl_success if operation successful, error code otherwise
 */
hpm_mcl_stat_t hpm_mcl_vsc_run(mcl_hw_loop_t *loop);

/**
 * @brief Run CLC (Current Loop Controller) hardware acceleration
 *
 * This function performs current loop control using hardware CLC module.
 * It calculates d-q axis voltage outputs based on expected and actual current values.
 *
 * @param loop Pointer to hardware loop structure
 * @param d_expect Expected d-axis current reference
 * @param q_expect Expected q-axis current reference
 * @return mcl_success if operation successful, error code otherwise
 */
hpm_mcl_stat_t hpm_mcl_clc_run(mcl_hw_loop_t *loop, float d_expect, float q_expect);

/**
 * @brief Run QEO (Quadrature Encoder Output) hardware acceleration
 *
 * This function performs inverse Park and inverse Clarke transformations using hardware QEO module.
 * It converts d-q voltages to three-phase PWM duty cycles.
 *
 * @param loop Pointer to hardware loop structure
 * @return mcl_success if operation successful, error code otherwise
 */
hpm_mcl_stat_t hpm_mcl_qeo_run(mcl_hw_loop_t *loop);

#ifdef __cplusplus
}
#endif

#endif