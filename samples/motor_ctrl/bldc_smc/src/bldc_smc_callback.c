/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_bldc_define.h"
#include "hpm_foc.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif
#include "bldc_smc_cfg.h"

const uint8_t pwm_uvw_conversion_tbl[4][6] = {
    {
        BOARD_BLDC_UH_PWM_OUTPIN,
        BOARD_BLDC_UL_PWM_OUTPIN,
        BOARD_BLDC_VH_PWM_OUTPIN,
        BOARD_BLDC_VL_PWM_OUTPIN,
        BOARD_BLDC_WH_PWM_OUTPIN,
        BOARD_BLDC_WL_PWM_OUTPIN
    }
};
#if defined(HPMSOC_HAS_HPMSDK_PWM)
PWM_Type *motor_pwm_tbl[4] = {
    MOTOR0_BLDCPWM
};
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
PWMV2_Type *motor_pwm_tbl[4] = {
    MOTOR0_BLDCPWM
};
#endif
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_disable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_disable_software_force(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
}

void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name)
{
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_enable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_enable_software_force(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
}

void hpm_mcl_bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par)
{
    uint32_t pwm_reload;
    uint32_t pwm_u_half, pwm_v_half, pwm_w_half;

    pwm_reload = par->i_pwm_reload >> 1;
    switch (par->i_motor_id) {
    case BLDC_MOTOR0_INDEX:
        pwm_u_half =  par->pwm_u >> 1;
        pwm_v_half =  par->pwm_v >> 1;
        pwm_w_half =  par->pwm_w >> 1;
#if defined(HPMSOC_HAS_HPMSDK_PWM)
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
        pwmv2_shadow_register_unlock(MOTOR0_BLDCPWM);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_1, (pwm_reload - pwm_u_half), 0, false);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_2, (pwm_reload + pwm_u_half), 0, false);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_3, (pwm_reload - pwm_v_half), 0, false);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_4, (pwm_reload + pwm_v_half), 0, false);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_5, (pwm_reload - pwm_w_half), 0, false);
        pwmv2_set_shadow_val(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_6, (pwm_reload + pwm_w_half), 0, false);
        pwmv2_shadow_register_lock(MOTOR0_BLDCPWM);
#endif
        break;

    default:
        break;
    }
}