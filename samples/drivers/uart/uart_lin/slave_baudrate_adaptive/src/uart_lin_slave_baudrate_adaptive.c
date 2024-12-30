/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_uart_lin.h"
#include "hpm_lin_clock_detection.h"

/* PLB section */
#define PLB_TRGM_IN_SRC BOARD_UART_LIN_PLB_TRGM_IN_SRC
#define SIZE_LIN_CLK_LENGTH (240U)
#define LIN_MAX_FREQ (20000U)
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint32_t src_buffer;
uint32_t run_buffer[SIZE_LIN_CLK_LENGTH * 2];
plb_lin_clock_t lin_clock_cfg;

/* uart section */
#define TEST_UART                           BOARD_UART_LIN
#define TEST_UART_IRQ                       BOARD_UART_LIN_IRQ
#define TEST_UART_CLK_NAME                  BOARD_UART_LIN_CLK_NAME
#define TEST_SLAVE_RECEIVE_ID               (0x31) /* lin salve receive id*/
#define TEST_SLAVE_RECEIVE_DATA_LENGTH      (8U)
#define TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM (1)
#define TEST_SLAVE_SEND_ID                  (0x30) /* lin salve sent id*/
#define TEST_SLAVE_SEND_DATA_LENGTH         (8U)
#define TEST_SLAVE_SEND_ENHANCE_CHECKSUM    (1)
#define UART_LIN_FRANME_DATA_COUNT_MAX      (8U)
uint8_t lin_pid;
uint32_t uart_source_clk;
uint32_t lin_baudrate;
volatile bool lin_clk_detected_and_supported;
volatile bool uart_lin_pid_parity_error;   /* pid parity error  */
volatile bool uart_lin_receive_data_error; /* receive data: data count error or checksum error */
volatile bool uart_lin_receive_complete;   /* receive data complete */
volatile bool uart_lin_send_complete;      /* send data complete */
volatile bool uart_lin_nonresponse_pid;    /* nonresponse pid */
uint8_t receive_data[UART_LIN_FRANME_DATA_COUNT_MAX]; /* store the data to be sent */
uint8_t send_data[UART_LIN_FRANME_DATA_COUNT_MAX];    /* store the received data */

uart_lin_slave_config_t uart_lin_config = {
    .ptr = TEST_UART,
};

/* uart fifo control */
uart_fifo_ctrl_t fifo_ctrl_1 = {
    .tx_fifo_level = uart_tx_fifo_trg_not_full,
    .rx_fifo_level = uart_rx_fifo_trg_not_empty,
    .dma_enable = false,
    .fifo_enable = true,
    .reset_tx_fifo = false,
    .reset_rx_fifo = false,
};

/* uart fifo control */
uart_fifo_ctrl_t fifo_ctrl_2 = {
    .tx_fifo_level = uart_tx_fifo_trg_not_full,
    .rx_fifo_level = uart_rx_fifo_trg_gt_three_quarters,
    .dma_enable = false,
    .fifo_enable = true,
    .reset_tx_fifo = false,
    .reset_rx_fifo = false,
};

void uart_lin_respond_pid(UART_Type *ptr, uint8_t pid);

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
    volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);

    /* receive pid  */
    if (irq_id == uart_intr_id_rx_data_avail) {
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        lin_pid = uart_read_byte(TEST_UART);
        uart_lin_respond_pid(TEST_UART, lin_pid);
    }

    /* sent data complete */
    if (irq_id == uart_intr_id_tx_slot_avail) {
        uart_lin_send_complete = true;
        uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
    }

    /* receive data complete */
    if (irq_id == uart_intr_id_rx_timeout) {
        uart_enable_rx_function(TEST_UART, false);
        uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        if (uart_lin_success == hpm_uart_lin_slave_receive_data(&uart_lin_config)) {
            uart_lin_receive_complete = true;
        } else {
            uart_lin_receive_data_error = true;
        }
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_APP_HDMA_IRQ, dma_isr)
void dma_isr(void)
{
    plb_lin_dma_isr_function(&lin_clock_cfg);
}

/* plb callback */
void callback(uint32_t tick)
{
    lin_baudrate = tick;
    hpm_stat_t stat = uart_set_baudrate(TEST_UART, lin_baudrate, uart_source_clk);
    if (status_success == stat) {
        lin_clk_detected_and_supported = true;
    } else {
        printf("uart not supports baudrate: %d!!\n", lin_baudrate);
        return;
    }
    uart_reset_rx_fifo(TEST_UART); /* reset rx fifo */
    uart_enable_rx_function(TEST_UART, true); /* enable rx function */
    uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout); /* enable rx fifo interrupt */
}

