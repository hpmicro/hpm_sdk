/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "udp_echo.h"
#include "lwip/api.h"
#include "lwip/sys.h"

static void udp_echo_thread(void *arg)
{
    struct netconn *conn;
    struct netbuf *buf;
    char buffer[1500];
    err_t err;
    LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
    conn = netconn_new(NETCONN_UDP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, UDP_ECHO_PORT);
#else  /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, UDP_ECHO_PORT);
#endif /* LWIP_IPV6 */
    LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

    while (1) {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) {
            if (netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len) {
                LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
            } else {
                buffer[buf->p->tot_len] = '\0';
                err = netconn_send(conn, buf);
                if (err != ERR_OK) {
                    LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d\n", (int)err));
                }
            }
            netbuf_delete(buf);
        }
    }
}

void udp_echo_init(void)
{
    sys_thread_new("udp_echo_thread", udp_echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
