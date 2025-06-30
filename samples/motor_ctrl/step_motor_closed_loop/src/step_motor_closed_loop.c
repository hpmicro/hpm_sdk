/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif
#include "hpm_trgm_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_QEI)
#include "hpm_qei_drv.h"
#endif
#include "hpm_gptmr_drv.h"
#if defined(HPMSOC_HAS_HPMSDK_QEIV2)
#include "hpm_qeiv2_drv.h"
#endif
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_adc.h"
#include "hpm_mcl_loop.h"
#include "hpm_mcl_abz.h"
#include "hpm_mcl_detect.h"

/* QEI/QEIV2 related macros */
#if defined(HPMSOC_HAS_HPMSDK_QEI)
#define STEP_MOTOR_QEI_BASE              BOARD_BLDC_QEI_BASE
#endif
#if defined(HPMSOC_HAS_HPMSDK_QEIV2)
#define STEP_MOTOR_QEI_BASE            BOARD_BLDC_QEIV2_BASE
#endif
#define STEP_MOTOR_QEI_TRGM              BOARD_BLDC_QEI_TRGM
#define STEP_MOTOR_QEI_TRGM_QEI_A_SRC    BOARD_BLDC_QEI_TRGM_QEI_A_SRC
#define STEP_MOTOR_QEI_TRGM_QEI_B_SRC    BOARD_BLDC_QEI_TRGM_QEI_B_SRC
#define STEP_MOTOR_QEI_FOC_PHASE_COUNT_PER_REV  BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV

/* Motor control parameters */
#define CURRENT_SET_TIME_MS    (200)    /* Time for current setting in milliseconds */
#define SPEED_MAX              (10)     /* Maximum speed limit */
#define PWM_FREQUENCY          (20000)  /* PWM frequency in Hz */
#define PWM_RELOAD             ((motor_clock_hz/PWM_FREQUENCY) - 1)  /* PWM reload value */
#define MOTOR0_BLDCPWM         BOARD_BLDCPWM
#define ADCU_INDEX             0        /* ADC U phase index */
#define ADCV_INDEX             1        /* ADC V phase index */
#define T_CURE_TIMES           (1)      /* Time cure multiplier */

/* ADC buffer for storing sampled values */
volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];
int32_t motor_clock_hz;                 /* Motor clock frequency */
float abs_position_theta;               /* Absolute position theta value */

/* ADC configuration structures */
adc_type hpm_adc_a = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_U_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_U_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};
adc_type hpm_adc_b = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_V_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_V_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE
};

/* Motor control structure definition */
typedef struct {
    mcl_encoder_t encoder;              /* Encoder interface */
    mcl_filter_iir_df1_t encoder_iir;   /* Encoder IIR filter */
    mcl_filter_iir_df1_memory_t encoder_iir_mem[2];  /* Encoder IIR filter memory */
    mcl_analog_t analog;                /* Analog interface */
    mcl_drivers_t drivers;              /* Motor drivers */
    mcl_control_t control;              /* Control interface */
    mcl_loop_t loop;                    /* Control loop */
    mcl_detect_t detect;                /* Fault detection */
    mcl_path_plan_t path;               /* Path planning */
    struct {
        mcl_cfg_t mcl;                  /* Motor control configuration */
        mcl_encoer_cfg_t encoder;       /* Encoder configuration */
        mcl_filter_iir_df1_cfg_t encoder_iir;  /* Encoder IIR filter configuration */
        mcl_filter_iir_df1_matrix_t encoder_iir_mat[2];  /* Encoder IIR filter matrix */
        mcl_analog_cfg_t analog;        /* Analog configuration */
        mcl_drivers_cfg_t drivers;      /* Driver configuration */
        mcl_control_cfg_t control;      /* Control configuration */
        mcl_loop_cfg_t loop;            /* Loop configuration */
        mcl_detect_cfg_t detect;        /* Detection configuration */
        mcl_path_plan_cfg_t path;       /* Path planning configuration */
    } cfg;
} motor0_t;

/* Global variables */
motor0_t motor0;                       /* Motor control instance */
uint32_t adc_u_midpoint, adc_v_midpoint;  /* ADC midpoint values */
uint32_t t_cure_times;                 /* Time cure counter */

