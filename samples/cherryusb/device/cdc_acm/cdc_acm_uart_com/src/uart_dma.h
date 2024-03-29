/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _UART_DMA_H
#define _UART_DMA_H

#define TEST_BUFFER_SIZE 512

void init_board_app_uart(void);
void init_board_app_dma(void);
void uart_task_entry_5ms(void);
void uart_set_dma_tx_transfer(uint32_t addr, uint32_t len);
void reinit_board_app_uart(struct cdc_line_coding *line_coding);

#endif
