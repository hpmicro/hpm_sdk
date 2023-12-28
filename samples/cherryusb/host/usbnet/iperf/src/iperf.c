/*
 * Copyright (c) 2023 HPMicro
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
#include "lwip/apps/lwiperf.h"

#include "hpm_common.h"

bool iperf_complete;

static hpm_stat_t url_convert_ip(char *target_name, ip_addr_t *target_addr)
{
    struct addrinfo hint, *res = NULL;
    struct sockaddr_in *h = NULL;
    struct in_addr ina;

    if ((target_name == NULL) || (target_addr == NULL)) {
        return status_invalid_argument;
    }

    memset(&hint, 0, sizeof(hint));
    if (lwip_getaddrinfo(target_name, NULL, &hint, &res) != 0) {
        printf("url: unknown host %s\n", target_name);
        return status_fail;
    }

    memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(&ina, &h->sin_addr, sizeof(ina));

    if (inet_aton(inet_ntoa(ina), target_addr) == 0) {
        printf("it's not url, continue convert ip %s\n", target_name);
        if (inet_aton(target_name, &target_addr) == 0) {
            printf("url: unknown host %s\n", target_name);
            return status_fail;
        }
    }
    lwip_freeaddrinfo(res);
    return status_success;
}

static void lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t *local_addr, uint16_t local_port, const ip_addr_t *remote_addr, uint16_t remote_port,
  uint32_t bytes_transferred, uint32_t ms_duration, uint32_t bandwidth_kbitpsec)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(local_addr);
    LWIP_UNUSED_ARG(local_port);

    LWIP_PLATFORM_DIAG(("iperf report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\n\n",
        (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec));
    iperf_complete = true;
}

hpm_stat_t iperf(char *target_name, uint16_t port)
{
    hpm_stat_t stat = status_success;
    ip_addr_t remote_addr;
    iperf_complete = false;
    stat = url_convert_ip(target_name, &remote_addr);
    if (stat != status_success) {
        return stat;
    }
    if (lwiperf_start_tcp_client(&remote_addr, port, LWIPERF_CLIENT, lwiperf_report, NULL) == NULL) {
        return status_fail;
    }
    while (iperf_complete == false) {
        vTaskDelay(50);
    }
    iperf_complete = false;
    return stat;
}
