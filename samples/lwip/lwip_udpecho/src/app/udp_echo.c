/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "udp_echo.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"

static void udpecho_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    (void)arg;

    struct pbuf *q;

    if (p != NULL) {
        /* allocate a new pbuf and copy payload from p to q */
        q = pbuf_clone(PBUF_TRANSPORT, PBUF_RAM, p);
        if (q != NULL) {
            udp_sendto(upcb, q, addr, port);
            pbuf_free(q);
        }
        pbuf_free(p);
    }
}

void udp_echo_init(void)
{
    struct udp_pcb *pcb = NULL;

    pcb = udp_new();
    udp_bind(pcb, IP_ADDR_ANY, UDP_LOCAL_PORT);
    udp_recv(pcb, udpecho_recv, NULL);
}
