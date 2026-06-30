/*
 * Copyright (c) 2025-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef LWIPOPTS_APP_H
#define LWIPOPTS_APP_H

#include "hpm_common.h"

/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#ifndef LWIP_SOCKET
#define LWIP_SOCKET 1
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable SO_RCVTIMEO for recv/recvfrom timeout.
 * Required for ping to avoid blocking indefinitely when a reply is lost.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO 1
#endif

/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#ifndef LWIP_DNS
#define LWIP_DNS 1
#endif

/*
 * DNS Server Config (for static IP mode, LWIP_DHCP=0)
 * Default 114.114.114.114 for China. Override DNS_SERVER_CONFIG before include if needed.
 * Tip: If MCU fails to resolve some domains while PC works, try gateway (e.g. 192.168.100.1)
 *      as DNS. When PC's ipconfig shows DNS=gateway (common for home routers), the gateway
 *      relays DNS. If PC uses a different DNS IP, use that same IP for MCU.
 *      Verify: ipconfig (Windows), resolv.conf (Linux), scutil (macOS).
 */
#if !LWIP_DHCP
#ifndef DNS_SERVER_CONFIG
#define DNS_SERVER_CONFIG 114.114.114.114
#endif
#define DNS_SERVER_ADDRESS(pdnsserver) ip4addr_aton(HPM_STRINGIFY(DNS_SERVER_CONFIG), pdnsserver)
#endif

/*
 * To use this feature let the following define uncommented.
 * To disable it and process by CPU comment the checksum.
*/
#define CHECKSUM_BY_HARDWARE 1

/*
 * Debug Options
*/
#define LWIP_DEBUG                 1
#define LWIP_DBG_MIN_LEVEL         0
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define ETHARP_DEBUG               LWIP_DBG_OFF

#endif /* LWIPOPTS_APP_H */
