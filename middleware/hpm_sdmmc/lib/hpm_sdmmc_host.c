/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_host.h"
#include "board.h"


static hpm_stat_t sdmmchost_check_host_availablity(sdmmc_host_t *host);

static hpm_stat_t sdmmchost_check_host_availablity(sdmmc_host_t *host)
{
    hpm_stat_t  status = status_success;
    if (host->host_param.card_detection_mode != sdmmc_host_card_detection_none) {
        if (!sdmmchost_is_card_detected(host)) {
            status = status_sdmmc_no_sd_card_inserted;
            host->card_inserted = false;
            host->card_init_done = false;
        }
    }
    return status;
}

hpm_stat_t sdmmchost_init(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(host == NULL);

        /* Initialize clock for the identification stage */
        host->host_param.host_clk_freq = host->host_param.clock_init_func(host->host_param.base, SDMMC_CLOCK_400KHZ);
        host->host_param.io_init_func(host->host_param.base);

        sdxc_config_t sdxc_config;
        sdxc_config.data_timeout = 1000; /* Data timeout interval, configure to 1000 milliseconds by default */

        sdxc_init(host->host_param.base, &sdxc_config);

        sdxc_wait_card_active(host->host_param.base);
        host->host_param.delay_ms(10);

        status = status_success;

    } while (false);

    return status;
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

    /* 3. Switch signaling to 1 */
    board_sd_switch_pins_to_1v8(host->host_param.base);

    /* 4. delay 5ms */
    host->host_param.delay_ms(5);

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
    sdxc_enable_mmc_boot(host->host_param.base, enable);
}

bool sdmmchost_is_card_detected(sdmmc_host_t *host)
{
    bool result;
    if (host->host_param.card_detect != NULL) {
        result = host->host_param.card_detect(host->host_param.base);
    } else {
        result = sdxc_is_card_inserted(host->host_param.base);
    }

    return result;
}

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width)
{
    sdxc_set_data_bus_width(host->host_param.base, bus_width);
}

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq, bool clock_inverse)
{
    host->host_param.host_clk_freq = host->host_param.clock_init_func(host->host_param.base, freq);
    return host->host_param.host_clk_freq;
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

    if (cmd->resp_type == (sdxc_dev_resp_type_t)sdmmc_resp_r1b) {
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
        uint32_t tx_rx_bytes_per_sec = host->host_param.host_clk_freq * bus_width / 8;
        uint32_t block_cnt = content->data->block_cnt;
        uint32_t block_size = content->data->block_size;
        uint32_t read_write_size = block_cnt * block_size;
        uint32_t timeout_ms = (uint32_t)(1.0f * read_write_size / tx_rx_bytes_per_sec) * 1000 + 100;
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

    if ((content->data != NULL) || (content->command->resp_type == (sdxc_dev_resp_type_t)sdmmc_resp_r1b)) {
        delay_cnt = 100000000UL; /* Delay more than 1 second based on the Bus Frequency */
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

    sdxc_set_speed_mode(host->host_param.base, (sdxc_speed_mode_t)speed_mode);

    return status_success;
}

hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *abort_cmd)
{
    sdxc_error_recovery(host->host_param.base);

    return sdmmchost_send_command(host, abort_cmd);
}


hpm_stat_t sdmmchost_setup_card_detection(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((host == NULL) || (host->host_param.base == NULL));

        if (host->host_param.card_detection_mode == sdmmc_host_card_detection_none) {
            status = status_success;
        } else if (host->host_param.card_detection_mode == sdmmc_host_card_detection_via_sdxc) {
            uint32_t signal_mask = SDXC_INT_SIGNAL_EN_CARD_INSERTION_SIGNAL_EN_MASK | SDXC_INT_SIGNAL_EN_CARD_REMOVAL_SIGNAL_EN_MASK;
            sdxc_enable_interrupt_signal(host->host_param.base, signal_mask, true);
            intc_m_enable_irq_with_priority(host->host_param.card_detection_irq_index, 1);
            status = status_success;
        } else {
            GPIO_Type *gpio = host->host_param.card_detection_gpio;
            uint32_t gpio_index = host->host_param.card_detection_gpio_index;
            uint32_t pin_index = host->host_param.card_detection_pin_index;
            gpio_config_pin_interrupt(gpio, gpio_index, pin_index, gpio_interrupt_trigger_edge_falling);
            gpio_enable_pin_interrupt(gpio, gpio_index, pin_index);
            intc_m_enable_irq_with_priority(host->host_param.card_detection_irq_index, 1);
            status = status_success;
        }
    } while (false);

    return status;
}

hpm_stat_t  sdmmchost_handle_card_detection(sdmmc_host_t *host)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if (host->host_param.card_detection_mode == sdmmc_host_card_detection_none) {
            status = status_success;
        } else if (host->host_param.card_detection_mode == sdmmc_host_card_detection_via_sdxc) {
            uint32_t int_stat = sdxc_get_interrupt_status(host->host_param.base);
            if ((int_stat & SDXC_INT_STAT_CARD_INSERTION_MASK) != 0) {
                host->card_inserted = true;
                /* Clear interrupt */
                sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_CARD_INSERTION_MASK);
            }
            if ((int_stat & SDXC_INT_STAT_CARD_REMOVAL_MASK) != 0) {
                /* Clear interrupt */
                sdxc_clear_interrupt_status(host->host_param.base, SDXC_INT_STAT_CARD_REMOVAL_MASK);
                host->card_inserted = false;
                host->card_init_done = false;
                sdxc_select_voltage(host->host_param.base, sdxc_bus_voltage_sd_3v3);
                sdxc_enable_sd_clock(host->host_param.base, false);
                sdmmchost_reset(host);
            }
            status = status_success;
        } else {
            GPIO_Type  *gpio = host->host_param.card_detection_gpio;
            uint32_t gpio_index = host->host_param.card_detection_gpio_index;
            uint32_t pin_index = host->host_param.card_detection_pin_index;
            if (IS_HPM_BIT_SET(gpio->DI[gpio_index].VALUE, pin_index)) {
                host->card_inserted = true;
            } else {
                host->card_inserted = false;
                host->card_init_done = false;
                sdxc_select_voltage(host->host_param.base, sdxc_bus_voltage_sd_3v3);
                sdxc_enable_sd_clock(host->host_param.base, false);
                sdmmchost_reset(host);
            }
            /* Clear interrupt flag: W1C */
            gpio->IF[gpio_index].VALUE = HPM_BITSMASK(1, pin_index);
            status = status_success;
        }
    } while (false);

    return status;
}
