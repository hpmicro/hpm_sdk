/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "tcp_client.h"
#include "lwip/tcp.h"
#include "netconf.h"

static struct tcp_pcb *client_pcb = NULL;

static void client_err(void *arg, err_t err)
{
    (void)arg;
    (void)err;

    printf("Connect error! Closed by core!!\n");
    printf("Try to connect to server again!!\n");

    tcp_close(client_pcb);
    tcp_client_init();
}


static err_t client_send(void *arg, struct tcp_pcb *tpcb)
{
    (void)arg;

    uint8_t send_buf[] = "This is a TCP Client test...\n";

    tcp_write(tpcb, send_buf, sizeof(send_buf), TCP_WRITE_FLAG_COPY);

    return ERR_OK;
}

static err_t client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg;

    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        tcp_write(tpcb, p->payload, p->tot_len, TCP_WRITE_FLAG_COPY);
        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        printf("Server has been disconnected!\n");
        tcp_close(tpcb);
        tcp_client_init();
    }

    return ERR_OK;
}

static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    (void)arg;
    (void)err;

    printf("Connect to server successfully!\n");

    tcp_poll(pcb, client_send, TCP_POOL_INTERVAL);
    tcp_recv(pcb, client_recv);

    return ERR_OK;
}

void tcp_client_init(void)
{
    ip4_addr_t server_ip;

    client_pcb = tcp_new();
    IP4_ADDR(&server_ip, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    tcp_connect(client_pcb, &server_ip, TCP_DEST_PORT, client_connected);
    tcp_err(client_pcb, client_err);
}