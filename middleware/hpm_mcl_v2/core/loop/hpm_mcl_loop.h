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
#include "hpm_mcl_debug.h"

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
    mcl_mode_offline_param_detection = 5,
} mcl_loop_mode_t;

/**
 * @brief Loop Configuration
 *
 */
typedef struct {
    mcl_loop_mode_t mode;
    bool enable_speed_loop;
    bool enable_position_loop;
    bool enable_offline_param_detection;
#if defined(MCL_CFG_EN_SENSORLESS_SMC) && MCL_CFG_EN_SENSORLESS_SMC
    bool enable_smc;
#endif
#if defined(MCL_CFG_EN_DQ_AXIS_DECOUPLING) && MCL_CFG_EN_DQ_AXIS_DECOUPLING
    bool enable_dq_axis_decoupling;
#endif
#if defined(MCL_CFG_EN_DEAD_AREA_COMPENSATION) && MCL_CFG_EN_DEAD_AREA_COMPENSATION
    bool enable_dead_area_compensation;
#endif
} mcl_loop_cfg_t;

/**
 * @brief CLC channel
 *
 */
typedef enum {
    loop_chn_id = 0,
    loop_chn_iq = 1
} mcl_loop_chn_t;

/**
 * @brief Hardware configuration for CLC
 *
 */
typedef struct {
    void (*clc_set_val)(mcl_loop_chn_t chn, int32_t val);
    int32_t (*convert_float_to_clc_val)(float realdata);
} mcl_hardware_clc_cfg_t;

/**
 * @brief Internal use, process control in the process
 *
 */
typedef enum {
    offline_param_detection_mode_init = 0,
    offline_param_detection_mode_rs = 1,
    offline_param_detection_mode_ld = 2,
    offline_param_detection_mode_lq = 3,
    offline_param_detection_mode_ls = 4,
    offline_param_detection_mode_flux = 5,
    offline_param_detection_mode_wait = 6,
    offline_param_detection_mode_end = 7,
    offline_param_detection_mode_error = 8,
} mcl_offline_param_detection_mode_t;

typedef struct {
    volatile mcl_offline_param_detection_mode_t mode;
    volatile mcl_offline_param_detection_result_t result;
    float last_ualpha;
    float last_ubeta;
    mcl_offline_param_detection_mode_t last_mode;
    uint32_t tick_count;
} mcl_offline_param_detection_rundata_t;

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
    mcl_debug_t *debug;
    mcl_path_plan_t *path;
    hpm_mcl_type_t *const_vbus;
    hpm_mcl_type_t *lq;
    hpm_mcl_type_t *ld;
    hpm_mcl_type_t *flux;
    struct {
        float *current_ts;
        float *speed_ts;
        float *position_ts;
        float dead_area_ts;
        float offline_detection_wait_ts;
    } const_time;
    struct {
        struct {
            mcl_motor_dir_t dir;
        } block;
        mcl_offline_param_detection_rundata_t offline_detection;
        uint32_t current_loop_tick;
    } rundata;
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
 * @brief Call this function in the interrupt function to update the motor's sector
 *
 * @param loop @ref mcl_loop_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_refresh_block(mcl_loop_t *loop);

/**
 * @brief Call this function in the interrupt function to start the motor's sector
 *
 * @param loop @ref mcl_loop_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_loop_start_block(mcl_loop_t *loop);

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
 * @brief Enables the loop's offline parameter detection
 *
 * @param loop @ref mcl_loop_t
 */
static inline void hpm_mcl_loop_enable_offline_param_detecion(mcl_loop_t *loop)
{
    loop->rundata.offline_detection.mode = offline_param_detection_mode_init;
    loop->cfg->enable_offline_param_detection = true;
}

/**
 * @brief Get offline parameter is done
 *
 * @param loop @ref mcl_loop_t
 * @return bool
 */
