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

#define TEST_PRINTF_PRED_SPEED 1

void test_mmc_pred_close_loop(void)
{
    mmc_set_sysclk_freq(TEST_MMC, clock_get_frequency(BOARD_MOTOR_CLK_NAME));

    /* track coef config */
    mmc_coef_input_t coef_para = {0};
    coef_para.coef_p = 5;   /* Adjust this parameter according to motor system */
    coef_para.coef_i = 0.5; /* Adjust this parameter according to motor system */
    coef_para.coef_a = 0;   /* Adjust this parameter according to motor system */
    coef_para.cmd_mask = mmc_coef_update_all;
    mmc_track_config_coef_para(TEST_MMC, &coef_para);

    /* track config */
    mmc_track_mode_t control;
    mmc_track_get_default_mode_config(TEST_MMC, &control);
    control.open_loop_mode = false;
    control.sync_new_pos = true;
    mmc_track_config_mode(TEST_MMC, &control);

    /* predictor config */
    mmc_pred_mode_t config;
    mmc_pred_get_default_mode_config(TEST_MMC, &config);
    config.open_loop_mode = false;
    config.pred_mode = mmc_pred_continuously_repeat;
    mmc_pred_config_mode(TEST_MMC, 0, &config);

    mmc_enable_pred(TEST_MMC, 0);
    mmc_enable_module(TEST_MMC);
}

void mmc_to_qeo_test(void)
{
    trgm_pos_matrix_config(HPM_TRGM0, trgm_pos_matrix_output_to_mmc0, trgm_pos_matrix_in_from_sei_pos1, false);
    trgm_pos_matrix_config(HPM_TRGM0, trgm_pos_matrix_output_to_qeo0, trgm_pos_matrix_in_from_mmc0_pos0, false);

    qeo_gen_pwm_output();

    sei_record_data();

    test_mmc_pred_close_loop();

    /* First enable MOTOR peripheral devices, such as MMC, and then enable timestamp for MOTOR */
    synt_enable_timestamp(HPM_SYNT, true);
    synt_enable_timestamp_debug_stop(HPM_SYNT, true);
}


int main(void)
{
    board_init();
    printf("MMC example\n");
    printf("MMC tracks position from SEI and predicts postion to QEO, QEO controls PWM module generates signals\n");

    mmc_to_qeo_test();

#if TEST_PRINTF_PRED_SPEED
    mmc_pos_out_t pred_pos_out = {0};
    while (1) {
        mmc_pred_get_result(TEST_MMC, 0, &pred_pos_out);
        printf("\n");
        printf("speed: %.08f\n", pred_pos_out.speed);
        printf("rev  : %d\n", pred_pos_out.revolution);
        printf("pos  : 0x%08x\n", pred_pos_out.position);
        board_delay_ms(100);
    }
#endif

    while (1) {
    }

    return 0;
}
