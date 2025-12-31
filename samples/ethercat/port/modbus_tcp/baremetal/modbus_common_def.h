/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MODBUS_COMMON_DEF_H
#define MODBUS_COMMON_DEF_H

typedef enum {
    send_start = 0,
    send_wait,
    send_finsh,
    recv_wait,
    recv_finsh,
} interface_rtx_sta_t;

#endif /* MODBUS_COMMON_DEF_H */