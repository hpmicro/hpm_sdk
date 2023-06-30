/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
/* #include "hpm_clock_drv.h" */
#include "hpm_uart_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"

#define TEST_UART HPM_UART1
#define TEST_UART_IRQ IRQn_UART1
/* #define TEST_UART_CLK_NAME BOARD_APP_UART_CLK_NAME */


volatile bool receive_spec_count_data_complete;
volatile bool timeout_before_receive_spec_count_data;
volatile uint8_t receive_data_count;
volatile uint8_t receive_data_buff[UART_SOC_FIFO_SIZE];
volatile uint8_t receive_unexcept_data_buff[UART_SOC_FIFO_SIZE];

void uart_isr(void)
{
    if (uart_get_irq_id(TEST_UART) == uart_intr_id_rx_data_avail) {
        receive_data_count = UART_LSR_RFIFO_NUM_GET(TEST_UART->LSR);
        /* read data from rx fifo */
        for (uint8_t i = 0; i < receive_data_count; i++) {
            receive_data_buff[i] = uart_read_byte(TEST_UART);
        }
        receive_spec_count_data_complete = true;
        /* uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout); */
    }
    if (uart_get_irq_id(TEST_UART) == uart_intr_id_rx_timeout) {
        receive_data_count = UART_LSR_RFIFO_NUM_GET(TEST_UART->LSR);
        /* read data from rx fifo */
        for (uint8_t i = 0; i < receive_data_count; i++) {
            receive_unexcept_data_buff[i] = uart_read_byte(TEST_UART);
        }
        timeout_before_receive_spec_count_data = true;
        /* uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout); */
    }

}

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)


void uart_hardware_trig(void)
{
    trgm_output_update_source(HPM_TRGM, HPM_TRGM_OUTPUT_SRC_UART_TRIG0, HPM_TRGM_INPUT_SRC_VDD);
    board_delay_us(10);
    trgm_output_update_source(HPM_TRGM, HPM_TRGM_OUTPUT_SRC_UART_TRIG0, HPM_TRGM_INPUT_SRC_VSS);
}

int main(void)
{
    hpm_stat_t stat;
    board_init();
    /* board_init_uart(TEST_UART); */

    printf("uart tamagawa encoder example\n");

    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    config.baudrate = 2500000U;
    config.fifo_enable = true;
    /* config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME); */
    config.src_freq_in_hz = 25000000U;
    config.using_new_fifo_thr = true;
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
    trig_config.trig_clr_rxfifo = true;
    trig_config.trig_mode = true;
    uart_config_trig_mode(TEST_UART, &trig_config);

    /* board_delay_ms(10);
    uart_trigger_communication(TEST_UART); */

    while (1) {
        board_delay_ms(600);
        /* write data into txfifo and not sent out */
        uint8_t cmd = 0x2;
        uart_write_byte(TEST_UART, cmd);
        TEST_UART->MOTO_CFG |= UART_MOTO_CFG_HWTRG_EN_MASK;
        uart_hardware_trig();

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
