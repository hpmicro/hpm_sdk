/*
 * Copyright (c) 2021 - 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_host.h"

hpm_stat_t sdmmchost_init(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);


        /* Initialize clock for the identification stage */
        host->clock_init_func(host->base, SDMMC_CLOCK_400KHZ);
        host->io_init_func(host->base);

        sdxc_config_t sdxc_config;
        sdxc_config.data_timeout = 0x8U;

        sdxc_init(host->base, &sdxc_config);

        sdxc_wait_card_active(host->base);
        host->delay_ms(10);

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host)
{
    if (host != NULL) {
        return status_invalid_argument;
    }

    uint32_t data3_0_level;
    uint32_t delay_cnt = 1000000UL;
    do {
        data3_0_level = sdxc_get_data3_0_level(host->base);
    } while ((data3_0_level != 0U) && (delay_cnt-- > 0U));
    if (delay_cnt < 1) {
        return status_timeout;
    }

    sdxc_enable_inverse_clock(host->base, false);
    sdxc_enable_sd_clock(host->base, false);

    host->delay_ms(5);
    sdxc_enable_inverse_clock(host->base, true);
    sdxc_enable_sd_clock(host->base, true);

    host->delay_ms(1);

    delay_cnt = 1000000UL;
    do {
        data3_0_level = sdxc_get_data3_0_level(host->base);
    } while ((data3_0_level == 0U) && (delay_cnt-- > 0U));
    if (delay_cnt < 1) {
        return status_timeout;
    }

    return status_success;
}

void sdmmchost_deinit(sdmmc_host_t *host)
{
    sdxc_reset(host->base, sdxc_reset_cmd_line, 0xffffu);
    sdxc_reset(host->base, sdxc_reset_data_line, 0xffffu);
}

void sdmmchost_reset(sdmmc_host_t *host)
{
    sdxc_reset(host->base, sdxc_reset_cmd_line, 0xffffu);
    sdxc_reset(host->base, sdxc_reset_data_line, 0xffffu);
}

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, sdmmchost_cmd_t *cmd)
{
    sdxc_send_command(host->base, cmd);
    hpm_stat_t status = sdxc_wait_cmd_done(host->base, cmd, true);

    return status;
}

bool sdmmchost_is_card_detected(sdmmc_host_t *host)
{
    bool result;
    if (host->card_detect != NULL) {
        result = host->card_detect(host->base);
    } else {
        result = sdxc_is_card_inserted(host->base);
    }

    return result;
}

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width)
{
    sdxc_set_data_bus_width(host->base, bus_width);
}

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq)
{
    return host->clock_init_func(host->base, freq);
}

void sdmmchost_wait_card_active(sdmmc_host_t *host)
{
    sdxc_wait_card_active(host->base);
    host->delay_ms(10);
}

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, sdmmchost_xfer_t *content)
{
    hpm_stat_t status;
    sdxc_adma_config_t *config_ptr = NULL;
    sdxc_adma_config_t dma_config;
    if (content->data != NULL) {
        dma_config.dma_type = sdxc_dmasel_adma2;
        dma_config.adma_table_words = sizeof(host->adma2_desc) / sizeof(uint32_t);
        dma_config.adma_table = (uint32_t *) &host->adma2_desc;
        config_ptr = &dma_config;
     }
     status = sdxc_transfer_blocking(host->base, config_ptr, content);

    return status;
}
