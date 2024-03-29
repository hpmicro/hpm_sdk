/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

typedef enum {
    send_start = 0,
    send_wait,
    send_finsh,
    recv_wait,
    recv_finsh,
} interface_rtx_sta_t;
