/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_host.h"
#include "board.h"

#define HPM_SDMMC_HOST_TIMEOUT_DEFAULT (1000UL)

static void sdmmchost_power_control(const sdmmc_host_t *host, hpm_sdmmc_power_option_t option);


static void sdmmchost_switch_to_3v3_as_needed(const sdmmc_host_t *host);

static hpm_stat_t sdmmchost_check_host_availability(sdmmc_host_t *host);

static hpm_stat_t sdmmchost_check_host_availability(sdmmc_host_t *host)
{
    hpm_stat_t status = status_success;

    if (!sdmmchost_is_card_detected(host)) {
        status = status_sdmmc_no_sd_card_inserted;
        host->card_inserted = false;
        host->card_init_done = false;
    }

    return status;
}

void sdmmchost_delay_ms(const sdmmc_host_t *host, uint32_t ms)
{
    hpm_sdmmc_osal_delay((void *)host, ms);
}


static void sdmmchost_switch_to_3v3_as_needed(const sdmmc_host_t *host)
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

        SDXC_Type *base = host->host_param.base;

        sdxc_init(base, &sdxc_config);

        sdmmchost_wait_card_active(host);
        sdmmchost_delay_ms(host, 10);

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        host->xfer_done_or_error_event = hpm_sdmmc_osal_event_create(host);
        if (host->xfer_done_or_error_event == NULL) {
            break;
        }
        const uint32_t irq_mask =
        SDXC_INT_STAT_CMD_COMPLETE_MASK | SDXC_INT_STAT_XFER_COMPLETE_MASK | SDXC_INT_STAT_ERR_INTERRUPT_MASK;
        sdxc_enable_interrupt_signal(base, irq_mask, true);
#endif

        status = status_success;
    } while (false);

    return status;
}

