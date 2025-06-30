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

#define CDF(n) (n)

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

    printf("SEI master nikon sample\n");

    /* [0] disable engine and enable timestamp */
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
    synt_enable_timestamp(HPM_SYNT, true);

    /* [1] transceiver config */
    transceiver_config.mode = sei_asynchronous_mode;
    transceiver_config.tri_sample = false;
    transceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    transceiver_config.asynchronous_config.wait_len = 0;
    transceiver_config.asynchronous_config.data_len = 16;
    transceiver_config.asynchronous_config.parity_enable = false;
    transceiver_config.asynchronous_config.data_idle_high_z = false;
    transceiver_config.asynchronous_config.data_idle_state = sei_idle_high_state;
    transceiver_config.asynchronous_config.baudrate = 2500000;
    sei_transceiver_config_init(BOARD_SEI, BOARD_SEI_CTRL, &transceiver_config);

    /* [2] data register config */
    /* data register 2: send Sink Code */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 3;
    data_format_config.last_bit = 2;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 2;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    sei_set_data_value(BOARD_SEI, SEI_DAT_2, 0x0002);
    /* CMD register: send CDF0 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 10;
    data_format_config.last_bit = 9;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 9;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_CMD, BOARD_SEI_CTRL, &data_format_config);
    /* data register 3: CDF check crc */
    data_format_config.mode = sei_crc_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 3;
    data_format_config.crc_invert = false;
    data_format_config.crc_shift_mode = false;
    data_format_config.crc_len = 3;
    data_format_config.last_bit = 2;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 2;
    data_format_config.min_bit = 0;
    data_format_config.crc_init_value = 0;
    data_format_config.crc_poly = (BIT0_MASK | BIT1_MASK);
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: recv IF.EAX */
    data_format_config.mode = sei_check_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 6;
    data_format_config.last_bit = 5;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 5;
    data_format_config.min_bit = 0;
    data_format_config.gold_value = 0x04;    /* checked value */
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: recv IF.CC */
    data_format_config.mode = sei_check_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 6;
    data_format_config.last_bit = 5;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 5;
    data_format_config.min_bit = 0;
    data_format_config.gold_value = CDF(0);    /* checked value */
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);
    /* data register 6: recv IF.ES */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 4;
    data_format_config.last_bit = 3;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 3;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_6, &data_format_config);
    /* data register 7: recv ST */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 20;
    data_format_config.last_bit = 19;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 19;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_7, &data_format_config);
    /* data register 8: recv MT */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 20;
    data_format_config.last_bit = 19;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 19;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_8, &data_format_config);
    /* data register 9: DF check crc */
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
    data_format_config.crc_poly = (BIT0_MASK | BIT2_MASK | BIT3_MASK | BIT4_MASK);
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_9, &data_format_config);

    /* [3] instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_2, 3);      /* Send Sink Code */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_3, SEI_DAT_CMD, 10);   /* Send CDF0 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_3, 3);      /* Send CRC */

    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_4, 6);  /* Recv IF.EAX*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_5, 6);  /* Recv IF.CC*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_6, 4);  /* Recv IF.ES*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_7, 20); /* Recv ST */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_8, 20); /* Recv MT */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* Recv CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 0);

    /* [4] sample config*/
    sample_config.latch_select = SEI_LATCH_0;
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);

    /* [5] update config*/
    update_config.pos_data_idx = SEI_DAT_7;
    update_config.rev_data_idx = SEI_DAT_8;
    update_config.pos_data_use_rx = true;
    update_config.rev_data_use_rx = true;
    update_config.update_on_err = false;
    update_config.latch_select = SEI_LATCH_1;
    update_config.data_register_select = BIT7_MASK | BIT8_MASK;    /* SEI_DAT_7, SEI_DAT_8 */
    sei_update_config_init(BOARD_SEI, BOARD_SEI_CTRL, &update_config);

    /* [6] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 1;
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
    state_transition_latch_config.delay = (2 * (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000));    /*  unit: 1us, 2us */
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

    /* [9] trigger config */
    /* When a trigger is generated, the hardware will automatically fill the command register from trigger table command register */
    sei_set_trig_input_command_value(BOARD_SEI, BOARD_SEI_CTRL, sei_trig_in_period, CDF(0) << 5);

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
        printf("ST:%#x, MT:%#x, CRC:%#x, sample_tm:%u, update_tm:%u, TimeDelay:%d*0.1us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_7),
                sei_get_data_value(BOARD_SEI, SEI_DAT_8),
                sei_get_data_value(BOARD_SEI, SEI_DAT_9) & 0xFF,
                sample_latch_tm, update_latch_tm, delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 10000000));
    }

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        printf("TRX Error!\n");
    }
}
