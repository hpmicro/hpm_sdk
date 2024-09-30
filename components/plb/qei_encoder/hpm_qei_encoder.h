/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_QEI_ENCODER_H
#define _HPM_QEI_ENCODER_H

#include "hpm_soc_feature.h"
#include "hpm_plb_drv.h"
#include "hpm_trgm_drv.h"

/* @brief Common status code definitions */
enum {
    status_unsupport_plb = MAKE_STATUS(status_group_plb_qei_encoder, 0),
};

/**
 * @brief abz channel
 *
 */
typedef enum {
    plb_qei_encoder_channel_0 = 0,
    plb_qei_encoder_channel_1 = 1,
    plb_qei_encoder_channel_2 = 2,
    plb_qei_encoder_channel_3 = 3,
    plb_qei_encoder_channel_4 = 4,
    plb_qei_encoder_channel_5 = 5,
    plb_qei_encoder_channel_6 = 6,
    plb_qei_encoder_channel_7 = 7,
} plb_qei_encoder_channel_t;

/**
 * @brief encoder mode
 *
 */
typedef enum {
    plb_ab_mode = 0,
    plb_abz_mode = 1
} plb_qei_encoder_mode_t;

/**
 * @brief Pinout resources via trgmux inputs
 *
 */
typedef struct {
    uint16_t qei_a;
    uint16_t qei_b;
    uint16_t qei_z;
} plb_qei_encoder_trgmux_input_t;

/**
 * @brief Configuration information for qei
 *
 */
typedef struct {
    uint32_t phase_cnt; /**< How many pulses per revolution, valid when enable_phase is enabled, The value must be greater than 4 */
    plb_qei_encoder_trgmux_input_t trgm_input;
    plb_qei_encoder_mode_t mode;
    uint16_t filter_length;
    bool enable_z_pin;  /**< Enable z pin, only valid in abz mode, if not enable the lap count from ab, if enable z pin, lap count from z signal */
    bool enable_phase;  /**< enable phase. when enabled, the count will change to zero when it reaches this pulse, and when the count reaches zero it will change to the value of phase_cnt. */
} plb_qei_encoder_cfg_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Initializing the encoder
 *
 * @param plb @ref PLB_Type
 * @param trgm @ref TRGM_Type
 * @param qei_chn @ref plb_qei_encoder_channel_t
 * @param cfg @ref plb_qei_encoder_cfg_t
 * @return hpm_stat_t Whether the initialization is successful or not
 */
hpm_stat_t plb_qei_encoder_init(PLB_Type *plb, TRGM_Type *trgm, plb_qei_encoder_channel_t qei_chn, plb_qei_encoder_cfg_t *cfg);

/**
 * @brief Get location information
 *
 * @param plb @ref PLB_Type
 * @param qei_chn @ref plb_qei_encoder_channel_t
 * @return Encoder position
 */
uint32_t plb_qei_encoder_get_phase_count(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn);

/**
 * @brief Set phase count
 *
 * @param plb @ref PLB_Type
 * @param qei_chn @ref plb_qei_encoder_channel_t
 * @param val value
 */
void plb_qei_encoder_set_phase_count(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn, uint32_t val);

/**
 * @brief Get Lap Data
 *
 * @param plb @ref PLB_Type
 * @param qei_chn @ref plb_qei_encoder_channel_t
 * @return laps
 */
uint32_t plb_qei_encoder_get_laps_number(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn);

/**
 * @brief Set Lap data
 *
 * @param plb @ref PLB_Type
 * @param qei_chn @ref plb_qei_encoder_channel_t
 * @param val laps
 */
void plb_qei_encoder_set_laps_number(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn, int32_t val);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
