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
void netif_show_ip_info(struct netif *netif);

#if defined(NO_SYS) && !NO_SYS
void netif0_update_link_status(void *pvParameters);
void netif1_update_link_status(void *pvParameters);
#endif
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
