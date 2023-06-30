/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_bldc_define.h"
#include "hpm_over_zero.h"
#include "hpm_block.h"

#define HPM_OVER_ZERO_INIT_FILTER_TIMES 15
#define HPM_OVER_ZERO_FILTER_TIMES 5
#define HPM_OVER_ZERO_INDEX_GET(interval, dir) (interval << dir)

void hpm_mcl_over_zero_pwm_ctrl(uint8_t motorindex, uint8_t step)
{
    switch (step) {
    case 2:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        break;
    case 1:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        break;
    case 6:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        break;
    case 5:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        break;
    case 4:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        break;
    case 3:
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UH);/*uh*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_UL);/*ul*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_VL);/*vl*/
        hpm_mcl_bldc_pwm_disable(motorindex, BLDC_PWM_PIN_WH);/*wh*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_WL);/*wl*/
        hpm_mcl_bldc_pwm_enable(motorindex, BLDC_PWM_PIN_VH);/*vh*/
        break;

    default:
        break;
    }
}

int8_t hpm_mcl_over_zero_step_get(hpm_mcl_over_zero_cfg_t *cfg)
{
    int32_t adc_over_zero_u, adc_over_zero_v, adc_over_zero_w;

    /**
     * @brief Determine the current state machine and perform different operations
     *
     */
    switch (cfg->fsm) {
    case hpm_mcl_over_zero_fsm_init:
        cfg->last_interval = 0;
        cfg->delay_degree_30 = 0;
        cfg->number_consecutive_zeros_w = 0;
        cfg->adc_zero_ph = 0;
        cfg->last_interval_tick = 0;
        cfg->number_consecutive_zeros = 0;
        cfg->interval_tick = 0;
        cfg->speed_tick = 0;
        cfg->interval = 0;
        cfg->fsm = hpm_mcl_over_zero_fsm_location;
        break;
    case hpm_mcl_over_zero_fsm_location:
        adc_over_zero_w = (cfg->adc_u + cfg->adc_v) >> 1;
        if (cfg->adc_zero_ph == 0) {
            if (adc_over_zero_w - cfg->adc_w > 0) {
                cfg->number_consecutive_zeros_w++;
                if (cfg->number_consecutive_zeros_w > HPM_OVER_ZERO_INIT_FILTER_TIMES) {
                    cfg->adc_zero_ph = 1;
                    cfg->number_consecutive_zeros_w = 0;
                }
            } else {
                cfg->number_consecutive_zeros_w = 0;
                /**< Continue waiting for positioning to complete */
                return -1;
            }
        } else if (cfg->adc_zero_ph == 1) {
            if (adc_over_zero_w - cfg->adc_w > 0) {
                cfg->number_consecutive_zeros_w++;
                if (cfg->number_consecutive_zeros_w > HPM_OVER_ZERO_INIT_FILTER_TIMES) {
                    cfg->adc_zero_ph = 2;
                }
            } else {
                cfg->number_consecutive_zeros_w = 0;
                /**< Continue waiting for positioning to complete */
                return -1;
            }
        } else if (cfg->adc_zero_ph == 2) {
            cfg->last_interval = hpm_mcl_interval_w_down;
            cfg->interval = cfg->last_interval;
            cfg->next_interval = hpm_mcl_interval_w_down;
            if (cfg->motor_dir) {
                cfg->next_interval = (cfg->interval % 6) + 1;
            } else {
                cfg->next_interval -= 1;
                if (cfg->next_interval == 0) {
                    cfg->next_interval = hpm_mcl_interval_u_up;
                }
            }
            cfg->delay_degree_30 = HPM_OVER_ZERO_INIT_FILTER_TIMES;
            cfg->interval_tick = HPM_OVER_ZERO_INIT_FILTER_TIMES;
            cfg->number_consecutive_zeros = 0;
            cfg->fsm = hpm_mcl_over_zero_fsm_running;
        }
        break;
    case hpm_mcl_over_zero_fsm_running:
        /**< Number of cumulative cycles */
        cfg->interval_tick++;
        cfg->delay_degree_30++;
        adc_over_zero_u = (cfg->adc_w + cfg->adc_v) >> 1;
        adc_over_zero_v = (cfg->adc_w + cfg->adc_u) >> 1;
        adc_over_zero_w = (cfg->adc_u + cfg->adc_v) >> 1;
        switch (HPM_OVER_ZERO_INDEX_GET(cfg->next_interval, cfg->motor_dir)) {
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_w_down, hpm_motor_dir_forward):
            /* w_down */
            if (adc_over_zero_w - cfg->adc_w > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_w_up, hpm_motor_dir_forward):
            /* w_up */
            if (adc_over_zero_w - cfg->adc_w < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_v_up, hpm_motor_dir_forward):
            /* v up */
            if (adc_over_zero_v - cfg->adc_v < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_v_down, hpm_motor_dir_forward):
            /* v down */
            if (adc_over_zero_v - cfg->adc_v > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_u_down, hpm_motor_dir_forward):
            /* u down */
            if (adc_over_zero_u - cfg->adc_u > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_u_up, hpm_motor_dir_forward):
            /* u up */
            if (adc_over_zero_u - cfg->adc_u < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_w_down, hpm_motor_dir_reverse):
            /* w_down */
            if (adc_over_zero_w - cfg->adc_w < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_w_up, hpm_motor_dir_reverse):
            /* w_up */
            if (adc_over_zero_w - cfg->adc_w > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_v_up, hpm_motor_dir_reverse):
            /* v up */
            if (adc_over_zero_v - cfg->adc_v > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_v_down, hpm_motor_dir_reverse):
            /* v down */
            if (adc_over_zero_v - cfg->adc_v < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_u_down, hpm_motor_dir_reverse):
            /* u down */
            if (adc_over_zero_u - cfg->adc_u < 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        case HPM_OVER_ZERO_INDEX_GET(hpm_mcl_interval_u_up, hpm_motor_dir_reverse):
            /* u up */
            if (adc_over_zero_u - cfg->adc_u > 0) {
                cfg->number_consecutive_zeros++;
            } else {
                cfg->number_consecutive_zeros = 0;
            }
            break;
        default:
            break;
        }
        if (cfg->number_consecutive_zeros >= HPM_OVER_ZERO_FILTER_TIMES) {
            cfg->number_consecutive_zeros = 0;
            cfg->last_interval_tick = cfg->interval_tick;
            cfg->interval_tick = 0;
            cfg->delay_degree_30 = HPM_OVER_ZERO_FILTER_TIMES;
            if (cfg->motor_dir == hpm_motor_dir_forward) {
                cfg->next_interval = (cfg->interval % 6) + 1;
            } else {
                cfg->next_interval -= 1;
                if (cfg->next_interval == 0) {
                    cfg->next_interval = hpm_mcl_interval_u_up;
                }
            }
        }
        if (cfg->delay_degree_30 > (cfg->last_interval_tick >> 1)) {
            cfg->speed_tick = cfg->last_interval_tick;
            cfg->interval =  cfg->next_interval;
        }
        break;
    default:
        break;
    }
    return cfg->interval;
}

