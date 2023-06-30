/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include <stdlib.h>
#include "hpm_bldc_define.h"
#include "hpm_smc.h"
#include "hpm_foc.h"
#include <math.h>

#define PI (3.1415926)

static void hpm_mcl_sin_cos(float angle, float *sin_angle, float *cos_angle)
{
    float angle_f;

    angle_f = angle * PI / 180;
    *sin_angle = sinf(angle_f);
    *cos_angle = cosf(angle_f);
}

void hpm_mcl_smc_pos_cal(hpm_mcl_para_t *par)
{
    float   alhpa_err;
    float   beta_err;

    par->ialpha_mem = par->i_motorpar->o_smc_f * par->ialpha_mem +
        par->i_motorpar->o_smc_g * (*par->ualpha - par->alpha_cal - par->zalpha_cal);
    par->ibeta_mem = par->i_motorpar->o_smc_f * par->ibeta_mem +
        par->i_motorpar->o_smc_g * (*par->ubeta - par->beta_cal - par->zbeta_cal);

    alhpa_err = par->ialpha_mem - *par->ialpha;
    beta_err = par->ibeta_mem - *par->ibeta;

    if (fabs(alhpa_err) < par->zero)
        par->zalpha_cal = 2 * alhpa_err * par->ksmc;
    else if (alhpa_err >= par->zero)
        par->zalpha_cal = par->ksmc;
    else if (alhpa_err <= -par->zero)
        par->zalpha_cal = -par->ksmc;

    if (fabs(beta_err) < par->zero)
        par->zbeta_cal = 2 * beta_err * par->ksmc;
    else if (beta_err >= par->zero)
        par->zbeta_cal = par->ksmc;
    else if (beta_err <= -par->zero)
        par->zbeta_cal = -par->ksmc;

    /**
     * @brief LOW-PASS
     *
     */
    par->alpha_cal = (1 - par->filter_coeff) * par->alpha_cal + par->filter_coeff * par->zalpha_cal;
    par->beta_cal = (1 - par->filter_coeff) * par->beta_cal + par->filter_coeff * par->zbeta_cal;
}

void hpm_mcl_smc_const_cal(hpm_motor_para_t *par)
{
    par->o_smc_g = par->i_samplingper_s / par->i_lstator_h;
    par->o_smc_f = 1 - (par->i_rstator_ohm / par->i_lstator_h) * par->i_samplingper_s;
}

float hpm_mcl_smc_pll(hpm_mcl_para_t *par, hpm_smc_pll_para_t *pll)
{
    float portion_asp = 0;
	float portion_asi = 0;

    pll->err = -par->alpha_cal * cosf(pll->theta_last) -
            par->beta_cal * sinf(pll->theta_last);
    /**
     * @brief PI
     *
     */
    portion_asp = pll->kp * pll->err;
    portion_asi = pll->ki * pll->err;
    portion_asi += pll->mem;

    if (portion_asi > pll->max_i) {
        portion_asi = pll->max_i;
    } else if (portion_asi < pll->min_i) {
        portion_asi = pll->min_i;
    }
    pll->mem = portion_asi;
    pll->speedout = portion_asi + portion_asp;

    if (pll->speedout > pll->max_o) {
        pll->speedout = pll->max_o;
    } else if (pll->speedout < pll->min_o) {
        pll->speedout = pll->min_o;
    }

    pll->theta_last += pll->speedout * pll->loop_in_sec;
    pll->theta_last = fmodf(pll->theta_last, PI*2);
    pll->theta =  fmodf(pll->theta_last * 57.29578 + pll->theta0, 360);

    return pll->theta;
}

void hpm_mcl_smc_loop(BLDC_CONTROL_FOC_PARA *par, hpm_mcl_para_t *smc, hpm_smc_pll_para_t *pll, bool is_smc_enable)
{

    float sin_angle = 0;
    float cos_angle = 0;

    par->samplcurpar.func_sampl(&par->samplcurpar);
    hpm_mcl_bldc_foc_clarke(par->samplcurpar.cal_u, par->samplcurpar.cal_v, par->samplcurpar.cal_w,
                    &par->ialpha, &par->ibeta);
    if (par->pos_estimator_par.func != NULL) {
        par->pos_estimator_par.func(par->pos_estimator_par.par);
        hpm_mcl_smc_pll(smc, pll);
        if (is_smc_enable) {
            par->electric_angle = pll->theta;
        }
    }
    hpm_mcl_sin_cos(par->electric_angle, &sin_angle, &cos_angle);
    hpm_mcl_bldc_foc_park(par->ialpha, par->ibeta,
                &par->currentdpipar.cur, &par->currentqpipar.cur,
                sin_angle, cos_angle);

    par->currentdpipar.func_pid(&par->currentdpipar);
    par->currentqpipar.func_pid(&par->currentqpipar);

    hpm_mcl_bldc_foc_inv_park(par->currentdpipar.outval, par->currentqpipar.outval,
        &par->ualpha, &par->ubeta, sin_angle, cos_angle);
    par->pwmpar.target_alpha = par->ualpha;
    par->pwmpar.target_beta = par->ubeta;
    par->pwmpar.func_spwm(&par->pwmpar);

}
