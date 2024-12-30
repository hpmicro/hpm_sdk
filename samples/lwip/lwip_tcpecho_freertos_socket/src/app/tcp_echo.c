/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tcp_echo.h"
#include "lwip/sys.h"
#include "lwip/api.h"


#include "lwip/sockets.h"
#include "FreeRTOS.h"

#define RECV_DATA  (1500U)

static void tcp_echo_thread(void *arg)
{
    int sock = -1;
    int flag = 1;
    int connected;
    int recv_data_len;
    char *recv_data;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    LWIP_UNUSED_ARG(arg);

    recv_data = (char *)pvPortMalloc(RECV_DATA);
    if (recv_data == NULL) {
        printf("Memory error\n");
        goto __exit;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(TCP_LOCAL_PORT);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Bind error\n");
        goto __exit;
    }

    if (listen(sock, TCP_DEFAULT_LISTEN_BACKLOG) == -1) {
        printf("Listen error\n");
        goto __exit;
    }

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        printf("new client connected from (%s, %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        setsockopt(connected,
                   IPPROTO_TCP,     /* set option at TCP level */
                   TCP_NODELAY,     /* name of option */
                   (void *) &flag,  /* the cast is historical cruft */
                   sizeof(int));    /* length of option value */

        while (1) {
            recv_data_len = recv(connected, recv_data, RECV_DATA, 0);

            if (recv_data_len <= 0) {
                break;
            }

            write(connected, recv_data, recv_data_len);
        }

        if (connected >= 0) {
            closesocket(connected);
        }
    }

__exit:
    if (sock >= 0) {
        closesocket(sock);
    }

    if (recv_data) {
        vPortFree(recv_data);
    }
}

void tcp_echo_init(void)
{
    sys_thread_new("tcp_echo_thread", tcp_echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}