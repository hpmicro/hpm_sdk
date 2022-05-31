/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "hpm_bldc_define.h"
#include "hpm_bldc_block_func.h"
/*HALL func begin*/
void bldc_block_ctrl(uint8_t motorindex, uint8_t dir ,uint8_t step)
{
    if(dir == BLDC_MOTOR_DIR_FORWARD){
        switch (step){
        case 2:
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 3:
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 4:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 5:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 6:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 1:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        
        default:
            break;
        }
    }
    else{
        switch (step)
        {
        case 2:
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UL);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 3:
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UL);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VH);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 4:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VH);/*vl*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WL);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 5:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*uh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UH);/*ul*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VL);/*vl*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WL);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wl*/
            break;
        case 6:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*uh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_UH);/*ul*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VL);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WH);/*wh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wl*/
            break;
        case 1:
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UH);/*uh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_UL);/*ul*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_VL);/*vh*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_VH);/*vl*/
            bldc_pwm_disable(motorindex,BLDC_PWM_PIN_WL);/*wh*/
            bldc_pwm_enable(motorindex,BLDC_PWM_PIN_WH);/*wl*/
            break;
        
        default:
            break;
        }
    }
}
/*BLDC Block HALL position*/
uint8_t bldc_block_step_get(bldc_hall_phase_t phase, uint8_t hall_u,uint8_t hall_v,uint8_t hall_w) 
{
    /* 
    *tbl_para = (u<<2)|(v<<1)|(w);
    * 0: resv;
    */
   const uint8_t hall_tbl_120[8] ={0,4,2,3,6,5,1,0};
   const uint8_t hall_tbl_60[8] ={5,4,0,3,6,0,1,2};
    if (phase == bldc_hall_phase_120){
        return hall_tbl_120[(hall_u<<2)|(hall_v<<1)|hall_w];
    }
    else if (phase == bldc_hall_phase_60){
        return hall_tbl_60[(hall_u<<2)|(hall_v<<1)|hall_w];
    }
    return 0;
}

/*pi_ctrl*/
float al_pi_ctrl_func(float *memory,float targetSpeed,float speed,float kp,float ki,float max)
{
	float result = 0;//PI运算结果
	float spderr = 0;       //测量值和实际值的差值
	float portion_asp = 0;  //本次比例部分的值
	float portion_asi = 0;  //本次积分部分的值
    float *portion_asi_last_speed;

	spderr = targetSpeed - speed;
    portion_asi_last_speed = memory;

	portion_asp = spderr * kp;
	portion_asi = spderr * ki;
	portion_asi += *portion_asi_last_speed;
	*portion_asi_last_speed = portion_asi;//保存这次的积分值
	if(*portion_asi_last_speed < (-max)){
		*portion_asi_last_speed = -max;
	}
	if(*portion_asi_last_speed > max){
		*portion_asi_last_speed = max;
	}
	result = portion_asi + portion_asp;
	if(result < -max){
		result = -max;
	}
	if(result > max){
		result = max;
	}

	return result;
}