void sdmmchost_vsel_pin_control(const sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt)
{
    if (sdmmchost_is_voltage_switch_supported(host)) {
        const hpm_sdmmc_pin_info_t *pin_info = &host->host_param.io_data.vsel_pin;
        if (pin_info->use_gpio) {
            bool polarity = pin_info->polarity;
            uint32_t gpio_index = pin_info->gpio_pin / 32;
            uint32_t pin_index = pin_info->gpio_pin % 32;
            volatile uint32_t *gpio_on = (polarity == false)
                                             ? &HPM_GPIO0->DO[gpio_index].SET
                                             : &HPM_GPIO0->DO[gpio_index].CLEAR;
            volatile uint32_t *gpio_off = (polarity == false)
                                              ? &HPM_GPIO0->DO[gpio_index].CLEAR
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
    sdmmchost_delay_ms(host, 50);

    /* 5. Provide SD clock the card again */
    sdxc_enable_inverse_clock(host->host_param.base, true);
    sdxc_enable_sd_clock(host->host_param.base, true);

    /* 6. wait 1ms */
    sdmmchost_delay_ms(host, 1);

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
    if (host != NULL) {

        sdxc_reset(host->host_param.base, sdxc_reset_cmd_line, 0xffffu);
        sdxc_reset(host->host_param.base, sdxc_reset_data_line, 0xffffu);

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        if (host->xfer_done_or_error_event != NULL) {
            hpm_sdmmc_osal_event_delete(host, host->xfer_done_or_error_event);
            host->xfer_done_or_error_event = NULL;
        }
#endif
    }
}

void sdmmchost_reset(const sdmmc_host_t *host)
{
    sdxc_reset(host->host_param.base, sdxc_reset_cmd_line, 0xffffu);

    sdxc_reset(host->host_param.base, sdxc_reset_data_line, 0xffffu);
}

void sdmmchost_enable_emmc_support(const sdmmc_host_t *host, bool enable)
{
    sdxc_enable_emmc_support(host->host_param.base, enable);
}

bool sdmmchost_is_card_detected(const sdmmc_host_t *host)
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

void sdmmchost_wait_card_active(const sdmmc_host_t *host)
{
    sdxc_wait_card_active(host->host_param.base);

    sdmmchost_delay_ms(host, 10);
}

static hpm_stat_t sdmmchost_wait_command_done(const sdmmc_host_t *host, uint32_t timeout_ms)
{
    hpm_stat_t status = status_success;
    SDMMCHOST_Type *base = host->host_param.base;
    const uint32_t event_to_wait = SDXC_INT_STAT_CMD_COMPLETE_MASK | SDXC_INT_STAT_ERR_INTERRUPT_MASK;
    do {
#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        status = hpm_sdmmc_osal_event_wait((void *) host, host->xfer_done_or_error_event, event_to_wait, timeout_ms);
        if (status != status_success) {
            break;
        }
#else
        bool has_done_or_error;
        bool timeout_occurred;
        uint64_t start_ticks = hpm_csr_get_core_mcycle();
        uint32_t ticks_per_ms = clock_get_core_clock_ticks_per_ms();
        uint64_t timeout_ticks = start_ticks + (timeout_ms * ticks_per_ms);

        do {
            uint32_t int_stat = sdxc_get_interrupt_status(base);
            has_done_or_error = IS_HPM_BITMASK_SET(int_stat, event_to_wait);
            timeout_occurred = (hpm_csr_get_core_mcycle() > timeout_ticks);
        } while ((!has_done_or_error) && (!timeout_occurred));

        if (timeout_occurred) {
            break;
        }
#endif
    } while (false);

    status = sdxc_parse_interrupt_status(base);
    sdxc_clear_interrupt_status(base, event_to_wait);

    return status;
}

static hpm_stat_t sdmmchost_wait_xfer_done(sdmmc_host_t *host, uint32_t timeout_ms)
{
    hpm_stat_t status = status_success;
    SDMMCHOST_Type *base = host->host_param.base;
    const uint32_t event_to_wait = SDXC_INT_STAT_XFER_COMPLETE_MASK | SDXC_INT_STAT_ERR_INTERRUPT_MASK;
    do {
#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        status = hpm_sdmmc_osal_event_wait(host, host->xfer_done_or_error_event, event_to_wait, timeout_ms);
        if (status != status_success) {
            break;
        }
#else
        bool has_done_or_error;
        bool timeout_occurred;
        uint64_t start_ticks = hpm_csr_get_core_mcycle();
        uint32_t ticks_per_ms = clock_get_core_clock_ticks_per_ms();
        uint64_t timeout_ticks = start_ticks + (timeout_ms * ticks_per_ms);

        do {
            uint32_t int_stat = sdxc_get_interrupt_status(base);
            has_done_or_error = IS_HPM_BITMASK_SET(int_stat, event_to_wait);
            timeout_occurred = (hpm_csr_get_core_mcycle() > timeout_ticks);
        } while ((!has_done_or_error) && (!timeout_occurred));

        if (timeout_occurred) {
            break;
        }
#endif
    } while (false);

    status = sdxc_parse_interrupt_status(base);
    sdxc_clear_interrupt_status(base, event_to_wait);

    return status;
}


static hpm_stat_t sdmmchost_wait_idle(const sdmmc_host_t *host)
{
    /* Wait a while until the BUS is idle after the previous command */
    uint32_t wait_cnt = 1000L;
    while (!sdxc_is_bus_idle(host->host_param.base) && (wait_cnt > 0U)) {
        wait_cnt--;
        sdmmchost_delay_ms(host, 1);
    }
    if (wait_cnt == 0) {
        return status_timeout;
    }
    return status_success;
}

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, const sdmmchost_cmd_t *cmd)
{
    hpm_stat_t status;
    SDXC_Type *base = NULL;
    do {
        status = sdmmchost_check_host_availability(host);
        if (status != status_success) {
            break;
        }

        status = sdmmchost_wait_idle(host);
        if (status != status_success) {
            break;
        }

        base = host->host_param.base;
        status = sdxc_send_command(base, cmd);
        if (status != status_success) {
            break;
        }

        uint32_t timeout_ms = (cmd->cmd_timeout_ms == 0) ? HPM_SDMMC_HOST_TIMEOUT_DEFAULT : cmd->cmd_timeout_ms;
        status = sdmmchost_wait_command_done(host, timeout_ms);
        if (status != status_success) {
            break;
        }
        if (cmd->resp_type == (sdxc_dev_resp_type_t) sdmmc_resp_r1b) {
            status = sdmmchost_wait_xfer_done(host, timeout_ms);
            if ((status != status_success) && (status != status_timeout)) {
                break;
            }
        }
        status = sdxc_receive_cmd_response(host->host_param.base, &host->cmd);
    } while (false);
    if (status != status_success) {
        sdmmchost_error_recovery(host, NULL);
    }

    return status;
}

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, const sdmmchost_xfer_t *content)
{
    hpm_stat_t status;

    do {
        status = sdmmchost_check_host_availability(host);
        if (status != status_success) {
            break;
        }

        status = sdmmchost_wait_idle(host);
        if (status != status_success) {
            break;
        }

        SDXC_Type *base = host->host_param.base;
        sdxc_adma_config_t dma_config = { 0 };
        uint32_t timeout_ms = HPM_SDMMC_HOST_TIMEOUT_DEFAULT;
        if (content->data != NULL) {
#if defined(HPM_SDMMC_USE_ADMA2) && (HPM_SDMMC_USE_ADMA2 == 1)
            dma_config.dma_type = sdxc_dmasel_adma2;
#else
            dma_config.dma_type = sdxc_dmasel_adma3;
#endif
             /* Ensure the ADMA descriptor starts at 8-byte aligned address */
            dma_config.adma_table = (uint32_t *)HPM_ALIGN_UP(&host->adma_table, HPM_SDMMC_HOST_ADMA3_ALIGN_SIZE);
            dma_config.adma_table_words = HPM_SDMMC_HOST_ADMA_TBL_SIZE;

            /***************************************************************************************************************
             *  Calculate the data timeout interval in millisecond =
             *  (block_count * block_size) / tx_rx_bytes_per_sec * 1000 + margin time
             *  Here set the margin time to 500 milliseconds
             **************************************************************************************************************/
            uint32_t bus_width = sdxc_get_data_bus_width(host->host_param.base);
            uint32_t tx_rx_bytes_per_sec = host->clock_freq * bus_width / 8;
            uint32_t block_cnt = content->data->block_cnt;
            uint32_t block_size = content->data->block_size;
            uint32_t read_write_size = block_cnt * block_size;
            timeout_ms = (uint32_t) (1.0f * read_write_size / tx_rx_bytes_per_sec) * 1000 + 500;
            sdxc_set_data_timeout(base, timeout_ms, NULL);
        }

        if (dma_config.dma_type == sdxc_dmasel_adma3) {
            sdxc_clear_interrupt_status(base, ~0U);
        }
        status = sdxc_transfer_nonblocking(base, &dma_config, content);
        if (status != status_success) {
            break;
        }

        if (dma_config.dma_type != sdxc_dmasel_adma3) {
            status = sdmmchost_wait_command_done(host, HPM_SDMMC_HOST_TIMEOUT_DEFAULT);
            if (status != status_success) {
                break;
            }
        }

        status = sdxc_parse_interrupt_status(base);
        if (status != status_success) {
            break;
        }

        if (content->data != NULL) {
            status = sdmmchost_wait_xfer_done(host, timeout_ms);
            if (status != status_success) {
                break;
            }
        }
        status = sdxc_receive_cmd_response(host->host_param.base, &host->cmd);
    } while (false);

    return status;
}

