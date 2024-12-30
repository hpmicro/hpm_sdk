/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "udp_echo.h"
#include "lwip/udp.h"

static void udpecho_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    (void)arg;

    if (p != NULL) {
        udp_sendto(upcb, p, addr, port);
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
