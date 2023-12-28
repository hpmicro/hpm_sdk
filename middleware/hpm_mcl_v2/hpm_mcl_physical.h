/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_PHYSICAL_H
#define HPM_MCL_PHYSICAL_H
#include "hpm_common.h"

#define MCL_ANALOG_CHN_NUM  10 /**< @ref mcl_analog_chn_t */

typedef struct {
    float res;    /**< Motor resistance in ohms */
    int32_t pole_num;  /**< Motor pole number */
    float vbus;     /**< Bus supply voltage */
    float ld;       /**< d-axis inductors */
    float lq;   /**< q-axis inductors */
    float ls;       /**< inductors */
    float i_max;    /**< Maximum current */
    float power;    /**< Rated power */
    float inertia; /**< kgm^2 */
    float rpm_max; /**< Maximum RPM */
} physical_motor_t;

typedef struct {
    int32_t res;    /**< Motor resistance in ohms */
    int32_t pole_num;  /**< Motor pole number */
    int32_t vbus;
    int32_t ld;
    int32_t lq;
    int32_t ls;
    int32_t i_max;
    int32_t power;
    int32_t inertia; /**< kgm^2 */
    int32_t rpm_max;
} physical_motor_q_t;

typedef struct {
    float sample_res; /**< Sampling Resistance Value, ohms */
    float adc_reference_vol; /**< adc reference voltage */
    float opamp_gain;   /**< operational amplifier amplification */
    int32_t sample_precision; /**< Sampling accuracy, e.g. 12-bit adc should be set to 4095 */
} physical_board_analog_t;

typedef struct {
    int32_t sample_res; /**< Sampling Resistance Value, ohms */
    int32_t opamp_gain;     /**< operational amplifier amplification */
    int32_t adc_reference_vol; /**< adc reference voltage */
    int32_t sample_precision;   /**< Sampling accuracy, e.g. 12-bit adc should be set to 4095 */
} physical_board_analog_q_t;

typedef struct {
    physical_board_analog_t analog[MCL_ANALOG_CHN_NUM];
    int32_t num_current_sample_res; /**< number of sampling resistors */
    int32_t pwm_reload;    /**< pwm reload maximum */
    float pwm_frequency;    /**< pwm frequency hz */
    int32_t pwm_dead_time_tick; /**< pwm dead time in ticks, A pwm cycle has two dead zones, and this is the time of one of the dead zones*/
} physical_board_t;

typedef struct {
    physical_board_analog_q_t analog[MCL_ANALOG_CHN_NUM];
    int32_t num_current_sample_res; /**< number of sampling resistors */
    int32_t pwm_reload;    /**< pwm reload maximum */
    int32_t pwm_frequency;    /**< pwm frequency hz */
    int32_t pwm_dead_tick; /**< pwm dead time in ticks, A pwm cycle has two dead zones, and this is the time of one of the dead zones*/
} physical_board_q_t;

typedef struct {
    int32_t pwm_clock_tick;   /**< pwm clock period */
    int32_t mcu_clock_tick;   /**< mcu clock period */
    float speed_loop_ts;    /**< velocity loop operating cycle in s */
    float current_loop_ts;
    float position_loop_ts;
    float encoder_process_ts; /**< Encoder Processing Cycle */
    float adc_sample_ts;    /**< adc sampling period in s */
} physical_time_t;

typedef struct {
    int32_t pwm_clock_tick;   /**< pwm clock period */
    int32_t mcu_clock_tick;   /**< mcu clock period */
    int32_t speed_loop_ts;    /**< velocity loop operating cycle in s */
    int32_t current_loop_ts;
    int32_t position_loop_ts;
    int32_t adc_sample_ts;    /**< adc sampling period in s */
} physical_time_q_t;

typedef struct {
    physical_motor_t motor;
    physical_board_t board;
    physical_time_t time;
} mcl_physical_para_t;

typedef struct {
    physical_motor_q_t motor;
    physical_board_q_t board;
    physical_time_q_t time;
} mcl_physical_para_q_t;

#endif