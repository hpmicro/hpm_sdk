/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NETCONF_H
#define NETCONF_H

#include "lwipopts.h"
#include "sys_arch.h"
#include "lwip/netif.h"

/* MAC Address */
#ifndef MAC_ADDR0
#define MAC_ADDR0 0x98
#endif

#ifndef MAC_ADDR1
#define MAC_ADDR1 0x2C
#endif

#ifndef MAC_ADDR2
#define MAC_ADDR2 0xBC
#endif

#ifndef MAC_ADDR3
#define MAC_ADDR3 0xB1
#endif

#ifndef MAC_ADDR4
#define MAC_ADDR4 0x9F
#endif

#ifndef MAC_ADDR5
#define MAC_ADDR5 0x17
#endif

/* Static IP Address */
#ifndef IP_ADDR0
#define IP_ADDR0 192
#endif

#ifndef IP_ADDR1
#define IP_ADDR1 168
#endif

#ifndef IP_ADDR2
#define IP_ADDR2 100
#endif

#ifndef IP_ADDR3
#define IP_ADDR3 10
#endif

/* Netmask */
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0 255
#endif

#ifndef NETMASK_ADDR1
#define NETMASK_ADDR1 255
#endif

#ifndef NETMASK_ADDR2
#define NETMASK_ADDR2 255
#endif

#ifndef NETMASK_ADDR3
#define NETMASK_ADDR3 0
#endif

/* Gateway Address */
#ifndef GW_ADDR0
#define GW_ADDR0 192
#endif

#ifndef GW_ADDR1
#define GW_ADDR1 168
#endif

#ifndef GW_ADDR2
#define GW_ADDR2 100
#endif

#ifndef GW_ADDR3
#define GW_ADDR3 1
#endif

/* Remote IP Address */
#ifndef REMOTE_IP_ADDR0
#define REMOTE_IP_ADDR0 192
#endif

#ifndef REMOTE_IP_ADDR1
#define REMOTE_IP_ADDR1 168
#endif

#ifndef REMOTE_IP_ADDR2
#define REMOTE_IP_ADDR2 100
#endif

#ifndef REMOTE_IP_ADDR3
#define REMOTE_IP_ADDR3 5
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables---------------------------------------------------------*/
#if defined(NO_SYS) && !NO_SYS
extern sys_mbox_t netif_status_mbox;
#endif

/* Exported functions ------------------------------------------------------- */

#if defined(LWIP_DHCP) && LWIP_DHCP
void LwIP_DHCP_task(void *pvParameters);
#endif

void netif_config(struct netif *netif);
void netif_user_notification(struct netif *netif);

#if defined(NO_SYS) && !NO_SYS
void netif_update_link_status(void *pvParameters);
#endif

#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
