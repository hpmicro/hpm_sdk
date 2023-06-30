/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_COMMON_H
#define HPM_SDMMC_COMMON_H

#include "hpm_common.h"
#include "hpm_sdmmc_card.h"
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

typedef enum {
    sdmmc_operation_voltage_1v8 = 5,
    sdmmc_operation_voltage_3v0 = 6,
    sdmmc_operation_voltage_3v3 = 7,
} sdmmc_operation_voltage_t;


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

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Switch device to Idle state
     * @param [in/out] host SD/MMC Host Context
     * @param [in] argument Argument for CMD0
     * @return status_success if operation is successful
     */
    hpm_stat_t sdmmc_go_idle_state(sdmmc_host_t *host, uint32_t argument);
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
     * @brief Extract Fields from raw CSD data
     * @param [in] raw_csd Raw CSD data array
     * @param [in] end_offset end offset of the specific field (in terms of bit)
     * @param [in] start_offset start offset of the specific field (in terms of bit)
     * @return Extracted CSD field
     */
    uint32_t extract_csd_field(const uint32_t *raw_csd, uint8_t end_offset, uint8_t start_offset);

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
