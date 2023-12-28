/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_hfi.h"

typedef enum {
    pole_stat_init = 0,
    pole_stat1 = 1,
    pole_stat2,
    pole_stat3,
    pole_stat4,
    pole_stat_ok
} pole_detect_stat_t;

static void hpm_mcl_hfi_sin_cos(HPM_MOTOR_MATH_TYPE angle,
                            HPM_MOTOR_MATH_TYPE *sin_angle, HPM_MOTOR_MATH_TYPE *cos_angle)
{
    float angle_f;

    angle_f = angle;
    *sin_angle = sinf(angle_f);
    *cos_angle = cosf(angle_f);
}

void hpm_mcl_hfi_core(float alpha, float beta, hpm_hfi_para_t *inject)
{
    float alpha_inject, beta_inject;

    inject->e_alpha = (inject->last_alpha + alpha) / 2;
    inject->e_beta = (inject->last_beta + beta) / 2;
    alpha_inject = (alpha - inject->last_alpha);
    beta_inject = (beta - inject->last_beta);

    if (inject->vh < 0) {
        alpha_inject = -alpha_inject;
        beta_inject = -beta_inject;
    }
    inject->alpha = alpha_inject;
    inject->beta = beta_inject;

    inject->last_beta = beta;
    inject->last_alpha = alpha;
}

float hpm_mcl_hfi_pll(float alpha, float beta, float sin_val, float cos_val, hpm_hfi_pll_para_t *pll)
{
    float portion_asp = 0;
    float portion_asi = 0;

    pll->err_last = beta * cos_val - alpha * sin_val;
    portion_asp = pll->kp * pll->err_last;
    portion_asi = pll->ki * pll->err_last;
    portion_asi += pll->mem;

    if (portion_asi > pll->mem_max) {
        portion_asi = pll->mem_max;
    } else if (portion_asi < pll->mem_min) {
        portion_asi = pll->mem_min;
    }
    pll->mem = portion_asi;
    portion_asi += portion_asp;
    pll->deta += portion_asi * pll->loop_s;
    if (pll->deta > PI) {
        pll->deta -= 2 * PI;
    } else if (pll->deta < -PI) {
        pll->deta += 2 * PI;
    }
    if (pll->deta < 0) {
        pll->deta += 2 * PI;
    }
    pll->theta = pll->deta;

    return pll->theta;
}

void hpm_mcl_hfi_loop(BLDC_CONTROL_FOC_PARA *par, hpm_hfi_para_t *inject,
        hpm_hfi_pll_para_t *pll, hpm_hfi_pole_detect_para_t *pole)
{
    static HPM_MOTOR_MATH_TYPE sin_angle = 0;
    static HPM_MOTOR_MATH_TYPE cos_angle = 0;

    par->samplcurpar.func_sampl(&par->samplcurpar);
    hpm_mcl_bldc_foc_clarke(par->samplcurpar.cal_u, par->samplcurpar.cal_v, par->samplcurpar.cal_w,
                    &par->ialpha, &par->ibeta);
    hpm_mcl_hfi_core(par->ialpha, par->ibeta, inject);
    hpm_mcl_hfi_pll(inject->alpha, inject->beta, sin_angle, cos_angle, pll);
    par->ialpha = inject->e_alpha;
    par->ibeta = inject->e_beta;
    hpm_mcl_hfi_sin_cos(par->electric_angle, &sin_angle, &cos_angle);
    hpm_mcl_bldc_foc_park(par->ialpha, par->ibeta,
                &par->currentdpipar.cur, &par->currentqpipar.cur,
                sin_angle, cos_angle);
    if (pole->currentd_force != 0) {
        par->currentdpipar.target = pole->currentd_force;
    }
    par->currentdpipar.func_pid(&par->currentdpipar);
    par->currentqpipar.func_pid(&par->currentqpipar);
    par->currentdpipar.outval += inject->vh;
    inject->vh = -inject->vh;
    hpm_mcl_bldc_foc_inv_park(par->currentdpipar.outval, par->currentqpipar.outval,
        &par->ualpha, &par->ubeta, sin_angle, cos_angle);
    par->pwmpar.target_alpha = par->ualpha;
    par->pwmpar.target_beta = par->ubeta;
    par->pwmpar.func_spwm(&par->pwmpar);

}

bool hpm_mcl_hfi_pole_detect(BLDC_CONTROL_FOC_PARA *foc, hpm_hfi_para_t *inject,
        hpm_hfi_pll_para_t *pll, hpm_hfi_pole_detect_para_t *pole)
{
    float idh, iqh;
    static float sin_angle, cos_angle;

    if (pole->status == pole_stat_init) {
        pole->currentd_force = 0;
        foc->currentqpipar.target =  0;
        foc->currentdpipar.target = 0;
        pole->theta_zero = 0;
        pole->theta_pi = 0;
    } else if (pole->status == pole_stat1) {
        pole->times++;
        if (pole->times >= 50) {
            pole->currentd_force = pole->current_d_init_val;
            if (pole->times >= 55) {
                hpm_mcl_hfi_sin_cos(foc->electric_angle, &sin_angle, &cos_angle);
                hpm_mcl_bldc_foc_park(inject->alpha, inject->beta, &idh, &iqh, sin_angle, cos_angle);
                pole->theta_zero += fabs(idh);
                if (pole->times >= 60) {
                    pole->currentd_force =  0;
                    pole->times = 0;
                    pole->status = pole_stat2;
                }
            }
        }
    } else if (pole->status == pole_stat2) {
        pole->times++;
        if (pole->times > 50) {
            pole->status = pole_stat3;
            pole->times = 0;
        }
    } else if (pole->status == pole_stat3) {
        pole->currentd_force = -pole->current_d_init_val;
        pole->times++;
        if (pole->times >= 5) {
            hpm_mcl_hfi_sin_cos(foc->electric_angle, &sin_angle, &cos_angle);
            hpm_mcl_bldc_foc_park(inject->alpha, inject->beta, &idh, &iqh, sin_angle, cos_angle);
            pole->theta_pi += fabs(idh);
            if (pole->times >= 10) {
                pole->currentd_force =  0;
                pole->times = 0;
                pole->status = pole_stat4;
                if (pole->theta_zero > pole->theta_pi) {
                    pll->deta += PI;
                }
                if (pll->deta > PI) {
                    pll->deta -= 2 * PI;
                } else if (pll->deta < -PI) {
                    pll->deta += 2 * PI;
                }

                if (pll->deta < 0) {
                    pll->deta += 2 * PI;
                }
            }
        }
    } else if (pole->status == pole_stat4) {
        pole->times++;
        if (pole->times > 500) {
            pole->status = pole_stat_ok;
            pole->times = 0;
            pole->theta_zero = 0;
            pole->theta_pi = 0;
        }
    } else if (pole->status == pole_stat_ok) {
        return true;
    }
    return false;
}
