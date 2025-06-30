/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "assert.h"
#include "board.h"
#include "hpm_mtgv2_regs.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_sei_drv.h"
#include "hpm_mtgv2_drv.h"

#ifndef APP_QEIV2_BASE
#define APP_QEIV2_BASE BOARD_BLDC_QEIV2_BASE
#endif

#ifndef APP_QEIV2_IRQ
#define APP_QEIV2_IRQ BOARD_BLDC_QEIV2_IRQ
#endif

#ifndef APP_MOTOR_CLK
#define APP_MOTOR_CLK BOARD_BLDC_QEI_CLOCK_SOURCE
#endif

#ifndef APP_MTGV2_BASE
#define APP_MTGV2_BASE  HPM_MTG0
#endif

#ifndef APP_MTGV2_TRA_INDEX
#define APP_MTGV2_TRA_INDEX  MTGV2_TRA_0
#endif

#include "hpm_gptmr_drv.h"
#include "hpm_debug_console.h"

uint32_t sei_mt, sei_st;
volatile uint32_t sei_pos_update_count;

void setup_sei(void)
{
    sei_transceiver_config_t transceiver_config = {0};
    sei_data_format_config_t data_format_config = {0};
    sei_engine_config_t engine_config = {0};
    sei_trigger_input_config_t trigger_input_conifg = {0};
    sei_state_transition_config_t state_transition_config = {0};
    sei_state_transition_latch_config_t state_transition_latch_config = {0};
    sei_sample_config_t sample_config = {0};
    sei_update_config_t update_config = {0};
    uint8_t instr_idx;

    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);

    /* [1] transceiver config */
    transceiver_config.mode = sei_asynchronous_mode;
    transceiver_config.tri_sample = false;
    transceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    transceiver_config.asynchronous_config.wait_len = 0;
    transceiver_config.asynchronous_config.data_len = 8;
    transceiver_config.asynchronous_config.parity_enable = false;
    transceiver_config.asynchronous_config.data_idle_high_z = false;
    transceiver_config.asynchronous_config.data_idle_state = sei_idle_high_state;
    transceiver_config.asynchronous_config.baudrate = 2500000;
    sei_transceiver_config_init(BOARD_SEI, BOARD_SEI_CTRL, &transceiver_config);

    /* [2] data register config */
    /* data register 2: send CF */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    sei_set_data_value(BOARD_SEI, SEI_DAT_2, 0x1A);    /* Data ID 3 */
    /* data register 3: recv CF */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: recv SF  */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: recv ABS0 ABS1 ABS2 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 23;
    data_format_config.last_bit = 31;
    data_format_config.first_bit = 9;
    data_format_config.max_bit = 31;
    data_format_config.min_bit = 9;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);
    /* data register 6: recv ENID */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_6, &data_format_config);
    /* data register 7: recv ABM0 ABM1 ABM2 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 24;
    data_format_config.last_bit = 23;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 23;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_7, &data_format_config);
    /* data register 8: recv ALMC */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_8, &data_format_config);
    /* data register 9: check crc */
    data_format_config.mode = sei_crc_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.crc_invert = false;
    data_format_config.crc_shift_mode = false;
    data_format_config.crc_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    data_format_config.crc_init_value = 0;
    data_format_config.crc_poly = 1;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_9, &data_format_config);

    /* [3] instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_2, 8);  /* Send CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_3, 8);  /* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_5, 23); /* 23bits of ST(POS) */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_0, 1); /* Drop the 24th bit of ST */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_6, 8);  /* ENID:0x17 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_7, 24); /* MT */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_8, 8);  /* ALMC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 0);

    /* [4] sample config*/
    sample_config.latch_select = SEI_LATCH_0;
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);

    /* [5] update config*/
    update_config.pos_data_idx = SEI_DAT_5;
    update_config.rev_data_idx = SEI_DAT_7;
    update_config.pos_data_use_rx = true;
    update_config.rev_data_use_rx = true;
    update_config.update_on_err = false;
    update_config.latch_select = SEI_LATCH_1;
    update_config.data_register_select = BIT5_MASK | BIT7_MASK;    /* SEI_DAT_5, SEI_DAT_7 */
    sei_update_config_init(BOARD_SEI, BOARD_SEI_CTRL, &update_config);

    /* [6] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 0;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_1_2, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_2_3, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = (48 * (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000)) / 100;    /*  unit: 1us, 0.48us */
    sei_state_transition_latch_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, &state_transition_latch_config);

    /* latch1 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = (instr_idx - 2);
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_1_2, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_2_3, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = 0;
    sei_state_transition_latch_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, &state_transition_latch_config);

    /* [7] engine config */
    printf("Started sei engine!\n");
    engine_config.arming_mode = sei_arming_wait_trigger;
    engine_config.data_cdm_idx = 0;
    engine_config.data_base_idx = 0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = true;
    engine_config.wdg_action = sei_wdg_exec_exception_instr;
    engine_config.wdg_instr_idx = (instr_idx - 1);
    engine_config.wdg_time = 1000;    /* 1000 bits time */
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [8] interrupt config */
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event, true);
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);

    /* [10] trigger config */
    trigger_input_conifg.trig_period_enable = true;
    trigger_input_conifg.trig_period_arming_mode = sei_arming_direct_exec;
    trigger_input_conifg.trig_period_sync_enable = false;
    trigger_input_conifg.trig_period_time = (1 * (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000));    /* unit: 1ms, 200ms */
    sei_trigger_input_config_init(BOARD_SEI, BOARD_SEI_CTRL, &trigger_input_conifg);
}

