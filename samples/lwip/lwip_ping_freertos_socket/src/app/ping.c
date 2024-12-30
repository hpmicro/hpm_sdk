/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/mem.h>
#include <lwip/icmp.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/inet.h>
#include <lwip/inet_chksum.h>
#include <lwip/ip.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

#include "hpm_common.h"

/** ping receive timeout - in milliseconds */
#define PING_RCV_TIMEO       (2 * configTICK_RATE_HZ)
/** ping delay - in milliseconds */
#define PING_DELAY           (1 * configTICK_RATE_HZ)

/** ping identifier - must fit on a uint16 */
#ifndef PING_ID
#define PING_ID              (0xAFAF)
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE       (32U)
#endif

/* ping variables */
static uint16_t ping_seq_num;
struct _ip_addr {
    uint8_t addr0, addr1, addr2, addr3;
};

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *) iecho)[sizeof(struct icmp_echo_hdr) + i] = (char) i;
    }

#if defined(CHECKSUM_GEN_ICMP) && CHECKSUM_GEN_ICMP
    inet_chksum(iecho, len);
#else
    iecho->chksum = 0;
#endif

}

/* Ping using the socket ip */
static err_t ping_send(int s, ip_addr_t *addr, int size)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    int ping_size = sizeof(struct icmp_echo_hdr) + size;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = pvPortMalloc(ping_size);
    if (iecho == NULL) {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (uint16_t) ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
#if LWIP_IPV4 && LWIP_IPV6
    to.sin_addr.s_addr = addr->u_addr.ip4.addr;
#elif LWIP_IPV4
    to.sin_addr.s_addr = addr->addr;
#elif LWIP_IPV6
#error Not supported IPv6.
#endif

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *) &to, sizeof(to));
    vPortFree(iecho);

    return (err == ping_size ? ERR_OK : ERR_VAL);
}


static int ping_recv(int s, int *ttl)
{
    char buf[64];
    int fromlen = sizeof(struct sockaddr_in), len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *) &from, (socklen_t *) &fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            iphdr = (struct ip_hdr *) buf;
            iecho = (struct icmp_echo_hdr *) (buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                *ttl = iphdr->_ttl;
                return len;
            }
        }
    }

    return len;
}

hpm_stat_t ping(char *target_name, uint32_t times, size_t size)
{
#if LWIP_VERSION_MAJOR >= 2U
    struct timeval timeout = { PING_RCV_TIMEO / configTICK_RATE_HZ, PING_RCV_TIMEO % configTICK_RATE_HZ };
#else
    int timeout = PING_RCV_TIMEO * 1000UL / configTICK_RATE_HZ;
#endif

    int s, ttl = 0, recv_len;
    ip_addr_t target_addr;
    uint32_t send_times;
    TickType_t recv_start_tick;
    struct addrinfo hint, *res = NULL;
    struct sockaddr_in *h = NULL;
    struct in_addr ina;
    err_t err;

    send_times = 0;
    ping_seq_num = 0;

    if (size == 0) {
        size = PING_DATA_SIZE;
    }

    memset(&hint, 0, sizeof(hint));
    /* convert URL to IP */
    if (lwip_getaddrinfo(target_name, NULL, &hint, &res) != 0) {
        printf("ping: unknown host %s\n", target_name);
        vTaskDelay(1000);
        return status_fail;
    }
    memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(&ina, &h->sin_addr, sizeof(ina));
    lwip_freeaddrinfo(res);
    if (inet_aton(inet_ntoa(ina), &target_addr) == 0) {
        printf("ping: unknown host %s\n", target_name);
        vTaskDelay(1000);
        return status_fail;
    }
    /* new a socket */
    s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
    if (s < 0) {
        printf("ping: create socket failed\n");
        vTaskDelay(1000);
        return status_fail;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    printf("Pinging %s [%s] ..................\n", target_name, inet_ntoa(ina));
    while (1) {
        int elapsed_time;
        err = ping_send(s, &target_addr, size);
        if (err == ERR_OK) {
            recv_start_tick = xTaskGetTickCount();
            recv_len = ping_recv(s, &ttl);
            if (recv_len >= 0) {
                elapsed_time = (xTaskGetTickCount() - recv_start_tick) * 1000UL / configTICK_RATE_HZ;
                printf("from %s bytes=%d icmp_seq=%d ttl=%d time=%d ms\n", inet_ntoa(ina), recv_len, send_times,
                        ttl, elapsed_time);
            } else {
                printf("From %s icmp_seq=%d timeout\n", inet_ntoa(ina), send_times);
            }
        } else {
            printf("Send %s - error\n", inet_ntoa(ina));
        }

        send_times++;
        if (send_times >= times) {
            /* send ping times reached, stop */
            break;
        }
        vTaskDelay(PING_DELAY); /* take a delay */
    }

    lwip_close(s);

    return status_success;
}

