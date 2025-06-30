/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_sei_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_interrupt.h"


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

    printf("SEI master EnDat sample\n");

    /* [0] disable engine and enable timestamp */
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
    synt_enable_timestamp(HPM_SYNT, true);

    /* [1] transceiver config */
    transceiver_config.mode = sei_synchronous_master_mode;
    transceiver_config.tri_sample = false;
    transceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    transceiver_config.synchronous_master_config.data_idle_high_z = false;
    transceiver_config.synchronous_master_config.data_idle_state = sei_idle_low_state;
    transceiver_config.synchronous_master_config.clock_idle_high_z = false;
    transceiver_config.synchronous_master_config.clock_idle_state = sei_idle_high_state;
    transceiver_config.synchronous_master_config.baudrate = 1000000;
    sei_transceiver_config_init(BOARD_SEI, BOARD_SEI_CTRL, &transceiver_config);
    sei_set_xcvr_rx_point(BOARD_SEI, BOARD_SEI_CTRL, sei_get_xcvr_ck0_point(BOARD_SEI, BOARD_SEI_CTRL));
    sei_set_xcvr_tx_point(BOARD_SEI, BOARD_SEI_CTRL, sei_get_xcvr_ck0_point(BOARD_SEI, BOARD_SEI_CTRL));

    /* [2] data register config */
    /* cmd register: mode command */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 6;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 5;
    data_format_config.max_bit = 5;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_CMD, BOARD_SEI_CTRL, &data_format_config);
    /* data register 2: recv Error Bit */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 1;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 0;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    /* data register 3: recv Single Turn Value */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 25;
    data_format_config.last_bit = 24;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 24;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: check crc */
    data_format_config.mode = sei_crc_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_msb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 5;
    data_format_config.crc_invert = true;
    data_format_config.crc_shift_mode = false;
    data_format_config.crc_len = 5;
    data_format_config.last_bit = 0;
    data_format_config.first_bit = 4;
    data_format_config.max_bit = 4;
    data_format_config.min_bit = 0;
    data_format_config.crc_init_value = 0x1F;
    data_format_config.crc_poly = 0x0B;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);

    /* [3] sei instructions */
    instr_idx = 0;
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_0, 1);   /* recv: 1 bit */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_0, 1);   /* recv: 1 bit */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_CMD, 6); /* send: mode cmd */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_0, 1);   /* recv: 1 bit */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_0, 1);   /* recv: 1 bit */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_WAIT, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DATA_CONST_0, 1);  /* wait: 0 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_WAIT, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DATA_CONST_1, 1);  /* wait: 1 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_4, SEI_DAT_2, 1);   /* error: 1 bit*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_4, SEI_DAT_3, 25);  /* recv: 25 bit ST*/
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV, SEI_INSTR_M_CK_FALL_RISE, SEI_DAT_0, SEI_DAT_4, 5);   /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_WAIT, SEI_INSTR_M_CK_HIGH, SEI_DAT_0, SEI_DATA_CONST_0, 0);       /* timeout */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, SEI_INSTR_M_CK_LOW, SEI_DAT_0, SEI_DAT_0, 0);

    /* [4] sample config*/
    sample_config.latch_select = SEI_LATCH_0;
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);

    /* [5] update config*/
    update_config.pos_data_idx = SEI_DAT_3;
    update_config.pos_data_use_rx = true;
    update_config.update_on_err = false;
    update_config.latch_select = SEI_LATCH_1;
    update_config.data_register_select = BIT3_MASK;    /* SEI_DAT_3 */
    sei_update_config_init(BOARD_SEI, BOARD_SEI_CTRL, &update_config);

    /* [6] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = false;
    state_transition_config.clk_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_high_match;
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
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = (instr_idx - 2);
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = 0;
    sei_state_transition_latch_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1, &state_transition_latch_config);

#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    /* latch2 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 5;    /* after wait 0 */
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = false;
    state_transition_config.rxd_cfg = sei_state_tran_condition_rise_entry;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_1_2, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_2_3, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = (instr_idx - 2);
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_1_2;
    state_transition_latch_config.delay = 0;
    sei_state_transition_latch_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, &state_transition_latch_config);
#endif

    /* [7] engine config */
    printf("Started sei engine!\n");
    engine_config.arming_mode = sei_arming_wait_trigger;
    engine_config.data_cdm_idx = 0;
    engine_config.data_base_idx = 0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = false;
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [8] interrupt config */
#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_latch2_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_latch2_event | sei_irq_trx_err_event, true);
#else
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch1_event | sei_irq_trx_err_event, true);
#endif
    intc_m_enable_irq_with_priority(BOARD_SEI_IRQn, 1);

    /* [9] trigger config */
    /* When a trigger is generated, the hardware will automatically fill the command register from trigger table command register */
    sei_set_trig_input_command_value(BOARD_SEI, BOARD_SEI_CTRL, sei_trig_in_period, 0x07);

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
#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    uint32_t rxd_rise_tm;
    uint32_t baud_div;
    uint32_t rise_pos;
    static volatile bool s_rx_point_calc_flag;
    static volatile uint8_t s_rx_point_calc_tm;
#endif

    irq_flag = sei_get_irq_flag(BOARD_SEI, BOARD_SEI_CTRL);
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, irq_flag);
    irq_flag &= sei_get_irq_enable_status(BOARD_SEI, BOARD_SEI_CTRL);

#if defined(HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE) && HPM_IP_FEATURE_SEI_RX_LATCH_FEATURE
    if ((irq_flag & sei_irq_latch2_event) != 0) {
        sample_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        rxd_rise_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2);
        delta = (rxd_rise_tm > sample_latch_tm) ? (rxd_rise_tm - sample_latch_tm) : (rxd_rise_tm - sample_latch_tm + 0xFFFFFFFFu);
        printf("delta: %u ns\n", delta * 1000 / ((clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000)));
        if (!s_rx_point_calc_flag) {
            s_rx_point_calc_tm++;
            if (s_rx_point_calc_tm > 2) {
                s_rx_point_calc_flag = true;
                sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch2_event, false);
                baud_div = sei_get_xcvr_baud_div(BOARD_SEI, BOARD_SEI_CTRL) + 1;
                rise_pos = delta % baud_div;
                if ((rise_pos < (baud_div >> 2)) || (rise_pos > ((baud_div >> 2) * 3))) {
                    sei_set_xcvr_rx_point(BOARD_SEI, BOARD_SEI_CTRL, sei_get_xcvr_ck1_point(BOARD_SEI, BOARD_SEI_CTRL));
                    printf("rx point changed!\n");
                }
            }
        }
    }
#endif

    if ((irq_flag & sei_irq_latch1_event) != 0) {
        sample_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
        update_latch_tm = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_1);
        delta = (update_latch_tm > sample_latch_tm) ? (update_latch_tm - sample_latch_tm) : (update_latch_tm - sample_latch_tm + 0xFFFFFFFFu);
        printf("E:%#x, ST:%#x, CRC:%#x, sample_tm:%u, update_tm:%u, TimeDelay:%d*0.1us\n",
                sei_get_data_value(BOARD_SEI, SEI_DAT_2),
                sei_get_data_value(BOARD_SEI, SEI_DAT_3),
                sei_get_data_value(BOARD_SEI, SEI_DAT_4) & 0x1F,
                sample_latch_tm, update_latch_tm, delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 10000000));
    }

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        printf("TRX Error!\n");
    }
}
