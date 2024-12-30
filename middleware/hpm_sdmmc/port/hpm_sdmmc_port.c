/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_sdmmc_host.h"
#include "board.h"


uint32_t sdmmc_get_sys_addr(const sdmmc_host_t *host, uint32_t addr)
{
    return core_local_mem_to_sys_address(host->host_param.hart_id, addr);
}

ATTR_WEAK hpm_stat_t board_init_sd_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base)
{
    sdmmc_host_param_t *param = &host->host_param;
    param->host_flags = 0;

    sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
    hpm_sdmmc_extra_io_data_t *io_data = &host->host_param.io_data;
    param->base = base;
    param->clock_init_func = board_sd_configure_clock;
    param->hart_id = BOARD_RUNNING_CORE;
    param->delay_ms = board_delay_ms;

    init_apis->cd_io_init = init_sdxc_cd_pin;
    init_apis->cmd_io_init = init_sdxc_cmd_pin;
    init_apis->clk_data_io_init = init_sdxc_clk_data_pins;

#if defined(BOARD_APP_SDCARD_SUPPORT_3V3) && (BOARD_APP_SDCARD_SUPPORT_3V3 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_3V3;
#endif
    bool support_1v8 = false;
    bool support_4bit = false;
    bool support_vsel = false;
    bool support_pwr = false;
    bool support_cd = false;
#if defined(BOARD_APP_SDCARD_SUPPORT_1V8) && (BOARD_APP_SDCARD_SUPPORT_1V8 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_1V8;
    support_1v8 = true;
#endif
#if defined(BOARD_APP_SDCARD_SUPPORT_4BIT) && (BOARD_APP_SDCARD_SUPPORT_4BIT == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_4BIT;
    support_4bit = true;
#endif
    if (support_1v8 && support_4bit) {
        param->host_flags |= HPM_SDMMC_HOST_SUPPORT_SDR50 | HPM_SDMMC_HOST_SUPPORT_SDR104;
        if (sdxc_is_ddr50_supported(base)) {
            param->host_flags |= HPM_SDMMC_HOST_SUPPORT_DDR;
        }
    }

#if defined(BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION) && (BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION;
    support_cd = true;
    init_apis->cd_io_init = init_sdxc_cd_pin;
#endif

#if defined(BOARD_APP_SDCARD_SUPPORT_POWER_SWITCH) && (BOARD_APP_SDCARD_SUPPORT_POWER_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH;
    support_pwr = true;
    init_apis->pwr_io_init = init_sdxc_pwr_pin;
#endif

#if defined(BOARD_APP_SDCARD_SUPPORT_VOLTAGE_SWITCH) && (BOARD_APP_SDCARD_SUPPORT_VOLTAGE_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH;
    support_vsel = true;
    init_apis->vsel_io_init = init_sdxc_vsel_pin;
#endif

    if (support_vsel) {
#if defined(BOARD_APP_SDCARD_VOLTAGE_SWITCH_USING_GPIO) && (BOARD_APP_SDCARD_VOLTAGE_SWITCH_USING_GPIO == 1)
        io_data->vsel_pin.use_gpio = true;
        io_data->vsel_pin.gpio_pin = BOARD_APP_SDCARD_VSEL_PIN;
#if defined(BOARD_APP_SDCARD_VOLTAGE_SWITCH_PIN_POL) && (BOARD_APP_SDCARD_VOLTAGE_SWITCH_PIN_POL == 1)
        io_data->vsel_pin.polarity = 1;
#else
        io_data->vsel_pin.polarity = 0;
#endif
#else
        io_data->vsel_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_VSEL_IN_IP;
#endif
    }
    if (support_cd) {
#if defined(BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO) && (BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO == 1)
        io_data->cd_pin.use_gpio = true;
        io_data->cd_pin.gpio_pin = BOARD_APP_SDCARD_CARD_DETECTION_PIN;
#if defined(BOARD_APP_SDCARD_CARD_DETECTION_PIN_POL) && (BOARD_APP_SDCARD_CARD_DETECTION_PIN_POL == 1)
        io_data->cd_pin.polarity = 1;
#else
        io_data->cd_pin.polarity = 0;
#endif
#else
        io_data->cd_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_CD_IN_IP;
#endif
    }

    if (support_pwr) {
#if defined(BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO) && (BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO == 1)
        io_data->pwr_pin.use_gpio = true;
        io_data->pwr_pin.gpio_pin = BOARD_APP_SDCARD_POWER_SWITCH_PIN;
#if defined(BOARD_APP_SDCARD_POWER_SWITCH_PIN_POL) && (BOARD_APP_SDCARD_POWER_SWITCH_PIN_POL == 1)
        io_data->pwr_pin.polarity = true;
#else
        io_data->pwr_pin.polarity = false;
#endif
#else
        io_data->pwr_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_PWR_IN_IP;
#endif
    }

    return status_success;
}

