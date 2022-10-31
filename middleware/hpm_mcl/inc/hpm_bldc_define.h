/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLDC_DEFINE_H
#define HPM_BLDC_DEFINE_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup  middleware_mcl_interfaces HPMicro Motor Control Library
 * @ingroup middleware_interfaces
 *
 */

#ifdef CONFIG_BLDC_HAS_EXTRA_CONFIG
#include CONFIG_BLDC_HAS_EXTRA_CONFIG
#endif

#include "hpm_motor_math.h"
/**
 * @addtogroup  mcl_common HPMicro MCL
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief bldc motor rotation direction
 *
 */
#define BLDC_MOTOR_DIR_FORWARD 0
#define BLDC_MOTOR_DIR_REVERSE 1

/**
 * @brief Motor pin definition U, V, W three pairs
 *
 */
#define BLDC_PWM_PIN_UH     0
#define BLDC_PWM_PIN_UL     1
#define BLDC_PWM_PIN_VH     2
#define BLDC_PWM_PIN_VL     3
#define BLDC_PWM_PIN_WH     4
#define BLDC_PWM_PIN_WL     5

/**
 * @brief Motor serial number, listing four motors
 *
 */
#define BLDC_MOTOR0_INDEX                (1)
#define BLDC_MOTOR1_INDEX                (2)
#define BLDC_MOTOR2_INDEX                (3)
#define BLDC_MOTOR3_INDEX                (4)

/**
 * @brief PWM output channel definition, used for internal calculations
 *
 */
#define BLDC_PWM_U          0
#define BLDC_PWM_V          1
#define BLDC_PWM_W          2

/**
 * @brief All function pointers defined in the middleware point to this function,
 * and if the implementation of the function is not initialized to make the call,
 * an error will be reported through this function.
 *
 */
extern void bldc_nullcallback_func(void);

/**
 * @brief The mounting angle of the Hall sensor, 60 degrees or 120 degrees,
 * is an inherent property of the motor.
 *
 */
typedef enum bldc_hall_phase {
    bldc_hall_phase_60, /*60 °*/
    bldc_hall_phase_120 /*120 °*/
} bldc_hall_phase_t;

/**
 * @brief Motor related structure definition.
 * Naming rules:
 * physical property description definition.
 * Input (I)/output (O)_physical_volume_description_units_of_physical_volume.
 *
 */
typedef struct bldc_motor_par {
    float  I_Rstator_ohm; 	        /**< Stator resistance (in ohm) */
    float  I_Poles_n;               /**< polar logarithm */
    float  I_MaxSpeed_rs;           /**< Maximum speed r/s */
    float  I_Lstator_h;				/**< Stator inductor */
    float  I_PhaseCur_a; 			/**< Rated current */
    float  I_PhaseVol_v;			/**< Rated voltage */
    float  I_SamplingPer_s;			/**< Current sampling period */
    HPM_MOTOR_MATH_TYPE  O_smc_f;   /**< Sliding mode control factor1 */
    HPM_MOTOR_MATH_TYPE  O_smc_g;   /**< Sliding mode control factor2 */
    void   (*func_smc_const)();     /**< Calculate the function of the sliding mode control coefficient */
} BLDC_MOTOR_PARA;

#define BLDC_MOTOR_PARA_DEFAULTS {0, 0, 0,\
                                        0, 0, 0,\
                                        0, 0, 0,\
                                        &bldc_nullcallback_func}

/**
 * @brief Speed calculation parameters
 *
 */
typedef struct  bldc_contrl_spd_par {
    uint16_t            I_speedacq;               /**< Update velocity data once after collecting n times of angle data */
    uint16_t            num;                    /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       I_speedLooptime_s;        /**< Time for n cycles. Unit s */
    HPM_MOTOR_MATH_TYPE       speedtheta;             /**< Current motor angle */
    HPM_MOTOR_MATH_TYPE       speedlasttheta;          /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       speedthetaLastN;        /**< Internal Data, Initialization Clear */
    HPM_MOTOR_MATH_TYPE       I_speedfilter;            /**< Low-pass filter coefficient */
    HPM_MOTOR_MATH_TYPE       O_speedout_filter;        /**< Speed after filter */
    HPM_MOTOR_MATH_TYPE       O_speedout;               /**< Speed before filter */
    BLDC_MOTOR_PARA     *I_motorpar;                /**< Motor operating parameters */
    void    (*func_getspd)();                     /**< Speed calculation function */
} BLDC_CONTRL_SPD_PARA;

