/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/pbuf.h"
#include <stdio.h>
#include <string.h>

#define TCP_ECHO_PORT 5001

/* Static Function Declaration */
static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcpecho_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/* Function Definition */
void tcp_server_init(void)
{
    struct tcp_pcb *pcb;

    pcb = tcp_new();

    tcp_bind(pcb, IP_ADDR_ANY, TCP_ECHO_PORT);

    pcb = tcp_listen(pcb);

    tcp_accept(pcb, tcpecho_accept);
}

static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    (void)arg;
    (void)err;
    tcp_recv(newpcb, tcpecho_recv);

    return ERR_OK;
}

static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg;
    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);

        tcp_write(tpcb, p->payload, p->tot_len, TCP_WRITE_FLAG_COPY);

        memset(p->payload, 0, p->tot_len);

        pbuf_free(p);
    } else if (err == ERR_OK) {
        return tcp_close(tpcb);
    } else {
        ;
    }

    return ERR_OK;
}
