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
#include "usbd_core.h"
#include "usbd_cdc_acm.h"
#include "cdc_acm.h"
#include "uart_dma.h"

/* Define */
#define TEST_UART            BOARD_USB_CDC_ACM_UART
#define TEST_UART_CLK_NAME   BOARD_USB_CDC_ACM_UART_CLK_NAME
#define TEST_UART_DMA_SRC_RX BOARD_USB_CDC_ACM_UART_RX_DMA_SRC
#define TEST_UART_DMA_SRC_TX BOARD_USB_CDC_ACM_UART_TX_DMA_SRC

#define TEST_UART_DMA_CONTROLLER    BOARD_APP_HDMA
#define TEST_UART_DMA_IRQ           BOARD_APP_HDMA_IRQ
#define TEST_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX

#define TEST_UART_RX_DMA_CH         0
#define TEST_UART_TX_DMA_CH         1
#define TEST_UART_RX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH)
#define TEST_UART_TX_DMAMUX_CH      DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH)

/* dma buffer should be 4-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4) uint8_t uart_rx_buf[TEST_BUFFER_SIZE];
#ifndef HPMSOC_HAS_HPMSDK_DMAV2
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors[2];
#endif
static uint32_t rx_front_index;
static uint32_t rx_rear_index;

void init_board_app_uart(void)
{
    hpm_stat_t stat;
    uart_config_t config = { 0 };
    struct cdc_line_coding line_coding;

    board_init_uart(TEST_UART);

    uart_default_config(TEST_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        while (1) {
        }
    }

    line_coding.dwDTERate = config.baudrate;
    if (config.word_length == word_length_5_bits) {
        line_coding.bDataBits = 5;
    } else if (config.word_length == word_length_6_bits) {
        line_coding.bDataBits = 6;
    } else if (config.word_length == word_length_7_bits) {
        line_coding.bDataBits = 7;
    } else {
        line_coding.bDataBits = 8;
    }
    line_coding.bParityType = config.parity;
    line_coding.bCharFormat = config.num_of_stop_bits;
    usbd_init_line_coding(&line_coding);
}

void init_board_app_dma(void)
{
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
    dma_handshake_config_t rx_ch_config = { 0 };
#else
    hpm_stat_t stat;
    dma_channel_config_t rx_ch_config = { 0 };
#endif
    dma_handshake_config_t tx_ch_config = { 0 };

    /* 1. config uart circle rx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_RX_DMAMUX_CH, TEST_UART_DMA_SRC_RX, true);

#ifdef HPMSOC_HAS_HPMSDK_DMAV2
    dma_default_handshake_config(TEST_UART_DMA_CONTROLLER, &rx_ch_config);
    rx_ch_config.ch_index = TEST_UART_RX_DMA_CH;
    rx_ch_config.dst = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    rx_ch_config.dst_fixed = false;
    rx_ch_config.src = (uint32_t)&TEST_UART->RBR;
    rx_ch_config.src_fixed = true;
    rx_ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    rx_ch_config.size_in_byte = TEST_BUFFER_SIZE;
    rx_ch_config.en_infiniteloop = true;
    rx_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &rx_ch_config, true);
#else
    /* 1.1 config chain descriptors */
    dma_default_channel_config(TEST_UART_DMA_CONTROLLER, &rx_ch_config);
    rx_ch_config.src_addr = (uint32_t)&TEST_UART->RBR;
    rx_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE; /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    rx_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    rx_ch_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf);
    rx_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE; /*  In DMA handshake case, source width and destination width must be BYTE. */
    rx_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    rx_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    rx_ch_config.size_in_byte = TEST_BUFFER_SIZE;
    rx_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T; /*  In DMA handshake case, source burst size must be 1 transfer, that is 0. */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[1]);
    stat = dma_config_linked_descriptor(TEST_UART_DMA_CONTROLLER, &descriptors[0], TEST_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_config_linked_descriptor(TEST_UART_DMA_CONTROLLER, &descriptors[1], TEST_UART_RX_DMA_CH, &rx_ch_config);
    if (stat != status_success) {
        while (1) {
        };
    }

    /* 1.2 config rx dma */
    rx_ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    stat = dma_setup_channel(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH, &rx_ch_config, true);
    if (stat != status_success) {
        while (1) {
        };
    }
#endif

    /* 2. config uart tx dma */
    dmamux_config(TEST_UART_DMAMUX_CONTROLLER, TEST_UART_TX_DMAMUX_CH, TEST_UART_DMA_SRC_TX, true);
    dma_default_handshake_config(TEST_UART_DMA_CONTROLLER, &tx_ch_config);
    tx_ch_config.ch_index = TEST_UART_TX_DMA_CH;
    tx_ch_config.dst = (uint32_t)&TEST_UART->THR;
    tx_ch_config.dst_fixed = true;
    tx_ch_config.src_fixed = false;
    tx_ch_config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    tx_ch_config.size_in_byte = TEST_BUFFER_SIZE;
    dma_setup_handshake(TEST_UART_DMA_CONTROLLER, &tx_ch_config, false);

    /* 3. enable irq */
    intc_m_enable_irq_with_priority(TEST_UART_DMA_IRQ, 2);
}

SDK_DECLARE_EXT_ISR_M(TEST_UART_DMA_IRQ, dma_isr)
void dma_isr(void)
{
    uint32_t status;

    status = dma_check_transfer_status(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH);

    if ((status & DMA_CHANNEL_STATUS_TC) != 0) {
        usbd_ep_start_read(USB_BUS_ID, CDC_OUT_EP, usbd_get_read_buffer_ptr(), usbd_get_ep_mps(USB_BUS_ID, CDC_OUT_EP));
    }
}

void uart_task_entry_5ms(void)
{
    uint32_t rx_data_size;
    uint32_t dma_remaining_size;
    uint8_t *usb_wr_buf_ptr = usbd_get_write_buffer_ptr();

    dma_remaining_size = dma_get_remaining_transfer_size(TEST_UART_DMA_CONTROLLER, TEST_UART_RX_DMA_CH);

    rx_rear_index = TEST_BUFFER_SIZE - dma_remaining_size;
    if (rx_front_index > rx_rear_index) {
        rx_data_size = TEST_BUFFER_SIZE + rx_rear_index - rx_front_index;
    } else {
        rx_data_size = rx_rear_index - rx_front_index;
    }

    for (uint32_t i = 0; i < rx_data_size; i++) {
        if ((rx_front_index + i) < TEST_BUFFER_SIZE) {
            usb_wr_buf_ptr[i] = uart_rx_buf[rx_front_index + i];
        } else {
            usb_wr_buf_ptr[i] = uart_rx_buf[rx_front_index + i - TEST_BUFFER_SIZE];
        }
    }
    rx_front_index = rx_rear_index;

    if (rx_data_size > 0) {
        if (usb_device_is_configured(USB_BUS_ID)) {
            if (!usbd_is_tx_busy()) {
                usbd_set_tx_busy();
                usbd_ep_start_write(USB_BUS_ID, CDC_IN_EP, usb_wr_buf_ptr, rx_data_size);
            }
        }
    }
}

void uart_set_dma_tx_transfer(uint32_t addr, uint32_t len)
{
    if (!dma_channel_is_enable(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH)) {
        dma_set_transfer_size(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH, len);
        dma_set_source_address(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, addr));
        dma_enable_channel(TEST_UART_DMA_CONTROLLER, TEST_UART_TX_DMA_CH);
    }
}

void reinit_board_app_uart(struct cdc_line_coding *line_coding)
{
    hpm_stat_t stat;
    uart_config_t config = { 0 };

    uart_default_config(TEST_UART, &config);

    config.baudrate = line_coding->dwDTERate;
    if (line_coding->bDataBits == 5) {
        config.word_length = word_length_5_bits;
    } else if (line_coding->bDataBits == 6) {
        config.word_length = word_length_6_bits;
    } else if (line_coding->bDataBits == 7) {
        config.word_length = word_length_7_bits;
    } else {
        config.word_length = word_length_8_bits;
    }
    config.parity = line_coding->bParityType;
    config.num_of_stop_bits = line_coding->bCharFormat;

    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    stat = uart_init(TEST_UART, &config);
    if (stat != status_success) {
        while (1) {
        }
    }
}