hpm_stat_t sdmmchost_start_transfer(sdmmc_host_t *host, const sdmmchost_xfer_t *content)
{
    hpm_stat_t status;

    do {
        status = sdmmchost_check_host_availability(host);
        if (status != status_success) {
            break;
        }

        status = sdmmchost_wait_idle(host);
        if (status != status_success) {
            break;
        }

        SDXC_Type *base = host->host_param.base;
        sdxc_adma_config_t dma_config = { 0 };
        uint32_t timeout_ms = HPM_SDMMC_HOST_TIMEOUT_DEFAULT;
        if (content->data != NULL) {
#if defined(HPM_SDMMC_USE_ADMA2) && (HPM_SDMMC_USE_ADMA2 == 1)
            dma_config.dma_type = sdxc_dmasel_adma2;
#else
            dma_config.dma_type = sdxc_dmasel_adma3;
#endif
             /* Ensure the ADMA descriptor starts at 8-byte aligned address */
            dma_config.adma_table = (uint32_t *)HPM_ALIGN_UP(&host->adma_table, HPM_SDMMC_HOST_ADMA3_ALIGN_SIZE);
            dma_config.adma_table_words = HPM_SDMMC_HOST_ADMA_TBL_SIZE;

            /***************************************************************************************************************
             *  Calculate the data timeout interval in millisecond =
             *  (block_count * block_size) / tx_rx_bytes_per_sec * 1000 + margin time
             *  Here set the margin time to 500 milliseconds
             **************************************************************************************************************/
            uint32_t bus_width = sdxc_get_data_bus_width(host->host_param.base);
            uint32_t tx_rx_bytes_per_sec = host->clock_freq * bus_width / 8;
            uint32_t block_cnt = content->data->block_cnt;
            uint32_t block_size = content->data->block_size;
            uint32_t read_write_size = block_cnt * block_size;
            timeout_ms = (uint32_t) (1.0f * read_write_size / tx_rx_bytes_per_sec) * 1000 + 500;
            sdxc_set_data_timeout(base, timeout_ms, NULL);
        }

        if (dma_config.dma_type == sdxc_dmasel_adma3) {
            sdxc_clear_interrupt_status(base, ~0U);
        }
        status = sdxc_transfer_nonblocking(base, &dma_config, content);
        if (status != status_success) {
            break;
        }
    } while (false);

    return status;
}