/* Function declarations */
hpm_mcl_stat_t enable_all_pwm_output(void);    /* Enable all PWM outputs */
hpm_mcl_stat_t disable_all_pwm_output(void);   /* Disable all PWM outputs */
hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty);  /* Set PWM duty cycle */
void pwm_init(void);                           /* Initialize PWM */
hpm_mcl_stat_t adc_init(void);                 /* Initialize ADC */
hpm_mcl_stat_t adc_value_get(mcl_analog_chn_t chn, int32_t *value);  /* Get ADC value */
hpm_mcl_stat_t encoder_get_theta(float *theta);  /* Get encoder theta value */
hpm_mcl_stat_t encoder_get_abs_theta(float *theta);  /* Get absolute encoder theta value */
hpm_mcl_stat_t qei_init(void);                 /* Initialize QEI */

/**
 * @brief Start encoder sampling
 * @return mcl_success if successful
 */
hpm_mcl_stat_t encoder_start_sample(void)
{
    return mcl_success;
}

/**
 * @brief Initialize motor control
 */
void motor0_control_init(void)
{
}

/**
 * @brief Update analog sample location
 * @param chn Channel number
 * @param tick Timer tick
 * @return mcl_success if successful
 */
hpm_mcl_stat_t analog_update_sample_location(mcl_analog_chn_t chn, uint32_t tick)
{
    (void)chn;
    (void)tick;
    return mcl_success;
}

/**
 * @brief Initialize motor parameters and configuration
 */
