/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_SMC_H
#define HPM_SMC_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup mcl_smc_interface HPMicro MCL SMC APIs
 * @ingroup middleware_mcl_interfaces
 * @{
 *
 */

/**
 * @brief smc phase locked loop parameters
 *
 */
typedef struct  hpm_smc_pll_para {
    float       theta_last; /**< the last angle of the filter */
    float       err;    /**< angle errors */
    float       speedout;   /**< speed resulting from phase locked loop processing */
    float       theta;  /**< electrical angle */
    float       kp; /**< pid kp */
    float       ki; /**< pid ki */
    float       max_i;    /**< max integral */
    float       min_i;    /**< min integral */
    float       max_o;    /**< max output */
    float       min_o;  /**< min output */
    float       mem;    /**< integral storage */
    float       theta0; /**< initial angle */
    float       loop_in_sec; /**< cycle time in s */
    void    (*func_getspd)();
} hpm_smc_pll_para_t;

#define BLDC_CONTROL_SMC_PLL_PARA_DEFAULTS {0, 0, 0, 0,\
                                            0, 0, 0, 0,\
                                            0, 0, 0, 0, 0,\
                                         &hpm_mcl_nullcallback_func}

/**
 * @brief sliding mode control(SMC)
 *
 */
typedef struct hpm_mcl_para {
    float       zero;    /**< slip mode convergence */
    float       ksmc;   /**< Slide coefficient */
    float       filter_coeff;   /**< low-pass filter coefficients */
    float       *ualpha;  /**< alpha voltage */
    float       *ubeta;   /**< beta voltage */
    float       *ialpha;  /**< alpha current */
    float       *ibeta;   /**< beta current */
    float       ialpha_mem;   /**< Internal Data */
    float       ibeta_mem;    /**< Internal Data */
    float       alpha_cal;    /**< Internal Data */
    float       zalpha_cal;   /**< Internal Data */
    float       beta_cal; /**< Internal Data */
    float       zbeta_cal;    /**< Internal Data */
    hpm_motor_para_t     *i_motorpar;    /**< Motor parameters @ref hpm_motor_para_t */
    void (*func_smc)();                 /**< Slide-mode controller */
} hpm_mcl_para_t;

#define BLDC_CONTROL_SMC_PARA_DEFAULTS {0, 0, 0, NULL, NULL,\
                                        NULL, NULL, 0, 0, 0,\
                                        0, 0, 0,\
                                        NULL,\
                                         &hpm_mcl_nullcallback_func}

/**
 * @brief Sliding mode control function
 *
 * @param[inout] par @ref hpm_mcl_para_t
 */
void hpm_mcl_smc_pos_cal(hpm_mcl_para_t *par);

/**
 * @brief Calculation of sliding mode control static parameters
 *
 * @param[inout] par @ref hpm_motor_para_t
 */
void hpm_mcl_smc_const_cal(hpm_motor_para_t *par);

/**
 * @brief Phase-locked loop filtering of angles after smc processing
 *
 * @param[in] par @refhpm_mcl_para_t
 * @param[inout] pll @ref hpm_smc_pll_para_t
 * @return angle
 */
float hpm_mcl_smc_pll(hpm_mcl_para_t *par, hpm_smc_pll_para_t *pll);

/**
 * @brief smc current loop
 *
 * @param[inout] par @ref BLDC_CONTROL_FOC_PARA
 * @param[inout] smc @ref hpm_mcl_para_t
 * @param[inout] pll @ref hpm_smc_pll_para_t
 * @param[in] is_smc_enable true: smc enable, false: smc disable
 */
void hpm_mcl_smc_loop(BLDC_CONTROL_FOC_PARA *par, hpm_mcl_para_t *smc, hpm_smc_pll_para_t *pll, bool is_smc_enable);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/**
 * @}
 *
 */

#endif

