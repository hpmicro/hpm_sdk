/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_bldc_define.h"
#include "hpm_foc.h"
#include "hpm_pwm_drv.h"
#include "bldc_foc_cfg.h"
/*输入 BLDC_PWM_PIN_UH,查找数组后输出BOARD_BLDCPWM_OUTPUT_PIN1 */
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
/*多电机对应的pwm*/
PWM_Type *motor_pwm_tbl[4] = {
  MOTOR0_BLDCPWM
};
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name)
{
  pwm_disable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
}

void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name)
{
  pwm_enable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
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
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, PWM_CMP_CMP_SET((pwm_reload + pwm_u_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_1, PWM_CMP_CMP_SET((pwm_reload - pwm_u_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_2, PWM_CMP_CMP_SET((pwm_reload + pwm_v_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_3, PWM_CMP_CMP_SET((pwm_reload - pwm_v_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_4, PWM_CMP_CMP_SET((pwm_reload + pwm_w_half)));
        pwm_cmp_force_value(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_5, PWM_CMP_CMP_SET((pwm_reload - pwm_w_half)));

    break;
  
    default:
    break;
  }
}