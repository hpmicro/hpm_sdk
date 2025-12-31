/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "udp_echo.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "lwip/sys.h"

static void udp_echo_thread(void *arg)
{
    struct netconn *conn;
    struct netbuf *inbuf;
    struct netbuf *outbuf;
    void *data;
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
        err = netconn_recv(conn, &inbuf);
        if (err == ERR_OK) {
            /* create a new netbuf */
            outbuf = netbuf_new();
            if (outbuf != NULL) {
                /* allocate memory for the new netbuf */
                data = netbuf_alloc(outbuf, netbuf_len(inbuf));
                if (data != NULL) {
                    /* copy payload from inbuf to outbuf */
                    if (netbuf_copy(inbuf, data, netbuf_len(inbuf)) == netbuf_len(inbuf)) {
                        /* send using the new netbuf with source address and port */
                        err = netconn_sendto(conn, outbuf, netbuf_fromaddr(inbuf), netbuf_fromport(inbuf));
                        if (err != ERR_OK) {
                            LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_sendto failed: %d\n", (int)err));
                        }
                    } else {
                        LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
                    }
                }
                netbuf_delete(outbuf);
            }
            netbuf_delete(inbuf);
        }
    }
}

void udp_echo_init(void)
{
    sys_thread_new("udp_echo_thread", udp_echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
