/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "tcp_echo.h"
#include "lwip/tcp.h"

static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg;

    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        tcp_write(tpcb, p->payload, p->tot_len, 1);
        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        return tcp_close(tpcb);
    }
    return ERR_OK;
}

static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    (void)arg;
    (void)err;

    tcp_recv(newpcb, tcpecho_recv);
    return ERR_OK;
}

void tcp_echo_init(void)
{
    struct tcp_pcb *pcb = NULL;

    pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, TCP_LOCAL_PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcpecho_accept);
}