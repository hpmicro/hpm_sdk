/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "udp_echo.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "lwip/sockets.h"
#include "FreeRTOS.h"

#define RECV_DATA (1500U)

static void udp_echo_thread(void *arg)
{
    (void)arg;

    int sock = -1;
    char *recv_data;
    struct sockaddr_in udp_addr, seraddr;
    int recv_data_len;
    socklen_t addrlen;

    while (1) {
        recv_data = (char *)pvPortMalloc(RECV_DATA);
        if (recv_data == NULL) {
            printf("Memory error\n");
            goto __exit;
        }

        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            printf("Socket error\n");
            goto __exit;
        }

        udp_addr.sin_family = AF_INET;
        udp_addr.sin_addr.s_addr = INADDR_ANY;
        udp_addr.sin_port = htons(UDP_ECHO_PORT);
        memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));

        if (bind(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) == -1) {
            printf("Bind error\n");
            goto __exit;
        }
        while (1) {
            recv_data_len = recvfrom(sock, recv_data, RECV_DATA, 0, (struct sockaddr *)&seraddr, &addrlen);
            sendto(sock, recv_data, recv_data_len, 0, (struct sockaddr *)&seraddr, addrlen);
        }

__exit:
        if (sock >= 0) {
            closesocket(sock);
        }

        if (recv_data) {
            free(recv_data);
        }
    }
}
/*-----------------------------------------------------------------------------------*/
void udp_echo_init(void)
{
    sys_thread_new("udp_echo_thread", udp_echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
