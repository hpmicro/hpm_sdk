/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_DRIVERS_H
#define HPM_MCL_DRIVERS_H
#include "hpm_mcl_common.h"

enum {
    mcl_drivers_init_error = MAKE_STATUS(mcl_group_drivers, 0),
    mcl_drivers_not_ready = MAKE_STATUS(mcl_group_drivers, 1),
    mcl_drivers_update_error = MAKE_STATUS(mcl_group_drivers, 2),
};

/**
 * @brief Status of the drive function
 *
 */
typedef enum {
    drivers_status_null = 0,
    drivers_status_init = 1,
    drivers_status_run = 2,
    drivers_status_fail = 3,
} mcl_drivers_status_t;

/**
 * @brief Channels for drive functions
 *
 */
typedef enum {
/**
 * @brief Drive channels for three-phase motors.
 *
 */
    mcl_drivers_chn_a,
    mcl_drivers_chn_b,
    mcl_drivers_chn_c,
/**
 * @brief Four channels of the stepper motor
 *
 */
    mcl_drivers_chn_a0,
    mcl_drivers_chn_a1,
    mcl_drivers_chn_b0,
    mcl_drivers_chn_b1,
} mcl_drivers_channel_t;

/**
 * @brief Callbacks for Driver Functions
 *
 */
typedef struct {
    void (*init)(void);
    hpm_mcl_stat_t (*update_duty_cycle)(mcl_drivers_channel_t chn, float duty);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*update_frequency)(mcl_drivers_channel_t chn, uint32_t freqc);
    _FUNC_OPTIONAL_ hpm_mcl_stat_t (*update_phase_offset)(mcl_drivers_channel_t chn, int32_t tick);
    hpm_mcl_stat_t (*disable_all_drivers)(void);
    hpm_mcl_stat_t (*enable_all_drivers)(void);
    hpm_mcl_stat_t (*disable_drivers)(mcl_drivers_channel_t chn);
    hpm_mcl_stat_t (*enable_drivers)(mcl_drivers_channel_t chn);
} mcl_drivers_callback_t;

/**
 * @brief Driver Function Configuration
 *
 */
typedef struct {
    mcl_drivers_callback_t callback;
} mcl_drivers_cfg_t;

/**
 * @brief Driver function running data
 *
 */
typedef struct {
    mcl_drivers_cfg_t *cfg;
    mcl_drivers_status_t status;
} mcl_drivers_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialising data for driver functions
 *
 * @param drivers @ref mcl_drivers_t
 * @param cfg @ref mcl_drivers_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_drivers_init(mcl_drivers_t *drivers, mcl_drivers_cfg_t *cfg);

/**
 * @brief Update the duty cycle of a brushless motor
 *
 * @param drivers @ref mcl_drivers_t
 * @param duty_a a-phase duty cycle
 * @param duty_b b-phase duty cycle
 * @param duty_c c-phase duty cycle
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_drivers_update_bldc_duty(mcl_drivers_t *drivers, float duty_a, float duty_b, float duty_c);

/**
 * @brief Update the duty cycle of a step motor
 *
 * @param drivers @ref mcl_drivers_t
 * @param duty_a0 a0-phase duty cycle
 * @param duty_a1 a1-phase duty cycle
 * @param duty_b0 b0-phase duty cycle
 * @param duty_b1 b1-phase duty cycle
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_drivers_update_step_duty(mcl_drivers_t *drivers, float duty_a0, float duty_a1, float duty_b0, float duty_b1);

/**
 * @brief Switch off all drive outputs
 *
 * @param drivers @ref mcl_drivers_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_disable_all_drivers(mcl_drivers_t *drivers);

/**
 * @brief Enable all drive outputs
 *
 * @param drivers @ref mcl_drivers_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_enable_all_drivers(mcl_drivers_t *drivers);

#ifdef __cplusplus
}
#endif

#endif
