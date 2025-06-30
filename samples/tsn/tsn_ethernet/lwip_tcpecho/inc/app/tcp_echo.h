/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H

#ifndef TCP_LOCAL_PORT
#define TCP_LOCAL_PORT (5001U)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_echo_init(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_ECHO_H */