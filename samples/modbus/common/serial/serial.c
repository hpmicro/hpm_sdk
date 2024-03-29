/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_uart_drv.h"

/* Define */
#define SERIAL_UART                    BOARD_MODBUS_UART_BASE
#define SERIAL_UART_CLK_NAME           BOARD_MODBUS_UART_CLK_NAME
#define SERIAL_UART_DMA_CONTROLLER     BOARD_APP_HDMA
#define SERIAL_UART_DMAMUX_CONTROLLER  BOARD_APP_DMAMUX
#define SERIAL_UART_RX_DMA_REQ         BOARD_MODBUS_UART_RX_DMA_REQ
#define SERIAL_UART_TX_DMA_REQ         BOARD_MODBUS_UART_TX_DMA_REQ
#define SERIAL_UART_RX_DMA_CH          (0U)
#define SERIAL_UART_TX_DMA_CH          (1U)
#define SERIAL_UART_RX_DMAMUX_CH       DMA_SOC_CHN_TO_DMAMUX_CHN(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_RX_DMA_CH)
#define SERIAL_UART_TX_DMAMUX_CH       DMA_SOC_CHN_TO_DMAMUX_CHN(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH)

#define SERIAL_BUFFER_SIZE             (1024U)

static void serial_uart_dma_init(void);

/* dma buffer should be 4-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_rx_buf[SERIAL_BUFFER_SIZE];
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors[2];
typedef struct {
    uint32_t front_index;
    uint32_t rear_index;
} serial_rx_t;

static volatile ATTR_PLACE_AT_NONCACHEABLE serial_rx_t serial_rx;
static volatile bool is_send = false;

void serial_uart_init(void)
{
    hpm_stat_t stat;
    uart_config_t config = { 0 };
    board_init_uart(SERIAL_UART);
    uart_default_config(SERIAL_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(SERIAL_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    stat = uart_init(SERIAL_UART, &config);
    if (stat != status_success) {
        while (1) {
        }
    }
    memset((void *)&serial_rx, 0, sizeof(serial_rx));
    serial_uart_dma_init();
}

int serial_uart_receive(uint8_t *data, int len)
{
    int rx_data_size;
    uint32_t dma_remaining_size;
    if (data == NULL) {
        return -1;
    }
    dma_remaining_size = dma_get_remaining_transfer_size(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_RX_DMA_CH);
    serial_rx.rear_index = SERIAL_BUFFER_SIZE - dma_remaining_size;
    if (serial_rx.front_index > serial_rx.rear_index) {
        rx_data_size = (SERIAL_BUFFER_SIZE - serial_rx.front_index) + serial_rx.rear_index;
    } else {
        rx_data_size = serial_rx.rear_index - serial_rx.front_index;
    }
    if (rx_data_size < len) {
        return -2;
    }
    for (int i = 0; i < rx_data_size; i++) {
        if ((serial_rx.front_index + i) < SERIAL_BUFFER_SIZE) {
            data[i] = uart_rx_buf[serial_rx.front_index + i];
        } else {
            data[i] = uart_rx_buf[serial_rx.front_index + i - SERIAL_BUFFER_SIZE];
        }
    }
    serial_rx.front_index = serial_rx.rear_index;
    return rx_data_size;
}

void serial_uart_flush(void)
{
    memset((void *)&serial_rx, 0, sizeof(serial_rx));
}

int serial_uart_send(uint8_t *data, uint32_t len)
{
    int tx_data_size = len;
    if ((data == NULL) &&
        (!dma_channel_is_enable(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH)) &&
        (is_send == true)) {
        return -1;
    }
    if (tx_data_size > 0) {
        dma_set_transfer_size(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH, tx_data_size);
        dma_set_source_address(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)data));
        dma_enable_channel(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH);
        is_send = true;
    }
    return tx_data_size;
}

bool serial_uart_send_finsh(void)
{
    if (is_send == false) {
        return true;
    }
    if ((dma_get_remaining_transfer_size(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_TX_DMA_CH) == 0) &&
        (uart_check_status(SERIAL_UART, uart_stat_transmitter_empty) == true)) {
        return true;
    } else {
        return false;
    }
}

static void serial_uart_dma_init(void)
{
    hpm_stat_t stat;
    dma_channel_config_t rx_ch_config = { 0 };
    dma_handshake_config_t tx_ch_config = { 0 };

    /* 1. config uart circle rx dma */
    dmamux_config(SERIAL_UART_DMAMUX_CONTROLLER, SERIAL_UART_RX_DMAMUX_CH, SERIAL_UART_RX_DMA_REQ, true);

    /* 1.1 config chain descriptors */
    dma_default_channel_config(SERIAL_UART_DMA_CONTROLLER, &rx_ch_config);
    rx_ch_config.src_addr = (uint32_t)&SERIAL_UART->RBR;
    rx_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;  /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    rx_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    rx_ch_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    rx_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;  /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    rx_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    rx_ch_config.size_in_byte = SERIAL_BUFFER_SIZE;
    rx_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;  /*  In DMA handshake case, source burst size must be 1 transfer, that is 0. */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[1]);
    stat = dma_config_linked_descriptor(SERIAL_UART_DMA_CONTROLLER, &descriptors[0], SERIAL_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_config_linked_descriptor(SERIAL_UART_DMA_CONTROLLER, &descriptors[1], SERIAL_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    /* 1.2 config rx dma */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_setup_channel(SERIAL_UART_DMA_CONTROLLER, SERIAL_UART_RX_DMA_CH, &rx_ch_config, true);
    if (stat != status_success) {
        while (1) {
        };
    }

    /* 2. config uart tx dma */
    dmamux_config(SERIAL_UART_DMAMUX_CONTROLLER, SERIAL_UART_TX_DMAMUX_CH, SERIAL_UART_TX_DMA_REQ, true);
    dma_default_handshake_config(SERIAL_UART_DMA_CONTROLLER, &tx_ch_config);
    tx_ch_config.ch_index = SERIAL_UART_TX_DMA_CH;
    tx_ch_config.dst = (uint32_t)&SERIAL_UART->THR;
    tx_ch_config.dst_fixed = true;
    tx_ch_config.src_fixed = false;
    tx_ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    tx_ch_config.size_in_byte = SERIAL_BUFFER_SIZE;
    dma_setup_handshake(SERIAL_UART_DMA_CONTROLLER, &tx_ch_config, false);
}
