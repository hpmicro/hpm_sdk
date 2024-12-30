/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tcp_echo.h"
#include "lwip/sys.h"
#include "lwip/api.h"

static void tcp_echo_thread(void *arg)
{
    struct netconn *conn, *newconn;
    struct netbuf *buf;
    void *data;
    u16_t len;
    err_t err;

    LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
    conn = netconn_new(NETCONN_TCP_IPV6);
    netconn_bind(conn, IP6_ADDR_ANY, TCP_LOCAL_PORT);
#else /* LWIP_IPV6 */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TCP_LOCAL_PORT);
#endif /* LWIP_IPV6 */
    LWIP_ASSERT("tcpecho: invalid conn", (conn != NULL));

    netconn_listen(conn);

    while (1) {
        if (netconn_accept(conn, &newconn) == ERR_OK) {
            while (netconn_recv(newconn, &buf) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
                    if (err != ERR_OK) {
                        LWIP_ASSERT("tcpecho: netconn_write: error %s\n", lwip_strerr(err));
                    }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

void tcp_echo_init(void)
{
    sys_thread_new("tcp_echo_thread", tcp_echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}