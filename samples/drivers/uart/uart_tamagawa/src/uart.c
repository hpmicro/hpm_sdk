/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_TRGM
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#endif

#define TEST_UART          BOARD_APP_UART_BASE
#define TEST_UART_IRQ      BOARD_APP_UART_IRQ
#define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME
#define TEST_UART_TRIG     BOARD_APP_UART_TRIG

volatile bool receive_spec_count_data_complete;
volatile bool timeout_before_receive_spec_count_data;
volatile uint8_t receive_data_count;
volatile uint8_t receive_data_buff[UART_SOC_FIFO_SIZE];
volatile uint8_t receive_unexcept_data_buff[UART_SOC_FIFO_SIZE];

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    if (uart_get_irq_id(TEST_UART) == uart_intr_id_rx_data_avail) {
        receive_data_count = uart_get_data_count_in_rx_fifo(TEST_UART);
        /* read data from rx fifo */
        for (uint8_t i = 0; i < receive_data_count; i++) {
            receive_data_buff[i] = uart_read_byte(TEST_UART);
        }
        receive_spec_count_data_complete = true;
        /* uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout); */
    }
    if (uart_get_irq_id(TEST_UART) == uart_intr_id_rx_timeout) {
        receive_data_count = uart_get_data_count_in_rx_fifo(TEST_UART);
        /* read data from rx fifo */
        for (uint8_t i = 0; i < receive_data_count; i++) {
            receive_unexcept_data_buff[i] = uart_read_byte(TEST_UART);
        }
        timeout_before_receive_spec_count_data = true;
        /* uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout); */
    }

}

#ifdef HPMSOC_HAS_HPMSDK_TRGM
void uart_hardware_trig(void)
{
    /* config gptmr to generate sync signal, period 2s */
    gptmr_channel_config_t config;
    clock_add_to_group(BOARD_UART_TRGM_GPTMR_CLK, (BOARD_RUNNING_CORE & 0x1));
    gptmr_channel_get_default_config(BOARD_UART_TRGM_GPTMR, &config);
    uint32_t freq = clock_get_frequency(BOARD_UART_TRGM_GPTMR_CLK);
    config.reload = 2 * freq; /* 2s*/
    config.cmp[0] = 1 * freq; /* 1s */
    config.cmp[1] = config.reload;
    config.cmp_initial_polarity_high = false;
    gptmr_channel_config(BOARD_UART_TRGM_GPTMR, BOARD_UART_TRGM_GPTMR_CH, &config, false);
    gptmr_start_counter(BOARD_UART_TRGM_GPTMR, BOARD_UART_TRGM_GPTMR_CH);

    trgm_output_t trgm_config = {0};
    trgm_config.invert = false;
    trgm_config.type = trgm_output_pulse_at_input_rising_edge;
    trgm_config.input = BOARD_UART_TRGM_GPTMR_INPUT;
    trgm_output_config(BOARD_UART_TRGM, TEST_UART_TRIG, &trgm_config);
}
#endif

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_uart(TEST_UART);

    printf("uart tamagawa encoder example\n");

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.baudrate = 2500000U;
    config.fifo_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.rx_fifo_level = uart_fifo_6_bytes;
    config.tx_fifo_level = uart_fifo_16_bytes;

    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        /* uart failed to be initialized */
        printf("failed to initialize uart\n");
        while (1) {
        }
    }

    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 1);

    uart_trig_config_t trig_config = {0};
    trig_config.en_stop_bit_insert = false;
    trig_config.trig_clr_rxfifo = true;    /* clear rx fifo after being triggered */
    trig_config.trig_mode = true;          /* start transmission after being triggered */
    trig_config.hardware_trig = true;      /* enable hardware trigger */
    uart_config_transfer_trig_mode(TEST_UART, &trig_config);

    while (1) {
        board_delay_ms(200);
        /* write data into txfifo but not sent out until sync siganl arrive */
        uint8_t cmd = 0x2;
        uart_write_byte(TEST_UART, cmd);
#ifdef HPMSOC_HAS_HPMSDK_TRGM
        static bool hardware_trig_config = false;
        if (!hardware_trig_config) {
            uart_hardware_trig(); /* generate a hardware trigger signal */
            hardware_trig_config = true;
        }
#else
        uart_software_trig_transfer(TEST_UART); /*disable hardware trig and triggered by sofware */
#endif

        while ((!receive_spec_count_data_complete) && (!timeout_before_receive_spec_count_data)) {
        }

        printf("uart tamagawa transfer complete.\n");
        printf("uart sent cmd: 0x%02x\n", cmd);
        if (receive_spec_count_data_complete) {
            printf("uart receive excepted count data: ");
            for (uint8_t i = 0; i < receive_data_count; i++) {
                printf("0x%02x ", receive_data_buff[i]);
            }
            receive_spec_count_data_complete = false;
        } else if (timeout_before_receive_spec_count_data) {
            printf("uart receive unexcepted count data: ");
            for (uint8_t i = 0; i < receive_data_count; i++) {
                printf("0x%02x ", receive_unexcept_data_buff[i]);
            }
            timeout_before_receive_spec_count_data = false;
        }
        printf("\n");
    }

    return 0;
}
