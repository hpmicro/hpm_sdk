/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_host.h"
#include "board.h"


static void sdmmchost_vsel_pin_control(sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt);

static void sdmmchost_power_control(sdmmc_host_t *host, hpm_sdmmc_power_option_t option);


static void sdmmchost_switch_to_3v3_as_needed(sdmmc_host_t *host);

static hpm_stat_t sdmmchost_check_host_availablity(sdmmc_host_t *host);

static hpm_stat_t sdmmchost_check_host_availablity(sdmmc_host_t *host)
{
    hpm_stat_t status = status_success;

    if (!sdmmchost_is_card_detected(host)) {
        status = status_sdmmc_no_sd_card_inserted;
        host->card_inserted = false;
        host->card_init_done = false;
    }

    return status;
}

void sdmmchost_delay_ms(sdmmc_host_t *host, uint32_t ms)
{
    if (host->host_param.delay_ms != NULL) {
        host->host_param.delay_ms(ms);
    }
}


static void sdmmchost_switch_to_3v3_as_needed(sdmmc_host_t *host)
{
    if (IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_3V3)) {
        sdmmchost_vsel_pin_control(host, hpm_sdmmc_io_voltage_3v3);
    }
}

hpm_stat_t sdmmchost_init(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);

        /* Initialize clock for the identification stage */
        host->clock_freq = host->host_param.clock_init_func(host->host_param.base, SDMMC_CLOCK_400KHZ, true);
        host->operation_mode = hpm_sdmmc_operation_mode_inactive;
        sdmmchost_init_io(host, host->operation_mode);

        sdmmchost_power_control(host, hpm_sdmmc_power_off);
        sdmmchost_delay_ms(host, 36);
        sdmmchost_switch_to_3v3_as_needed(host);
        sdmmchost_delay_ms(host, 36);
        sdmmchost_power_control(host, hpm_sdmmc_power_on);

        sdxc_config_t sdxc_config;
        sdxc_config.data_timeout = 1000; /* Data timeout interval, configure to 1000 milliseconds by default */

        sdxc_init(host->host_param.base, &sdxc_config);

        sdmmchost_wait_card_active(host);
        sdmmchost_delay_ms(host, 10);

        status = status_success;

    } while (false);

    return status;
}

static void sdmmchost_vsel_pin_control(sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt)
{
    const hpm_sdmmc_pin_info_t *pin_info;
    if (sdmmchost_is_voltage_switch_supported(host)) {
        pin_info = &host->host_param.io_data.vsel_pin;
        if (pin_info->use_gpio) {
            bool polarity = pin_info->polarity;
            uint32_t gpio_index = pin_info->gpio_pin / 32;
            uint32_t pin_index = pin_info->gpio_pin % 32;
            volatile uint32_t *gpio_on = (polarity == false) ? &HPM_GPIO0->DO[gpio_index].SET
                                                             : &HPM_GPIO0->DO[gpio_index].CLEAR;
            volatile uint32_t *gpio_off = (polarity == false) ? &HPM_GPIO0->DO[gpio_index].CLEAR
                                                              : &HPM_GPIO0->DO[gpio_index].SET;

            HPM_GPIO0->OE[gpio_index].SET = (1UL << pin_index);
            if (io_volt == hpm_sdmmc_io_voltage_3v3) {
                *gpio_off = (1UL << pin_index);
            } else {
                *gpio_on = (1UL << pin_index);
            }
        }
    }
}

hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host)
{
    if (host == NULL) {
        return status_invalid_argument;
    }

    /* 1. Stop providing clock to the card */
    sdxc_enable_inverse_clock(host->host_param.base, false);
    sdxc_enable_sd_clock(host->host_param.base, false);

    /* 2. Wait until DAT[3:0] are 4'b0000 */
    uint32_t data3_0_level;
    uint32_t delay_cnt = 1000000UL;
    do {
        data3_0_level = sdxc_get_data3_0_level(host->host_param.base);
        --delay_cnt;
    } while ((data3_0_level != 0U) && (delay_cnt > 0U));
    if (delay_cnt < 1) {
        return status_timeout;
    }

    /* 3. Switch signaling to 1.8v */
    sdmmchost_vsel_pin_control(host, hpm_sdmmc_io_voltage_1v8);
    sdmmchost_select_voltage(host, hpm_sdmmc_io_voltage_1v8);

    /* 4. delay 5ms */
    host->host_param.delay_ms(50);

    /* 5. Provide SD clock the card again */
    sdxc_enable_inverse_clock(host->host_param.base, true);
    sdxc_enable_sd_clock(host->host_param.base, true);

    /* 6. wait 1ms */
    host->host_param.delay_ms(1);

    /* 7. Check DAT[3:0], make sure the value is 4'b0000 */
    delay_cnt = 1000000UL;
    do {
        data3_0_level = sdxc_get_data3_0_level(host->host_param.base);
        --delay_cnt;
    } while ((data3_0_level == 0U) && (delay_cnt > 0U));
    if (delay_cnt < 1) {
        return status_timeout;
    }

    return status_success;
}

