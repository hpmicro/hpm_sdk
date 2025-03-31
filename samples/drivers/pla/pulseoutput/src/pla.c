/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_pla_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"

#define PWM_PERIOD_IN_MS (1)

uint32_t reload;

void pla_7bit_counter_init(void)
{
    pla_filter_cfg_t filter_level1_chn_cfg;
    pla_filter_cfg_t filter_level2_chn_cfg;
    pla_filter_cfg_t filter_level3_chn_cfg;
    pla_aoi_16to8_chn_cfg_t aoi_16to8_chn_cfg;
    pla_aoi_8to7_chn_cfg_t aoi_8to7_chn_cfg;
    pla_ff_cfg_t pla_ff_cfg;

    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = true;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter1_in(BOARD_PLA_COUNTER, BOARD_PLA_PWM_IN_CHN, &filter_level1_chn_cfg);
    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_height;
    for (uint16_t i = 1; i <= BOARD_PLA_LEVEL1_FILTER_IN_END; i++) {
        pla_set_filter1_in(BOARD_PLA_COUNTER, i, &filter_level1_chn_cfg);
    }
    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = true;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = 0; i <= BOARD_PLA_LEVEL1_FILTER_OUT_END; i++) {
        pla_set_filter1_out(BOARD_PLA_COUNTER, i, &filter_level1_chn_cfg);
    }
    /**
     * @brief PLA CHN0 D-flip-flop 0 Aout`Bout`Cout`...Gout`
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_0;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;

    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }

    for (uint16_t i = pla_level1_filter_out_9; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_xor_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 1 Ain
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_0;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_1;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_0].op = pla_aoi_operation_xor_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_0].signal = pla_level1_filter_out_0;
    pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 2 Aout
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_0;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_2;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_8].op = pla_aoi_operation_xor_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_8].signal = pla_level1_filter_out_8;
    pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 3-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_3; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_0;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(BOARD_PLA_COUNTER, pla_chn_0, i, &filter_level2_chn_cfg);
    }

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief D
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_0;
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_0;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].signal = pla_level2_filter_out_1;
    pla_set_aoi_8to7_one_channel(BOARD_PLA_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i < pla_filter3_chn1; i++) {
        pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, pla_filter3_chn2, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, pla_filter3_chn3, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, pla_filter3_chn4, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, pla_filter3_chn5, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(BOARD_PLA_COUNTER, pla_chn_0, pla_filter3_chn6, &filter_level3_chn_cfg);
    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_level3_filter0;
    pla_ff_cfg.sel_clk_source = 0;
    pla_set_ff(BOARD_PLA_COUNTER, pla_chn_0, &pla_ff_cfg);

    /**
     * @brief PLA CHN1 D-flip-flop 1
     *
     */
    uint8_t pla_level1_filter_out_num = pla_level1_filter_out_8;
    for (uint8_t m = pla_chn_1; m <= pla_chn_7; m++) {
        /* Aout chn0 */
        aoi_16to8_chn_cfg.chn = m;
        aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
        for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        aoi_16to8_chn_cfg.input[pla_level1_filter_out_num].op = pla_aoi_operation_and_1;
        aoi_16to8_chn_cfg.input[pla_level1_filter_out_num].signal = pla_level1_filter_out_num;
        pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);
        /**
         * @brief Bout' chn1
         *
         */
        aoi_16to8_chn_cfg.chn = m;
        aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_1;
        for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_level1_filter_out_num++;
        aoi_16to8_chn_cfg.input[pla_level1_filter_out_num].op = pla_aoi_operation_xor_1;
        aoi_16to8_chn_cfg.input[pla_level1_filter_out_num].signal = pla_level1_filter_out_num;
        pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);

        for (uint16_t j = pla_aoi_16to8_chn_2; j <= pla_aoi_16to8_chn_7; j++) {
            aoi_16to8_chn_cfg.chn = m;
            aoi_16to8_chn_cfg.aoi_16to8_chn = j;
            for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
                aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
                aoi_16to8_chn_cfg.input[i].signal = i;
            }
            pla_set_aoi_16to8_one_channel(BOARD_PLA_COUNTER, &aoi_16to8_chn_cfg);
        }
        /**
         * @brief pla filter2 value(chn 1 - 7)
         *
         */
        filter_level2_chn_cfg.val = 0;
        filter_level2_chn_cfg.sync_edge_filter_disable = true;
        filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
        for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
            pla_set_filter2(BOARD_PLA_COUNTER, m, i, &filter_level2_chn_cfg);
        }
        /**
         * @brief D
         *
         */
        aoi_8to7_chn_cfg.chn = m;
        aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_0;
        for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
            aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
            aoi_8to7_chn_cfg.input[i].signal = i;
        }
        aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].op = pla_aoi_operation_and_1;
        aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].signal = pla_level2_filter_out_1;
        pla_set_aoi_8to7_one_channel(BOARD_PLA_COUNTER, &aoi_8to7_chn_cfg);
        /**
         * @brief CLK
         *
         */
        aoi_8to7_chn_cfg.chn = m;
        aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_1;
        for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
            aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
            aoi_8to7_chn_cfg.input[i].signal = i;
        }
        aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
        aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
        pla_set_aoi_8to7_one_channel(BOARD_PLA_COUNTER, &aoi_8to7_chn_cfg);

        /**
         * @brief filter level3
         *
         */
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.sync_edge_filter_disable = true;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
        for (uint16_t i = pla_filter3_chn0; i < pla_filter3_chn2; i++) {
            pla_set_filter3(BOARD_PLA_COUNTER, m, i, &filter_level3_chn_cfg);
        }
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn2, &filter_level3_chn_cfg);
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn3, &filter_level3_chn_cfg);
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn4, &filter_level3_chn_cfg);
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn5, &filter_level3_chn_cfg);
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn6, &filter_level3_chn_cfg);
        pla_ff_cfg.val = 0;
        pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_dff;
        pla_ff_cfg.sel_clk_source = 1;
        pla_set_ff(BOARD_PLA_COUNTER, m, &pla_ff_cfg);
    }

    /**
     * @brief Construct a new pla channel enable object
     *
     */
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_0);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_1);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_2);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_3);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_4);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_5);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_6);
    pla_channel_enable(BOARD_PLA_COUNTER, pla_chn_7);

    /**
     * @brief set Flip-Flop
     *
     */
    for (uint8_t m = pla_chn_1; m <= pla_chn_7; m++) {
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn3, &filter_level3_chn_cfg);
    }
    board_delay_us(10);
    for (uint8_t m = pla_chn_1; m <= pla_chn_7; m++) {
        filter_level3_chn_cfg.val = 0;
        filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
        pla_set_filter3(BOARD_PLA_COUNTER, m, pla_filter3_chn3, &filter_level3_chn_cfg);
    }
}

