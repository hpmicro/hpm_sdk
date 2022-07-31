/*
 * Copyright (c) 2021 - 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_COMMON_H
#define HPM_SDMMC_COMMON_H

#include "hpm_common.h"
#include "hpm_sdmmc_host.h"

/******************************************************************
 *
 * @brief SD/MMC Typical Clock frequency definitions
 *
 ******************************************************************/
#define SDMMC_CLOCK_400KHZ (400000UL)
#define SD_CLOCK_25MHZ (25000000UL)
#define SD_CLOCK_50MHZ (50000000UL)
#define SD_CLOCK_100MHZ (100000000UL)
#define SD_CLOCK_208MHZ (208000000UL)

#define MMC_CLOCK_26MHz (26000000UL)
#define MMC_CLOCK_52MHz (52000000UL)
#define MMC_CLOCK_DDR52 (52000000UL)
#define MMC_CLOCK_HS200 (200000000UL)
#define MMC_CLOCK_HS400 (200000000UL)


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

enum {
    sdmmc_state_idle = 0,
    sdmmc_state_ready = 1,
    sdmmc_state_identify = 2,
    sdmmc_state_standby = 3,
    sdmmc_state_transfer = 4,
    sdmmc_state_send_data = 5,
    sdmmc_state_receive_data = 6,
    sdmmc_state_program = 7,
    sdmmc_state_disconnect = 8
};

enum
{
    sdmmc_cmd_go_idle_state = 0,
    sdmmc_cmd_all_send_cid = 2,
    sdmmc_cmd_set_dsr = 4,
    sdmmc_cmd_select_card = 7,
    sdmmc_cmd_send_csd = 9,
    sdmmc_cmd_send_cid = 10,
    sdmmc_cmd_top_transmission = 12,
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


typedef enum {
    sdmmc_operation_voltage_1v8 = 5,
    sdmmc_operation_voltage_3v0 = 6,
    sdmmc_operation_voltage_3v3 = 7,
}sdmmc_operation_voltage_t;


typedef enum {
    sdmmc_resp_none = 0,
    sdmmc_resp_r1,
    sdmmc_resp_r1b,
    sdmmc_resp_r2,
    sdmmc_resp_r3,
    sdmmc_resp_r4,
    sdmmc_resp_r5,
    sdmmc_resp_r5b,
    sdmmc_resp_r6,
    sdmmc_resp_r7,
} sdmmc_resp_type_t;

enum {
    status_sdmmc_card_not_support = MAKE_STATUS(status_group_sdmmc, 0),
    status_sdmmc_wait_card_insert_timeout = MAKE_STATUS(status_group_sdmmc, 1),
    status_sdmmc_no_sd_card_inserted = MAKE_STATUS(status_group_sdmmc, 2),
    status_sdmmc_device_init_required = MAKE_STATUS(status_group_sdmmc, 3),
    status_sdmmc_wait_busy_timeout = MAKE_STATUS(status_group_sdmmc, 4),

};

#define SDMMC_BLOCK_SIZE_DEFAULT (512U)

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Switch device to Idle state
     * @param [in/out] host SD/MMC Host Context
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_go_idle_state(sdmmc_host_t *host);
    /**
     * @brief Switch device to Inactive state
     * @param [in/out] host SD/MMC Host Context
     * @param [in] relative_addr device relative address
     * @return status_succes if operation is successful
     */
    hpm_stat_t sdmmc_go_inactive_state(sdmmc_host_t *host, uint16_t relative_addr);
    /**
     * @brief Select/De-select the device
     * @param [in/out] host SD/MMC Host Context
     * @param [in] relative_addr device relative address
     * @param [in] is_selected true: select, false: de-select
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_select_card(sdmmc_host_t *host, uint16_t relative_addr, bool is_selected);
    /**
     * @brief Send Application Command
     * @param [in/out] host SD/MMC Host Context
     * @param [in] relative_addr device related address
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_send_application_command(sdmmc_host_t *host, uint16_t relative_addr);
    /**
     * @brief Set block count
     * @param [in/out] host SD/MMC Host Context
     * @param [in] block_count  SD/MMC Block count
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_set_block_count(sdmmc_host_t *host, uint32_t block_count);
    /**
     * @brief Set Block size
     * @param [in/out] host SD/MMC Host Context
     * @param [in] block_size SD/MMC Block size
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_set_block_size(sdmmc_host_t *host, uint32_t block_size);

    /**
     * @brief Enable Auto Tuning mode
     * @param [in/out] host SD/MMC Host Context
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_enable_auto_tuning(sdmmc_host_t *host);

    /**
     * @brief Get System address
     * @param [in] host SD/MMC Host Context
     * @param [in] addr memory address
     * @return Converted system address
     */
    extern uint32_t sdmmc_get_sys_addr(sdmmc_host_t *host, uint32_t addr);


#ifdef __cplusplus
}
#endif


#endif /* HPM_SDMMC_COMMON_H */
