/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_sei_drv.h"


int main(void)
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

    board_init();
    board_init_sei_pins(BOARD_SEI, BOARD_SEI_CTRL);

    printf("SEI master hiperface sample\n");

    /* [0] disable engine and enable timestamp */
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
    synt_enable_timestamp(HPM_SYNT, true);

    /* [1] transceiver config */
    transceiver_config.mode = sei_asynchronous_mode;
    transceiver_config.tri_sample = false;
    transceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    transceiver_config.asynchronous_config.wait_len = 0;
    transceiver_config.asynchronous_config.data_len = 8;
    transceiver_config.asynchronous_config.parity_enable = true;
    transceiver_config.asynchronous_config.parity = sei_asynchronous_parity_even;
    transceiver_config.asynchronous_config.data_idle_high_z = false;
    transceiver_config.asynchronous_config.data_idle_state = sei_idle_high_state;
    transceiver_config.asynchronous_config.baudrate = 9600;
    sei_transceiver_config_init(BOARD_SEI, BOARD_SEI_CTRL, &transceiver_config);

    /* [2] data register config */
    /* data register 2: send addr */
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
    sei_set_data_value(BOARD_SEI, SEI_DAT_2, 0x40);    /* addr */
    /* data register 3: send cmd */
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
    sei_set_data_value(BOARD_SEI, SEI_DAT_3, 0x42);    /* cmd */
    /* data register 4: crc */
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
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: recv addr  */
    data_format_config.mode = sei_check_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    data_format_config.gold_value = 0x40;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);
    /* data register 6: recv cmd */
    data_format_config.mode = sei_check_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    data_format_config.gold_value = 0x42;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_6, &data_format_config);

    /* Depending on the soc, there are different numbers of DAT registers available */
#if defined(HPM_IP_FEATURE_SEI_HAVE_DAT10_31) && HPM_IP_FEATURE_SEI_HAVE_DAT10_31
    /* data register 9: recv pos */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_9, &data_format_config);
    /* data register 11: recv pos */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 3;
    data_format_config.first_bit = 4;
    data_format_config.max_bit = 11;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_11, &data_format_config);
    /* data register 10: recv pos */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 27;
    data_format_config.first_bit = 28;
    data_format_config.max_bit = 31;
    data_format_config.min_bit = 20;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_10, &data_format_config);
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_12, &data_format_config);
#else
    /* data register 7: recv pos */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 24;
    data_format_config.max_bit = 31;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_7, &data_format_config);
#endif
    /* data register 8: check crc */
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
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_8, &data_format_config);

    /* [3] instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_4, SEI_DAT_2, 8);  /* Send addr */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_4, SEI_DAT_3, 8);  /* Send cmd */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_4, 8);  /* Send crc */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 5);  /* halt some cycles */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_5, 1);  /* recv addr */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_5, 7);  /* recv addr */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_6, 8);  /* recv cmd */
    /* Depending on the soc, there are different numbers of DAT registers available */
#if defined(HPM_IP_FEATURE_SEI_HAVE_DAT10_31) && HPM_IP_FEATURE_SEI_HAVE_DAT10_31
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_9, 8); /* recv null */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_11, 8); /* recv rev */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_10, 4); /* recv pos */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_11, 4); /* recv rev */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_10, 8); /* recv pos */
#else
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_8, SEI_DAT_7, 32); /* recv pos */
#endif
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_8, 8);  /* check crc */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 0);

    /* [4] sample config*/
    sample_config.latch_select = SEI_LATCH_0;
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);

    /* [5] update config*/
    update_config.pos_data_idx = SEI_DAT_7;
    update_config.pos_data_use_rx = true;
    update_config.update_on_err = false;
    update_config.latch_select = SEI_LATCH_1;
    update_config.data_register_select = BIT7_MASK;    /* SEI_DAT_7 */
    sei_update_config_init(BOARD_SEI, BOARD_SEI_CTRL, &update_config);

    /* [6] state transition config */
    /* latch0 */
#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = false;
    state_transition_config.rxd_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_high_match;
    state_transition_config.instr_ptr_value = 4;
#else
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 4;
#endif
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
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = (instr_idx - 2);
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = 0;
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
    engine_config.wdg_time = 500;    /* 500 bits time */
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [8] interrupt config */
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event, true);
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);

    /* [9] trigger config */
    trigger_input_conifg.trig_period_enable = true;
    trigger_input_conifg.trig_period_arming_mode = sei_arming_direct_exec;
    trigger_input_conifg.trig_period_sync_enable = false;
    trigger_input_conifg.trig_period_time = (200 * (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000));    /* unit: 1ms, 200ms */
    sei_trigger_input_config_init(BOARD_SEI, BOARD_SEI_CTRL, &trigger_input_conifg);

    while (1) {
        ;
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_SEI_IRQn, isr_sei)
void isr_sei(void)
{
    uint32_t irq_flag;
    uint32_t sample_latch_tm;
    uint32_t update_latch_tm;
    uint32_t delta;

    irq_flag = sei_get_irq_flag(BOARD_SEI, BOARD_SEI_CTRL);
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, irq_flag);
    irq_flag &= sei_get_irq_enable_status(BOARD_SEI, BOARD_SEI_CTRL);

    if ((irq_flag & sei_irq_latch1_event) != 0) {
        sample_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        update_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1);
        delta = (update_latch_tm > sample_latch_tm) ? (update_latch_tm - sample_latch_tm) : (update_latch_tm - sample_latch_tm + 0xFFFFFFFFu);
#if defined(HPM_IP_FEATURE_SEI_HAVE_DAT10_31) && HPM_IP_FEATURE_SEI_HAVE_DAT10_31
        printf("rev:%#x, pos:%#x, addr:%#x, CRC:%#x, sample_tm:%u, update_tm:%u, TimeDelay:%d us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_11),
                sei_get_data_value(BOARD_SEI, SEI_DAT_10),
                sei_get_data_value(BOARD_SEI, SEI_DAT_5),
#else
        printf("rev:%#x, pos:%#x, addr:%#x, CRC:%#x, sample_tm:%u, update_tm:%u, TimeDelay:%d us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_7) >> 12,
                (sei_get_data_value(BOARD_SEI, SEI_DAT_7) & 0xFFF) << 20,
                sei_get_data_value(BOARD_SEI, SEI_DAT_5),
#endif
                sei_get_data_value(BOARD_SEI, SEI_DAT_8) & 0xFF,
                sample_latch_tm,
                update_latch_tm,
                delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000));
    }

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        printf("TRX Error!\n");
    }
}