void init_pwm_pla_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_PLA_PWM_TRG;
    trgm_output_config(ptr, BOARD_PLA_IN_TRG_NUM, &trgm_output_cfg);

    /* trig0 p5 */
    trgm_output_cfg.invert = true;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_PLA_OUT_TRG;
    trgm_output_config(ptr, BOARD_PLA_IO_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1 << BOARD_PLA_IO_TRG_NUM);
}

void init_pwm(void)
{
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_stop_counter(BOARD_PLA_PWM_BASE);
    pwm_get_default_pwm_config(BOARD_PLA_PWM_BASE, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * reload and start counter
     */
    pwm_set_reload(BOARD_PLA_PWM_BASE, 0, reload);
    pwm_set_start_count(BOARD_PLA_PWM_BASE, 0, 0);

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_PLA_PWM_CMP;  /* start channel 8 */
    pwm_output_ch_cfg.cmp_end_index   = BOARD_PLA_PWM_CMP;  /* end channel 8 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(BOARD_PLA_PWM_BASE, BOARD_PLA_PWM_CHN, &pwm_output_ch_cfg);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_load_cmp_shadow_on_match(BOARD_PLA_PWM_BASE, BOARD_PLA_PWM_CMP, &cmp_config[0]);

    pwm_start_counter(BOARD_PLA_PWM_BASE);
    pwm_issue_shadow_register_lock_event(BOARD_PLA_PWM_BASE);

}

int main(void)
{
    uint32_t freq;

    board_init();
    init_pla_pins();
    freq = clock_get_frequency(BOARD_PLA_PWM_CLOCK_NAME);
    reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;
    pla_7bit_counter_init();
    init_pwm_pla_trgm(BOARD_PLA_TRGM);
    init_pwm();
    pwm_cmp_update_cmp_value(BOARD_PLA_PWM_BASE, BOARD_PLA_PWM_CMP, reload >> 1, 0);
    printf("PLA DEMO.\r\n");
    while (1) {
    };
    return 0;
}
