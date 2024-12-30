/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdint.h>
#include "lwip/netif.h"

#ifndef TCP_DEST_PORT
#define TCP_DEST_PORT   (5001U)
#endif

#ifndef TCP_POLL_INTERVAL
#define TCP_POLL_INTERVAL (2)       /* 1s */
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint8_t state;
    struct tcp_pcb *pcb;
    struct pbuf *p;
} tcp_client_t;

void tcp_client_init(void);
void tcp_client_reconnect(struct netif *netif);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_CLIENT_H */