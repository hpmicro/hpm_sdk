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
static uint32_t mock_rev = 0x8888;
static uint32_t sample_latch_tm1;
static uint32_t sample_latch_tm2;
static uint32_t s_eeprom_page;
static uint32_t s_eeprom[6][127];
volatile bool flag_trx_error;

int main(void)
{
    sei_transceiver_config_t transceiver_config = { 0 };
    sei_data_format_config_t data_format_config = { 0 };
    sei_engine_config_t engine_config = { 0 };
    sei_state_transition_config_t state_transition_config = { 0 };
    sei_state_transition_latch_config_t state_transition_latch_config = { 0 };
    sei_sample_config_t sample_config = { 0 };
    sei_command_table_config_t command_table_config = { 0 };
    uint8_t instr_idx;

    board_init();
    board_init_sei_pins(BOARD_SEI, BOARD_SEI_CTRL);

    printf("SEI slave tamagawa sample\n");

    /* [0] disable engine and enable timestamp */
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, false);
    synt_enable_timestamp(HPM_SYNT, true);

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
    /* cmd register : recv CF */
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
    /* data register 2 : recv ADF */
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
    /* data register 3 : recv EDF */
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
    /* data register 4: send SF  */
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
    /* data register 5: send ABS0 ABS1 ABS2 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 24;
    data_format_config.last_bit = 23;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 23;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(BOARD_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);
    /* data register 6: send ENID */
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
    /* data register 7: send ABM0 ABM1 ABM2 */
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
    /* data register 8: send ALMC */
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
    /* data register 9: send crc */
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
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_CMD, 8);                       /* Recv CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_CMD_TABLE_INSTR_IDX0); /* jump to cmd table instr_idx[0] */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 1);                             /* halt 1 bit for update */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 6);                             /* halt some bits for waiting */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_CMD_TABLE_INSTR_IDX1); /* jump to cmd table instr_idx[1] */
    /* Data ID0 : 0x02 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_CMD, 8);/* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_5, 24); /* ST(POS) */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);
    /* Data ID1 : 0x8A */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_CMD, 8);/* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_7, 24); /* MT */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);
    /* Data ID2 : 0x92 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_CMD, 8);/* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_6, 8);  /* ENID:0x17 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);
    /* Data ID3 : 0x1A */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_CMD, 8);/* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_5, 24); /* ST(POS) */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_6, 8);  /* ENID:0x17 */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_7, 24); /* MT */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_8, 8);  /* ALMC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);
    /* Data ID6 : 0x32 */ /*write to eeprom */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_2, 8);                         /* ADF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_3, 8);                         /* EDF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_9, 8);                         /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_CMD_TABLE_INSTR_IDX1); /* jump to cmd table instr_idx[1] */
    /* Data IDD : 0xEA */ /*read from eeprom */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_9, SEI_DAT_2, 8);                         /* ADF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_RECV_WDG, 0, SEI_DAT_0, SEI_DAT_9, 8);                         /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_CMD_TABLE_INSTR_IDX1); /* jump to cmd table instr_idx[1] */
    /* Response to write/read eeprom */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 1); /* halt 1 bit for update */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 6); /* halt some bits for waiting */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_CMD, 8); /* CF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_2, 8); /* ADF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_9, SEI_DAT_3, 8); /* EDF */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_9, 8); /* CRC */
    sei_set_instr(BOARD_SEI, instr_idx++, SEI_INSTR_OP_JUMP, 0, SEI_DAT_0, SEI_DAT_0, SEI_JUMP_INIT_INSTR_IDX);

    /* [4] command table */
    command_table_config.cmd_min_value = 0x02u;
    command_table_config.cmd_max_value = 0x02u;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 2;
    command_table_config.instr_idx[1] = 05;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_0, &command_table_config);
    command_table_config.cmd_min_value = 0x8Au;
    command_table_config.cmd_max_value = 0x8Au;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 2;
    command_table_config.instr_idx[1] = 10;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_1, &command_table_config);
    command_table_config.cmd_min_value = 0x92u;
    command_table_config.cmd_max_value = 0x92u;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 2;
    command_table_config.instr_idx[1] = 15;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_2, &command_table_config);
    command_table_config.cmd_min_value = 0x1Au;
    command_table_config.cmd_max_value = 0x1Au;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 2;
    command_table_config.instr_idx[1] = 20;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_3, &command_table_config);
    command_table_config.cmd_min_value = 0x32u;
    command_table_config.cmd_max_value = 0x32u;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 28;
    command_table_config.instr_idx[1] = 35;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_4, &command_table_config);
    command_table_config.cmd_min_value = 0xEAu;
    command_table_config.cmd_max_value = 0xEAu;
    command_table_config.cmd_mask_value = 0xFFu;
    command_table_config.instr_idx[0] = 32;
    command_table_config.instr_idx[1] = 35;
    sei_cmd_table_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_CMD_TABLE_5, &command_table_config);

    /* [5] sample config*/
    sample_config.pos_data_idx = SEI_DAT_5;
    sample_config.rev_data_idx = SEI_DAT_7;
    sample_config.pos_data_use_rx = false;
    sample_config.rev_data_use_rx = false;
    sample_config.sample_window = 0x5;
    sample_config.sample_once = true;
    sample_config.latch_select = SEI_LATCH_0;
    sample_config.data_register_select = BIT5_MASK | BIT7_MASK | BIT9_MASK; /* POS and REV data will be sampled into DAT5 and DAT7, CRC(DAT9) value will be set to init value */
    sei_sample_config_init(BOARD_SEI, BOARD_SEI_CTRL, &sample_config);
    sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);
    sei_set_sample_rev_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_rev);

    /* [6] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 2;
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
    state_transition_config.instr_ptr_value = 35;
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

    /* latch2 */
    /* used to clear trx error flag */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_rise_entry;
    state_transition_config.instr_ptr_value = 0;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
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
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = 0;
    sei_state_transition_latch_config_init(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_2, &state_transition_latch_config);

    /* [7] engine config */
    printf("Started sei engine!\n");
    engine_config.arming_mode = sei_arming_direct_exec;
    engine_config.data_cdm_idx = 0;
    engine_config.data_base_idx = 0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = true;
    engine_config.wdg_action = sei_wdg_exec_exception_instr;
    engine_config.wdg_instr_idx = 0;
    engine_config.wdg_time = 1000; /* 1000 bits time */
    sei_engine_config_init(BOARD_SEI, BOARD_SEI_CTRL, &engine_config);
    sei_set_engine_enable(BOARD_SEI, BOARD_SEI_CTRL, true);

    /* [8] interrupt config */
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_latch2_event | sei_irq_wdog_event | sei_irq_trx_err_event);
    sei_set_irq_enable(BOARD_SEI, BOARD_SEI_CTRL, sei_irq_latch0_event | sei_irq_latch1_event | sei_irq_latch2_event | sei_irq_wdog_event | sei_irq_trx_err_event, true);
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
    uint32_t cmd;

    irq_flag = sei_get_irq_flag(BOARD_SEI, BOARD_SEI_CTRL);
    sei_clear_irq_flag(BOARD_SEI, BOARD_SEI_CTRL, irq_flag);
    irq_flag &= sei_get_irq_enable_status(BOARD_SEI, BOARD_SEI_CTRL);

    if ((irq_flag & sei_irq_trx_err_event) != 0) {
        flag_trx_error = true;
        sei_set_engine_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        printf("TRX Error!\n");
    }

    if ((irq_flag & sei_irq_latch2_event) != 0) {
        flag_trx_error = false;
    }

    if ((irq_flag & sei_irq_latch0_event) != 0) {
        /* Something wrong happened, so we rewind engine to the init instr */
        if (flag_trx_error) {
            sei_set_engine_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        } else {
            sample_latch_tm1 = sei_get_latch_time(BOARD_SEI, BOARD_SEI_CTRL, SEI_LATCH_0);
            mock_pos++;
            if (mock_pos > 0x00FFFFFF) {
                mock_pos = 0;
                mock_rev++;
                if (mock_rev > 0x00FFFFFF) {
                    mock_rev = 0;
                }
            }
            sei_set_sample_pos_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_pos);
            sei_set_sample_rev_override_value(BOARD_SEI, BOARD_SEI_CTRL, mock_rev);
            sei_set_data_value(BOARD_SEI, SEI_DAT_4, 0x00);
            sei_set_data_value(BOARD_SEI, SEI_DAT_6, 0x17);
            sei_set_data_value(BOARD_SEI, SEI_DAT_8, 0x00);
            delta = (sample_latch_tm1 > sample_latch_tm2) ? (sample_latch_tm1 - sample_latch_tm2) : (sample_latch_tm1 - sample_latch_tm2 + 0xFFFFFFFFu);
            printf("CMD:%#x, SF:%#x, ST:%#x, ENID:%#x, MT:%#x, ALMC:%#x, CRC:%#x, sample_tm1:%u, sample_tm2:%u, sample_interval:%d us\n",
                    sei_get_command_value(BOARD_SEI, BOARD_SEI_CTRL),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_4),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_5),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_6),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_7),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_8),
                    sei_get_data_value(BOARD_SEI, SEI_DAT_9) & 0xFF,
                    sample_latch_tm1, sample_latch_tm2, delta / (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000));
            sample_latch_tm2 = sample_latch_tm1;
        }
    } else if ((irq_flag & sei_irq_latch1_event) != 0) {
        /* Something wrong happened, so we rewind engine to the init instr */
        if (flag_trx_error) {
            sei_set_engine_rewind(BOARD_SEI, BOARD_SEI_CTRL);
        } else {
            cmd = sei_get_command_value(BOARD_SEI, BOARD_SEI_CTRL);
            if (cmd == 0x32u) {
                uint32_t addr = sei_get_data_value(BOARD_SEI, SEI_DAT_2) & 0x7Fu;
                uint32_t data = sei_get_data_value(BOARD_SEI, SEI_DAT_3);
                if (addr == 127) {
                    s_eeprom_page = data;
                    printf("Change EEPORM page to %d\n", s_eeprom_page);
                } else {
                    s_eeprom[s_eeprom_page][addr] = data;
                    printf("Write EEPORM - Page: %d, Addr: %d, Data: %d\n", s_eeprom_page, addr, data);
                }
            } else if (cmd == 0xEAu) {
                uint32_t addr = sei_get_data_value(BOARD_SEI, SEI_DAT_2) & 0x7Fu;
                sei_set_data_value(BOARD_SEI, SEI_DAT_2, addr);
                sei_set_data_value(BOARD_SEI, SEI_DAT_3, s_eeprom[s_eeprom_page][addr]);
                printf("Read EEPORM - Page: %d, Addr: %d, Data: %d\n", s_eeprom_page, addr, s_eeprom[s_eeprom_page][addr]);
            } else {
                ;
            }
        }
    } else {
        ;
    }

    if ((irq_flag & sei_irq_wdog_event) != 0) {
        sei_set_command_rewind(BOARD_SEI, BOARD_SEI_CTRL);
    }
}
