/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "hpm_soc.h"
#include "sdmmc_port.h"
#include "hpm_sdmmc_host.h"
#include "board.h"


ATTR_PLACE_AT_NONCACHEABLE_BSS static sdmmc_host_t s_sdmmc_host;


static hpm_stat_t sdmmchost_get_board_config(sdmmchost_config_t *config);

uint32_t sdmmc_get_sys_addr(uint32_t addr)
{
    return core_local_mem_to_sys_address(RUNNING_CORE, addr);
}


static hpm_stat_t sdmmchost_get_board_config(sdmmchost_config_t *config)
{
    if (config == NULL) {
        return status_invalid_argument;
    }
    config->base = BOARD_APP_SDCARD_SDXC_BASE;
    config->clock_init_func = board_sd_configure_clock;
    config->io_init_func = board_init_sd_pins;
    config->switch_to_1v8 = board_sd_switch_pins_to_1v8;
    config->card_detect = board_sd_detect_card;
    config->delay_ms = board_delay_ms;
#if defined(BOARD_APP_SDCARD_SUPPORT_1V8) && (BOARD_APP_SDCARD_SUPPORT_1V8 == 1)
    config->support_1v8 = true;
#else
    config->support_1v8 = false;
#endif

    return status_success;
}

sdmmc_host_t *sdmmchost_get_host(void)
{
    (void) memset(&s_sdmmc_host, 0, sizeof(s_sdmmc_host));

    sdmmchost_config_t host_config;
    sdmmchost_get_board_config(&host_config);

    s_sdmmc_host.base = host_config.base;
    s_sdmmc_host.clock_init_func = host_config.clock_init_func;
    s_sdmmc_host.io_init_func = host_config.io_init_func;
    s_sdmmc_host.delay_ms = host_config.delay_ms;
    s_sdmmc_host.card_detect = host_config.card_detect;
    s_sdmmc_host.switch_to_1v8 = host_config.switch_to_1v8;
    s_sdmmc_host.support_1v8 = host_config.support_1v8;

    return &s_sdmmc_host;
}