SDK_DECLARE_EXT_ISR_M(BOARD_SEI_IRQn, isr_sei)
void isr_sei(void)
{
    uint32_t sample_latch_tm;
    uint32_t update_latch_tm;

    if (sei_get_irq_status(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event)) {
        sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event);
        sei_pos_update_count++;
        sample_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        update_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1);
        (void)sample_latch_tm;
        (void)update_latch_tm;
        sei_mt = sei_get_data_value(BOARD_SEI, SEI_DAT_7);
        sei_st = sei_get_data_value(BOARD_SEI, SEI_DAT_5);
    }

    if (sei_get_irq_status(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_trx_err_event)) {
        sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_trx_err_event);
        printf("TRX Error!\n");
    }
}

void mtg_init(MTGV2_Type *base)
{

    mtg_filter_param_t filter_config = { 0 };
    mtg_tra_shift_t tra_shift_config = { 0 };

    /* Global Reset Mtg First */
    mtg_set_global_reset(base);
    board_delay_ms(100);
    mtg_stop_global_reset(base);

    /* Setup Filter Configuration */
    mtg_filter_get_default_filter_stage_param(&filter_config);
    filter_config.enable = true;
    filter_config.timeout_en = false;
    filter_config.err_bypass_en = true;
    filter_config.err_init = true;

    mtg_setup_filter(base, &filter_config);

    /* Setup Time Model */
    mtg_time_init_param_t time_cfg = { 0 };
    time_cfg.enable = true;
    time_cfg.source = mtg_time_input_from_filter;
    time_cfg.adjust_value = 0;
    time_cfg.index = APP_MTGV2_TRA_INDEX;
    mtg_setup_time(base, &time_cfg);

    /* Setup Trajectory Configutration */
    /* Setup Trajectory Shift, This is Binding To Filter Configuration */
    mtg_get_default_tra_shift(&tra_shift_config);
    mtg_tra_set_shift(base, APP_MTGV2_TRA_INDEX, tra_shift_config.jerk_shift, tra_shift_config.acc_shift, tra_shift_config.vel_shift);
    /* Setup Trajectory Limit and One Way Mode Configuration */
    mtg_tra_limit_param_t tra_limit_cfg = { 0 };
    tra_limit_cfg.pos_step_limit_en = false;
    tra_limit_cfg.vel_step_limit_en = false;

    mtg_setup_tra_limit(base, APP_MTGV2_TRA_INDEX, &tra_limit_cfg);
    mtg_disable_tra_pos_one_way_mode(base, APP_MTGV2_TRA_INDEX);

}

static void mtg_test_loop(void)
{
    mtg_lock_value_t result = {0};
    uint32_t mot_clock = clock_get_frequency(APP_MOTOR_CLK);
    printf("SEI ISR CNT, TIME, SEI_POS, MTGV2_POS, MTGV2_VEL, MTGV2_POS-QEI_POS\r\n");
    static uint32_t last_isr_count;
    while (1) {
        if (last_isr_count != sei_pos_update_count) {
            last_isr_count = sei_pos_update_count;

            mtg_get_tra_lock_result(APP_MTGV2_BASE, MTGV2_TRA_0, &result);

            float delta = 1.0f * (float)result.pos / (float)0xFFFFFFFF - (float)sei_st * 1.0f / (float)0xFFFFFFFF;

            printf("%d,%f,%f,%f,%d,%f\n", sei_pos_update_count, \
                                          result.time_stamp * 1.0f / mot_clock, \
                                          (float)sei_st * 1.0f / (float)0xFFFFFFFF, \
                                          (float)result.pos * 1.0f / (float)0xFFFFFFFF, \
                                          result.vel, \
                                          delta);
        }
    }
}

int main(void)
{
    board_init();

    board_init_sei_pins(BOARD_SEI, BOARD_SEI_CTRL);

    synt_enable_timestamp(HPM_SYNT, true);

    mtg_init(APP_MTGV2_BASE);

    setup_sei();

    trgm_pos_matrix_config(HPM_TRGM0, BOARD_TRGM_POS_DEST_MTG, BOARD_TRGM_POS_SOURCE_SEI, false);

    printf("mtg sei tamagawa example\n");

    mtg_test_loop();

    return 0;
}