void sdmmchost_deinit(sdmmc_host_t *host)
{
    sdxc_reset(host->host_param.base, sdxc_reset_cmd_line, 0xffffu);
    sdxc_reset(host->host_param.base, sdxc_reset_data_line, 0xffffu);
}

void sdmmchost_reset(sdmmc_host_t *host)
{
    sdxc_reset(host->host_param.base, sdxc_reset_cmd_line, 0xffffu);

    sdxc_reset(host->host_param.base, sdxc_reset_data_line, 0xffffu);
}

void sdmmchost_enable_emmc_support(sdmmc_host_t *host, bool enable)
{
    sdxc_enable_emmc_support(host->host_param.base, enable);
}

bool sdmmchost_is_card_detected(sdmmc_host_t *host)
{
    bool result = true;
    if (IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION)) {
        const hpm_sdmmc_pin_info_t *pin_info = &host->host_param.io_data.cd_pin;
        if (!pin_info->use_gpio) {
            result = sdxc_is_card_inserted(host->host_param.base);
        } else {
            uint32_t gpio_index = pin_info->gpio_pin / 32;
            uint32_t pin_index = pin_info->gpio_pin % 32;
            uint32_t pin_val = HPM_GPIO0->DI[gpio_index].VALUE & (1UL << pin_index);

            result = (pin_info->polarity == 0) ? (pin_val != 0) : (pin_val == 0);
        }
    }
    return result;
}

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width)
{
    host->bus_width = bus_width;
    sdmmchost_init_io(host, hpm_sdmmc_operation_mode_transfer);
    sdxc_set_data_bus_width(host->host_param.base, (sdxc_bus_width_t) bus_width);
}

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq, bool clock_inverse)
{
    uint32_t clk_freq = host->host_param.clock_init_func(host->host_param.base, freq, clock_inverse);
    host->clock_freq = clk_freq;
    return clk_freq;
}

