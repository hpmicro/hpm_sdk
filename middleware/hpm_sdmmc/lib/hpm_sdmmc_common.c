/*
 * Copyright (c) 2021 - 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include <string.h>

hpm_stat_t sdmmc_go_idle_state(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);
        sdmmchost_cmd_t *host_cmd = &host->cmd;
        (void) memset(host_cmd, 0, sizeof(sdmmchost_cmd_t));

        host_cmd->cmd_index = sdmmc_cmd_go_idle_state;
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
