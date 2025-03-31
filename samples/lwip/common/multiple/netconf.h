/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NETCONF_H
#define NETCONF_H

#include "lwipopts.h"
#include "sys_arch.h"
#include "lwip/netif.h"

/* Exported typedef ------------------------------------------------------------*/
typedef struct {
    char *ip_addr;
    char *netmask;
    char *gw;
} ip_init_t;

typedef struct {
    char *mac_addr;
} mac_init_t;

/* MAC ADDRESS */
#ifndef MAC0_CONFIG
#define MAC0_CONFIG 98:2c:bc:b1:9f:27
#endif

#ifndef MAC1_CONFIG
#define MAC1_CONFIG 98:2c:bc:b1:9f:37
#endif

/* Static IP ADDRESS */
#ifndef IP0_CONFIG
#define IP0_CONFIG 192.168.100.10
#endif

#ifndef IP1_CONFIG
#define IP1_CONFIG 192.168.200.10
#endif

/* Netmask */
#ifndef NETMASK0_CONFIG
#define NETMASK0_CONFIG 255.255.255.0
#endif

#ifndef NETMASK1_CONFIG
#define NETMASK1_CONFIG 255.255.255.0
#endif

/* Gateway Address */
#ifndef GW0_CONFIG
#define GW0_CONFIG 192.168.100.1
#endif

#ifndef GW1_CONFIG
#define GW1_CONFIG 192.168.200.1
#endif

/* Remote IP Address */
#ifndef REMOTE_IP0_CONFIG
#define REMOTE_IP0_CONFIG 192.168.100.5
#endif

#ifndef REMOTE_IP1_CONFIG
#define REMOTE_IP1_CONFIG 192.168.200.5
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables---------------------------------------------------------*/
#if defined(NO_SYS) && !NO_SYS
extern sys_mbox_t netif_status_mbox[];
#endif

extern mac_init_t mac_init[];
/* Exported functions ------------------------------------------------------- */
#if defined(LWIP_DHCP) && LWIP_DHCP
void LwIP_DHCP_task(void *pvParameters);
#endif

void netif_config(struct netif *netif, uint8_t i);
void netif_user_notification(struct netif *netif);

#if defined(NO_SYS) && !NO_SYS
void netif0_update_link_status(void *pvParameters);
void netif1_update_link_status(void *pvParameters);
#endif
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
