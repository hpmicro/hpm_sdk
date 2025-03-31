/*
 * Copyright (c) 2024 HPMicro
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

    ip4addr_aton(HPM_STRINGIFY(REMOTE_IP_CONFIG), &ipaddr);

    while (!netif_is_link_up(netif)) {
        vTaskDelay(100);
    }

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

        if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) {
            LWIP_DEBUGF(TCP_CLIENT_DEBUG, ("Connect error!\n"));
            closesocket(sock);
            vTaskDelay(10);
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
