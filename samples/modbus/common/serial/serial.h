/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MODBUS_SERIAL
#define MODBUS_SERIAL

#include "stdint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief serial uart init
 */
void serial_uart_init(void);

/**
 * @brief serial uart receive
 * @param [in] data data point
 * @param [in] len  data length
 * @retval -1 if receive error, >=0 if receive success
 */
int serial_uart_receive(uint8_t *data, int len);

/**
 * @brief serial uart flush
 */
void serial_uart_flush(void);

/**
 * @brief serial uart send
 * @param [in] data data point
 * @param [in] len  data length
 * @retval <= 0 if send error, >0 if send success
 */
int serial_uart_send(uint8_t *data, uint32_t len);

/**
 * @brief serial uart send finsh status
 * @retval true if send finsh
 */
bool serial_uart_send_finsh(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