void motor_init(void)
{
    /* Configure analog parameters for current sensing */
    motor0.cfg.mcl.physical.board.analog[analog_a_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].opamp_gain = 1;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_a_current].sample_res = 2.4;

    motor0.cfg.mcl.physical.board.analog[analog_b_current].adc_reference_vol = 3.3;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].opamp_gain = 1;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_precision = 4095;
    motor0.cfg.mcl.physical.board.analog[analog_b_current].sample_res = 2.4;

    /* Configure board parameters */
    motor0.cfg.mcl.physical.board.num_current_sample_res = 2;
    motor0.cfg.mcl.physical.board.pwm_dead_time_tick = 0;
    motor0.cfg.mcl.physical.board.pwm_frequency = PWM_FREQUENCY;
    motor0.cfg.mcl.physical.board.pwm_reload = PWM_RELOAD;

    /* Configure motor physical parameters */
    motor0.cfg.mcl.physical.motor.i_max = 5;
    motor0.cfg.mcl.physical.motor.inertia = 0.005;
    motor0.cfg.mcl.physical.motor.ls = 0.00263;
    motor0.cfg.mcl.physical.motor.pole_num = 50;
    motor0.cfg.mcl.physical.motor.power = 20;
    motor0.cfg.mcl.physical.motor.res = 0.0011;
    motor0.cfg.mcl.physical.motor.rpm_max = 600;
    motor0.cfg.mcl.physical.motor.vbus = 24;

    /* Configure timing parameters */
    motor0.cfg.mcl.physical.time.adc_sample_ts = MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY);
    motor0.cfg.mcl.physical.time.current_loop_ts = MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY);
    motor0.cfg.mcl.physical.time.speed_loop_ts = (MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY));
    motor0.cfg.mcl.physical.time.position_loop_ts = (MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY)) * 20;
    motor0.cfg.mcl.physical.time.mcu_clock_tick = clock_get_frequency(clock_cpu0);
    motor0.cfg.mcl.physical.time.pwm_clock_tick = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);

    /* Configure analog settings */
    motor0.cfg.analog.enable_a_current = true;
    motor0.cfg.analog.enable_b_current = true;
    memset(motor0.cfg.analog.enable_filter, false, MCL_ANALOG_CHN_NUM);
    motor0.cfg.analog.enable_vbus = true;

    /* Configure encoder settings */
    motor0.cfg.encoder.communication_interval_us = 0;
    motor0.cfg.encoder.disable_start_sample_interrupt = true;
    motor0.cfg.encoder.period_call_time_s = MCL_FREQUENCY_TO_PERIOD(PWM_FREQUENCY);
    motor0.cfg.encoder.precision = BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV;
    motor0.cfg.encoder.speed_abs_switch_m_t = 5;
    motor0.cfg.encoder.speed_cal_method = encoder_method_m;
    motor0.cfg.encoder.timeout_s = 0.5;

    /* Configure IIR filter parameters */
    motor0.cfg.encoder_iir.section = 2;
    motor0.cfg.encoder_iir.matrix = motor0.cfg.encoder_iir_mat;
    motor0.cfg.encoder_iir_mat[0].a1 = -1.947404031871316831825424742419272661209f;
    motor0.cfg.encoder_iir_mat[0].a2 = 0.95152023575172306468772376319975592196f;
    motor0.cfg.encoder_iir_mat[0].b0 = 1;
    motor0.cfg.encoder_iir_mat[0].b1 = 2;
    motor0.cfg.encoder_iir_mat[0].b2 = 1;
    motor0.cfg.encoder_iir_mat[0].scale = 0.001029050970101526990552187612593115773f;

    motor0.cfg.encoder_iir_mat[1].a1 = -1.88285893096534651114382086234400048852f;
    motor0.cfg.encoder_iir_mat[1].a2 = 0.886838706662149367510039610351668670774f;
    motor0.cfg.encoder_iir_mat[1].b0 = 1;
    motor0.cfg.encoder_iir_mat[1].b1 = 2;
    motor0.cfg.encoder_iir_mat[1].b2 = 1;
    motor0.cfg.encoder_iir_mat[1].scale = 0.000994943924200649039424337871651005116f;

    /* Configure control parameters */
    motor0.cfg.control.callback.init = motor0_control_init;

    /* Configure current D-axis PID parameters */
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_max = 20;
    motor0.cfg.control.currentd_pid_cfg.cfg.integral_min = -20;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_max = 20;
    motor0.cfg.control.currentd_pid_cfg.cfg.output_min = -20;
    motor0.cfg.control.currentd_pid_cfg.cfg.kp = 2;
    motor0.cfg.control.currentd_pid_cfg.cfg.ki = 0.000001;

    /* Configure current Q-axis PID parameters */
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_max = 20;
    motor0.cfg.control.currentq_pid_cfg.cfg.integral_min = -20;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_max = 20;
    motor0.cfg.control.currentq_pid_cfg.cfg.output_min = -20;
    motor0.cfg.control.currentq_pid_cfg.cfg.kp = 2;
    motor0.cfg.control.currentq_pid_cfg.cfg.ki = 0.0001;

    /* Configure speed PID parameters */
    motor0.cfg.control.speed_pid_cfg.cfg.integral_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.integral_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_max = 5;
    motor0.cfg.control.speed_pid_cfg.cfg.output_min = -5;
    motor0.cfg.control.speed_pid_cfg.cfg.kp = 0.0074f;
    motor0.cfg.control.speed_pid_cfg.cfg.ki = 0.0005f;

    /* Configure position PID parameters */
    motor0.cfg.control.position_pid_cfg.cfg.integral_max = 10;
    motor0.cfg.control.position_pid_cfg.cfg.integral_min = -10;
    motor0.cfg.control.position_pid_cfg.cfg.output_max = 10;
    motor0.cfg.control.position_pid_cfg.cfg.output_min = -10;
    motor0.cfg.control.position_pid_cfg.cfg.kp = 154.7f;
    motor0.cfg.control.position_pid_cfg.cfg.ki =  0.00001f;

    /* Configure driver callbacks */
    motor0.cfg.drivers.callback.init = pwm_init;
    motor0.cfg.drivers.callback.enable_all_drivers = enable_all_pwm_output;
    motor0.cfg.drivers.callback.disable_all_drivers = disable_all_pwm_output;
    motor0.cfg.drivers.callback.update_duty_cycle = pwm_duty_set;

    /* Configure analog callbacks */
    motor0.cfg.analog.callback.init = adc_init;
    motor0.cfg.analog.callback.update_sample_location = analog_update_sample_location;
    motor0.cfg.analog.callback.get_value = adc_value_get;

    /* Configure encoder callbacks */
#if defined(HW_CURRENT_FOC_ENABLE)
    motor0.cfg.encoder.callback.init = NULL;
#else
    motor0.cfg.encoder.callback.init = qei_init;