static inline bool hpm_mcl_loop_offline_param_detection_is_done(mcl_loop_t *loop)
{
    if (loop->rundata.offline_detection.mode == offline_param_detection_mode_end) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Get offline parameter is error
 *
 * @param loop @ref mcl_loop_t
 * @return true or false
 */
static inline bool hpm_mcl_loop_offline_param_detection_is_error(mcl_loop_t *loop)
{
    if (loop->rundata.offline_detection.mode == offline_param_detection_mode_error) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Get offline parameter detection result
 *
 * @param loop @ref mcl_loop_t
 * @param result @ref mcl_offline_param_detection_result_t
 */
static inline void hpm_mcl_loop_offline_param_detection_get_result(mcl_loop_t *loop, mcl_offline_param_detection_result_t *result)
{
    *result = loop->rundata.offline_detection.result;
}

/**
 * @brief Set loop mode
 *
 * @param loop @ref mcl_loop_t
 * @param mode @ref mcl_loop_mode_t
 */
static inline void hpm_mcl_loop_mode_set(mcl_loop_t *loop, mcl_loop_mode_t mode)
{
    loop->cfg->mode = mode;
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

/**
 * @brief disable dead area compensation
 *
 * @param loop @ref mcl_loop_t
 */
static inline hpm_mcl_stat_t hpm_mcl_disable_dead_area_compensation(mcl_loop_t *loop)
{
#if defined(MCL_CFG_EN_DEAD_AREA_COMPENSATION) && MCL_CFG_EN_DEAD_AREA_COMPENSATION
    MCL_ASSERT_OPT(loop->cfg != NULL, mcl_invalid_pointer);
    loop->cfg->enable_dead_area_compensation = false;
    return mcl_success;
#else
    (void)loop;
    return mcl_fail;
#endif
}

/**
 * @brief enable dead area compensation
 *
 * @param loop @ref mcl_loop_t
 */
static inline hpm_mcl_stat_t hpm_mcl_enable_dead_area_compensation(mcl_loop_t *loop)
{
#if defined(MCL_CFG_EN_DEAD_AREA_COMPENSATION) && MCL_CFG_EN_DEAD_AREA_COMPENSATION
    MCL_ASSERT_OPT(loop->cfg != NULL, mcl_invalid_pointer);
    loop->cfg->enable_dead_area_compensation = true;
    return mcl_success;
#else
    (void)loop;
    return mcl_fail;
#endif
}

/**
 * @brief disable dq axis decoupling
 *
 * @param loop @ref mcl_loop_t
 */
static inline hpm_mcl_stat_t hpm_mcl_disable_dq_axis_decoupling(mcl_loop_t *loop)
{
#if defined(MCL_CFG_EN_DQ_AXIS_DECOUPLING) && MCL_CFG_EN_DQ_AXIS_DECOUPLING
    MCL_ASSERT_OPT(loop->cfg != NULL, mcl_invalid_pointer);
    loop->cfg->enable_dq_axis_decoupling = false;
    return mcl_success;
#else
    (void)loop;
    return mcl_fail;
#endif
}

/**
 * @brief enable dq axis decoupling
 *
 * @param loop @ref mcl_loop_t
 */
static inline hpm_mcl_stat_t hpm_mcl_enable_dq_axis_decoupling(mcl_loop_t *loop)
{
#if defined(MCL_CFG_EN_DQ_AXIS_DECOUPLING) && MCL_CFG_EN_DQ_AXIS_DECOUPLING
    MCL_ASSERT_OPT(loop->cfg != NULL, mcl_invalid_pointer);
    loop->cfg->enable_dq_axis_decoupling = true;
    return mcl_success;
#else
    (void)loop;
    return mcl_fail;
#endif
}

/**
 * @brief Get current loop run tick
 *
 * @param loop @ref mcl_loop_t
 * @return uint32_t tick
 */
static inline uint32_t hpm_mcl_get_current_loop_run_tick(mcl_loop_t *loop)
{
#if defined(MCL_EN_LOOP_TIME_COUNT) && MCL_EN_LOOP_TIME_COUNT
    return loop->rundata.current_loop_tick;
#else
    (void)loop;
    return 0;
#endif
}

#ifdef __cplusplus
}
#endif

#endif
