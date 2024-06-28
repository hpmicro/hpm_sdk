/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_bldc_define.h"
#include "hpm_block.h"
#if defined(HPMSOC_HAS_HPMSDK_PWM)
#include "hpm_pwm_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
#include "hpm_pwmv2_drv.h"
#endif
#include "bldc_over_zero_cfg.h"

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
  motor_index -= 1;
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_disable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_disable_software_force(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
}

void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name)
{
  motor_index -= 1;
#if defined(HPMSOC_HAS_HPMSDK_PWM)
    pwm_enable_pwm_sw_force_output(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
#if defined(HPMSOC_HAS_HPMSDK_PWMV2)
    pwmv2_enable_software_force(motor_pwm_tbl[motor_index], pwm_uvw_conversion_tbl[motor_index][pin_name]);
#endif
}