#endif
    motor0.cfg.encoder.callback.start_sample = encoder_start_sample;
    motor0.cfg.encoder.callback.get_theta = encoder_get_theta;
    motor0.cfg.encoder.callback.get_absolute_theta = encoder_get_abs_theta;

    /* Configure loop settings */
    motor0.cfg.loop.mode = mcl_mode_step_foc;
    motor0.cfg.loop.enable_speed_loop = true;
    motor0.cfg.loop.enable_step_motor_closed_loop = true;

    /* Configure detection settings */
    motor0.cfg.detect.enable_detect = true;
    motor0.cfg.detect.callback.disable_output = disable_all_pwm_output;

    /* Configure path planning */
    motor0.cfg.path.loop_ts = 1.0f / (PWM_FREQUENCY) * T_CURE_TIMES;
    motor0.cfg.path.t_cure.time = 0;

    /* Initialize all motor control components */
    hpm_mcl_path_init(&motor0.path, &motor0.cfg.path, &motor0.cfg.mcl);
    hpm_mcl_analog_init(&motor0.analog, &motor0.cfg.analog, &motor0.cfg.mcl);
    hpm_mcl_filter_iir_df1_init(&motor0.encoder_iir, &motor0.cfg.encoder_iir, &motor0.encoder_iir_mem[0]);
    hpm_mcl_encoder_init(&motor0.encoder, &motor0.cfg.mcl, &motor0.cfg.encoder, &motor0.encoder_iir);
    hpm_mcl_drivers_init(&motor0.drivers, &motor0.cfg.drivers);
    hpm_mcl_control_init(&motor0.control, &motor0.cfg.control);
    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                    &motor0.encoder, &motor0.analog, &motor0.control, &motor0.drivers, &motor0.path, NULL);
    hpm_mcl_detect_init(&motor0.detect, &motor0.cfg.detect, &motor0.loop, NULL, &motor0.analog, &motor0.drivers);
}

/**
 * @brief Initialize QEI (Quadrature Encoder Interface)
 * @return mcl_success if successful
 */
#ifdef HPMSOC_HAS_HPMSDK_QEI
hpm_mcl_stat_t qei_init(void)
{
    trgm_output_t config = {0};
    qei_mode_config_t mode_config = {0};

    init_qei_trgm_pins();
    /* Configure QEI trigger outputs */
    config.invert = false;
    config.input = STEP_MOTOR_QEI_TRGM_QEI_A_SRC;
    trgm_output_config(STEP_MOTOR_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &config);
    config.input = STEP_MOTOR_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(STEP_MOTOR_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &config);

    mode_config.work_mode = qei_work_mode_abz;
    mode_config.z_count_inc_mode = qei_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = STEP_MOTOR_QEI_FOC_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.phcnt_idx = 0;
    qei_config_mode(STEP_MOTOR_QEI_BASE, &mode_config);

    return mcl_success;
}
#endif

#ifdef HPMSOC_HAS_HPMSDK_QEIV2
hpm_mcl_stat_t qei_init(void)
{
    qeiv2_mode_config_t mode_config = {0};

    init_qei_trgm_pins();
    qeiv2_config_filter(STEP_MOTOR_QEI_BASE, qeiv2_filter_phase_a, false, qeiv2_filter_mode_delay, true, 100);
    qeiv2_config_filter(STEP_MOTOR_QEI_BASE, qeiv2_filter_phase_b, false, qeiv2_filter_mode_delay, true, 100);

    mode_config.work_mode = qeiv2_work_mode_abz;
    mode_config.spd_tmr_content_sel = qeiv2_spd_tmr_as_spd_tm;
    mode_config.z_count_inc_mode = qeiv2_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = STEP_MOTOR_QEI_FOC_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.z_cali_ignore_ab = false;
    mode_config.phcnt_idx = 0;
    qeiv2_config_mode(STEP_MOTOR_QEI_BASE, &mode_config);

    return mcl_success;
}
#endif

/**
 * @brief Get encoder theta value
 * @param theta Pointer to store theta value
 * @return mcl_success if successful
 */
