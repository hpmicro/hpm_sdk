/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "netconf.h"
#include "lwip/tcp.h"
#include "hpm_common.h"

static err_t tcp_s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_s_accept(void *arg, struct tcp_pcb *newpcb, err_t err);

ATTR_PLACE_AT_NONCACHEABLE uint8_t tcp_s_rx_buf[NETWORK_BUFFER_SIZE];
static struct tcp_pcb *pcb;
static struct tcp_pcb *client_pcb;
static network_rx_t network_rx;
static bool is_active = false;

hpm_stat_t network_tcp_server_init(void)
{
    pcb = tcp_new();
    if (pcb == NULL) {
        return status_invalid_argument;
    }
    tcp_bind(pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcp_s_accept);
    memset(&network_rx, 0, sizeof(network_rx));
    return status_success;
}

int network_tcp_s_receive(uint8_t *data, int len)
{
    int rx_data_size;
    if (data == NULL) {
        return -1;
    }
    if (network_rx.front_index > network_rx.rear_index) {
        rx_data_size = (NETWORK_BUFFER_SIZE - network_rx.front_index) + network_rx.rear_index;
    } else {
        rx_data_size = network_rx.rear_index - network_rx.front_index;
    }
    if (rx_data_size < len) {
        return -2;
    }
    for (int i = 0; i < rx_data_size; i++) {
        if ((network_rx.front_index + i) < NETWORK_BUFFER_SIZE) {
            data[i] = tcp_s_rx_buf[network_rx.front_index + i];
        } else {
            data[i] = tcp_s_rx_buf[network_rx.front_index + i - NETWORK_BUFFER_SIZE];
        }
    }
    network_rx.front_index = network_rx.rear_index;
    return rx_data_size;
}

void network_tcp_s_flush(void)
{
    memset(&network_rx, 0, sizeof(network_rx));
}

int network_tcp_s_send(uint8_t *data, uint32_t len)
{
    if ((client_pcb == NULL) || (len == 0)) {
        return -1;
    }
    if (is_active == false) {
        return -2;
    }
    if (tcp_write(client_pcb, data, len, 1) != ERR_OK) {
        return -3;
    }
    return len;
}

static err_t tcp_s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg;
    (void)err;
    uint32_t remain_len;

    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        client_pcb = tpcb;
        remain_len = NETWORK_BUFFER_SIZE - network_rx.rear_index;
        if (p->tot_len > remain_len) {
            memcpy(&tcp_s_rx_buf[network_rx.rear_index], p->payload, remain_len);
            memcpy(&tcp_s_rx_buf[0], &((uint8_t *)p->payload)[remain_len], p->tot_len - remain_len);
        } else {
            memcpy(&tcp_s_rx_buf[network_rx.rear_index], p->payload, p->tot_len);
        }
        network_rx.rear_index += p->tot_len;
        network_rx.rear_index %= NETWORK_BUFFER_SIZE;
        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        return tcp_close(tpcb);
    }
    return ERR_OK;
}

static err_t tcp_s_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    (void)arg;
    (void)err;
    printf("server litened\n");
    is_active = true;
    tcp_recv(newpcb, tcp_s_recv);
    client_pcb = newpcb;
    return ERR_OK;
}

