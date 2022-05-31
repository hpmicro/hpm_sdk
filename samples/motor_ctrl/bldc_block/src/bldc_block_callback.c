/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_bldc_define.h"
#include "hpm_bldc_block_func.h"
#include "hpm_pwm_drv.h"
#include "bldc_block_cfg.h"
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
  motor_index -= 1;
  pwm_disable_pwm_sw_force_output(motor_pwm_tbl[motor_index],pwm_uvw_conversion_tbl[motor_index][pin_name]);
}

void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name)
{
  motor_index -= 1;
  pwm_enable_pwm_sw_force_output(motor_pwm_tbl[motor_index],pwm_uvw_conversion_tbl[motor_index][pin_name]);
}