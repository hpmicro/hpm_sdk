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

#ifndef MODBUS_APP_TASK_STACK_SIZE
#define MODBUS_APP_TASK_STACK_SIZE (2048)
#endif

#ifndef MODBUS_APP_TASK_PRIORITY
#define MODBUS_APP_TASK_PRIORITY (2)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void modbus_tcp_server_init(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MODBUS_APPLICATIPN_H */