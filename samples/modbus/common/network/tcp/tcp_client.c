/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "netconf.h"
#include "lwip/tcp.h"
#include "hpm_common.h"
#include "hpm_clock_drv.h"

static void client_err(void *arg, err_t err);
static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err);

ATTR_PLACE_AT_NONCACHEABLE uint8_t tcp_c_rx_buf[NETWORK_BUFFER_SIZE];
static struct tcp_pcb *client_pcb = NULL;
static network_rx_t network_rx;
static bool is_active = false;

hpm_stat_t network_tcp_client_init(void)
{
    ip4_addr_t server_ip;
    client_pcb = tcp_new();
    if (client_pcb == NULL) {
        return status_invalid_argument;
    }
    IP4_ADDR(&server_ip, REMOTE_IP_ADDR0, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3);
    tcp_connect(client_pcb, &server_ip, TCP_SERVER_PORT, client_connected);
    tcp_err(client_pcb, client_err);
    return status_success;
}

int network_tcp_c_receive(uint8_t *data, int len)
{
    int rx_data_size;
    if (data == NULL) {
        return -1;
    }
    if (is_active == false) {
        return -2;
    }
    if (network_rx.front_index > network_rx.rear_index) {
        rx_data_size = (NETWORK_BUFFER_SIZE - network_rx.front_index) + network_rx.rear_index;
    } else {
        rx_data_size = network_rx.rear_index - network_rx.front_index;
    }
    if (rx_data_size < len) {
        return  -3;
    }
    for (int i = 0; i < rx_data_size; i++) {
        if ((network_rx.front_index + i) < NETWORK_BUFFER_SIZE) {
            data[i] = tcp_c_rx_buf[network_rx.front_index + i];
        } else {
            data[i] = tcp_c_rx_buf[network_rx.front_index + i - NETWORK_BUFFER_SIZE];
        }
    }
    network_rx.front_index = network_rx.rear_index;
    return rx_data_size;
}

void network_tcp_c_flush(void)
{
    memset(&network_rx, 0, sizeof(network_rx));
}

int network_tcp_c_send(uint8_t *data, uint32_t len)
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

bool tcp_client_connect_state(void)
{
    return is_active;
}

static void client_err(void *arg, err_t err)
{
    (void)arg;
    (void)err;
    printf("connect error! closed by core!!\n");
    printf("try to connect to server again!!\n");
    tcp_close(client_pcb);
    network_tcp_client_init();
}

static err_t client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg;
    (void)err;
    uint32_t remain_len;
    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        remain_len = NETWORK_BUFFER_SIZE - network_rx.rear_index;
        if (p->tot_len > remain_len) {
            memcpy(&tcp_c_rx_buf[network_rx.rear_index], p->payload, remain_len);
            memcpy(&tcp_c_rx_buf[0], &((uint8_t *)p->payload)[remain_len], p->tot_len - remain_len);
        } else {
            memcpy(&tcp_c_rx_buf[network_rx.rear_index], p->payload, p->tot_len);
        }
        network_rx.rear_index += p->tot_len;
        network_rx.rear_index %= NETWORK_BUFFER_SIZE;
        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        printf("server has been disconnected!\n");
        tcp_close(tpcb);
        network_tcp_client_init();
    }
    return ERR_OK;
}

static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    (void)arg;
    (void)err;
    printf("client connected server\n");
    is_active = true;
    tcp_recv(pcb, client_recv);
    return ERR_OK;
}


