/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H
#include "lwip/netif.h"

#define TCP_LOCAL_PORT0 (5001U)
#define TCP_LOCAL_PORT1 (5002U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_echo_init(struct netif *netif);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_ECHO_H */