float hpm_mcl_over_zero_speed_filter(hpm_mcl_over_zero_spd_para_t *par)
{
    float portion_asp = 0;
    float portion_asi = 0;

    /**
     * @brief Low pass
     *
     */
    par->err = par->speed - par->speedout;
    par->err_last = par->filter_coef * par->err + (1 - par->filter_coef) * par->err_last;
    /**
     * @brief PI
     *
     */
    portion_asp = par->kp * par->err_last;
    portion_asi = par->ki * par->err_last;
    portion_asi += par->mem;

    if (portion_asi > par->integral_max) {
        portion_asi = par->integral_max;
    } else if (portion_asi < par->integral_min) {
        portion_asi = par->integral_min;
    }
    par->mem = portion_asi;
    portion_asi += portion_asp;

    if (portion_asi > par->output_max) {
        portion_asi = par->output_max;
    } else if (portion_asi < par->output_min) {
        portion_asi = par->output_min;
    }

    par->speedout = portion_asi;

    return par->speedout;
}

float hpm_mcl_over_zero_cal_speed(hpm_mcl_over_zero_cfg_t *cfg)
{
    return ((float)((60.0f / cfg->pole_num) / 360.0f))/(cfg->loop_time_in_sec *
        (cfg->speed_tick + 1));
}

float hpm_mcl_over_zero_pi_contrl(hpm_mcl_over_zero_pi_para_t *par)
{
    float result = 0;

    float curerr = 0;
    float portion_asp = 0;
    float portion_asi = 0;

    curerr = par->target - par->cur;
    portion_asp = curerr * par->kp;
    portion_asi = curerr * par->ki + par->mem;
    if (portion_asi < par->integral_min) {
        portion_asi = par->integral_min;
    } else if (portion_asi > par->integral_max) {
        portion_asi = par->integral_max;
    }
    par->mem = portion_asi;
    result = portion_asi + portion_asp;
    if (result < par->output_min) {
        result = par->output_min;
    } else if (result > par->output_max) {
        result = par->output_max;
    }

    par->outval = result;
    return result;
}