hpm_mcl_stat_t encoder_get_theta(float *theta)
{
    return hpm_mcl_abz_get_theta(STEP_MOTOR_QEI_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        -((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), theta);
}

/**
 * @brief Get absolute encoder theta value
 * @param theta Pointer to store absolute theta value
 * @return mcl_success if successful
 */
hpm_mcl_stat_t encoder_get_abs_theta(float *theta)
{
    return hpm_mcl_abz_get_abs_theta(STEP_MOTOR_QEI_BASE, BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV,\
        ((MCL_PI * 2) / BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV), -abs_position_theta, theta);
}

/**
 * @brief Set PWM duty cycle for a specific channel
 * @param chn Channel number
 * @param duty Duty cycle value
 * @return mcl_success if successful
 */
hpm_mcl_stat_t pwm_duty_set(mcl_drivers_channel_t chn, float duty)
{
    uint32_t pwm_reload;
    uint32_t pwm_cmp_half, pwm_reload_half;
    uint32_t index0, index1;

    /* Calculate PWM parameters */
    pwm_reload = PWM_RELOAD;
    pwm_cmp_half = (uint32_t)(duty * pwm_reload) >> 1;
    pwm_reload_half =  pwm_reload >> 1;

    /* Select PWM channel indices */
    switch (chn) {
    case mcl_drivers_chn_a0:
        index0 = BOARD_BLDCPWM_CMP_INDEX_0;
        index1 = BOARD_BLDCPWM_CMP_INDEX_1;
        break;
    case mcl_drivers_chn_a1:
        index0 = BOARD_BLDCPWM_CMP_INDEX_2;
        index1 = BOARD_BLDCPWM_CMP_INDEX_3;
        break;
    case mcl_drivers_chn_b0:
        index0 = BOARD_BLDCPWM_CMP_INDEX_4;
        index1 = BOARD_BLDCPWM_CMP_INDEX_5;
        break;
    case mcl_drivers_chn_b1:
        index0 = BOARD_BLDCPWM_CMP_INDEX_6;
        index1 = BOARD_BLDCPWM_CMP_INDEX_7;
        break;
    default:
        return mcl_fail;
    }

    /* Set PWM compare values */
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index0, PWM_CMP_CMP_SET((pwm_reload_half - pwm_cmp_half)));
    pwm_cmp_force_value(MOTOR0_BLDCPWM, index1, PWM_CMP_CMP_SET((pwm_reload_half + pwm_cmp_half)));
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (index0 + 1), (pwm_reload_half - pwm_cmp_half), 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, (index1 + 1), (pwm_reload_half + pwm_cmp_half), 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif

    return mcl_success;
}

/**
 * @brief Get ADC value for a specific channel
 * @param chn Channel number
 * @param value Pointer to store ADC value
 * @return mcl_success if successful
 */
hpm_mcl_stat_t adc_value_get(mcl_analog_chn_t chn, int32_t *value)
{
    int32_t sens_value;

    /* Get ADC value based on channel */
    switch (chn) {
    case analog_a_current:
        sens_value = MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[ADCU_INDEX][BOARD_BLDC_ADC_TRG*4]);
        *value = sens_value - adc_u_midpoint;
        break;
    case analog_b_current:
        sens_value = MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[ADCV_INDEX][BOARD_BLDC_ADC_TRG*4]);
        *value = sens_value - adc_v_midpoint;
        break;
    default:
        return mcl_fail;
    }

    return mcl_success;
}

/**
 * @brief Enable all PWM outputs
 * @return mcl_success if successful
 */
hpm_mcl_stat_t enable_all_pwm_output(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_disable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_disable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
#endif

    return mcl_success;
}

/**
 * @brief Disable all PWM outputs
 * @return mcl_success if successful
 */
hpm_mcl_stat_t disable_all_pwm_output(void)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    /* Configure PWM force output timing */
    pwm_config_force_cmd_timing(MOTOR0_BLDCPWM, pwm_force_immediately);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    /* Configure PWMV2 force output timing */
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_immediately);
    pwmv2_set_force_update_time(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_immediately);

    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_update_shadow_immediately);
    pwmv2_force_update_time_by_shadow(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_update_shadow_immediately);

    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_force_by_software(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwmv2_enable_software_force(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, pwm_force_output_1, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif

    return mcl_success;
}

#if defined(HPMSOC_HAS_HPMSDK_PWM)
/**
 * @brief Initialize PWM module
 */
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    /* Initialize PWM module */
    pwm_deinit(MOTOR0_BLDCPWM);
    pwm_stop_counter(MOTOR0_BLDCPWM);
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);

    /* Configure comparator settings */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].enable_ex_cmp  = false;
    cmp_config[2].mode           = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = 5;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = PWM_RELOAD;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;

    /* Configure PWM settings */
    pwm_get_default_pwm_config(MOTOR0_BLDCPWM, &pwm_config);
    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = true;

    /* Configure trigger comparator channel */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    /* Setup PWM waveforms for each phase */
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_config, cmp_index + 2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_config, cmp_index + 4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_config, cmp_index + 6, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while (1) {
        }
    }

    /* Configure comparator and start PWM */
    pwm_load_cmp_shadow_on_match(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP,  &cmp_config[3]);
    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);

    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}