ATTR_WEAK hpm_stat_t board_init_emmc_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base)
{
    bool support_ds = false;
    sdmmc_host_param_t *param = &host->host_param;
    param->host_flags = 0;

    sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
    hpm_sdmmc_extra_io_data_t *io_data = &host->host_param.io_data;
    param->base = BOARD_APP_EMMC_SDXC_BASE;
    param->clock_init_func = board_sd_configure_clock;
    param->hart_id = BOARD_RUNNING_CORE;
    param->delay_ms = board_delay_ms;

    init_apis->cd_io_init = init_sdxc_cd_pin;
    init_apis->cmd_io_init = init_sdxc_cmd_pin;
    init_apis->clk_data_io_init = init_sdxc_clk_data_pins;
#if defined(BOARD_APP_EMMC_SUPPORT_DS) && (BOARD_APP_EMMC_SUPPORT_DS == 1)
    init_apis->ds_io_init = init_sdxc_ds_pin;
    support_ds = true;
#endif

#if defined(BOARD_APP_EMMC_SUPPORT_3V3) && (BOARD_APP_EMMC_SUPPORT_3V3 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_3V3;
#endif
    bool support_1v8 = false;
    bool support_4bit = false;
    bool support_8bit = false;
    bool support_vsel = false;
    bool support_pwr = false;
#if defined(BOARD_APP_EMMC_SUPPORT_1V8) && (BOARD_APP_EMMC_SUPPORT_1V8 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_1V8;
    support_1v8 = true;
#endif
#if defined(BOARD_APP_EMMC_SUPPORT_4BIT) && (BOARD_APP_EMMC_SUPPORT_4BIT == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_4BIT;
    support_4bit = true;
#endif
#if defined(BOARD_APP_EMMC_SUPPORT_8BIT) && (BOARD_APP_EMMC_SUPPORT_8BIT == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_4BIT | HPM_SDMMC_HOST_SUPPORT_8BIT;
    support_8bit = true;
    support_4bit = true;
#endif
    if (support_1v8 && support_4bit) {
        param->host_flags |= HPM_SDMMC_HOST_SUPPORT_HS200;
    }
    if (support_1v8 && support_8bit && support_ds) {
        param->host_flags |= HPM_SDMMC_HOST_SUPPORT_HS400;
    }
    if (sdxc_is_ddr50_supported(base)) {
        param->host_flags |= HPM_SDMMC_HOST_SUPPORT_DDR;
    }

#if defined(BOARD_APP_EMMC_SUPPORT_DS) && (BOARD_APP_EMMC_SUPPORT_DS == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_DATA_STROBE;
#endif

#if defined(BOARD_APP_EMMC_SUPPORT_POWER_SWITCH) && (BOARD_APP_EMMC_SUPPORT_POWER_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH;
    support_pwr = true;
    init_apis->pwr_io_init = init_sdxc_pwr_pin;
#endif

#if defined(BOARD_APP_EMMC_SUPPORT_VOLTAGE_SWITCH) && (BOARD_APP_EMMC_SUPPORT_VOLTAGE_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH;
    support_vsel = true;
    init_apis->vsel_io_init = init_sdxc_vsel_pin;
#endif

    if (support_vsel) {
#if defined(BOARD_APP_EMMC_VOLTAGE_SWITCH_USING_GPIO) && (BOARD_APP_EMMC_VOLTAGE_SWITCH_USING_GPIO == 1)
        io_data->vsel_pin.use_gpio = true;
        io_data->vsel_pin.gpio_pin = BOARD_APP_EMMC_VSEL_PIN;
#if defined(BOARD_APP_EMMC_VOLTAGE_SWITCH_PIN_POL) && (BOARD_APP_EMMC_VOLTAGE_SWITCH_PIN_POL == 1)
        io_data->vsel_pin.polarity = 1;
#else
        io_data->vsel_pin.polarity = 0;
#endif
#else
        io_data->vsel_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_VSEL_IN_IP;
#endif
    }
    if (support_pwr) {
#if defined(BOARD_APP_EMMC_POWER_SWITCH_USING_GPIO) && (BOARD_APP_EMMC_POWER_SWITCH_USING_GPIO == 1)
        io_data->pwr_pin.use_gpio = true;
        io_data->pwr_pin.gpio_pin = BOARD_APP_EMMC_POWER_SWITCH_PIN;
#if defined(BOARD_APP_EMMC_POWER_SWITCH_PIN_POL) && (BOARD_APP_EMMC_POWER_SWITCH_PIN_POL == 1)
        io_data->pwr_pin.polarity = true;
#else
        io_data->pwr_pin.polarity = false;
#endif
#else
        io_data->pwr_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_PWR_IN_IP;
#endif
    }
    return status_success;
}

