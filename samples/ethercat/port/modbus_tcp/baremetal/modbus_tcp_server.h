/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MODBUS_TCP_SERVER
#define MODBUS_TCP_SERVER

#include "stdint.h"
#include "hpm_common.h"

#ifndef MODBUS_TCP_SERVER_PORT
#define MODBUS_TCP_SERVER_PORT (502)
#endif

#ifndef MODBUS_TCP_BUFF_SIZE
#define MODBUS_TCP_BUFF_SIZE (1024)
#endif

typedef struct {
    uint32_t front_index;
    uint32_t rear_index;
} network_rx_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief serial uart init
 * @retval status_success if receive success
 */
hpm_stat_t network_tcp_server_init(void);

/**
 * @brief network tcp receive for tcp_server
 * @param [in] data data point
 * @param [in] len  data length
 * @retval -1 if receive error, >=0 if receive success
 */
int network_tcp_s_receive(uint8_t *data, int len);

/**
 * @brief  network tcp flush
 */
void network_tcp_s_flush(void);

/**
 * @brief network tcp send for tcp_server
 * @param [in] data data point
 * @param [in] len  data length
 * @retval <= 0 if send error, >0 if send success
 */
int network_tcp_s_send(uint8_t *data, uint32_t len);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


