/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_CARD_H
#define HPM_SDMMC_CARD_H

#include <stdint.h>

/**
 * @brief Common SD/MMC commands
 */
enum {
    sdmmc_cmd_go_idle_state = 0,
    sdmmc_cmd_all_send_cid = 2,
    sdmmc_cmd_set_dsr = 4,
    sdmmc_cmd_select_card = 7,
    sdmmc_cmd_send_csd = 9,
    sdmmc_cmd_send_cid = 10,
    sdmmc_cmd_stop_transmission = 12,
    sdmmc_cmd_send_status = 13,
    sdmmc_cmd_go_inactive_state = 15,
    sdmmc_cmd_set_block_length = 16,
    sdmmc_cmd_read_single_block = 17,
    sdmmc_cmd_read_multiple_block = 18,
    sdmmc_cmd_set_block_count = 23,
    sdmmc_cmd_write_single_block = 24,
    sdmmc_cmd_write_multiple_block = 25,
    sdmmc_cmd_program_csd = 27,
    sdmmc_cmd_set_write_protect = 29,
    sdmmc_cmd_clear_write_protect = 30,
    sdmmc_cmd_erase = 38,
    sdmmc_cmd_lock_unlock = 42,
    sdmmc_cmd_app_cmd = 55,
    sdmmc_cmd_general_cmd = 56,
    sdmmc_cmd_read_ocr = 58,
};

/**
 * @brief SD Card specific commands
 */
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
    sd_cmd_crc_option = 59,
};

/**
 * @brief MMC specific commands
 */
enum {
    emmc_cmd_send_op_cond = 1,
    emmc_cmd_all_send_cid = sdmmc_cmd_all_send_cid,
    emmc_cmd_set_relative_addr = 3,
    emmc_cmd_set_dsr = sdmmc_cmd_set_dsr,
    emmc_cmd_sleep_awake = 5,
    emmc_cmd_switch = 6,
    emmc_cmd_select = sdmmc_cmd_select_card,
    emmc_cmd_send_ext_csd = 8,
    emmc_cmd_send_csd = sdmmc_cmd_send_csd,
    emmc_cmd_send_cid = sdmmc_cmd_send_cid,
    emmc_cmd_stop_transmission = sdmmc_cmd_stop_transmission,
    emmc_cmd_send_status = sdmmc_cmd_send_status,
    emmc_cmd_bus_test = 14,
    emmc_cmd_go_inactive_state = sdmmc_cmd_go_inactive_state,

    emmc_cmd_set_block_length = sdmmc_cmd_set_block_length,
    emmc_cmd_read_single_block = sdmmc_cmd_read_single_block,
    emmc_cmd_read_multiple_block = sdmmc_cmd_read_multiple_block,
    emmc_cmd_send_tuning_block = 21,
    emmc_cmd_set_block_count = sdmmc_cmd_set_block_count,
    emmc_cmd_write_single_block = sdmmc_cmd_write_single_block,
    emmc_cmd_write_multiple_block = sdmmc_cmd_write_multiple_block,
    emmc_cmd_program_cid = 26,
    emmc_cmd_program_csd = sdmmc_cmd_program_csd,
    emmc_cmd_set_time = 49,

    emmc_cmd_erase_group_start = 35,
    emmc_cmd_erase_group_end = 36,
    emmc_cmd_erase = sdmmc_cmd_erase,

    emmc_cmd_set_write_prot = 28,
    emmc_cmd_clear_write_prot = 29,
    emmc_cmd_send_write_prot = 30,
    emmc_cmd_send_write_prot_type = 31,

    emmc_cmd_fast_io = 39,
    emmc_cmd_go_irq_state = 40,
    emmc_cmd_lock_unlock = sdmmc_cmd_lock_unlock,

    emmc_cmd_app_cmd = sdmmc_cmd_app_cmd,
    emmc_cmd_gen_cmd = sdmmc_cmd_general_cmd,

    emmc_cmd_protocol_read = 53,
    emmc_cmd_protocol_write = 54,

    emmc_cmd_queued_task_params = 44,
    emmc_cmd_queued_task_address = 45,
    emmc_cmd_execute_read_task = 46,
    emmc_cmd_execute_write_task = 47,
    emmc_cmd_cmdq_task_mgmt = 48,
};


/**
 * @brief SD/MMC R1 register information
 */
typedef union {
    uint32_t status;
    struct {
        uint32_t : 3;
        uint32_t ake_seq_error: 1;
        uint32_t : 1;
        uint32_t app_cmd: 1;
        uint32_t fx_event: 1;
        uint32_t : 1;
        uint32_t ready_for_data: 1;
        uint32_t current_state: 4;
        uint32_t erase_reset: 1;
        uint32_t card_ecc_disabled: 1;
        uint32_t wp_erase_skip: 1;
        uint32_t csd_overwrite: 1;
        uint32_t : 2;
        uint32_t error: 1;
        uint32_t cc_error: 1;
        uint32_t card_ecc_failed: 1;
        uint32_t illegal_command: 1;
        uint32_t com_crc_error: 1;
        uint32_t lock_unlock_failed: 1;
        uint32_t card_is_locked: 1;
        uint32_t wp_violation: 1;
        uint32_t erase_param: 1;
        uint32_t erase_seq_error: 1;
        uint32_t block_len_error: 1;
        uint32_t address_error: 1;
        uint32_t out_of_range: 1;
    };
} sdmmc_r1_status_t;

/**
 * @brief SD Card Satus Register Information
 */
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

/**
 * @brief SD CID Register Information
 */
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
    uint32_t cid_words[4];
} sd_cid_t;

/**
 * @brief SD OCR register information
 */
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


/**
 * @brief SD Card CSD register information
 */
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

/**
 * @brief SD Card SCR register information
 */
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



typedef enum {
    card_type_mmc       = 0,
    card_type_sd_v1     = 1,
    card_type_sd_v2     = 2,
    card_type_sd_v2_hc  = 4,
} sdcard_type_t;

#define SDMMC_BLOCK_SIZE_DEFAULT (512U)


/**
 * @brief Dummy Byte for SD when Card works in SPI mode
 */
#define SPISD_DUMMY_BYTE                  0xFFU

/**
 * @brief sdcard block size in SPI mode
 */
#define SPI_SD_BLOCK_SIZE                 (512U)

/**
 * @brief R1 register bits when CARD works in SPI mode
 */
#define SPISD_R1_IDLE_FLAG                  (0x01)
#define SPISD_R1_ERASE_RESET_FLAG           (0x02)
#define SPISD_R1_ILLEGAL_CMD_FLAG           (0x04)
#define SPISD_R1_CMD_CRC_FLAG               (0x08)
#define SPISD_R1_ERASE_SEQ_ERROR_FLAG       (0x10)
#define SPISD_R1_ADDR_ERROR_FLAG            (0x20)
#define SPISD_R1_PARAM_ERROR_FLAG           (0x40)
#define SPISD_R1_ZERO_FLAG                  (0x80)


#endif /* HPM_SDMMC_CARD_H */