#define BLDC_CONTRL_SPD_PARA_DEFAULTS {0, 0, 0, 0, 0,\
                                        0, 0, 0, 0, NULL,\
                                        &bldc_nullcallback_func}

/**
 * @brief pid control parameters
 *
 */
typedef struct bldc_contrl_pid_par {
    HPM_MOTOR_MATH_TYPE       I_kp;                   /**< Kp */
    HPM_MOTOR_MATH_TYPE       I_ki;                   /**< Ki */
    HPM_MOTOR_MATH_TYPE       I_kd;                   /**< Kd */
    HPM_MOTOR_MATH_TYPE       I_max;                  /**< Output max,  min = -max */
    HPM_MOTOR_MATH_TYPE       target;                 /**< Target parameters */
    HPM_MOTOR_MATH_TYPE       mem;                    /**< Intenal Data */
    HPM_MOTOR_MATH_TYPE       cur;                    /**< Sampling data  */
    HPM_MOTOR_MATH_TYPE       outval;                 /**< Output Data */
    void (*func_pid)();                               /**< Pid function */
} BLDC_CONTRL_PID_PARA;
#define BLDC_CONTRL_PID_PARA_DEFAULTS {0, 0, 0, 0,\
                                        0, 0, 0, 0,\
                                       &bldc_nullcallback_func}

/**
 * @brief Current sampling parameters
 *
 */
typedef struct bldc_contrl_current_par {
    uint16_t            adc_u;                  /**< u Phase current AD sampling value */
    uint16_t            adc_v;                  /**< v Phase current AD sampling value */
    uint16_t            adc_w;                  /**< W Phase current AD sampling value */
    uint16_t            adc_u_middle;           /**< u Phase current midpoint AD sampling value */
    uint16_t            adc_v_middle;           /**< v Phase current midpoint AD sampling value */
    uint16_t            adc_w_middle;           /**< w Phase current midpoint AD sampling value */
    HPM_MOTOR_MATH_TYPE       cal_u;            /**< Calculated U-phase current */
    HPM_MOTOR_MATH_TYPE       cal_v;            /**< Calculated V-phase current */
    HPM_MOTOR_MATH_TYPE       cal_w;            /**< Calculated W-phase current */
    void *userdata;                             /**< user data */
    void (*func_sampl)();                       /**< current samples */
} BLDC_CONTROL_CURRENT_PARA;
#define BLDC_CONTROL_CURRENT_PARA_DEFAULTS {0, 0, 0,\
                                            0, 0, 0,\
                                            0, 0, 0,\
                                            NULL, &bldc_nullcallback_func}

/**
 * @brief PWM output parameters
 *
 */
typedef struct bldc_control_pwmout_par {
    uint8_t             I_motor_id;           /**< Motor id @ref BLDC_MOTOR0_INDEX ... BLDC_MOTOR3_INDEX */
    uint8_t             I_sync_id;            /**< Synchronization id */
    uint32_t            pwm_u;                /**< u pwm duty cycle */
    uint32_t            pwm_v;                /**< v pwm duty cycle */
    uint32_t            pwm_w;                /**< w pwm duty cycle */
    uint32_t            I_pwm_reload;         /**< pwm reload value, pwm configuration related */
    void (*func_set_pwm)();                   /**< pwm output function */
} BLDC_CONTROL_PWMOUT_PARA;
#define BLDC_CONTROL_PWMOUT_PARA_DEFAULTS {0, 0, 0,\
                                            0, 0, 0,\
                                            &bldc_nullcallback_func}
/**
 * @brief svpwm parameters
 *
 */
typedef struct bldc_control_pwm_par {
    HPM_MOTOR_MATH_TYPE       target_alpha; /**< alpha voltage */
    HPM_MOTOR_MATH_TYPE       target_beta;  /**< beta voltage */
    int8_t             sector;             /**< Sector Number */
    uint32_t            I_pwm_reload_max;       /**< Maximum duty cycle the pwm module can output */
    BLDC_CONTROL_PWMOUT_PARA    pwmout; /**< @ref BLDC_CONTROL_PWMOUT_PARA */
    void (*func_spwm)();                    /**< svpwm function */
} BLDC_CONTROL_PWM_PARA;
#define BLDC_CONTROL_PWM_PARA_DEFAULTS {0, 0, 0, 0,\
                                           BLDC_CONTROL_PWMOUT_PARA_DEFAULTS,\
                                            &bldc_nullcallback_func}