#if !defined(BOARD_APP_SDIO_SUPPORT_3V3) && defined(BOARD_APP_SDCARD_SUPPORT_3V3)
#define BOARD_APP_SDIO_SUPPORT_3V3 BOARD_APP_SDCARD_SUPPORT_3V3
#endif

#if !defined(BOARD_APP_SDIO_SUPPORT_1V8) && defined(BOARD_APP_SDCARD_SUPPORT_1V8)
#define BOARD_APP_SDIO_SUPPORT_1V8 BOARD_APP_SDCARD_SUPPORT_1V8
#endif

#if !defined(BOARD_APP_SDIO_SUPPORT_4BIT) && defined(BOARD_APP_SDCARD_SUPPORT_4BIT)
#define BOARD_APP_SDIO_SUPPORT_4BIT BOARD_APP_SDCARD_SUPPORT_4BIT
#endif

#if !defined(BOARD_APP_SDIO_SUPPORT_CARD_DETECTION) && defined(BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION)
#define BOARD_APP_SDIO_SUPPORT_CARD_DETECTION BOARD_APP_SDCARD_SUPPORT_CARD_DETECTION
#endif

#if !defined(BOARD_APP_SDIO_SUPPORT_POWER_SWITCH) && defined(BOARD_APP_SDCARD_SUPPORT_POWER_SWITCH)
#define BOARD_APP_SDIO_SUPPORT_POWER_SWITCH BOARD_APP_SDCARD_SUPPORT_POWER_SWITCH
#endif

#if !defined(BOARD_APP_SDIO_SUPPORT_VOLTAGE_SWITCH) && defined(BOARD_APP_SDCARD_SUPPORT_VOLTAGE_SWITCH)
#define BOARD_APP_SDIO_SUPPORT_VOLTAGE_SWITCH BOARD_APP_SDCARD_SUPPORT_VOLTAGE_SWITCH
#endif

#if !defined(BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO) && defined(BOARD_APP_SDCARD_VOLTAGE_SWITCH_USING_GPIO)
#define BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO BOARD_APP_SDCARD_VOLTAGE_SWITCH_USING_GPIO
#endif

#if !defined(BOARD_APP_SDIO_VSEL_PIN) && defined(BOARD_APP_SDCARD_VSEL_PIN)
#define BOARD_APP_SDIO_VSEL_PIN BOARD_APP_SDCARD_VSEL_PIN
#endif

#if !defined(BOARD_APP_SDIO_VOLTAGE_SWITCH_PIN_POL) && defined(BOARD_APP_SDCARD_VOLTAGE_SWITCH_PIN_POL)
#define BOARD_APP_SDIO_VOLTAGE_SWITCH_PIN_POL BOARD_APP_SDCARD_VOLTAGE_SWITCH_PIN_POL
#endif

#if !defined(BOARD_APP_SDIO_CARD_DETECTION_USING_GPIO) && defined(BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO)
#define BOARD_APP_SDIO_CARD_DETECTION_USING_GPIO BOARD_APP_SDCARD_CARD_DETECTION_USING_GPIO
#endif

#if !defined(BOARD_APP_SDIO_CARD_DETECTION_PIN) && defined(BOARD_APP_SDCARD_CARD_DETECTION_PIN)
#define BOARD_APP_SDIO_CARD_DETECTION_PIN BOARD_APP_SDCARD_CARD_DETECTION_PIN
#endif

#if !defined(BOARD_APP_SDIO_CARD_DETECTION_PIN_POL) && defined(BOARD_APP_SDCARD_CARD_DETECTION_PIN_POL)
#define BOARD_APP_SDIO_CARD_DETECTION_PIN_POL BOARD_APP_SDCARD_CARD_DETECTION_PIN_POL
#endif

#if !defined(BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO) && defined(BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO)
#define BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO BOARD_APP_SDCARD_POWER_SWITCH_USING_GPIO
#endif

#if !defined(BOARD_APP_SDIO_POWER_SWITCH_PIN_POL) && defined(BOARD_APP_SDCARD_POWER_SWITCH_PIN_POL)
#define BOARD_APP_SDIO_POWER_SWITCH_PIN_POL BOARD_APP_SDCARD_POWER_SWITCH_PIN_POL
#endif

#if !defined(BOARD_APP_SDIO_POWER_SWITCH_PIN) && defined(BOARD_APP_SDCARD_POWER_SWITCH_PIN)
#define BOARD_APP_SDIO_POWER_SWITCH_PIN BOARD_APP_SDCARD_POWER_SWITCH_PIN
#endif


