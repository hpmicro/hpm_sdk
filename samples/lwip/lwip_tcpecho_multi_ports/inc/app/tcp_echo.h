/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H

#include "lwip/netif.h"

#ifndef TCP_LOCAL_PORT0
#define TCP_LOCAL_PORT0 (5001U)
#endif

#ifndef TCP_LOCAL_PORT1
#define TCP_LOCAL_PORT1 (5002U)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_echo_init(struct netif *netif);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_ECHO_H */