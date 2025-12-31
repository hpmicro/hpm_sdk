/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tcp_client.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "netconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "utils.h"

uint8_t send_buf[] = "This is a TCP Client test...\n";

#ifndef TCP_CLIENT_DEBUG
#define TCP_CLIENT_DEBUG  LWIP_DBG_OFF
#endif

static void tcp_client_thread(void *arg)
{
    int sock = -1;
    ip4_addr_t ipaddr;
    struct sockaddr_in client_addr;
    struct netif *netif = (struct netif *)arg;
    static uint32_t retry = 0;
    uint8_t cmd_str_buff[20];


    while (!netif_is_link_up(netif)) {
        vTaskDelay(100);
    }

    while (!fetch_ip_addr_from_serial_terminal(0, cmd_str_buff, sizeof(cmd_str_buff))) {

    }

    ip4addr_aton((char *)cmd_str_buff, &ipaddr);

    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Socket error\n");
            vTaskDelay(10);
            continue;
        }

        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(TCP_DEST_PORT);
        client_addr.sin_addr.s_addr = ipaddr.addr;
        memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));

        if (++retry > 1) {
            printf("Reconnecting ...\n");
        }

        if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) {
            LWIP_DEBUGF(TCP_CLIENT_DEBUG, ("Connect error!\n"));
            closesocket(sock);
            vTaskDelay(500);
            continue;
        }

        LWIP_DEBUGF(LWIP_DBG_ON | LWIP_DBG_TRACE, ("Connect to server successfully!\n"));

        while (1) {
            if (write(sock, send_buf, sizeof(send_buf)) < 0) {
                LWIP_DEBUGF(TCP_CLIENT_DEBUG, ("Send error!\n"));
                break;
            }
            vTaskDelay(1000);
        }

        closesocket(sock);
    }
}

void tcp_client_init(void *arg)
{
    sys_thread_new("tcp_client_thread", tcp_client_thread, arg, 3*DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