/*sliding mode control(SMC)*/
typedef struct bldc_control_smc_par {
    HPM_MOTOR_MATH_TYPE       I_Ezero;    /**< Default value 0.5, slip mode convergence */
    HPM_MOTOR_MATH_TYPE       I_ksmc;   /**< Slide coefficient */
    HPM_MOTOR_MATH_TYPE       I_kfil;   /**< Slide coefficient */
    HPM_MOTOR_MATH_TYPE       *ualpha;  /**< alpha voltage */
    HPM_MOTOR_MATH_TYPE       *ubeta;   /**< beta voltage */
    HPM_MOTOR_MATH_TYPE       *ialpha;  /**< alpha current */
    HPM_MOTOR_MATH_TYPE       *ibeta;   /**< beta current */
    HPM_MOTOR_MATH_TYPE       ialpha_mem;   /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       ibeta_mem;    /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       alpha_cal;    /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       zalpha_cal;   /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       beta_cal; /**< Internal Data */
    HPM_MOTOR_MATH_TYPE       zbeta_cal;    /**< Internal Data */
    /*输出*/
    HPM_MOTOR_MATH_TYPE       *theta;   /**< Estimated angle value */
    BLDC_MOTOR_PARA     *I_motorpar;    /**< Motor parameters @ref BLDC_MOTOR_PARA */
    void (*func_smc)();                 /**< Slide-mode controller */
} BLDC_CONTROL_SMC_PARA;

#define BLDC_CONTROL_SMC_PARA_DEFAULTS {0, 0, 0, NULL, NULL,\
                                        NULL, NULL, 0, 0, 0,\
                                        0, 0, 0, NULL,\
                                        NULL,\
                                         &bldc_nullcallback_func}
/**
 * @brief Location estimation function
 *
 */
typedef struct bldc_func_cal {
    void *par;
    void (*func)();
} BLDC_FUNC_CAL;

#define BLDC_FUNC_CAL_DEFAULTS {NULL, &bldc_nullcallback_func}

/**
 * @brief foc control
 *
 */
typedef struct bldc_contrl_foc_par {
    BLDC_CONTRL_PID_PARA        CurrentDPiPar;             /**< D-axis current pi parameters */
    BLDC_CONTRL_PID_PARA        CurrentQPiPar;             /**< Q-axis current pi parameters */
    BLDC_CONTRL_SPD_PARA        SpeedCalPar;               /**< Speed calculation parameters */
    HPM_MOTOR_MATH_TYPE         electric_angle;            /**< Electric angle */
    BLDC_CONTROL_CURRENT_PARA   samplCurpar;               /**< Sampling current */
    BLDC_MOTOR_PARA             motorpar;                  /**< Motor parameters */
    BLDC_CONTROL_PWM_PARA       pwmpar;                    /**< PWM parameters */
    BLDC_FUNC_CAL               pos_estimator_par;         /**< Null pointers do not run the position estimation algorithm, pointers are assigned for position estimation */
    HPM_MOTOR_MATH_TYPE               u_alpha;             /**< alpha voltage */
    HPM_MOTOR_MATH_TYPE               u_beta;              /**< beta voltage */
    HPM_MOTOR_MATH_TYPE               i_alpha;             /**< alpha current */
    HPM_MOTOR_MATH_TYPE               i_beta;              /**< beta current */
    void (*func_dqsvpwm)();                                /**< dq axis current to svpwm function */
} BLDC_CONTROL_FOC_PARA;
#define BLDC_CONTROL_FOC_PARA_DEFAULTS {BLDC_CONTRL_PID_PARA_DEFAULTS, BLDC_CONTRL_PID_PARA_DEFAULTS,\
                                        BLDC_CONTRL_SPD_PARA_DEFAULTS, 0,\
                                        BLDC_CONTROL_CURRENT_PARA_DEFAULTS, BLDC_MOTOR_PARA_DEFAULTS,\
                                        BLDC_CONTROL_PWM_PARA_DEFAULTS,\
                                        BLDC_FUNC_CAL_DEFAULTS,\
                                        0, 0, 0, 0, &bldc_nullcallback_func}

/**
 * @}
 *
 */

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif
