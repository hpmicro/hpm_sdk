/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tcp_client.h"
#include "lwip/tcp.h"
#include "netconf.h"
#include "hpm_common.h"
#include "utils.h"

volatile bool tcp_client_flag;
tcp_client_t tcp_pcb_cb_arg;
ip4_addr_t server_ip;

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

void tcp_client_connect(ip4_addr_t *server_ip)
{
    struct tcp_pcb *pcb = NULL;

    pcb = tcp_new();

    tcp_err(pcb, client_err);
    tcp_pcb_cb_arg.pcb = pcb;
    tcp_arg(pcb, &tcp_pcb_cb_arg);
    tcp_connect(pcb, server_ip, TCP_DEST_PORT, client_connected);
}

void tcp_client_reconnect(struct netif *netif, ip4_addr_t *server_ip)
{
    uint32_t sys_tick = sys_now();
    static uint64_t target_sys_tick = 0;
    static uint32_t retry = 0;

    if (target_sys_tick <= sys_tick) {
        target_sys_tick = sys_tick + 500;

        if (netif_is_link_up(netif)) {
            if (!tcp_pcb_cb_arg.state) {
                tcp_client_disconnect();
                tcp_client_connect(server_ip);
                if (++retry > 1) {
                    printf("Reconnecting ...\n");
                }
            }
        } else {
            if (tcp_pcb_cb_arg.state) {
                tcp_pcb_cb_arg.state = false;
            }
        }
    }
}

void tcp_client_init(ip4_addr_t *server_ip)
{
    uint8_t cmd_str_buff[20];

    while (!fetch_ip_addr_from_serial_terminal(0, cmd_str_buff, sizeof(cmd_str_buff))) {

    }

    ip4addr_aton((char *)cmd_str_buff, server_ip);
    tcp_client_connect(server_ip);
}
