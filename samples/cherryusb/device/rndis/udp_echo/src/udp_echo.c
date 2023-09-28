/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "udp_echo.h"
#include <stdio.h>
#include <string.h>

#define UDP_ECHO_PORT 5001

/* Variable Definition */
static char *arg_buf = "Udp_server get a data:\n";

/* Static Function Declaration */
static void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                              const ip_addr_t *addr, u16_t port);

/* Function Definition */
void udp_echo_init(void)
{
    struct udp_pcb *udp_echo_pcb;

    udp_echo_pcb = udp_new();

    udp_bind(udp_echo_pcb, IP_ADDR_ANY, UDP_ECHO_PORT);

    udp_recv(udp_echo_pcb, udp_recv_callback, arg_buf);
}

static void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                              const ip_addr_t *addr, u16_t port)
{
    struct pbuf *q     = NULL;
    const char *reply = "This is reply!\n";
    uint8_t *dataptr;
    err_t send_err;

    if (arg != NULL) {
        printf("%s", arg_buf);
    }
    if ((p->tot_len > 0) && (p->tot_len < 1500)) {
        dataptr = (uint8_t *)p->payload;
        dataptr[p->tot_len] = (uint8_t)'\0';
        printf("%s\n", (char *)p->payload);
        udp_sendto(pcb, p, addr, port);
    }
    pbuf_free(p);

    do {
        q = pbuf_alloc(PBUF_TRANSPORT, strlen(reply) + 1, PBUF_RAM);
        if (!q) {
            printf("out of PBUF_RAM\n");
            return;
        }
        memcpy(q->payload, reply, strlen(reply) + 1);
        send_err = udp_sendto(pcb, q, addr, port);
        pbuf_free(q);
    } while (send_err == ERR_BUF);

    printf("%s", reply);
}
