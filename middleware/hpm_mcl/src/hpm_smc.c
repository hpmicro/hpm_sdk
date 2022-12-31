/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "stdlib.h"
#include "hpm_bldc_define.h"
#include <math.h>
void hpm_mcl_bldc_smc_pos_cal(BLDC_CONTROL_SMC_PARA *par)
{
    HPM_MOTOR_MATH_TYPE   alhpa_err;
    HPM_MOTOR_MATH_TYPE   beta_err;
    float theta;
    par->ialpha_mem = HPM_MOTOR_MATH_MUL(par->i_motorpar->o_smc_f, par->ialpha_mem) +
        HPM_MOTOR_MATH_MUL(par->i_motorpar->o_smc_g, (*par->ualpha - par->alpha_cal - par->zalpha_cal));
    par->ibeta_mem = HPM_MOTOR_MATH_MUL(par->i_motorpar->o_smc_f, par->ibeta_mem) +
        HPM_MOTOR_MATH_MUL(par->i_motorpar->o_smc_g, (*par->ubeta - par->beta_cal - par->zbeta_cal));

    alhpa_err = par->ialpha_mem - *par->ialpha;
    beta_err = par->ibeta_mem - *par->ibeta;

    if (HPM_MOTOR_MATH_FABS(alhpa_err) < par->i_ezero)
        par->zalpha_cal = HPM_MOTOR_MATH_MUL(par->i_ksmc, HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(2), alhpa_err));
    else if (alhpa_err >= par->i_ezero)
        par->zalpha_cal = par->i_ksmc;
    else if (alhpa_err <= -par->i_ezero)
        par->zalpha_cal = -par->i_ksmc;

    if (HPM_MOTOR_MATH_FABS(beta_err) < par->i_ezero)
        par->zbeta_cal = HPM_MOTOR_MATH_MUL(par->i_ksmc, HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(2), beta_err));
    else if (beta_err >= par->i_ezero)
        par->zbeta_cal = par->i_ksmc;
    else if (beta_err <= -par->i_ezero)
        par->zbeta_cal = -par->i_ksmc;

    par->alpha_cal = par->alpha_cal + HPM_MOTOR_MATH_MUL(par->i_kfil, (par->zalpha_cal-par->alpha_cal));
    par->beta_cal = par->beta_cal + HPM_MOTOR_MATH_MUL(par->i_kfil, (par->zbeta_cal-par->beta_cal));

    theta = atan2(HPM_MOTOR_MATH_MDF_FL(-par->alpha_cal), HPM_MOTOR_MATH_MDF_FL(par->beta_cal))*(180/3.1415926);
    if (theta < 0) {
        theta = theta + 360;
    }
    theta += 90;
    if (theta > 360) {
        theta = theta - 360;
    }
    *par->theta = HPM_MOTOR_MATH_FL_MDF(theta);
}

void hpm_mcl_bldc_smc_const_cal(BLDC_MOTOR_PARA *par)
{
    par->o_smc_g = HPM_MOTOR_MATH_FL_MDF((par->i_phasevol_v/par->i_phasecur_a)*(1/par->i_rstator_ohm)*(1-par->o_smc_f));
	par->o_smc_f = HPM_MOTOR_MATH_FL_MDF(exp((-par->i_rstator_ohm/par->i_lstator_h)*(par->i_samplingper_s)));
}

