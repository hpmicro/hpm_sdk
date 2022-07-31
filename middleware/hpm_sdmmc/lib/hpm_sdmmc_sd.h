/*
 * Copyright (c) 2021 - 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_SD_H
#define HPM_SDMMC_SD_H

#include "hpm_sdmmc_common.h"

enum {
    sd_acmd_set_bus_width = 6,
    sd_acmd_sd_status = 13,
    sd_acmd_set_num_wr_blocks = 22,
    sd_acmd_set_wr_blk_erase_count = 23,
    sd_acmd_sd_send_op_cond = 41,
    sd_acmd_set_clear_card_detect = 42,
    sd_acmd_send_scr = 51,

    sd_cmd_all_send_cid = 2,
    sd_cmd_send_relative_addr = 3,
    sd_cmd_switch = 6,
    sd_cmd_send_if_cond = 8,
    sd_cmd_send_csd = 9,
    sd_voltage_switch = 11,
    sd_cmd_send_tuning_block = 19,
    sd_cmd_erase_start = 32,
    sd_cmd_erase_end = 33,

};

typedef struct {
    uint8_t bus_width;
    uint8_t secure_mode;
    uint16_t card_type;
    uint32_t protected_size;
    uint8_t speed_class;
    uint8_t performance_move;
    uint8_t uhs_speed_grade;
    uint8_t erase_offset;
    uint32_t au_size;
    uint32_t erase_timeout;
    uint32_t erase_size;

    uint32_t uhs_au_size;
    uint8_t reserved[3];
} sd_status_t;

typedef union {
    struct {
        uint64_t : 1;
        uint64_t crc7: 7;
        uint64_t mdt: 12;
        uint64_t : 4;
        uint64_t psn: 32;
        uint64_t prv: 8;
        uint64_t pnm: 40;
        uint64_t oid: 16;
        uint64_t mid: 8;
    };
    uint32_t cid_words[2];
} sd_cid_t;

typedef union {
    uint32_t ocr_word;
    struct {
        uint32_t : 7;
        uint32_t low_voltage_range: 1;
        uint32_t : 7;
        uint32_t support_2v7_2v8: 1;
        uint32_t support_2v8_2v9: 1;
        uint32_t support_2v9_3v0: 1;
        uint32_t support_3v0_3v1: 1;
        uint32_t support_3v1_3v2: 1;
        uint32_t support_3v2_3v3: 1;
        uint32_t support_3v3_3v4: 1;
        uint32_t support_3v4_3v5: 1;
        uint32_t support_3v5_3v6: 1;
        uint32_t switching_to_1v8_accepted: 1;
        uint32_t : 2;
        uint32_t over_2tb_support: 1;
        uint32_t : 1;
        uint32_t uhs2_card_status: 1;
        uint32_t card_capacity_status: 1;
        uint32_t card_power_up_status: 1;
    };
} sd_ocr_t;


typedef struct _sd_csd {
    uint8_t csd_structure;
    uint8_t data_read_access_time1;
    uint8_t data_read_access_time2;
    uint8_t transfer_speed;
    uint16_t card_command_class;
    bool support_read_block_partial;
    bool support_write_block_misalignment;
    bool support_read_block_misalignment;
    bool is_dsr_implemented;
    bool support_sdxc;
    bool is_erase_block_enabled;
    bool is_write_protection_group_enabled;
    bool support_write_block_partial;
    bool support_file_format_group;
    bool support_copy;
    bool support_permanent_write_protect;
    bool support_temporary_write_protect;

    uint8_t read_current_vdd_min;
    uint8_t read_current_vdd_max;
    uint8_t write_current_vdd_min;
    uint8_t write_current_vdd_max;
    uint8_t device_size_multiplier;
    uint8_t write_speed_factor;
    uint8_t file_format;

    uint32_t device_size;
    uint32_t read_block_len;
    uint32_t erase_sector_size;
    uint32_t max_write_block_len;
    uint32_t write_protect_group_size;
} sd_csd_t;

typedef enum {
    sd_timing_sdr12_default = 0,
    sd_timing_sdr25_highspeed = 1,
    sd_timing_sdr50 = 2,
    sd_timing_sdr104 = 3,
    sd_timing_ddr50 = 4,
} sd_timing_mode_t;

typedef enum {
    sd_drive_strength_typeb = 0,
    sd_drive_strength_typea = 1,
    sd_drive_strength_typec = 2,
    sd_drive_strength_typed = 3
} sd_drive_strength_t;

typedef enum {
    sd_current_limit_200ma,
    sd_current_limit_400ma,
    sd_current_limit_600ma,
    sd_current_limit_800ma,
} sd_max_current_t;

typedef union {
    struct {
        uint32_t reserved;
        uint32_t support_cmd20: 1;
        uint32_t support_cmd23: 1;
        uint32_t support_cmd48_or_cmd49: 1;
        uint32_t support_cmd58_or_cmd59: 1;
        uint32_t : 2;
        uint32_t sd_specx: 4;
        uint32_t sd_spec4: 1;
        uint32_t ex_security: 4;
        uint32_t sd_spec3: 1;
        uint32_t sd_bus_widths: 4;
        uint32_t sd_security: 3;
        uint32_t data_stat_after_erase: 1;
        uint32_t sd_spec: 4;
        uint32_t scr_structure: 4;
    };
    uint32_t scr_word[2];
} sd_scr_t;

typedef union {
    uint32_t flags;
    struct {
        uint32_t support_speed_class_control_cmd: 1;
        uint32_t support_set_block_count_cmd: 1;
        uint32_t support_4bit_width: 1;
    };
} sd_flags_t;

enum {
    sd_switch_function_group_access_mode = 1,
    sd_switch_function_group_command_system = 2,
    sd_switch_function_group_drive_strength = 3,
    sd_switch_function_group_power_limit = 4,
};

enum {
    sd_switch_function_mode_check = 0,
    sd_switch_function_mode_set = 1,
};

typedef union {
    uint32_t status_word[16];
    struct {
        uint32_t reserved0[8];
        uint16_t reserved1;
        uint16_t busy_status_for_function_in_group1;
        uint16_t busy_status_for_function_in_group2;
        uint16_t busy_status_for_function_in_group3;
        uint16_t busy_status_for_function_in_group4;
        uint16_t busy_status_for_function_in_group5;

        uint32_t busy_status_for_function_in_group6: 16;
        uint32_t data_structure_version: 8;
        uint32_t function_selection_of_function_group1: 4;
        uint32_t function_selection_of_function_group2: 4;

        uint32_t function_selection_of_function_group3: 4;
        uint32_t function_selection_of_function_group4: 4;
        uint32_t function_selection_of_function_group5: 4;
        uint32_t function_selection_of_function_group6: 4;
        uint32_t support_bits_of_functions_in_function_group1: 16;
        uint32_t support_bits_of_functions_in_function_group2: 16;
        uint32_t support_bits_of_functions_in_function_group3: 16;
        uint32_t support_bits_of_functions_in_function_group4: 16;
        uint32_t support_bits_of_functions_in_function_group5: 16;
        uint32_t support_bits_of_functions_in_function_group6: 16;
        uint32_t max_current_or_power_consumption: 16;
    };
} switch_function_status_t;

typedef struct _sdmmc_sdcard {
    sdmmc_host_t *host;
    uint16_t relative_addr;
    sd_cid_t cid;
    sd_ocr_t ocr;
    sd_csd_t csd;
    sd_scr_t scr;
    switch_function_status_t sfs;
    sd_status_t status;
    sdmmc_r1_status_t r1_status;
    sd_flags_t sd_flags;

    uint32_t block_count;
    uint32_t block_size;
    uint32_t max_freq;
    uint64_t card_size_in_bytes;

    sd_timing_mode_t current_timing;
    sd_drive_strength_t drive_strength;
    sd_max_current_t max_current;
    sdmmc_operation_voltage_t operation_voltage;

    bool is_host_ready;
} sd_card_t;


#ifdef __cplusplus
extern "C" {
#endif

hpm_stat_t sd_init(sd_card_t *card);

void sd_deinit(sd_card_t *card);

hpm_stat_t sd_card_init(sd_card_t *card);

void sd_card_deinit(sd_card_t *card);

hpm_stat_t sd_host_init(sd_card_t *card);

hpm_stat_t sd_host_deinit(sd_card_t *card);

void sd_set_card_power(sd_card_t *card, bool enable);

bool sd_is_card_present(sd_card_t *card);

bool sd_check_readonly(sd_card_t *card);

hpm_stat_t sd_select_card(sd_card_t *card, bool is_selected);

hpm_stat_t sd_read_status(sd_card_t *card);

hpm_stat_t sd_read_blocks(sd_card_t *card, uint8_t *buffer, uint32_t start_block, uint32_t block_count);

hpm_stat_t sd_write_blocks(sd_card_t *card, const uint8_t *buffer, uint32_t start_block, uint32_t block_count);

hpm_stat_t sd_erase_blocks(sd_card_t *card, uint32_t start_block, uint32_t block_count);

hpm_stat_t sd_set_driver_strength(sd_card_t *card, sd_drive_strength_t driver_strength);

hpm_stat_t sd_set_max_current(sd_card_t *card, sd_max_current_t max_current);

hpm_stat_t sd_polling_card_status_busy(sd_card_t *card, uint32_t timeout_ms);


#ifdef __cplusplus
}
#endif

#endif /* HPM_SDMMC_SD_H */
