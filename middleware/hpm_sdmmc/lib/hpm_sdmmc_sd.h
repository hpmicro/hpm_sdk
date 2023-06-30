/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_SD_H
#define HPM_SDMMC_SD_H

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_card.h"

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