void sdmmchost_wait_card_active(sdmmc_host_t *host)
{
    sdxc_wait_card_active(host->host_param.base);
    host->host_param.delay_ms(10);
}

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, sdmmchost_cmd_t *cmd)
{
    hpm_stat_t status;

    status = sdmmchost_check_host_availablity(host);
    if (status != status_success) {
        return status;
    }

    status = sdxc_send_command(host->host_param.base, cmd);
    if (status != status_success) {
        return status;
    }

    int64_t delay_cnt = 1000000;
    uint32_t int_stat;
    bool has_done_or_error = false;
    do {
        int_stat = sdxc_get_interrupt_status(host->host_param.base);
        if (!IS_HPM_BITMASK_SET(int_stat, SDXC_INT_STAT_CMD_COMPLETE_MASK)) {
            delay_cnt--;

        } else {
            has_done_or_error = true;
        }

        status = sdxc_parse_interrupt_status(host->host_param.base);
        if (status != status_success) {
            has_done_or_error = true;
        }

    } while ((!has_done_or_error) && (delay_cnt > 0));

    if ((delay_cnt <= 0) && (!has_done_or_error)) {
        status = status_timeout;
        return status;
    }
    if (status != status_success) {
        return status;
    }
    status = sdxc_receive_cmd_response(host->host_param.base, &host->cmd);
    sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_CMD_COMPLETE_MASK);

    if (cmd->resp_type == (sdxc_dev_resp_type_t) sdmmc_resp_r1b) {
        uint32_t delay_ms = (cmd->cmd_timeout_ms == 0) ? 100 : cmd->cmd_timeout_ms;
        delay_cnt = 10 * 1000 * delay_ms;
        has_done_or_error = false;
        do {
            int_stat = sdxc_get_interrupt_status(host->host_param.base);
            if (!IS_HPM_BITMASK_SET(int_stat, SDXC_INT_STAT_XFER_COMPLETE_MASK)) {
                delay_cnt--;
            } else {
                has_done_or_error = true;
            }
            status = sdxc_parse_interrupt_status(host->host_param.base);
            if (status != status_success) {
                has_done_or_error = true;
            }
        } while ((!has_done_or_error) && (delay_cnt > 0));

        if ((delay_cnt <= 0) && (!has_done_or_error)) {
            status = status_timeout;
        }

        sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_XFER_COMPLETE_MASK);
    }

    return status;
}

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, sdmmchost_xfer_t *content)
{
    hpm_stat_t status;

    status = sdmmchost_check_host_availablity(host);
    if (status != status_success) {
        return status;
    }

    sdxc_adma_config_t *config_ptr = NULL;
    sdxc_adma_config_t dma_config;

    if (content->data != NULL) {
        dma_config.dma_type = sdxc_dmasel_adma2;
        dma_config.adma_table_words = sizeof(host->adma2_desc) / sizeof(uint32_t);
        dma_config.adma_table = (uint32_t *) &host->adma2_desc;
        config_ptr = &dma_config;

        /***************************************************************************************************************
         *  Calculate the data timeout interval in millisecond =
         *  (block_count * block_size) / tx_rx_bytes_per_sec * 1000 + margin time
         *  Here set the margin time to 100 milliseconds
         **************************************************************************************************************/
        uint32_t bus_width = sdxc_get_data_bus_width(host->host_param.base);
        uint32_t tx_rx_bytes_per_sec = host->clock_freq * bus_width / 8;
        uint32_t block_cnt = content->data->block_cnt;
        uint32_t block_size = content->data->block_size;
        uint32_t read_write_size = block_cnt * block_size;
        uint32_t timeout_ms = (uint32_t) (1.0f * read_write_size / tx_rx_bytes_per_sec) * 1000 + 100;
        sdxc_set_data_timeout(host->host_param.base, timeout_ms, NULL);
    }
    status = sdxc_transfer_nonblocking(host->host_param.base, config_ptr, content);

    int32_t delay_cnt = 1000000U;
    uint32_t int_stat;
    bool has_done_or_error = false;
    do {
        int_stat = sdxc_get_interrupt_status(host->host_param.base);
        if (!IS_HPM_BITMASK_SET(int_stat, SDXC_INT_STAT_CMD_COMPLETE_MASK)) {
            delay_cnt--;
        } else {
            has_done_or_error = true;
        }

        status = sdxc_parse_interrupt_status(host->host_param.base);
        if (status != status_success) {
            has_done_or_error = true;
        }
    } while ((!has_done_or_error) && (delay_cnt > 0));

    if ((delay_cnt <= 0) && (!has_done_or_error)) {
        status = status_timeout;
        return status;
    }
    if (status != status_success) {
        return status;
    }
    status = sdxc_receive_cmd_response(host->host_param.base, &host->cmd);

    sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_CMD_COMPLETE_MASK);

    if ((content->data != NULL) || (content->command->resp_type == (sdxc_dev_resp_type_t) sdmmc_resp_r1b)) {
        delay_cnt = 10000000UL; /* Delay more than 1 second based on the Bus Frequency */
        uint32_t xfer_done_or_error_mask = SDXC_INT_STAT_XFER_COMPLETE_MASK | SDXC_STS_ERROR;
        bool has_done_or_error = false;
        do {
            int_stat = sdxc_get_interrupt_status(host->host_param.base);
            if (!IS_HPM_BITMASK_SET(int_stat, xfer_done_or_error_mask)) {
                delay_cnt--;
            } else {
                has_done_or_error = true;
            }
            status = sdxc_parse_interrupt_status(host->host_param.base);
            if (status != status_success) {
                has_done_or_error = true;
            }
        } while ((delay_cnt > 0) && (!has_done_or_error));

        if (delay_cnt <= 0) {
            status = status_sdxc_data_timeout_error;
        }

        sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_XFER_COMPLETE_MASK);
    }

    return status;
}

hpm_stat_t sdmmchost_set_speed_mode(sdmmc_host_t *host, sdmmc_speed_mode_t speed_mode)
{
    if ((host == NULL) || (host->host_param.base == NULL)) {
        return status_invalid_argument;
    }

    sdxc_set_speed_mode(host->host_param.base, (sdxc_speed_mode_t) speed_mode);
    return status_success;
}

hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *abort_cmd)
{
    sdxc_error_recovery(host->host_param.base);
    return sdmmchost_send_command(host, abort_cmd);
}

void sdmmchost_set_cardclk_delay_chain(sdmmc_host_t *host)
{
    SDXC_Type *base = host->host_param.base;
    bool need_inverse = sdxc_is_inverse_clock_enabled(base);
    sdxc_enable_inverse_clock(base, false);
    sdxc_enable_sd_clock(base, false);
    uint32_t num_delaycells = sdxc_get_default_cardclk_delay_chain(base, host->clock_freq);
    sdxc_set_cardclk_delay_chain(host->host_param.base, num_delaycells);
    sdxc_enable_inverse_clock(base, need_inverse);
    sdxc_enable_sd_clock(base, true);
}

bool sdmmchost_is_8bit_supported(sdmmc_host_t *host)
{
    return IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_8BIT);
}

bool sdmmchost_is_voltage_switch_supported(sdmmc_host_t *host)
{
    return IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH);
}

void sdmmchost_power_control(sdmmc_host_t *host, hpm_sdmmc_power_option_t option)
{
    if (IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH)) {
        const hpm_sdmmc_pin_info_t *pin_info = &host->host_param.io_data.pwr_pin;
        const sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
        bool use_gpio = pin_info->use_gpio;
        SDMMCHOST_Type *base = host->host_param.base;
        init_apis->pwr_io_init(base, use_gpio);
        if (pin_info->use_gpio) {
            uint32_t gpio_index = pin_info->gpio_pin / 32;
            uint32_t pin_index = pin_info->gpio_pin % 32;
            volatile uint32_t *gpio_reg_off = (pin_info->polarity == 0) ? &HPM_GPIO0->DO[gpio_index].CLEAR
                                                                        : &HPM_GPIO0->DO[gpio_index].SET;
            volatile uint32_t *gpio_reg_on = (pin_info->polarity == 0) ? &HPM_GPIO0->DO[gpio_index].SET
                                                                       : &HPM_GPIO0->DO[gpio_index].CLEAR;
            HPM_GPIO0->OE[gpio_index].SET = (1UL << pin_index);
            if (option == hpm_sdmmc_power_off) {
                *gpio_reg_off = (1UL << pin_index);
            } else if (option == hpm_sdmmc_power_on) {
                *gpio_reg_on = (1UL << pin_index);
            } else {
                *gpio_reg_off = (1UL << pin_index);
                host->host_param.delay_ms(1);
                *gpio_reg_on = (1UL << pin_index);
            }
        } else {
            if (option == hpm_sdmmc_power_off) {
                sdxc_enable_power(host->host_param.base, false);
            } else if (option == hpm_sdmmc_power_on) {
                sdxc_enable_power(host->host_param.base, true);
            } else {
                sdxc_enable_power(host->host_param.base, false);
                host->host_param.delay_ms(1);
                sdxc_enable_power(host->host_param.base, true);
            }
        }
        host->host_param.delay_ms(1);
    }
}

