/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_plb_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"

#define PWM_PERIOD_IN_MS (1)
uint32_t reload;

void plb_set_counter(uint32_t val)
{
    plb_type_b_set_cmp_val(BOARD_PLB_COUNTER, BOARD_PLB_CHN, plb_type_b_cmp0, val + 1);
}

void plb_reset_counter(void)
{
    plb_type_a_inject_by_sw(BOARD_PLB_COUNTER, BOARD_PLB_CHN, 0);
}

void plb_32bit_counter_init(void)
{
    plb_type_a_truth_t plb_type_a_lut;
    plb_type_b_inject_by_sw(BOARD_PLB_COUNTER, BOARD_PLB_CHN, 0);
    plb_type_a_inject_by_sw(BOARD_PLB_COUNTER, BOARD_PLB_CHN, 0);

    /**
     * @brief pwm
     *
     */
    plb_type_a_lut.val = 0x1111;
    plb_type_a_set_lut(BOARD_PLB_COUNTER, BOARD_PLB_CHN, plb_type_a_table0, &plb_type_a_lut);
    /**
     * @brief pulse
     *
     */
    plb_type_a_lut.val = 0x0303;
    plb_type_a_set_lut(BOARD_PLB_COUNTER, BOARD_PLB_CHN, plb_type_a_table1, &plb_type_a_lut);

    for (uint8_t i = plb_type_b_slice_0; i <= plb_type_b_slice_15; i++) {
        plb_type_b_set_lut(BOARD_PLB_COUNTER, BOARD_PLB_CHN, i, plb_slice_opt_keep);
    }
    for (uint8_t i = plb_type_b_slice_1; i <= plb_type_b_slice_15; i += 2) {
        plb_type_b_set_lut(BOARD_PLB_COUNTER, BOARD_PLB_CHN, i, plb_slice_opt_add_one);
    }
    plb_type_b_set_cmp_mode(BOARD_PLB_COUNTER, BOARD_PLB_CHN, plb_type_b_cmp0, plb_cmp_mode_ge);
    plb_type_b_inject_by_sw(BOARD_PLB_COUNTER, BOARD_PLB_CHN, 0);
    plb_type_a_inject_by_sw(BOARD_PLB_COUNTER, BOARD_PLB_CHN, 0);
}

void init_pwm_plb_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_PLB_PWM_TRG;
    trgm_output_config(ptr, BOARD_PLB_IN_PWM_TRG_NUM, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT16;
    trgm_output_config(ptr, TRGM_TRGOCFG_PLB_IN_01, &trgm_output_cfg);


    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
    trgm_output_cfg.input  = BOARD_PLB_PWM_TRG;
    trgm_output_config(ptr, BOARD_PLB_IN_PWM_PULSE_TRG_NUM, &trgm_output_cfg);

    trgm_output_cfg.invert = true;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT01;
    trgm_output_config(ptr, TRGM_TRGOCFG_PLB_IN_16, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_PLB_OUT_TRG;
    trgm_output_config(ptr, BOARD_PLB_IO_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1 << BOARD_PLB_IO_TRG_SHIFT);
}

void init_pwm(void)
{
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_stop_counter(BOARD_PLB_PWM_BASE);
    pwm_get_default_pwm_config(BOARD_PLB_PWM_BASE, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * reload and start counter
     */
    pwm_set_reload(BOARD_PLB_PWM_BASE, 0, reload);
    pwm_set_start_count(BOARD_PLB_PWM_BASE, 0, 0);

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_PLB_PWM_CMP;
    pwm_output_ch_cfg.cmp_end_index   = BOARD_PLB_PWM_CMP;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(BOARD_PLB_PWM_BASE, BOARD_PLB_PWM_CHN, &pwm_output_ch_cfg);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload >> 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_load_cmp_shadow_on_match(BOARD_PLB_PWM_BASE, BOARD_PLB_PWM_CMP, &cmp_config[0]);

    pwm_start_counter(BOARD_PLB_PWM_BASE);
    pwm_issue_shadow_register_lock_event(BOARD_PLB_PWM_BASE);

}

int main(void)
{
    uint32_t freq;

    board_init();
    init_plb_pins();
    freq = clock_get_frequency(BOARD_PLB_PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;
    init_pwm_plb_trgm(BOARD_PLB_TRGM);
    init_pwm();
    plb_set_counter(10);
    plb_32bit_counter_init();
    printf("PLB DEMO.\r\n");
    while (1) {
    };
    return 0;
}