ATTR_WEAK hpm_stat_t board_init_sdio_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base)
{
    sdmmc_host_param_t *param = &host->host_param;
    param->host_flags = 0;

    sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
    hpm_sdmmc_extra_io_data_t *io_data = &host->host_param.io_data;
    param->base = base;
    param->clock_init_func = board_sd_configure_clock;
    param->hart_id = BOARD_RUNNING_CORE;
    param->delay_ms = board_delay_ms;

    init_apis->cd_io_init = init_sdxc_cd_pin;
    init_apis->cmd_io_init = init_sdxc_cmd_pin;
    init_apis->clk_data_io_init = init_sdxc_clk_data_pins;

#if defined(BOARD_APP_SDIO_SUPPORT_3V3) && (BOARD_APP_SDIO_SUPPORT_3V3 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_3V3;
#endif
    bool support_1v8 = false;
    bool support_4bit = false;
    bool support_vsel = false;
    bool support_pwr = false;
    bool support_cd = false;
#if defined(BOARD_APP_SDIO_SUPPORT_1V8) && (BOARD_APP_SDIO_SUPPORT_1V8 == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_1V8;
    support_1v8 = true;
#endif
#if defined(BOARD_APP_SDIO_SUPPORT_4BIT) && (BOARD_APP_SDIO_SUPPORT_4BIT == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_4BIT;
    support_4bit = true;
#endif
    if (support_1v8 && support_4bit) {
        param->host_flags |= HPM_SDMMC_HOST_SUPPORT_SDR50 | HPM_SDMMC_HOST_SUPPORT_SDR104;
        if (sdxc_is_ddr50_supported(base)) {
            param->host_flags |= HPM_SDMMC_HOST_SUPPORT_DDR;
        }
    }

#if defined(BOARD_APP_SDIO_SUPPORT_CARD_DETECTION) && (BOARD_APP_SDIO_SUPPORT_CARD_DETECTION == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION;
    support_cd = true;
    init_apis->cd_io_init = init_sdxc_cd_pin;
#endif

#if defined(BOARD_APP_SDIO_SUPPORT_POWER_SWITCH) && (BOARD_APP_SDIO_SUPPORT_POWER_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH;
    support_pwr = true;
    init_apis->pwr_io_init = init_sdxc_pwr_pin;
#endif

#if defined(BOARD_APP_SDIO_SUPPORT_VOLTAGE_SWITCH) && (BOARD_APP_SDIO_SUPPORT_VOLTAGE_SWITCH == 1)
    param->host_flags |= HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH;
    support_vsel = true;
    init_apis->vsel_io_init = init_sdxc_vsel_pin;
#endif

    if (support_vsel) {
#if defined(BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO) && (BOARD_APP_SDIO_VOLTAGE_SWITCH_USING_GPIO == 1)
        io_data->vsel_pin.use_gpio = true;
        io_data->vsel_pin.gpio_pin = BOARD_APP_SDIO_VSEL_PIN;
#if defined(BOARD_APP_SDIO_VOLTAGE_SWITCH_PIN_POL) && (BOARD_APP_SDIO_VOLTAGE_SWITCH_PIN_POL == 1)
        io_data->vsel_pin.polarity = 1;
#else
        io_data->vsel_pin.polarity = 0;
#endif
#else
        io_data->vsel_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_VSEL_IN_IP;
#endif
    }
    if (support_cd) {
#if defined(BOARD_APP_SDIO_CARD_DETECTION_USING_GPIO) && (BOARD_APP_SDIO_CARD_DETECTION_USING_GPIO == 1)
        io_data->cd_pin.use_gpio = true;
        io_data->cd_pin.gpio_pin = BOARD_APP_SDIO_CARD_DETECTION_PIN;
#if defined(BOARD_APP_SDIO_CARD_DETECTION_PIN_POL) && (BOARD_APP_SDIO_CARD_DETECTION_PIN_POL == 1)
        io_data->cd_pin.polarity = 1;
#else
        io_data->cd_pin.polarity = 0;
#endif
#else
        io_data->cd_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_CD_IN_IP;
#endif
    }

    if (support_pwr) {
#if defined(BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO) && (BOARD_APP_SDIO_POWER_SWITCH_USING_GPIO == 1)
        io_data->pwr_pin.use_gpio = true;
        io_data->pwr_pin.gpio_pin = BOARD_APP_SDIO_POWER_SWITCH_PIN;
#if defined(BOARD_APP_SDIO_POWER_SWITCH_PIN_POL) && (BOARD_APP_SDIO_POWER_SWITCH_PIN_POL == 1)
        io_data->pwr_pin.polarity = true;
#else
        io_data->pwr_pin.polarity = false;
#endif
#else
        io_data->pwr_pin.use_gpio = false;
        param->host_flags |= HPM_SDMMC_HOST_PWR_IN_IP;
#endif
    }

    return status_success;
}
