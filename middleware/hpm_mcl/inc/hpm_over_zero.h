/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_OVER_ZERO_H
#define HPM_OVER_ZERO_H

#include "hpm_common.h"
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Over-zero method fsm
 *
 */
typedef enum {
    hpm_mcl_over_zero_fsm_init = 0,  /**< Initialization data */
    hpm_mcl_over_zero_fsm_location = 1,  /**< Initial phase positioning */
    hpm_mcl_over_zero_fsm_running = 2    /**< Normal run */
} hpm_mcl_over_zero_fsm_t;

/**
 * @brief Motor Direction
 *
 */
typedef enum {
    hpm_motor_dir_forward = 0,  /**< forward */
    hpm_motor_dir_reverse = 3   /**< reverse */
} hpm_mcl_dir_t;

typedef enum {
    hpm_mcl_interval_w_down = 1,    /**< w down */
    hpm_mcl_interval_v_up = 2,      /**< v up */
    hpm_mcl_interval_u_down = 3,    /**< u down */
    hpm_mcl_interval_w_up = 4,      /**< w up */
    hpm_mcl_interval_v_down = 5,    /**< v down */
    hpm_mcl_interval_u_up = 6,      /**< u up */
} hpm_mcl_over_zero_interval_t;


/**
 * @brief Over-zero method configuration data
 *
 */
typedef struct hpm_mcl_over_zero_cfg {
    int32_t adc_u;                     /**< adc u value */
    int32_t adc_v;                     /**< adc v value */
    int32_t adc_w;                     /**< adc w value */
    uint32_t number_consecutive_zeros;      /**< Number of consecutive zero passes any phase */
    uint32_t number_consecutive_zeros_w;  /**< Number of consecutive zero passes w phase */
    uint32_t delay_degree_30;           /**< Thirty-degree delay value */
    uint32_t last_interval_tick;         /**< The cycle interval of the last commutation is used to deduce the moment of the next commutation of 30 degrees */
    uint32_t interval_tick;             /**< interval ticks */
    hpm_mcl_dir_t motor_dir;                  /**< Motor Direction forward: Positive(1-6), reverse Reverse(6-1) */
    uint8_t adc_zero_ph;                /**< Phase of adc over zero 1: u, 2: v, 3: w*/
    hpm_mcl_over_zero_interval_t last_interval;     /**< last interval */
    hpm_mcl_over_zero_interval_t next_interval; /**< next interval */
    uint32_t speed_tick;                /**< Speed count, unit cycle */
    hpm_mcl_over_zero_interval_t interval;                   /**< The phase sequence number corresponding to the passing zero point */
    uint8_t pole_num;                   /**< Number of motor pole pairs */
    float loop_time_in_sec;                /*< Time interval to get the position, unit: s*/
    hpm_mcl_over_zero_fsm_t fsm;
} hpm_mcl_over_zero_cfg_t;

/**
 * @brief Filtering and phase-locked loop parameters
 *
 */
typedef struct  hpm_mcl_over_zero_spd_par {
    float       speed;  /**< speed */
    float       err;    /**< speed error */
    float       err_last;   /**< Last error */
    float       filter_coef; /**< Low-pass filter, filter coefficient */
    float       speedout;   /**< Post-processing speed */
    float       kp; /**< pi kp */
    float       ki; /**< pi ki */
    float       mem;    /**< integral storage space */
    float       integral_max;   /**< integral max */
    float       integral_min;   /**< integral min */
    float       output_max;     /**< output max */
    float       output_min;     /**< output min*/
    void    (*func_getspd)();
} hpm_mcl_over_zero_spd_para_t;

/**
 * @brief Speed PID control
 *
 */
typedef struct hpm_mcl_over_zero_pi_par {
    float       kp;                   /**< Kp */
    float       ki;                   /**< Ki */
    float       kd;                   /**< Kd */
    float       integral_max;         /**< integral max */
    float       integral_min;         /**< integral min */
    float       output_max;           /**< output max */
    float       output_min;           /**< output min */
    float       target;                 /**< Target parameters */
    float       mem;                    /**< integral Data */
    float       cur;                    /**< Sampling data  */
    float       outval;                 /**< Output Data */
    void (*func_pid)();                               /**< Pid function */
} hpm_mcl_over_zero_pi_para_t;

/**
 * @brief Get commutation position
 *
 * @param[inout] cfg @ref hpm_mcl_over_zero_cfg_t
 * @return Current commutation interval
 */
int8_t hpm_mcl_over_zero_step_get(hpm_mcl_over_zero_cfg_t *cfg);

/**
 * @brief Speed filter
 *
 * @param[inout] par @ref hpm_mcl_over_zero_spd_para_t
 * @return speed value
 */
float hpm_mcl_over_zero_speed_filter(hpm_mcl_over_zero_spd_para_t *par);

/**
 * @brief pwm output for different angle intervals
 *
 * @param[in] motorindex motor index
 * @param[in] step angle intervals
 */
void hpm_mcl_over_zero_pwm_ctrl(uint8_t motorindex, uint8_t step);

/**
 * @brief alculating speed by clock cycles
 *
 * @param[inout] cfg @ref hpm_mcl_over_zero_cfg_t
 * @return speed raw value
 */
float hpm_mcl_over_zero_cal_speed(hpm_mcl_over_zero_cfg_t *cfg);

/**
 * @brief Speed pi control
 *
 * @param[in] par @ref hpm_mcl_over_zero_pi_para_t
 * @return  pwm duty cycle
 */
float hpm_mcl_over_zero_pi_contrl(hpm_mcl_over_zero_pi_para_t *par);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
