/*
 * Copyright (c) 2021 - 2022 hpmicro
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



uint32_t sdmmc_get_sys_addr(sdmmc_host_t *host, uint32_t addr)
{
    return core_local_mem_to_sys_address(host->host_param.hart_id, addr);
}


ATTR_WEAK hpm_stat_t sdmmchost_get_board_config(sdmmchost_config_t *config)
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
    config->hart_id = BOARD_RUNNING_CORE;
#if defined(BOARD_APP_SDCARD_SUPPORT_1V8) && (BOARD_APP_SDCARD_SUPPORT_1V8 == 1)
    config->support_1v8 = true;
#else
    config->support_1v8 = false;
#endif

#if defined(BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION) && (BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION == 1)
#if defined(BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO) && (BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO == 1)
    config->card_detection_mode = sdmmc_host_card_detection_via_gpio;
#else
    config->card_detection_mode = sdmmc_host_card_detection_via_sdxc;
#endif
#else
    config->card_detection_mode = sdmmc_host_card_detection_none;
#endif

#ifdef BOARD_APP_SDCARD_CARD_DETECTION_GPIO
    config->card_detection_gpio = BOARD_APP_SDCARD_CARD_DETECTION_GPIO;
#else
    config->card_detection_gpio = NULL;
#endif

#ifdef BOARD_APP_SDCARD_CARD_DETECTION_IRQ
    config->card_detection_irq_index = BOARD_APP_SDCARD_CARD_DETECTION_IRQ;
#endif

#if defined(SDMMC_HOST_ENABLE_IRQ_MODE) && (SDMMC_HOST_ENABLE_IRQ_MODE == 1)
    config->use_polling_mode = false;
#else
    config->use_polling_mode = true;
#endif

    return status_success;
}

sdmmc_host_t *sdmmchost_get_host(void)
{
    (void) memset(&s_sdmmc_host, 0, sizeof(s_sdmmc_host));

    sdmmchost_config_t host_config;
    sdmmchost_get_board_config(&host_config);

    s_sdmmc_host.host_param.base = host_config.base;
    s_sdmmc_host.host_param.clock_init_func = host_config.clock_init_func;
    s_sdmmc_host.host_param.io_init_func = host_config.io_init_func;
    s_sdmmc_host.host_param.delay_ms = host_config.delay_ms;
    s_sdmmc_host.host_param.card_detect = host_config.card_detect;
    s_sdmmc_host.host_param.switch_to_1v8 = host_config.switch_to_1v8;
    s_sdmmc_host.host_param.support_1v8 = host_config.support_1v8;
    s_sdmmc_host.host_param.card_detection_mode = host_config.card_detection_mode;
    s_sdmmc_host.host_param.card_detection_gpio_index = host_config.card_detection_gpio_index;
    s_sdmmc_host.host_param.card_detection_pin_index = host_config.card_detection_pin_index;
    s_sdmmc_host.host_param.card_detection_gpio = host_config.card_detection_gpio;
    s_sdmmc_host.host_param.card_detection_irq_index = host_config.card_detection_irq_index;
    s_sdmmc_host.host_param.hart_id = host_config.hart_id;
    s_sdmmc_host.host_param.use_polling_mode = host_config.use_polling_mode;

    return &s_sdmmc_host;
}