#endif

#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
/**
 * @brief Initialize PWMV2 module
 */
void pwm_init(void)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_config_t pwm_cfg;

    /* Configure comparator settings */
    cmp_cfg[0].cmp = PWM_RELOAD;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = cmp_value_from_shadow_val;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(1);
    cmp_cfg[0].update_trigger = pwm_shadow_register_update_on_reload;
    cmp_cfg[1].cmp = PWM_RELOAD;
    cmp_cfg[1].enable_half_cmp = false;
    cmp_cfg[1].enable_hrcmp = false;
    cmp_cfg[1].cmp_source = cmp_value_from_shadow_val;
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(2);
    cmp_cfg[1].update_trigger = pwm_shadow_register_update_on_reload;

    /* Configure PWM settings */
    pwm_cfg.enable_output = true;
    pwm_cfg.enable_async_fault = false;
    pwm_cfg.enable_sync_fault = false;
    pwm_cfg.invert_output = true;
    pwm_cfg.enable_four_cmp = false;
    pwm_cfg.update_trigger = pwm_reload_update_on_reload;
    pwm_cfg.dead_zone_in_half_cycle = 0;

    /* Initialize PWM counter */
    pwmv2_disable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_reset_counter(MOTOR0_BLDCPWM, pwm_counter_0);

    /* Configure shadow registers */
    pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(0), PWM_RELOAD, 0, false);
    pwmv2_set_shadow_val(MOTOR0_BLDCPWM, PWMV2_SHADOW_INDEX(9), 1, 0, false);
    pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);

    /* Setup PWM waveforms for each phase */
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_VH_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(3);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(4);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_VL_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(5);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(6);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_WH_PWM_OUTPIN * 2, &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(7);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(8);
    pwmv2_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_cfg, BOARD_BLDC_WL_PWM_OUTPIN * 2, &cmp_cfg[0], 2);

    /* Configure counter settings */
    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_0, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_1, pwm_reload_update_on_reload);

    pwmv2_counter_select_data_offset_from_shadow_value(MOTOR0_BLDCPWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_set_reload_update_time(MOTOR0_BLDCPWM, pwm_counter_2, pwm_reload_update_on_reload);

    /* Configure trigger settings */
    pwmv2_select_cmp_source(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP, cmp_value_from_shadow_val, PWMV2_SHADOW_INDEX(9));
    pwmv2_set_trigout_cmp_index(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_OUT_CHN, BOARD_BLDCPWM_CMP_TRIG_CMP);
    pwmv2_cmp_select_counter(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP, pwm_counter_0);
    pwmv2_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);

    /* Start PWM counters and outputs */
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_enable_counter(MOTOR0_BLDCPWM, pwm_counter_2);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_0);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_1);
    pwmv2_start_pwm_output(MOTOR0_BLDCPWM, pwm_counter_2);
}
#endif

/**
 * @brief GPTMR interrupt handler
 */
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    if (gptmr_check_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP))) {
        gptmr_clear_status(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
        hpm_mcl_detect_loop(&motor0.detect);
    }
}

/**
 * @brief Initialize timer for motor control
 */
static void timer_init(void)
{
    gptmr_channel_config_t config;

    /* Configure timer clock and channel */
    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD + 1;
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.cmp[1] = config.reload;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);
}

/**
 * @brief Initialize trigger multiplexer
 * @param ptr TRGM base address
 */
void init_trigger_mux(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    /* Configure trigger output for ADC */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(ptr, BOARD_BLDC_TRG_ADC, &trgm_output_cfg);
}

/**
 * @brief ADC interrupt handler
 */
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;

    status = hpm_adc_get_status_flags(&hpm_adc_a);
    if ((status & BOARD_BLDC_ADC_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_a, BOARD_BLDC_ADC_TRIG_FLAG);
        hpm_mcl_encoder_process(&motor0.encoder, motor0.cfg.mcl.physical.time.mcu_clock_tick / PWM_FREQUENCY);
        hpm_mcl_loop(&motor0.loop);
    }
}

/**
 * @brief Initialize trigger configuration
 * @param trig_ch Trigger channel
 * @param inten Interrupt enable flag
 */
