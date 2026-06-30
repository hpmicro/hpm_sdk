/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "tcp_echo.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    err_t write_err;

    (void)arg;

    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        /* Echo each pbuf segment when RX used a chain (eth_len > PBUF_POOL_BUFSIZE). */
        for (q = p; q != NULL; q = q->next) {
            write_err = tcp_write(tpcb, q->payload, q->len, TCP_WRITE_FLAG_COPY);
            if (write_err != ERR_OK) {
                pbuf_free(p);
                return write_err;
            }
            memset(q->payload, 0, q->len);
        }
        pbuf_free(p);
        return tcp_output(tpcb);
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