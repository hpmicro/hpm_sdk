/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_PMBUS_PRIVATE_H
#define __HPM_PMBUS_PRIVATE_H

#include "hpm_pmbus.h"

const hpm_pmbus_cmd_param_t pmbus_cmd_param_table[] = {
    {
        .command_code = PMBUS_CODE_PAGE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_OPERATION,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_ON_OFF_CONFIG,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_CLEAR_FAULTS,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = send_byte
    },

    {
        .command_code = PMBUS_CODE_PHASE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_PAGE_PLUS_WRITE,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = none,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_PAGE_PLUS_READ,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_1,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_2,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_3,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_4,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_5,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_6,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_7,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_8,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_9,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_WRITE_PROTECT,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STORE_DEFAULT_ALL,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = send_byte
    },

    {
        .command_code = PMBUS_CODE_RESTORE_DEFAULT_ALL,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = send_byte
    },

    {
        .command_code = PMBUS_CODE_STORE_DEFAULT_CODE,
        .data_length = 1,
        .read_transaction_type = none,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_RESTORE_DEFAULT_CODE,
        .data_length = 1,
        .read_transaction_type = none,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STORE_USER_ALL,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = send_byte
    },

    {
        .command_code = PMBUS_CODE_RESTORE_USER_ALL,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = send_byte
    },

    {
        .command_code = PMBUS_CODE_STORE_USER_CODE,
        .data_length = 1,
        .read_transaction_type = none,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_RESTORE_USER_CODE,
        .data_length = 1,
        .read_transaction_type = none,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_CAPABILITY,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_QUERY,
        .data_length = 1,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_SMBALERT_MASK,
        .data_length = 2,
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_RESERVED_10,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_11,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_12,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_13,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_VOUT_MODE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_VOUT_COMMAND,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_TRIM,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_CAL_OFFSET,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_MARGIN_HIGH,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_MARGIN_LOW,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_TRANSITION_RATE,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_DROOP,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_SCALE_LOOP,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_SCALE_MONITOR,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_RESERVED_14,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_15,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_16,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_17,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_18,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_COEFFICIENTS,
        .data_length = 5,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_POUT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_MAX_DUTY,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_FREQUENCY_SWITCH,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_RESERVED_19,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_VIN_ON,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VIN_OFF,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_INTERLEAVE,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_CAL_GAIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_CAL_OFFSET,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_FAN_CONFIG_1_2,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_FAN_COMMAND_1,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_FAN_COMMAND_2,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_FAN_CONFIG_3_4,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_FAN_COMMAND_3,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_FAN_COMMAND_4,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_OV_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_OV_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_VOUT_OV_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_UV_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_UV_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VOUT_UV_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_IOUT_OC_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_OC_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_IOUT_OC_LV_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_OC_LV_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_IOUT_OC_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_UC_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IOUT_UC_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

   {
        .command_code = PMBUS_CODE_RESERVED_20,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

   {
        .command_code = PMBUS_CODE_RESERVED_21,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_OT_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_OT_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_OT_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_UT_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_UT_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_UT_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_VIN_OV_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VIN_OV_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_VIN_OV_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VIN_UV_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VIN_UV_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_VIN_UV_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_IIN_OC_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_IIN_OC_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_IIN_OC_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_POWER_GOOD_ON,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_POWER_GOOD_OFF,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TON_DELAY,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TON_RISE,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TON_MAX_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TON_MAX_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_TOFF_DELAY,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TOFF_FALL,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_TOFF_MAX_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_RESERVED_22,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_POUT_OP_FAULT_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_POUT_OP_FAULT_RESPONSE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_POUT_OP_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_PIN_OP_WARN_LIMIT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_RESERVED_23,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_24,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_25,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_26,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_27,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_28,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_29,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_30,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_31,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_32,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_33,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_34,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_STATUS_BYTE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_WORD,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_STATUS_VOUT,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_IOUT,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_INPUT,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_TEMPERATURE,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_CML,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_OTHER,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_MFR_SPECIFIC,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_FANS_1_2,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_STATUS_FANS_3_4,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = write_byte
    },

    {
        .command_code = PMBUS_CODE_RESERVED_35,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_36,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_37,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_EIN,
        .data_length = 5,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_EOUT,
        .data_length = 5,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_VIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_IIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_VCAP,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_VOUT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_IOUT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_TEMPERATURE_1,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_TEMPERATURE_2,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_TEMPERATURE_3,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_FAN_SPEED_1,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_FAN_SPEED_2,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_FAN_SPEED_3,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_FAN_SPEED_4,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_DUTY_CYCLE,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_FREQUENCY,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_POUT,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_READ_PIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_PMBUS_REVISION,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_ID,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_MODEL,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_REVISION,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_LOCATION,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_DATE,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_SERIAL,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_VIN_MIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_VIN_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_IIN_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_PIN_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_VOUT_MIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_VOUT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_IOUT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_POUT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_TAMBIENT_MAX,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_TAMBIENT_MIN,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_EFFICIENCY_LL,
        .data_length = 14,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_EFFICIENCY_HL,
        .data_length = 14,
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_PIN_ACCURACY,
        .data_length = 1,
        .read_transaction_type = read_byte,
        .write_transaction_type = none
    },


    {
        .command_code = PMBUS_CODE_IC_DEVICE_ID,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_IC_DEVICE_REV,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_38,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_00,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_01,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_02,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_03,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_04,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_05,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_06,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_07,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_08,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_09,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_10,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_11,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_12,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_13,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_14,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_USER_DATA_15,
        .data_length = 0xFFFFFFFF, /* data length is variable */
        .read_transaction_type = read_block,
        .write_transaction_type = write_block
    },

    {
        .command_code = PMBUS_CODE_MFR_MAX_TEMP_1,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_MFR_MAX_TEMP_2,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_MFR_MAX_TEMP_3,
        .data_length = 2,
        .read_transaction_type = read_word,
        .write_transaction_type = write_word
    },

    {
        .command_code = PMBUS_CODE_RESERVED_39,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_40,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_41,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_42,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_43,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_44,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_45,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_46,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_47,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_48,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_49,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_50,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_RESERVED_51,
        .data_length = 0,
        .read_transaction_type = none,
        .write_transaction_type = none
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_00,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_01,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_02,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_03,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_04,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_05,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_06,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_07,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_08,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_09,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_10,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_11,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_12,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_13,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_14,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_15,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_16,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_17,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_18,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_19,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_20,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_21,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_22,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_23,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_24,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_25,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_26,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_27,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_28,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_29,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_30,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_31,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_32,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_33,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_34,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_35,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_36,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_37,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_38,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_39,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_40,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_41,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_42,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_43,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_44,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_45,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = mfr_defined,
        .write_transaction_type = mfr_defined
    },

    {
        .command_code = PMBUS_CODE_MFR_SPECIFIC_COMMAND_EXT,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = extended_command,
        .write_transaction_type = extended_command
    },

    {
        .command_code = PMBUS_CODE_PMBUS_COMMAND_EXT,
        .data_length = 0, /* data length is mfr_defined */
        .read_transaction_type = extended_command,
        .write_transaction_type = extended_command
    },

};

#endif
