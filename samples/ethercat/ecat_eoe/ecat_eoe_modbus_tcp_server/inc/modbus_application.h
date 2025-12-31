/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MODBUS_APPLICATIPN_H
#define MODBUS_APPLICATIPN_H

#ifndef MODBUS_SLAVE_ADDRESS
#define MODBUS_SLAVE_ADDRESS (1)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void modbus_tcp_server_init(void);

void modbus_tcp_server_process(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MODBUS_APPLICATIPN_H */