void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc_pmt_config_t pmt_cfg;

    /* Configure ADC preempt trigger */
    pmt_cfg.module = BOARD_BLDC_ADC_MODULE;
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.trig_ch   = trig_ch;
    pmt_cfg.config.adc12.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc12.inten[0] = inten;
#else
    pmt_cfg.config.adc16.trig_ch   = trig_ch;
    pmt_cfg.config.adc16.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc16.inten[0] = inten;
#endif
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
}

/**
 * @brief Initialize ADC for motor control
 * @return mcl_success if successful
 */
hpm_mcl_stat_t adc_init(void)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;

    /* Initialize ADC configuration */
    cfg.module = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_default_config(&cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    /* Initialize ADC12 clocks and configuration */
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc12.res            = adc12_res_12_bits;
    cfg.config.adc12.conv_mode      = adc12_conv_mode_preemption;
    cfg.config.adc12.diff_sel       = adc12_sample_signal_single_ended;
    cfg.config.adc12.adc_clk_div    = adc12_clock_divider_3;
    cfg.config.adc12.sel_sync_ahb   = false;
    cfg.config.adc12.adc_ahb_en = true;

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);
#else
    /* Initialize ADC16 clocks and configuration */
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = adc16_clock_divider_4;
    cfg.config.adc16.sel_sync_ahb   = false;
    cfg.config.adc16.adc_ahb_en = true;

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);
#endif

    /* Initialize ADC channels */
    ch_cfg.module = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_channel_default_config(&ch_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    ch_cfg.config.adc12_ch.diff_sel      = adc12_sample_signal_single_ended;
    ch_cfg.config.adc12_ch.sample_cycle  = 15;

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);
#else
    ch_cfg.config.adc16_ch.sample_cycle  = 20;

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);
#endif

    /* Initialize trigger and DMA */
    init_trigger_mux(BOARD_BLDCPWM_TRGM);
    init_trigger_cfg(BOARD_BLDC_ADC_TRG, true);

#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_U_BASE, BOARD_BLDC_ADC_TRG);
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_V_BASE, BOARD_BLDC_ADC_TRG);
#endif

    hpm_adc_init_pmt_dma(&hpm_adc_a, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCU_INDEX]));
    hpm_adc_init_pmt_dma(&hpm_adc_b, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[ADCV_INDEX]));

    return mcl_success;
}

/**
 * @brief Enable ADC interrupts
 */
void adc_isr_enable(void)
{
    hpm_adc_enable_interrupts(&hpm_adc_a, BOARD_BLDC_ADC_TRIG_FLAG);
    intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 1);
}

/**
 * @brief Calculate ADC midpoint values
 */
void motor_adc_midpoint(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t times = 0;

    /* Set PWM duty cycle for midpoint calculation */
    pwm_duty_set(mcl_drivers_chn_a, 0.5);
    pwm_duty_set(mcl_drivers_chn_b, 0.5);
    do {
        adc_u_sum += MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[0][BOARD_BLDC_ADC_TRG*4]);
        adc_v_sum += MCL_GET_ADC_12BIT_VALID_DATA(adc_buff[1][BOARD_BLDC_ADC_TRG*4]);
        times++;
        board_delay_ms(1);
        if (times >= CURRENT_SET_TIME_MS) {
            break;
        }
    } while (1);
    adc_u_midpoint = adc_u_sum / CURRENT_SET_TIME_MS;
    adc_v_midpoint = adc_v_sum / CURRENT_SET_TIME_MS;
}

/**
 * @brief User delay function
 * @param tick Delay time in microseconds
 */
void mcl_user_delay_us(uint64_t tick)
{
    board_delay_us(tick);
}

/**
 * @brief Perform motor angle alignment
 */
void motor_angle_align(void)
{
    mcl_user_value_t id, iq;

    /* Set initial current values */
    id.enable = true;
    id.value = 1;
    hpm_mcl_loop_set_current_d(&motor0.loop, id);
    iq.enable = true;
    iq.value = 0;
    hpm_mcl_loop_set_current_q(&motor0.loop, iq);

    /* Perform angle alignment sequence */
    hpm_mcl_encoder_force_theta(&motor0.encoder, 0, true);
    board_delay_ms(1000);
    hpm_mcl_encoder_set_initial_theta(&motor0.encoder, hpm_mcl_encoder_get_raw_theta(&motor0.encoder));
    board_delay_ms(100);
    hpm_mcl_encoder_force_theta(&motor0.encoder, HPM_PI / 4, true);
    board_delay_ms(100);
    hpm_mcl_encoder_force_theta(&motor0.encoder, 0, false);
    board_delay_ms(100);

    /* Clear current values */
    id.enable = true;
    id.value = 0;
    hpm_mcl_loop_set_current_d(&motor0.loop, id);
    iq.enable = false;
    iq.value = 0;
    hpm_mcl_loop_set_current_q(&motor0.loop, iq);
}

