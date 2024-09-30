/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SQUARE_WAVE_INJECT_H
#define HPM_SQUARE_WAVE_INJECT_H

#include "hpm_common.h"
#include "hpm_bldc_define.h"
#include "hpm_foc.h"
#include "hpm_smc.h"

/**
 * @addtogroup mcl_hfi_interface HPMicro MCL HFI APIs
 * @ingroup middleware_hfi_interfaces
 * @{
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#ifndef PI
#define PI HPM_PI
#endif

/**
 * @brief HFI parameters and calculation data
 *
 */
typedef struct  hpm_hfi_para {
    float       vh;         /**< Injection Voltage */
    float       last_alpha;
    float       last_beta;
    float       e_alpha;
    float       e_beta;
    float       e_theta;
    float       alpha;
    float       beta;
    bool       period;  /**< loop tick */
    void    (*func)(void *str);
} hpm_hfi_para_t;

#define BLDC_CONTROL_INJECT_PARA_DEFAULTS {\
                                            0, 0, 0,\
                                            0, 0, 0, 0, 0, 0,\
                                         NULL\
}

/**
 * @brief HFI of magnetic pole identification parameters
 *
 */
typedef struct hpm_hfi_pole_detect_para {
    int16_t    currentd_force;
    volatile uint8_t     status;
    uint16_t    times;
    uint16_t    current_d_init_val;     /**< Voltage injected during identification */
    int32_t     theta_pi;               /**< Voltage at 180 degrees */
    int32_t     theta_zero;             /**< Voltage at 0 degrees */
    bool    (*func)(void *foc, void *inject,
        void *pll, void *pole);
} hpm_hfi_pole_detect_para_t;

#define BLDC_CONTROL_INJECT_POLE_DETECT_PARA_DEFAULTS {\
                                        0, 0, 0, 0, 0, 0,\
                                        NULL\
}

/**
 * @brief HFI pll Filter Parameters
 *
 */
typedef struct  hpm_hfi_pll_para {
    float       theta_last;
    float       err_last;
    float       low_pass;
    float       filter;
    float       deta;
    float       theta;
    float       kp;
    float       ki;
    float       mem_max;
    float       mem_min;
    float       mem;
    float       loop_s;
    float       err;
    void    (*func)(void *str);
} hpm_hfi_pll_para_t;

#define BLDC_CONTROL_INJECT_PLL_PARA_DEFAULTS {\
                                            0, 0, 0, 0, 0,\
                                            0, 0, 0, 0, 0, 0, 0, 0,\
                                         NULL\
}

/**
 * @brief HFI current loop
 *
 * @param par @ref BLDC_CONTROL_FOC_PARA
 * @param inject @ref hpm_hfi_para_t
 * @param pll @ref hpm_hfi_pll_para_t
 * @param pole @ref hpm_hfi_pole_detect_para_t
 */
void hpm_mcl_hfi_loop(BLDC_CONTROL_FOC_PARA *par, hpm_hfi_para_t *inject,
        hpm_hfi_pll_para_t *pll, hpm_hfi_pole_detect_para_t *pole);

/**
 * @brief HFI Magnetic Pole Detection
 *
 * @param foc @ref BLDC_CONTROL_FOC_PARA
 * @param inject @ref hpm_hfi_para_t
 * @param pll @ref hpm_hfi_pll_para_t
 * @param pole @ref hpm_hfi_pole_detect_para_t
 * @retval true Complete pole identification
 *         false pole detection failure
 */
bool hpm_mcl_hfi_pole_detect(BLDC_CONTROL_FOC_PARA *foc, hpm_hfi_para_t *inject,
        hpm_hfi_pll_para_t *pll, hpm_hfi_pole_detect_para_t *pole);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

/**
 * @}
 *
 */

#endif
