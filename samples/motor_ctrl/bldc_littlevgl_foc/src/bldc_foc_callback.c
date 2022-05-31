/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_bldc_define.h"
#include "hpm_bldc_foc_func.h"
#include "hpm_pwm_drv.h"
#include "bldc_foc_cfg.h"
/*输入 BLDC_PWM_PIN_UH,查找数组后输出BOARD_BLDCPWM_OUTPUT_PIN1 */
const uint8_t pwm_uvw_conversion_tbl[4][6] ={
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
PWM_Type* motor_pwm_tbl[4] ={
  MOTOR0_BLDCPWM
};
void bldc_pwm_enable(uint8_t motor_index,uint8_t pin_name)
{
  pwm_disable_pwm_sw_force_output(motor_pwm_tbl[motor_index],pwm_uvw_conversion_tbl[motor_index][pin_name]);
}

void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name)
{
  pwm_enable_pwm_sw_force_output(motor_pwm_tbl[motor_index],pwm_uvw_conversion_tbl[motor_index][pin_name]);
}

void bldc_foc_pwmset(BLDC_CONTROL_PWMOUT_PARA *par)
{
  uint32_t pwm_reload ;
  pwm_reload = par->I_pwm_reload;
  switch (par->I_motor_id){
    case BLDC_MOTOR0_INDEX:
      pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, BOARD_BLDCPWM_CMP_INDEX_1,
            (pwm_reload + par->pwm_u) >> 1, (pwm_reload - par->pwm_u) >> 1);
      pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_2, BOARD_BLDCPWM_CMP_INDEX_3,
            (pwm_reload + par->pwm_v) >> 1, (pwm_reload - par->pwm_v) >> 1);
      pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_4, BOARD_BLDCPWM_CMP_INDEX_5, 
            (pwm_reload + par->pwm_w) >> 1, (pwm_reload - par->pwm_w) >> 1);
    break;
  
  default:
    break;
  }
  switch (par->I_sync_id){
    case BLDC_MOTOR0_PWM_T:
      pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
    break;
    default:
    break;
  }
}