void uart_lin_respond_pid(UART_Type *ptr, uint8_t pid)
{
    uint8_t id = pid & 0x3f;

    if (pid != hpm_uart_lin_calculate_protected_id(id)) {
        uart_enable_rx_function(TEST_UART, false); /* stop receive */
        uart_lin_pid_parity_error = true;
        return;
    }

    /* change rx fifo threshold, using rx time out interrupt to receive data */
    uart_config_fifo_ctrl(ptr, &fifo_ctrl_2);

    switch (id) {
    case TEST_SLAVE_RECEIVE_ID:
        uart_enable_irq(ptr, uart_intr_rx_data_avail_or_timeout); /* continue to receive data by rx timeout */
        uart_lin_config.pid = pid;
        uart_lin_config.data.buff = receive_data;
        uart_lin_config.data.length = TEST_SLAVE_RECEIVE_DATA_LENGTH;
        uart_lin_config.data.enhance_checksum = TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM;
        break;
    case TEST_SLAVE_SEND_ID:
        uart_enable_rx_function(TEST_UART, false); /* stop receive */
        uint8_t data[TEST_SLAVE_SEND_DATA_LENGTH] = {0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
        memcpy(send_data, data, TEST_SLAVE_SEND_DATA_LENGTH);
        uart_lin_config.pid = pid;
        uart_lin_config.data.buff = send_data;
        uart_lin_config.data.length = TEST_SLAVE_SEND_DATA_LENGTH;
        uart_lin_config.data.enhance_checksum = TEST_SLAVE_SEND_ENHANCE_CHECKSUM;
        hpm_uart_lin_slave_send_data(&uart_lin_config);
        uart_enable_irq(ptr, uart_intr_tx_slot_avail); /* enable sent fifo empty interrupt */
        break;
    default:
        uart_enable_rx_function(TEST_UART, false); /* stop receive */
        uart_lin_nonresponse_pid = true;
        return;
    }
}

int main(void)
{
    board_init();
    board_init_uart(TEST_UART);
    init_plb_lin_pins();
    printf("Test uart lin slave baudrate adaptive example\n");

    /* uart initialization */
    uart_config_t config = {0};
    uart_default_config(TEST_UART, &config);
    uart_source_clk = clock_get_frequency(TEST_UART_CLK_NAME);
    config.src_freq_in_hz = uart_source_clk;
    config.baudrate = 19200; /* an initialization baud rate */
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    config.rx_enable = false;
    if (status_success != uart_init(TEST_UART, &config)) {
        printf("failed to initialize uart\n");
    }
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 3);

    /* PLB detect lin baudrate uart initialization */
    lin_clock_cfg.lin_max_freq = LIN_MAX_FREQ;
    lin_clock_cfg.clock_error = 0.1; /* baudrate tolerance */
    lin_clock_cfg.freq = clock_get_frequency(BOARD_PLB_PWM_CLOCK_NAME);
    lin_clock_cfg.device.dma = BOARD_APP_HDMA;
    lin_clock_cfg.device.dma_chn = 4;
    lin_clock_cfg.device.dma_mux = BOARD_APP_DMAMUX;
    lin_clock_cfg.device.plb = HPM_PLB;
    lin_clock_cfg.device.plb_type_b_chn = plb_chn0;
    lin_clock_cfg.device.trgmux_pin = PLB_TRGM_IN_SRC;
    lin_clock_cfg.device.trgm = BOARD_APP_TRGM;
    lin_clock_cfg.device.trgm_dma_dst_index = 0;
    lin_clock_cfg.device.trgm_dma_src_index = 0;
    lin_clock_cfg.dma_buffer_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&src_buffer);
    lin_clock_cfg.dma_buffer = (uint32_t *)&src_buffer;
    lin_clock_cfg.run_buffer = run_buffer;
    lin_clock_cfg.run_buffer_len = SIZE_LIN_CLK_LENGTH * 2;
    lin_clock_cfg.filter_length = 50;
    lin_clock_cfg.call_back = callback;
    plb_lin_clock_detection_init(&lin_clock_cfg);
    intc_m_enable_irq_with_priority(BOARD_APP_HDMA_IRQ, 2);

    while (1) {
        /* waiting for plb to detect lin break and 0x55 signal */
        while (!lin_clk_detected_and_supported) {
        }

        /* waiting for uart to receive PID and process frame */
        while (!(uart_lin_receive_complete || uart_lin_send_complete || uart_lin_pid_parity_error || uart_lin_receive_data_error || uart_lin_nonresponse_pid)) {
        }
        lin_clk_detected_and_supported = false;
        uart_reset_rx_fifo(TEST_UART); /* reset rx fifo */
        uart_config_fifo_ctrl(TEST_UART, &fifo_ctrl_1); /* change rx fifo threshold */

        if (uart_lin_receive_complete) {
            uart_lin_receive_complete = false;
            printf("uart lin receive ID: 0x%x\n", uart_lin_config.pid & 0x3f);
            printf("uart receive %d data:\n", uart_lin_config.data.length);
            for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
                if (i < uart_lin_config.data.length - 1) {
                    printf("0x%x ", receive_data[i]);
                } else {
                    printf("0x%x\n", receive_data[i]);
                }
            }
        }

        if (uart_lin_send_complete) {
            uart_lin_send_complete = false;
            printf("uart lin receive ID: 0x%x\n", uart_lin_config.pid & 0x3f);
            printf("uart send %d data:\n", uart_lin_config.data.length);
            for (uint8_t i = 0; i < uart_lin_config.data.length; i++) {
                if (i < uart_lin_config.data.length - 1) {
                    printf("0x%x ", send_data[i]);
                } else {
                    printf("0x%x\n", send_data[i]);
                }
            }
        }

        if (uart_lin_pid_parity_error) {
            uart_lin_pid_parity_error = false;
            printf("uart lin pid parity error\n");
        }

        if (uart_lin_receive_data_error) {
            uart_lin_receive_data_error = false;
            printf("uart lin receive data error\n");
        }

        if (uart_lin_nonresponse_pid) {
            uart_lin_nonresponse_pid = false;
            printf("uart lin receive nonresponse id: 0x%x\n", lin_pid);
        }
    }

    return 0;
}