void sdmmchost_init_io(sdmmc_host_t *host, hpm_sdmmc_operation_mode_t operation_mode)
{
    const hpm_sdmmc_pin_info_t *pin_info;
    bool as_gpio;
    const sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
    SDMMCHOST_Type *base = host->host_param.base;
    uint32_t host_flags = host->host_param.host_flags;

    if (operation_mode == hpm_sdmmc_operation_mode_inactive) {
        uint32_t bus_width = host->bus_width;
        bool support_3v3 = IS_HPM_BITMASK_SET(host_flags, HPM_SDMMC_HOST_SUPPORT_3V3);
        host->io_voltage = support_3v3 ? hpm_sdmmc_io_voltage_3v3 : hpm_sdmmc_io_voltage_1v8;
        /* Initialize IO */

        bool support_pwr = IS_HPM_BITMASK_SET(host_flags, HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH);
        if (support_pwr) {
            pin_info = &host->host_param.io_data.pwr_pin;
            as_gpio = pin_info->use_gpio;
            if (init_apis->pwr_io_init != NULL) {
                init_apis->pwr_io_init(base, as_gpio);
            }
        }

        bool is_1v8 = host->io_voltage == hpm_sdmmc_io_voltage_3v3 ? false : true;
        init_apis->cmd_io_init(base, true, is_1v8); /* Configure CMD pin to open-drain mode */
        init_apis->clk_data_io_init(base, bus_width, is_1v8);

        bool support_cd = IS_HPM_BITMASK_SET(host_flags, HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION);
        if (support_cd) {
            pin_info = &host->host_param.io_data.cd_pin;
            as_gpio = pin_info->use_gpio;
            if (init_apis->cd_io_init != NULL) {
                init_apis->cd_io_init(base, as_gpio);
            }
        }
        bool support_vsel = sdmmchost_is_voltage_switch_supported(host);
        if (support_vsel) {
            pin_info = &host->host_param.io_data.vsel_pin;
            as_gpio = pin_info->use_gpio;
            if (init_apis->vsel_io_init != NULL) {
                init_apis->vsel_io_init(base, as_gpio);
            }
            /* Since eMMC device doesn't support dynamic voltage switch, so set the voltage at initialization phase */
            if (!support_3v3) {
                sdmmchost_vsel_pin_control(host, hpm_sdmmc_io_voltage_1v8);
                sdmmchost_select_voltage(host, hpm_sdmmc_io_voltage_1v8);
            }
        }
    } else if (operation_mode == hpm_sdmmc_operation_mode_identification) {
        bool is_1v8 = host->io_voltage == hpm_sdmmc_io_voltage_3v3 ? false : true;
        bool is_opendrain = true;
        init_apis->cmd_io_init(base, is_opendrain, is_1v8);
        uint32_t bus_width = 1;
        switch (host->bus_width) {
        case sdmmc_bus_width_4bit:
            bus_width = 4;
            break;
        case sdmmc_bus_width_8bit:
            bus_width = 8;
            break;
        default:
            bus_width = 1;
            break;
        }
        init_apis->clk_data_io_init(base, bus_width, is_1v8);
        bool support_ds = IS_HPM_BITMASK_SET(host_flags, HPM_SDMMC_HOST_SUPPORT_HS400);
        if (support_ds) {
            init_apis->ds_io_init(base);
        }
    } else if (operation_mode == hpm_sdmmc_operation_mode_transfer) {
        bool is_1v8 = host->io_voltage == hpm_sdmmc_io_voltage_3v3 ? false : true;
        bool is_opendrain = false;
        init_apis->cmd_io_init(base, is_opendrain, is_1v8);
        uint32_t bus_width = 1;
        switch (host->bus_width) {
        case sdmmc_bus_width_4bit:
            bus_width = 4;
            break;
        case sdmmc_bus_width_8bit:
            bus_width = 8;
            break;
        default:
            bus_width = 1;
            break;
        }
        init_apis->clk_data_io_init(base, bus_width, is_1v8);
        bool support_ds = IS_HPM_BITMASK_SET(host_flags, HPM_SDMMC_HOST_SUPPORT_HS400);
        if (support_ds) {
            init_apis->ds_io_init(base);
        }
    } else if (operation_mode == hpm_sdmmc_operation_mode_interrupt) {
        bool is_1v8 = host->io_voltage == hpm_sdmmc_io_voltage_3v3 ? false : true;
        bool is_opendrain = true;
        init_apis->cmd_io_init(base, is_opendrain, is_1v8);
    } else {
        /* DO nothing */
    }
    /* Wait a while in case the IO initialization requires small delays to get effective */
    for (volatile uint32_t delay_cnt = 100; delay_cnt > 0; delay_cnt--) {
        NOP();
    }
}

void sdmmchost_enable_enhanced_data_strobe(sdmmc_host_t *host, bool enable)
{
    sdxc_enable_enhanced_strobe(host->host_param.base, enable);
}


void sdmmchost_set_data_strobe_delay(sdmmc_host_t *host)
{
    uint32_t num_delaycells = sdxc_get_default_strobe_delay(host->host_param.base);
    sdxc_set_data_strobe_delay(host->host_param.base, num_delaycells);
}

void sdmmchost_select_voltage(sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt)
{
    sdxc_bus_voltage_option_t vsel = sdxc_bus_voltage_sd_3v3;
    if (io_volt == hpm_sdmmc_io_voltage_1v8) {
        vsel = sdxc_bus_voltage_sd_1v8;
        host->io_voltage = hpm_sdmmc_io_voltage_1v8;
    } else {
        host->io_voltage = hpm_sdmmc_io_voltage_3v3;
    }
    sdxc_select_voltage(host->host_param.base, vsel);

}
