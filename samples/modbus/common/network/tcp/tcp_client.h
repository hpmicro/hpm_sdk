/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *//*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MODBUS_TCP_SERVER
#define MODBUS_TCP_SERVER

#include "stdint.h"
#include "hpm_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  tcp client init
 * @retval status_success if receive success
 */
hpm_stat_t network_tcp_client_init(void);

/**
 * @brief network tcp receive for tcp_client
 * @param [in] data data point
 * @param [in] len  data length
 * @retval -1 if receive error, >=0 if receive success
 */
int network_tcp_c_receive(uint8_t *data, int len);

/**
 * @brief  network tcp flush
 */
void network_tcp_c_flush(void);

/**
 * @brief network tcp send for tcp_client
 * @param [in] data data point
 * @param [in] len  data length
 * @retval <= 0 if send error, >0 if send success
 */
int network_tcp_c_send(uint8_t *data, uint32_t len);

bool tcp_client_connect_state(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif


