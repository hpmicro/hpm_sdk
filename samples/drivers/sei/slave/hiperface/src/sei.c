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


static uint32_t mock_pos = 0xA5A5;
static uint32_t sample_latch_tm1;
static uint32_t sample_latch_tm2;


int main(void)
{
    sei_transceiver_config_t transceiver_config = {0};
    sei_data_format_config_t data_format_config = {0};
    sei_engine_config_t engine_config = {0};
    sei_state_transition_config_t state_transition_config = {0};
    sei_state_transition_latch_config_t state_transition_latch_config = {0};
    sei_sample_config_t sample_config = {0};
    sei_command_table_config_t command_table_config = {0};
    uint8_t instr_idx;

    board_init();
    board_init_sei_pins(BOARD_SEI, BOARD_SEI_CTRL);

    printf("SEI slave hiperface sample\n");

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
    /* data register 2: recv addr */
    data_format_config.mode = sei_check_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    data_format_config.gold_value = 0x40;    /* Check value */
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    /* cmd register: recv cmd */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_CMD, BOARD_SEI_CTRL, &data_format_config);
    /* data register 3: recv crc */
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
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: send pos */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_reverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 24;
    data_format_config.max_bit = 31;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: calc crc */
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
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);

    /* [3] instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_3, SEI_DAT_2, 8);  /* recv addr */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_3, SEI_DAT_CMD, 8);/* recv cmd */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_3, 8);  /* recv crc */
    /* jump to cmd */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_CMD_TABLE_INSTR_IDX0);  /* jump to cmd table instr_idx[0] */
    /* 0x42 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 31);     /* halt some cycles for timeout + response */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 31);     /* halt some cycles for timeout + response */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 31);     /* halt some cycles for timeout + response */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 3);      /* halt some cycles for timeout + response */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_5, SEI_DAT_2, 8);      /* send addr */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_5, SEI_DAT_CMD, 8);    /* send cmd */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_5, SEI_DAT_4, 32);     /* send pos */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_5, 8);      /* send crc */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);

    /* [4] command table */
    command_table_config.cmd_min_value = 0x42u;
    command_table_config.cmd_max_value = 0x42u;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 4;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_0, &command_table_config);

    /* [5] sample config*/
    sample_config.pos_data_idx = SEI_DAT_4;
    sample_config.pos_data_use_rx = false;
    sample_config.sample_window = 0x5;
    sample_config.sample_once = true;
    sample_config.latch_select = SEI_LATCH_0;
    sample_config.data_register_select = BIT4_MASK;    /* SEI_DAT_4 */
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);
    sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);

    /* [6] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_rise_entry;
    state_transition_config.instr_ptr_value = 8;
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
    engine_config.arming_mode = sei_arming_direct_exec;
    engine_config.data_cdm_idx = 0;
    engine_config.data_base_idx = 0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = true;
    engine_config.wdg_action = sei_wdg_exec_exception_instr;
    engine_config.wdg_instr_idx = 0;
    engine_config.wdg_time = 500;    /* 500 bits time */
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [8] interrupt config */
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_wdog_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_wdog_event | sei_irq_trx_err_event, true);
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);

    while (1) {
        ;
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_SEI_IRQn, isr_sei)
void isr_sei(void)
{
    uint32_t irq_flag;
    uint32_t delta;

    irq_flag = sei_get_irq_flag(BOARD_SEI, BOARD_SEI_CTRL);
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, irq_flag);
    irq_flag &= sei_get_irq_enable_status(BOARD_SEI, BOARD_SEI_CTRL);

    if ((irq_flag & sei_irq_latch0_event) != 0) {
        mock_pos++;
        sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);
    }

    if ((irq_flag & sei_irq_latch1_event) != 0) {
        sample_latch_tm1 = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        delta = (sample_latch_tm1 > sample_latch_tm2) ? (sample_latch_tm1 - sample_latch_tm2) : (sample_latch_tm1 - sample_latch_tm2 + 0xFFFFFFFFu);
        printf("ADDR:%#x, CMD:%#x, POS:%#x, CRC:%#x, sample_tm1:%u, sample_tm2:%u, sample_interval:%d us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_2),
                sei_get_command_value(BOARD_SEI, BOARD_SEI_CTRL),
                sei_get_data_value(BOARD_SEI, SEI_DAT_4),
                sei_get_data_value(BOARD_SEI, SEI_DAT_5) & 0xFF,
                sample_latch_tm1, sample_latch_tm2, delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000));
        sample_latch_tm2 = sample_latch_tm1;
    }

    if ((irq_flag & sei_irq_wdog_event) != 0) {
        sei_set_command_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        sei_set_data_rewind(BOARD_SEI, SEI_DAT_2);
        sei_set_data_rewind(BOARD_SEI, SEI_DAT_3);
    }

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        printf("TRX Error!\n");
        sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
        sei_set_engine_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        sei_set_command_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        sei_set_data_rewind(BOARD_SEI, SEI_DAT_2);
        sei_set_data_rewind(BOARD_SEI, SEI_DAT_3);
        sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);
    }
}
