/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_trgm_soc_drv.h"
#include "hpm_trgm_drv.h"

#define TEST_UART BOARD_FREEMASTER_UART_BASE
#define TEST_UART_IRQ BOARD_FREEMASTER_UART_IRQ
#define TEST_UART_CLK_NAME BOARD_FREEMASTER_UART_CLK_NAME

volatile bool receive_spec_count_data_complete;
volatile bool timeout_before_receive_spec_count_data;
volatile uint8_t receive_data_count;
volatile uint8_t receive_data_buff[UART_SOC_FIFO_SIZE];
volatile uint8_t receive_unexcept_data_buff[UART_SOC_FIFO_SIZE];

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
SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)

void uart_hardware_trig(void)
{
    /* UART0~3 use HPM_TRGM0_OUTPUT_SRC_UART_TRIG0, UART4~7 sue HPM_TRGM0_OUTPUT_SRC_UART_TRIG1 */
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_UART_TRIG0, HPM_TRGM0_INPUT_SRC_VDD);
    trgm_output_update_source(HPM_TRGM0, HPM_TRGM0_OUTPUT_SRC_UART_TRIG0, HPM_TRGM0_INPUT_SRC_VSS);
}

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
        /* write data into txfifo and not sent out */
        uint8_t cmd = 0x2;
        uart_write_byte(TEST_UART, cmd);
        uart_hardware_trig(); /* generate a hardware trigger signal */

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
