/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_DETECT_H
#define HPM_MCL_DETECT_H

#include "hpm_mcl_common.h"
#include "hpm_mcl_loop.h"

/**
 * @brief Fault Detection Override Functions
 *
 */
typedef struct {
    mcl_loop_status_t *control_loop;
    mcl_encoder_status_t *encoder;
    mcl_analog_status_t *analog;
    mcl_drivers_status_t *drivers;
} mcl_detect_status;

/**
 * @brief Fault Detection Callback Functions
 *
 */
typedef struct {
    hpm_mcl_stat_t (*disable_output)(void); /**< Turn off power module output */
    void (*user_process)(mcl_detect_status status); /**< After detecting the fault, the user performs some processing */
} mcl_detect_callback_t;

/**
 * @brief Fault Detection Configuration
 *
 */
typedef struct {
    bool enable_detect; /**< Enable all detection, if off, no detection is made */
    struct {
        bool loop;
        bool encoder;
        bool analog;
        bool drivers;
    } en_submodule_detect;  /**< Enable detection of submodules */
    mcl_detect_callback_t callback;
} mcl_detect_cfg_t;

/**
 * @brief Fault detection running data
 *
 */
typedef struct {
    mcl_detect_cfg_t *cfg;
    mcl_detect_status status;
    mcl_drivers_t *drivers;
} mcl_detect_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialising data structures for fault detection
 *
 * @param detect @ref mcl_detect_t
 * @param cfg @ref mcl_detect_cfg_t
 * @param loop @ref mcl_loop_t
 * @param encoder @ref mcl_encoder_t
 * @param analog @ref mcl_analog_t
 * @param drivers @ref mcl_drivers_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_detect_init(mcl_detect_t *detect, mcl_detect_cfg_t *cfg, mcl_loop_t *loop, mcl_encoder_t *encoder,
    mcl_analog_t *analog, mcl_drivers_t *drivers);

/**
 * @brief Loop for detection function
 *
 * @param detect @ref mcl_detect_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_detect_loop(mcl_detect_t *detect);

#ifdef __cplusplus
}
#endif

#endif