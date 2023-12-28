/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_LOOP_H
#define HPM_MCL_LOOP_H

#include "hpm_mcl_common.h"
#include "hpm_mcl_encoder.h"
#include "hpm_mcl_analog.h"
#include "hpm_mcl_control.h"
#include "hpm_mcl_drivers.h"
#include "hpm_mcl_path_plan.h"

typedef enum {
    loop_status_null = 0,
    loop_status_init = 1,
    loop_status_run = 2,
    loop_status_fail = 3,
} mcl_loop_status_t;

/**
 * @brief Algorithms used in loops
 *
 */
typedef enum {
    mcl_mode_foc = 1,
    mcl_mode_block = 2,
    mcl_mode_hardware_foc = 3,
    mcl_mode_step_foc = 4,
} mcl_loop_mode_t;

/**
 * @brief Loop Configuration
 *
 */
typedef struct {
    mcl_loop_mode_t mode;
    bool enable_speed_loop;
    bool enable_position_loop;
} mcl_loop_cfg_t;

/**
 * @brief Loop operation data
 *
 */
typedef struct {
    mcl_loop_status_t status;
    mcl_loop_cfg_t *cfg;
    mcl_encoder_t *encoder;
    mcl_analog_t *analog;
    mcl_control_t *control;
    mcl_drivers_t *drivers;
    mcl_path_plan_t *path;
    hpm_mcl_type_t *const_vbus;
    struct {
        float *current_ts;
        float *speed_ts;
        float *position_ts;
    } const_time;
    mcl_user_value_t ref_id;
    mcl_user_value_t ref_iq;
    mcl_user_value_t ref_speed;
    mcl_user_value_t ref_position;
    struct {
        float id;
        float iq;
        float speed;
        float position;
    } exec_ref;
    struct {
        float speed_ts;
        float position_ts;
    } time;
    void *hardware;
    bool enable;
} mcl_loop_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialisation loop data
 *
 * @param loop @ref mcl_loop_t
 * @param cfg @ref mcl_loop_cfg_t
 * @param mcl_cfg @ref mcl_cfg_t
 * @param encoder @ref mcl_encoder_t
 * @param analog @ref mcl_analog_t
 * @param control @ref mcl_control_t
 * @param drivers @ref mcl_drivers_t
 * @param path @ref mcl_path_plan_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                mcl_encoder_t *encoder, mcl_analog_t *analog,
                                mcl_control_t *control, mcl_drivers_t *drivers, mcl_path_plan_t *path);

/**
 * @brief Setting the d-axis current
 *
 * @param loop @ref mcl_loop_t
 * @param id @ref mcl_user_value_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_set_current_d(mcl_loop_t *loop, mcl_user_value_t id);

/**
 * @brief Setting the q-axis current
 *
 * @param loop @ref mcl_loop_t
 * @param iq @ref mcl_user_value_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_set_current_q(mcl_loop_t *loop, mcl_user_value_t iq);

/**
 * @brief Setting the speed loop speed feed
 *
 * @param loop @ref mcl_loop_t
 * @param speed @ref mcl_user_value_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_set_speed(mcl_loop_t *loop, mcl_user_value_t speed);

/**
 * @brief Setting the position parameters
 *
 * @param loop @ref mcl_loop_t
 * @param position @ref mcl_user_value_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_set_position(mcl_loop_t *loop, mcl_user_value_t position);

/**
 * @brief Invalid user-set values in all loops
 *
 * @param loop @ref mcl_loop_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_disable_all_user_set_value(mcl_loop_t *loop);

/**
 * @brief Motor Loop, Periodic Recall
 *
 * @param loop @ref mcl_loop_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop(mcl_loop_t *loop);

/**
 * @brief Enable Loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_loop_enable(mcl_loop_t *loop)
{
    loop->enable = true;
}

/**
 * @brief Disable Loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_loop_disable(mcl_loop_t *loop)
{
    loop->enable = false;
}

/**
 * @brief Enable position loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_enable_position_loop(mcl_loop_t *loop)
{
    loop->cfg->enable_position_loop = true;
}

/**
 * @brief Disable position loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_disable_position_loop(mcl_loop_t *loop)
{
    loop->cfg->enable_position_loop = false;
}

/**
 * @brief Enable speed loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_enable_speed_loop(mcl_loop_t *loop)
{
    loop->cfg->enable_speed_loop = true;
}

/**
 * @brief Disable speed loop
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_disable_speed_loop(mcl_loop_t *loop)
{
    loop->cfg->enable_speed_loop = false;
}

#ifdef __cplusplus
}
#endif

#endif
