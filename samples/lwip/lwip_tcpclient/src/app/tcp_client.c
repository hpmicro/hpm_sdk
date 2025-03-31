/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "tcp_client.h"
#include "lwip/tcp.h"
#include "netconf.h"
#include "hpm_common.h"

volatile bool tcp_client_flag;
tcp_client_t tcp_pcb_cb_arg;

static void client_err(void *arg, err_t err)
{
    (void)arg;
    (void)err;
}

static err_t client_send(void *arg, struct tcp_pcb *tpcb)
{
     if (arg == NULL) {
        return ERR_ARG;
    }

    tcp_client_t *tcp_pcb_arg = (tcp_client_t *)arg;

    uint8_t send_buf[] = "This is a TCP Client test...\n";

    if (tcp_pcb_arg->state) {
        tcp_write(tpcb, send_buf, sizeof(send_buf), TCP_WRITE_FLAG_COPY);
    }

    return ERR_OK;
}

static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    (void)err;

    if (arg == NULL) {
        return ERR_ARG;
    }

    tcp_client_t *tcp_pcb_arg = (tcp_client_t *)arg;

    printf("Connected to server successfully!\n");

    tcp_pcb_arg->state = true;
    tcp_pcb_arg->pcb = pcb;
    tcp_pcb_arg->p = NULL;

    tcp_poll(pcb, client_send, TCP_POLL_INTERVAL);

    return ERR_OK;
}

static err_t tcp_client_disconnect(void)
{
    tcp_abort(tcp_pcb_cb_arg.pcb);
    tcp_err(tcp_pcb_cb_arg.pcb, NULL);
    tcp_poll(tcp_pcb_cb_arg.pcb, NULL, 0);

    tcp_pcb_cb_arg.state = false;

    return ERR_OK;
}

void tcp_client_connect(void)
{
    ip4_addr_t server_ip;
    struct tcp_pcb *pcb = NULL;

    pcb = tcp_new();
    ip4addr_aton(HPM_STRINGIFY(REMOTE_IP_CONFIG), &server_ip);
    tcp_err(pcb, client_err);
    tcp_pcb_cb_arg.pcb = pcb;
    tcp_arg(pcb, &tcp_pcb_cb_arg);
    tcp_connect(pcb, &server_ip, TCP_DEST_PORT, client_connected);
}

void tcp_client_reconnect(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
        if (!tcp_pcb_cb_arg.state) {
          tcp_client_disconnect();
          tcp_client_connect();
          sys_msleep(10);
        }
    } else {
        if (tcp_pcb_cb_arg.state) {
            tcp_pcb_cb_arg.state = false;
        }
    }
}

void tcp_client_init(void)
{
    tcp_client_connect();
}
