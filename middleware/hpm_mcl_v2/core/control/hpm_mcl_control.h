/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_CONTROL_H
#define HPM_MCL_CONTROL_H
#include "hpm_mcl_common.h"
#include "hpm_mcl_math.h"

/**
 * @brief Control Module Status
 *
 */
typedef enum {
    control_status_null = 0,
    control_status_init = 1,
    control_status_run = 2,
    control_status_fail = 3,
} mcl_control_status_t;

/**
 * @brief Six sectors of svpwm
 *
 */
typedef enum {
    svpwm_sector1 = 1,
    svpwm_sector2 = 2,
    svpwm_sector3 = 3,
    svpwm_sector4 = 4,
    svpwm_sector5 = 5,
    svpwm_sector6 = 6,
} mcl_svpwm_sector_t;

/**
 * @brief Results of offline parameter testing
 *
 */
typedef struct {
    float rs;
    float ld;
    float lq;
    float ls;
    float flux;
} mcl_offline_param_detection_result_t;

/**
 * @brief Configuration data for pid
 *
 */
typedef struct {
    hpm_mcl_type_t kp;
    hpm_mcl_type_t ki;
    hpm_mcl_type_t kd;
    hpm_mcl_type_t integral_max;    /**< Maximum value of integral */
    hpm_mcl_type_t integral_min;    /**< Minimum value of the integral */
    hpm_mcl_type_t output_max;      /**< Maximum value of output */
    hpm_mcl_type_t output_min;      /**< Minimum value of output */
} mcl_control_pid_cfg_t;

/**
 * @brief pid running data
 *
 */
typedef struct {
    mcl_control_pid_cfg_t cfg;
    hpm_mcl_type_t integral;
} mcl_control_pid_t;

/**
 * @brief Used to compensate for pwm dead zones, Duty cycle ratio, between 0 and 1
 *
 */
typedef struct {
    float a_offset;
    float b_offset;
    float c_offset;
} mcl_control_dead_area_pwm_offset_t;

/**
 * @brief DeadAera Compensation Configuration
 *
 */
typedef struct {
    float lowpass_k;
} mcl_control_dead_area_compensation_cfg_t;

/**
 * @brief DeadAera Compensation data
 *
 */
typedef struct {
    mcl_control_dead_area_compensation_cfg_t cfg;
    float d_mem;
    float q_mem;
} mcl_control_dead_area_compensation_t;

/**
 * @brief Sensorless smc configuration
 *
 */
typedef struct {
    struct {
        float res;          /**< Stator resistance (in ohm) */
        float inductor;     /**< Stator inductor */
        float sample_ts;    /**< Current sampling period */
        float loop_ts;      /**< cycle time in s */
    } const_data;
    struct {
        float smc_f;
        float smc_g;
        float zero;    /**< slip mode convergence */
        float ksmc;   /**< Slide coefficient */
        float filter_coeff;   /**< low-pass filter coefficients */
    } factor;
    float theta0;       /**< initial angle */
    mcl_control_pid_t pll;
} mcl_control_smc_cfg_t;

/**
 * @brief Sensorless smc structure
 *
 */
typedef struct {
    mcl_control_smc_cfg_t cfg;
    float ialpha_mem;   /**< Internal Data */
    float ibeta_mem;    /**< Internal Data */
    float alpha_cal;    /**< Internal Data */
    float zalpha_cal;   /**< Internal Data */
    float beta_cal;     /**< Internal Data */
    float zbeta_cal;    /**< Internal Data */
    float theta_mem;
    float theta;
    float ualpha;
    float ubeta;
} mcl_control_smc_t;

/**
 * @brief Offline Detection Configuration
 *
 */
typedef struct {
    float current_half_rated; /**< Half the rated current */
    float ud_delta; /**< Measurement of resistance, the value of each voltage increment, the use of smaller values of error is small, the use of larger values, speed */
    float vbus; /**< power supply voltage */
    float inductor_detection_times; /**< Number of times to detect the inductor, The time is not easy to be too large, usually 1ms*/
    float flux_detection_times; /**< Number of times to detect the flux, The longer the time, the more accurate*/
    float delay_times;  /**< Intervals between different tests, Ensure that the current drain is clean to prevent interference with the next item. */
    float detection_loop_ts;    /**< Recall interval for detecting loops */
    float lowpass_k; /**< Low-pass filter coefficients, used when solving for magnetic chains */
} mcl_control_offline_param_detection_cfg_t;

/**
 * @brief Offline Detection Data
 *
 */
typedef struct {
    mcl_control_offline_param_detection_cfg_t cfg;
    mcl_offline_param_detection_result_t result;
    float tick_count;
    struct {
        float ud_mem;
    } rs;
    struct {
        float is_last;
    } ls;
    struct {
        float val0_mem;
        float val1_mem;
        float val_filter;
        float val_filter_mem;
    } flux;
} mcl_control_offline_param_detection_t;

/**
 * @brief pwm duty cycle
 *
 */