/**
 * @brief Main function for step motor control
 */
int main(void)
{
    char input_data[100], input_end;
    mcl_user_value_t  user_speed;
    mcl_user_value_t user_position;
    uint8_t i;
    uint8_t user_mode;
    float speed;
    int32_t position;

    /* Initialize board and peripherals */
    board_init();
    printf("step motor demo.\r\n");
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    motor_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    motor_init();
    adc_isr_enable();
    timer_init();
    motor_adc_midpoint();
    hpm_mcl_loop_enable(&motor0.loop);
    motor_angle_align();

    /* Set initial speed */
    speed = 1;
    user_speed.enable = true;
    user_speed.value = speed * MCL_PI * 2;
    hpm_mcl_loop_set_speed(&motor0.loop, user_speed);

    /* Main control loop */
    while (1) {
        printf("loop current tick: %d\r\n", hpm_mcl_get_current_loop_run_tick(&motor0.loop));
        printf("Mode selection:\r\n");
        printf("0. Location mode.\r\n");
        printf("1. Speed mode.\r\n");
        printf("Enter mode code:\r\n");
        char option = getchar();
        if (option == '0') {
            user_mode = 0;
            break;
        } else if (option == '1') {
            user_mode = 1;
            break;
        }
    }

    /* Speed control mode */
    if (user_mode == 1) {
        printf("\r\nSpeed mode, motor run, speed is: %f.\r\nInput speed:\r\n", (double)speed);
        while (1) {
            memset(input_data, 0, sizeof(input_data));
            input_end = 1;
            i = 0;
            while (input_end) {
                char option = getchar();
                switch (option) {
                case '\n':
                case '\r':
                    input_end = 0;
                    break;
                default:
                    input_data[i++] = option;
                    break;
                }
                if (i >= sizeof(input_data)) {
                    i = 0;
                    printf("Input Err. Please try again.\r\n");
                    break;
                }
            }
            if (i != 0) {
                speed = atof(input_data);
                if (speed > SPEED_MAX) {
                    speed = SPEED_MAX;
                } else if (speed < -SPEED_MAX) {
                    speed = -SPEED_MAX;
                }
                user_speed.value = speed * MCL_PI * 2;
                hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
                printf("loop current tick: %d\r\n", hpm_mcl_get_current_loop_run_tick(&motor0.loop));
                printf("\r\nSpeed mode, motor run, speed is: %f.\r\nInput speed:\r\n", (double)speed);
            }
        }
    }
    /* Position control mode */
    else if (user_mode == 0) {
        user_speed.enable = false;
        hpm_mcl_encoder_get_absolute_theta(&motor0.encoder, &abs_position_theta);
        hpm_mcl_loop_set_speed(&motor0.loop, user_speed);
        hpm_mcl_enable_position_loop(&motor0.loop);
        user_position.enable = true;
        user_position.value = 0;
        position = 0;
        hpm_mcl_loop_set_position(&motor0.loop, user_position);
        printf("\r\nLocation mode, motor run, The location is: %d.\r\nInput Location:\r\n ", position);
        while (1) {
            memset(input_data, 0, sizeof(input_data));
            input_end = 1;
            i = 0;
            while (input_end) {
                char option = getchar();

                switch (option) {
                case '\n':
                case '\r':
                    input_end = 0;
                    break;
                default:
                    input_data[i++] = option;
                    break;
                }
                if (i >= sizeof(input_data)) {
                    printf("Input Err. Please try again.\r\n");
                    i = 0;
                    break;
                }
            }
            if (i != 0) {
                position = atoi(input_data);
                user_position.value = (float)position * 0.00157079632f;
                hpm_mcl_loop_set_position(&motor0.loop, user_position);
                printf("loop current tick: %d\r\n", hpm_mcl_get_current_loop_run_tick(&motor0.loop));
                printf("\r\nLocation mode, motor run, The location is: %d.\r\nInput Location:\r\n", position);
            }
        }
    }
}
