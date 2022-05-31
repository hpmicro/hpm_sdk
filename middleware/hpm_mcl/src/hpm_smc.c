/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "stdlib.h"
#include "hpm_bldc_define.h"
#include <math.h>
void bldc_smc_pos_cal(BLDC_CONTROL_SMC_PARA *par)
{	   
    HPM_MOTOR_MATH_TYPE   alhpa_err;
    HPM_MOTOR_MATH_TYPE   beta_err;
    float theta;
    par->ialpha_mem = HPM_MOTOR_MATH_MUL(par->I_motorpar->O_smc_f,par->ialpha_mem) + HPM_MOTOR_MATH_MUL(par->I_motorpar->O_smc_g,(*par->ualpha - par->alpha_cal - par->zalpha_cal));
    par->ibeta_mem = HPM_MOTOR_MATH_MUL(par->I_motorpar->O_smc_f,par->ibeta_mem) + HPM_MOTOR_MATH_MUL(par->I_motorpar->O_smc_g,(*par->ubeta - par->beta_cal - par->zbeta_cal));

    alhpa_err = par->ialpha_mem - *par->ialpha;
    beta_err = par->ibeta_mem - *par->ibeta;

    if (HPM_MOTOR_MATH_FABS(alhpa_err) < par->I_Ezero)
       par->zalpha_cal = HPM_MOTOR_MATH_MUL(par->I_ksmc,HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(2),alhpa_err));  
    else if (alhpa_err >= par->I_Ezero) 
       par->zalpha_cal = par->I_ksmc;
    else if (alhpa_err <= -par->I_Ezero) 
       par->zalpha_cal = -par->I_ksmc;

    if (HPM_MOTOR_MATH_FABS(beta_err) < par->I_Ezero)
       par->zbeta_cal = HPM_MOTOR_MATH_MUL(par->I_ksmc,HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(2),beta_err));  
    else if (beta_err >= par->I_Ezero) 
       par->zbeta_cal = par->I_ksmc;
    else if (beta_err <= -par->I_Ezero) 
       par->zbeta_cal = -par->I_ksmc;

    par->alpha_cal = par->alpha_cal + HPM_MOTOR_MATH_MUL(par->I_kfil,(par->zalpha_cal-par->alpha_cal));
    par->beta_cal = par->beta_cal + HPM_MOTOR_MATH_MUL(par->I_kfil,(par->zbeta_cal-par->beta_cal));
    
    theta = atan2(HPM_MOTOR_MATH_MDF_FL(-par->alpha_cal),HPM_MOTOR_MATH_MDF_FL(par->beta_cal))*(180/3.1415926); 
    if(theta < 0){
        theta = theta+ 360;
    }
    theta += 90;
    if(theta > 360){
        theta = theta - 360;
    }
    *par->theta = HPM_MOTOR_MATH_FL_MDF(theta);
}

void bldc_smc_const_cal(BLDC_MOTOR_PARA *par)
{	
   par->O_smc_g = HPM_MOTOR_MATH_FL_MDF((par->I_PhaseVol_v/par->I_PhaseCur_a)*(1/par->I_Rstator_ohm)*(1-par->O_smc_f));
	par->O_smc_f = HPM_MOTOR_MATH_FL_MDF(exp((-par->I_Rstator_ohm/par->I_Lstator_h)*(par->I_SamplingPer_s)));
}