hpm_stat_t sdmmchost_set_speed_mode(const sdmmc_host_t *host, sdmmc_speed_mode_t speed_mode)
{
    if ((host == NULL) || (host->host_param.base == NULL)) {
        return status_invalid_argument;
    }

    sdxc_set_speed_mode(host->host_param.base, (sdxc_speed_mode_t) speed_mode);
    return status_success;
}

hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *cmd)
{
    return sdxc_error_recovery(host->host_param.base, cmd);
}

void sdmmchost_set_cardclk_delay_chain(const sdmmc_host_t *host)
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

bool sdmmchost_is_8bit_supported(const sdmmc_host_t *host)
{
    return IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_8BIT);
}

bool sdmmchost_is_voltage_switch_supported(const sdmmc_host_t *host)
{
    return IS_HPM_BITMASK_SET(host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH);
}

void sdmmchost_power_control(const sdmmc_host_t *host, hpm_sdmmc_power_option_t option)
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
            volatile uint32_t *gpio_reg_off = (pin_info->polarity == 0)
                                                  ? &HPM_GPIO0->DO[gpio_index].CLEAR
                                                  : &HPM_GPIO0->DO[gpio_index].SET;
            volatile uint32_t *gpio_reg_on = (pin_info->polarity == 0)
                                                 ? &HPM_GPIO0->DO[gpio_index].SET
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
    bool as_gpio;
    const sdmmc_io_init_apis_t *init_apis = &host->host_param.io_init_apis;
    SDMMCHOST_Type *base = host->host_param.base;
    uint32_t host_flags = host->host_param.host_flags;

    if (operation_mode == hpm_sdmmc_operation_mode_inactive) {
        uint32_t bus_width = host->bus_width;
        const hpm_sdmmc_pin_info_t *pin_info;
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

void sdmmchost_enable_enhanced_data_strobe(const sdmmc_host_t *host, bool enable)
{
    sdxc_enable_enhanced_strobe(host->host_param.base, enable);
}


void sdmmchost_set_data_strobe_delay(const sdmmc_host_t *host)
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

ATTR_RAMFUNC void sdmmchost_irq_handler(sdmmc_host_t *host)
{
    SDXC_Type *base = host->host_param.base;

    uint32_t int_stat = sdxc_get_interrupt_status(base);
    uint32_t int_signal_en = sdxc_get_interrupt_signal(base);
    if (((int_stat & SDXC_INT_STAT_CARD_INTERRUPT_MASK) != 0) &&
        ((int_signal_en & SDXC_INT_STAT_CARD_INTERRUPT_MASK) != 0)) {
        if (host->sdio_irq_handler != NULL) {
            host->sdio_irq_handler(host->sdio_irq_param);
        }
        sdmmchost_enable_sdio_interrupt(host, false);
    }

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
    const uint32_t xfer_done_or_err_int_mask = SDXC_INT_STAT_CMD_COMPLETE_MASK | SDXC_INT_STAT_XFER_COMPLETE_MASK | SDXC_INT_STAT_ERR_INTERRUPT_MASK;
    hpm_sdmmc_osal_event_clear(host, host->xfer_done_or_error_event, xfer_done_or_err_int_mask);
    if ((int_signal_en & xfer_done_or_err_int_mask) && (int_stat & xfer_done_or_err_int_mask)) {
        uint32_t event_flags = int_stat & xfer_done_or_err_int_mask;
        hpm_sdmmc_osal_event_set(host, host->xfer_done_or_error_event, event_flags);
#if !defined(HPM_SDMMC_XFER_CALLBACK_ENABLE) || (HPM_SDMMC_XFER_CALLBACK_ENABLE == 1)
        if (event_flags & SDXC_INT_STAT_XFER_COMPLETE_MASK) {
            if (host->xfer_complete_callback != NULL) {
                host->xfer_complete_callback(host->xfer_complete_param);
            }
        }
        if (event_flags & SDXC_INT_STAT_CMD_COMPLETE_MASK) {
            if (host->cmd_complete_callback != NULL) {
                host->cmd_complete_callback(host->cmd_complete_param);
            }
        }
#endif
        sdxc_clear_interrupt_status(base, event_flags);
    }
#endif
}

void sdmmchost_enable_sdio_interrupt(sdmmc_host_t *host, bool enable)
{
    if (host != NULL) {
        if (enable) {
            sdxc_enable_interrupt_signal(host->host_param.base, SDXC_INT_STAT_CARD_INTERRUPT_MASK, true);
            sdxc_enable_interrupt_status(host->host_param.base, SDXC_INT_STAT_CARD_INTERRUPT_MASK, true);
        } else {
            sdxc_enable_interrupt_status(host->host_param.base, SDXC_INT_STAT_CARD_INTERRUPT_MASK, false);
        }
    }
}

void sdmmchost_register_sdio_callback(sdmmc_host_t *host, void (*sdio_irq_callback)(void *param), void *param)
{
    if (host != NULL) {
        host->sdio_irq_handler = sdio_irq_callback;
        host->sdio_irq_param = param;
    }
}

uint32_t sdmmchost_get_data_pin_level(sdmmc_host_t *host)
{
    uint32_t data_pin_level = 0;
    if ((host != NULL) && (host->host_param.base != NULL)) {
        SDXC_Type *base = host->host_param.base;
        data_pin_level = sdxc_get_data3_0_level(base);
        if (sdmmchost_is_8bit_supported(host)) {
            data_pin_level |= (sdxc_get_data7_4_level(base) << 4);
        }
    }
    return data_pin_level;
}

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
#if !defined(HPM_SDMMC_XFER_CALLBACK_ENABLE) || (HPM_SDMMC_XFER_CALLBACK_ENABLE == 1)
void sdmmchost_register_xfer_complete_callback(sdmmc_host_t *host, void (*irq_callback)(void *param), void *param)
{
    if (host != NULL) {
        host->xfer_complete_callback = irq_callback;
        host->xfer_complete_param = param;
    }
}

void sdmmchost_register_cmd_complete_callback(sdmmc_host_t *host, void (*irq_callback)(void *param), void *param)
{
    if (host != NULL) {
        host->cmd_complete_callback = irq_callback;
        host->cmd_complete_param = param;
    }
}
#endif
#endif