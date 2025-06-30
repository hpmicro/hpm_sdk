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

static uint16_t mock_pos = 0x5A5;
static uint16_t mock_rev = 0xA5A;
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
    uint8_t instr_idx;

    board_init();
    board_init_sei_pins(BOARD_SEI, BOARD_SEI_CTRL);

    printf("SEI slave BiSS_C sample\n");

    /* [0] disable engine and enable timestamp */
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
    synt_enable_timestamp(HPM_SYNT, true);

    /* [1] transceiver config */
    transceiver_config.mode = sei_synchronous_slave_mode;
    transceiver_config.tri_sample = false;
    transceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    transceiver_config.synchronous_slave_config.data_idle_high_z = false;
    transceiver_config.synchronous_slave_config.data_idle_state = sei_idle_high_state;
    transceiver_config.synchronous_slave_config.clock_idle_high_z = false;
    transceiver_config.synchronous_slave_config.clock_idle_state = sei_idle_high_state;
    transceiver_config.synchronous_slave_config.max_baudrate = 10000000;    /* 10 MHz */
    transceiver_config.synchronous_slave_config.ck0_timeout_us = 20;
    transceiver_config.synchronous_slave_config.ck1_timeout_us = 20;        /* min_baudrate: 50kHz */
    sei_transceiver_config_init(BOARD_SEI, BOARD_SEI_CTRL, &transceiver_config);
    sei_set_xcvr_rx_point(BOARD_SEI, BOARD_SEI_CTRL, SEI_SYNC_SLAVE_RX_TX_POINT_ON_FALL_EDGE);
    sei_set_xcvr_tx_point(BOARD_SEI, BOARD_SEI_CTRL, SEI_SYNC_SLAVE_RX_TX_POINT_ON_RISE_EDGE);

    /* [2] data register config */
    /* data register 2: Multi Turn Value */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 12;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 11;
    data_format_config.max_bit = 11;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    /* data register 3: Single Turn Value */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 12;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 11;
    data_format_config.max_bit = 11;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: error and warn Value */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 2;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 1;
    data_format_config.max_bit = 1;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: crc */
    data_format_config.mode = sei_crc_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 6;
    data_format_config.crc_invert = true;
    data_format_config.crc_shift_mode = false;
    data_format_config.crc_len = 6;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 5;
    data_format_config.max_bit = 5;
    data_format_config.min_bit = 0;
    data_format_config.crc_init_value = 0;
    data_format_config.crc_poly = 3;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);

    /* [3] sei instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DATA_CONST_1, 1);
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DATA_CONST_0, 1);  /* ACK:0 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DATA_CONST_1, 1);  /* START:1 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DATA_CONST_0, 1);  /* CDS */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_5, SEI_DAT_2, 12);  /* MT */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_5, SEI_DAT_3, 12);  /* ST */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_5, SEI_DAT_4, 2);   /* Error Warn*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DAT_5, 6);   /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_S_CK_TIMEOUT_EN, SEI_DAT_0, SEI_DATA_CONST_0, 10); /* timeout */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, SEI_INSTR_S_CK_DEFAULT, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);

    /* [4] sample config*/
    sample_config.pos_data_idx = SEI_DAT_3;
    sample_config.rev_data_idx = SEI_DAT_2;
    sample_config.pos_data_use_rx = false;
    sample_config.rev_data_use_rx = false;
    sample_config.sample_window = 0x2;
    sample_config.sample_once = true;
    sample_config.latch_select = SEI_LATCH_0;
    sample_config.data_register_select = BIT2_MASK | BIT3_MASK;    /* SEI_DAT_2, SEI_DAT_3 */
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);
    sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);
    sei_set_sample_rev_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_rev);

    /* [5] state transition config */
    /* latch0 */
#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    state_transition_config.disable_clk_check = false;
    state_transition_config.clk_cfg = sei_state_tran_condition_rise_entry;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_high_match;
    state_transition_config.instr_ptr_value = 0;
#else
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 0;
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
    state_transition_config.disable_timeout_check = false;
    state_transition_config.timeout_cfg = sei_state_tran_condition_rise_entry;
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
    state_transition_config.instr_ptr_value = (instr_idx - 3);
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


    /* [6] engine config */
    printf("Started sei engine!\n");
    engine_config.arming_mode = sei_arming_direct_exec;
    engine_config.data_cdm_idx = SEI_DAT_0;
    engine_config.data_base_idx = SEI_DAT_0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = false;
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [7] interrupt config */
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_trx_err_event, true);
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
        if (mock_pos > 0xFFF) {
            mock_pos = 0;
            mock_rev++;
        }
        sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);
        sei_set_sample_rev_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_rev);
        sei_set_data_value(BOARD_SEI, SEI_DAT_4, 0x03);
    }

    if ((irq_flag & sei_irq_latch1_event) != 0) {
        sample_latch_tm1 = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        delta = (sample_latch_tm1 > sample_latch_tm2) ? (sample_latch_tm1 - sample_latch_tm2) : (sample_latch_tm1 - sample_latch_tm2 + 0xFFFFFFFFu);
        printf("MT:%#x, ST:%#x, EW:%#x, CRC:%#x, sample_tm1:%u, sample_tm2:%u, sample_interval:%d us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_2),
                sei_get_data_value(BOARD_SEI, SEI_DAT_3),
                sei_get_data_value(BOARD_SEI, SEI_DAT_4),
                sei_get_data_value(BOARD_SEI, SEI_DAT_5) & 0x3F,
                sample_latch_tm1, sample_latch_tm2, delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000));
        sample_latch_tm2 = sample_latch_tm1;
    }

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        printf("TRX Error!\n");
    }
}

