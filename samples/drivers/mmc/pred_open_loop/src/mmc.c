/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_mmc_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "moto.h"

#define TEST_MMC HPM_MMC0
#define TEST_PRED_CH0 0
#define TEST_MMC_IRQ IRQn_MMC0

volatile uint32_t speed_irq_count;    /* speed out threshold irq count */
volatile uint32_t position_irq_count; /* position out threshold irq count */
void mmc_pred_speed_trig_update_pred_para(double trig_speed, double new_accel);
void mmc_pred_position_trig_update_pred_para(int32_t trig_revolution, uint32_t trig_position, double new_accel, bool less_than);

#define TEST_PRINTF_PRED_SPEED 1

SDK_DECLARE_EXT_ISR_M(TEST_MMC_IRQ, mmc_isr)
void mmc_isr(void)
{
    uint32_t pred_stat = mmc_pred_get_status(TEST_MMC, TEST_PRED_CH0);
    mmc_pos_out_t pred_result;
    mmc_pred_get_result(TEST_MMC, TEST_PRED_CH0, &pred_result);

    if ((pred_stat & mmc_pred_pos_trig_valid) == mmc_pred_pos_trig_valid) {
        position_irq_count += 1;
        mmc_pred_speed_trig_update_pred_para(4.0, 0);
        mmc_pred_disable_position_trig(TEST_MMC, TEST_PRED_CH0);
    }

    if ((pred_stat & mmc_pred_speed_trig_valid) == mmc_pred_speed_trig_valid) {
        speed_irq_count += 1;
        if (pred_result.speed > 0) {
            mmc_pred_position_trig_update_pred_para(48, 0, -1.0, false);
        } else {
            mmc_pred_position_trig_update_pred_para(8, 0, 1.0, true);
        }
        mmc_pred_disable_speed_trig(TEST_MMC, TEST_PRED_CH0);
    }

    mmc_pred_clear_status(TEST_MMC, TEST_PRED_CH0, pred_stat);
}

/**
 * @brief mmc pred moudle config speed event trig update pred parameter
 *
 * @param [in] trig_speed absolute value of speed trigger threshold - r/s
 * @param [in] new_accel value of target accel - r/s
 */
void mmc_pred_speed_trig_update_pred_para(double trig_speed, double new_accel)
{
    /* speed trig config */
    mmc_speed_trig_t speed_trig = {0};
    speed_trig.less_than = false;       /*!< Speed greater than threshold */
    speed_trig.absolute_compare = true; /*!< Comparing absolute values of speed */
    speed_trig.speed_thr = trig_speed;  /*!< r/s */
    speed_trig.enable = true;           /*!< enable speed trig */
    mmc_pred_config_speed_trig(TEST_MMC, TEST_PRED_CH0, &speed_trig);

    /* parameter updates config */
    mmc_pos_or_delta_pos_input_t pred_pos = {0};
    pred_pos.accel = new_accel;
    pred_pos.cmd_mask = mmc_pos_update_accel;
    pred_pos.trigger = mmc_pos_update_by_self_speed_thr;
    mmc_pred_config_pos_para(TEST_MMC, TEST_PRED_CH0, &pred_pos, true);
}

/**
 * @brief mmc pred moudle config position event trig update pred parameter
 *
 * @param [in] trig_revolution value of revolution trigger threshold
 * @param [in] trig_position value of position trigger threshold
 * @param [in] new_accel target accel value r/s
 * @param [in] less_than true for less than, false for greater than
 */
void mmc_pred_position_trig_update_pred_para(int32_t trig_revolution, uint32_t trig_position, double new_accel, bool less_than)
{
    /* position trig config */
    mmc_pos_trig_t pos_trig = {0};
    pos_trig.less_than = less_than ? true : false;
    pos_trig.enable = true;
    pos_trig.position_thr = trig_position;
    pos_trig.revolution_thr = trig_revolution;
    mmc_pred_config_position_trig(TEST_MMC, TEST_PRED_CH0, &pos_trig);

    /* parameter updates config */
    mmc_pos_or_delta_pos_input_t pred_pos = {0};
    pred_pos.accel = new_accel;
    pred_pos.cmd_mask = mmc_pos_update_accel;
    pred_pos.trigger = mmc_pos_update_by_self_pos_thr;
    mmc_pred_config_pos_para(TEST_MMC, TEST_PRED_CH0, &pred_pos, true);
}

void test_mmc_open_loop_pred(void)
{
    mmc_set_sysclk_freq(TEST_MMC, clock_get_frequency(BOARD_MOTOR_CLK_NAME));

    /* config pred work mode */
    mmc_pred_mode_t config;
    mmc_pred_get_default_mode_config(TEST_MMC, &config);
    config.speed_trig_int = true;    /*!< enable pred speed trig interrupt */
    config.position_trig_int = true; /*!< enable pred position trig interrupt */
    config.open_loop_mode = true;    /*!< open loop mode */
    config.pred_mode = mmc_pred_continuously_repeat;  /*!< pred work mode */
    mmc_pred_config_mode(TEST_MMC, TEST_PRED_CH0, &config);

    /* config pred init parameter, Take effect immediately by setting timestamp value 0 */
    mmc_pos_or_delta_pos_input_t pred_pos = {0};
    pred_pos.pos_time = 0;
    pred_pos.position = 0;   /*!< position in a circle*/
    pred_pos.revolution = 0; /*!< r */
    pred_pos.speed = 0.0;    /*!< r/s */
    pred_pos.accel = 1.0;    /*!< r/s */
    pred_pos.cmd_mask = mmc_pos_update_all;
    pred_pos.trigger = mmc_pos_update_by_timestamp;
    mmc_pred_config_pos_para(TEST_MMC, TEST_PRED_CH0, &pred_pos, true);

    /* enable interrupt */
    intc_m_enable_irq_with_priority(TEST_MMC_IRQ, 1);

    /* config speed event trig position parameter change */
    mmc_pred_speed_trig_update_pred_para(4.0, 0);

    mmc_enable_pred(TEST_MMC, TEST_PRED_CH0); /*!< enable pred */
    mmc_enable_module(TEST_MMC);  /*!< enable mmc module  */
}

void test_mmc_to_qeo(void)
{
    trgm_pos_matrix_config(HPM_TRGM0, trgm_pos_matrix_output_to_qeo0, trgm_pos_matrix_in_from_mmc0_pos0, false);

    if (status_success != qeo_gen_abz_signal()) {
        printf("config QEO abz signal failed\n");
    }
    qeo_gen_pwm_output();

    test_mmc_open_loop_pred();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}

int main(void)
{
    board_init();
    printf("MMC example\n");
    printf("MMC openloop pred postion to QEO\n");

    test_mmc_to_qeo();

#if TEST_PRINTF_PRED_SPEED
    mmc_pos_out_t pred_result;
    printf("MMC pred speed value:\n");
    while (1) {
        mmc_pred_get_result(TEST_MMC, TEST_PRED_CH0, &pred_result);
        printf("%.08f\n", pred_result.speed);
        board_delay_ms(20U);
    }
#endif

    while (1) {
    }

    return 0;
}
