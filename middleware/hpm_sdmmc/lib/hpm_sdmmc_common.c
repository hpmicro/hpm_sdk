/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_card.h"
#include <string.h>

hpm_stat_t sdmmc_go_idle_state(sdmmc_host_t *host, uint32_t argument)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_go_idle_state;
        host_cmd->cmd_argument = argument;
        host_cmd->resp_type = sdxc_dev_resp_none;

        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_go_inactive_state(sdmmc_host_t *host, uint16_t relative_addr)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_go_inactive_state;
        host_cmd->cmd_argument = ((uint32_t) relative_addr) << 16;
        host_cmd->resp_type = sdxc_dev_resp_none;

        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_select_card(sdmmc_host_t *host, uint16_t relative_addr, bool is_selected)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_select_card;
        host_cmd->cmd_argument = ((uint32_t) relative_addr) << 16;

        if (is_selected) {
            host_cmd->resp_type = sdxc_dev_resp_r1b;
        } else {
            host_cmd->resp_type = sdxc_dev_resp_none;
        }
        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_send_application_command(sdmmc_host_t *host, uint16_t relative_addr)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_app_cmd;
        host_cmd->cmd_argument = ((uint32_t) relative_addr) << 16;

        host_cmd->resp_type = sdxc_dev_resp_r1;
        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_set_block_count(sdmmc_host_t *host, uint32_t block_count)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_set_block_count;
        host_cmd->cmd_argument = block_count;
        host_cmd->resp_type = sdxc_dev_resp_r1;

        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_set_block_size(sdmmc_host_t *host, uint32_t block_size)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_set_block_length;
        host_cmd->cmd_argument = block_size;
        host_cmd->resp_type = sdxc_dev_resp_r1;

        status = sdmmchost_send_command(host, host_cmd);

    } while (false);

    return status;
}

hpm_stat_t sdmmc_enable_auto_tuning(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((host == NULL) || (host->host_param.base == NULL));
        sdxc_enable_auto_tuning(host->host_param.base, true);
        status = status_success;
    } while (false);

    return status;
}

uint32_t extract_csd_field(const uint32_t *raw_csd, uint8_t end_offset, uint8_t start_offset)
{
    assert((start_offset <= end_offset) && ((end_offset - start_offset) <= 31U));
    uint32_t result = 0;

    uint32_t start_word_index = start_offset / 32;
    uint32_t end_word_index = end_offset / 32;
    uint32_t end_offset_in_word = end_offset % 32;
    uint32_t start_offset_in_word = start_offset % 32;

    /* If all bits of the field are in the same raw_csd word */
    if (start_word_index == end_word_index) {
        uint32_t field_width = end_offset - start_offset + 1U;
        uint32_t field_mask = ((1UL << field_width) - 1U) << start_offset;
        result = (raw_csd[start_word_index] & field_mask) >> start_offset_in_word;
    } else {
        /* If the bits of the field crosses two raw_csd words */
        uint32_t lsb_bits = 32U - start_offset_in_word;
        uint32_t result_lsb = raw_csd[start_word_index] >> start_offset_in_word;
        uint32_t result_msb = raw_csd[end_word_index] & ((1UL << end_offset_in_word) - 1U);
        result = (result_msb << lsb_bits) | result_lsb;
    }

    return result;
}
