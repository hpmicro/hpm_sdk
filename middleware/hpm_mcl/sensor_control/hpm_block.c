/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "hpm_bldc_define.h"
#include "hpm_block.h"
/*HALL func begin*/
void hpm_mcl_bldc_block_ctrl(uint8_t motorindex, uint8_t dir, uint8_t step)
{
    if (dir == BLDC_MOTOR_DIR_FORWARD) {
        switch (step) {
        case 2:
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 3:
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 4:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 5:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 6:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 1:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;

        default:
            break;
        }
    } else {
        switch (step) {
        case 2:
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 3:
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 4:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vl*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 5:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*uh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*ul*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 6:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*uh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*ul*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 1:
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vh*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vl*/
            hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wh*/
            hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wl*/
            break;

        default:
            break;
        }
    }
}
/*BLDC Block HALL position*/
uint8_t hpm_mcl_bldc_block_step_get(bldc_hall_phase_t phase, uint8_t hall_u, uint8_t hall_v, uint8_t hall_w)
{
    /*
     *tbl_para = (u<<2)|(v<<1)|(w);
     * 0: resv;
     */
   const uint8_t hall_tbl_120[8] = {0, 4, 2, 3, 6, 5, 1, 0};
   const uint8_t hall_tbl_60[8] = {5, 4, 0, 3, 6, 0, 1, 2};
    if (phase == bldc_hall_phase_120) {
        return hall_tbl_120[(hall_u<<2)|(hall_v<<1)|hall_w];
    } else if (phase == bldc_hall_phase_60) {
        return hall_tbl_60[(hall_u<<2)|(hall_v<<1)|hall_w];
    }
    return 0;
}

/*pi_ctrl*/
float hpm_mcl_al_pi_ctrl_func(float *memory, float targetspeed, float speed, float kp, float ki, float max)
{
	float result = 0;
	float spderr = 0;
	float portion_asp = 0;
	float portion_asi = 0;
    float *portion_asi_last_speed;

	spderr = targetspeed - speed;
    portion_asi_last_speed = memory;

	portion_asp = spderr * kp;
	portion_asi = spderr * ki;
	portion_asi += *portion_asi_last_speed;
	*portion_asi_last_speed = portion_asi;
	if (*portion_asi_last_speed < (-max)) {
		*portion_asi_last_speed = -max;
	}
	if (*portion_asi_last_speed > max) {
		*portion_asi_last_speed = max;
	}
	result = portion_asi + portion_asp;
	if (result < -max) {
		result = -max;
	}
	if (result > max) {
		result = max;
	}

	return result;
}