typedef struct {
    hpm_mcl_type_t a;
    hpm_mcl_type_t b;
    hpm_mcl_type_t c;
    /**
     * @brief step motor pwm
     *
     */
    hpm_mcl_type_t a0;
    hpm_mcl_type_t a1;
    hpm_mcl_type_t b0;
    hpm_mcl_type_t b1;
} mcl_control_svpwm_duty_t;

/**
 * @brief Supported control algorithms
 *
 */
typedef struct {
    hpm_mcl_type_t (*sin_x)(hpm_mcl_type_t x);
    hpm_mcl_type_t (*cos_x)(hpm_mcl_type_t x);
    hpm_mcl_type_t (*arctan_x)(hpm_mcl_type_t y, hpm_mcl_type_t x);
    hpm_mcl_stat_t (*park)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta,
                hpm_mcl_type_t sin_x, hpm_mcl_type_t cos_x,
                hpm_mcl_type_t *d, hpm_mcl_type_t *q);
    hpm_mcl_stat_t (*clarke)(hpm_mcl_type_t ia, hpm_mcl_type_t ib, hpm_mcl_type_t ic,
                hpm_mcl_type_t *alpha, hpm_mcl_type_t *beta);
    hpm_mcl_stat_t (*currentd_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*currentq_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*speed_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*position_pid)(hpm_mcl_type_t ref, hpm_mcl_type_t sens, mcl_control_pid_t *pid_x, hpm_mcl_type_t *output);
    hpm_mcl_stat_t (*invpark)(hpm_mcl_type_t d, hpm_mcl_type_t q, hpm_mcl_type_t sin_x,
                    hpm_mcl_type_t cos_x, hpm_mcl_type_t *alpha, hpm_mcl_type_t *beta);
    hpm_mcl_stat_t (*svpwm)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta, hpm_mcl_type_t vbus, mcl_control_svpwm_duty_t *duty);
    hpm_mcl_stat_t (*step_svpwm)(hpm_mcl_type_t alpha, hpm_mcl_type_t beta, hpm_mcl_type_t vbus, mcl_control_svpwm_duty_t *duty);
    hpm_mcl_stat_t (*get_block_sector)(hall_phase_t hall, uint8_t u, uint8_t v, uint8_t w, uint8_t *sector);
    hpm_mcl_stat_t (*dead_area_polarity_detection)(mcl_control_dead_area_compensation_t *dead_area,
                float id, float iq, float theta,
                float deadtime, float ts, mcl_control_dead_area_pwm_offset_t *pwm_out);
    void (*smc_init)(mcl_control_smc_t *smc_cfg);
    void (*smc_process)(mcl_control_smc_t *smc_cfg, float ualpha, float ubeta, float ialpha, float ibeta);
    hpm_mcl_stat_t (*offline_param_detection_rs)(mcl_control_offline_param_detection_t *detection, float ialpha, float ibeta, float *ud, float *uq);
    hpm_mcl_stat_t (*offline_param_detection_ld)(mcl_control_offline_param_detection_t *detection, float ialpha, float ibeta, float *ud, float *uq);
    hpm_mcl_stat_t (*offline_param_detection_lq)(mcl_control_offline_param_detection_t *detection, float ialpha, float ibeta, float *ud, float *uq);
    hpm_mcl_stat_t (*offline_param_detection_ls)(mcl_control_offline_param_detection_t *detection);
    hpm_mcl_stat_t (*offline_param_detection_flux)(mcl_control_offline_param_detection_t *detection, float ialpha, float ibeta, float ualpha, float ubeta, float *ref_d, float *ref_q);
    hpm_mcl_stat_t (*offline_param_detection_init)(mcl_control_offline_param_detection_t *detection);
} mcl_control_method_t;

/**
 * @brief callback function
 *
 */
typedef struct {
    void (*init)(void); /**< User-defined function initialisation */
    _FUNC_OPTIONAL_ mcl_control_method_t method;    /**< User-defined algorithms */
} mcl_control_callback_t;

/**
 * @brief Control Function Configuration
 *
 */
typedef struct {
    mcl_control_callback_t callback;
    mcl_control_pid_t currentd_pid_cfg;
    mcl_control_pid_t currentq_pid_cfg;
    mcl_control_pid_t speed_pid_cfg;
    mcl_control_pid_t position_pid_cfg;
    mcl_control_dead_area_compensation_t dead_area_compensation_cfg;
    mcl_control_smc_t smc_cfg;
    mcl_control_offline_param_detection_t offline_param_detection_cfg;
} mcl_control_cfg_t;

/**
 * @brief Control function running data
 *
 */
typedef struct {
    mcl_control_cfg_t *cfg;
    mcl_control_method_t method;
} mcl_control_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise the operating data for the control function
 *
 * @param control @ref mcl_control_t
 * @param cfg @ref mcl_control_cfg_t
 * @return hpm_mcl_stat_t
 */
hpm_mcl_stat_t hpm_mcl_control_init(mcl_control_t *control, mcl_control_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
