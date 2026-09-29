/*
 * Copyright (c) 2025-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef LWIPOPTS_APP_H
#define LWIPOPTS_APP_H

#include "common_cfg.h"

/*
 * Zero-copy TX holds each pbuf until DMA completes (up to one per TX descriptor).
 * Keep PBUF_POOL_SIZE >= ENET_TX_BUFF_COUNT so TX hold does not starve the pbuf pool.
 * RX zero-copy uses a separate custom pbuf pool sized by ENET_RX_BUFF_COUNT.
 */
#define PBUF_POOL_SIZE          (ENET_TX_BUFF_COUNT)

/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#ifndef LWIP_SOCKET
#define LWIP_SOCKET 1
#endif

/*
 * To use this feature let the following define uncommented.
 * To disable it and process by CPU comment the checksum.
*/
#define CHECKSUM_BY_HARDWARE

/*
 * ENET TX HW CRC mode (see enet_tx_hw_crc_mode_t in hpm_enet_drv.h):
 * 0 = enet_tx_hw_crc_append, 1 = enet_tx_hw_crc_replace, 2 = enet_tx_hw_crc_off
 */
#ifndef ENET_TX_HW_CRC_MODE
#define ENET_TX_HW_CRC_MODE 1
#endif

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
