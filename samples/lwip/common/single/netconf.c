/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"

#if defined(LWIP_DHCP) && LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "common.h"

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "rtthread.h"
#endif

/* Public variables ---------------------------------------------------------*/
#if defined(NO_SYS) && !NO_SYS
sys_mbox_t netif_status_mbox;
#endif

#if defined(LWIP_DHCP) && LWIP_DHCP
/**
* @brief  LwIP_DHCP_Process_Handle
* @param  parameter the parameter of thread enter function
* @retval None
*/
void LwIP_DHCP_task(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;

    dhcp_start(netif);

    for (;;) {
        enet_update_dhcp_state(netif);
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
        vTaskDelay(250);
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_thread_mdelay(250);
#endif
    }
}
#endif

static void netif_update_status(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
#if defined(LWIP_DHCP) && !LWIP_DHCP
        netif_user_notification(netif);
#endif
    }
}

void netif_config(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#if defined(LWIP_DHCP) && LWIP_DHCP
    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);
#else
    ip4addr_aton(HPM_STRINGIFY(IP_CONFIG), &ipaddr);
    ip4addr_aton(HPM_STRINGIFY(NETMASK_CONFIG), &netmask);
    ip4addr_aton(HPM_STRINGIFY(GW_CONFIG), &gw);
#endif

#if defined(NO_SYS) && NO_SYS
    netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
    netif_set_up(netif);
    netif_set_default(netif);
#else
    netifapi_netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netifapi_netif_set_up(netif);
    netifapi_netif_set_default(netif);
#endif

    netif_set_link_callback(netif, netif_update_status);
}

void netif_user_notification(struct netif *netif)
{
    if (netif_is_up(netif)) {
        printf("IPv4 Address: %s\n", ipaddr_ntoa(&netif->ip_addr));
        printf("IPv4 Netmask: %s\n", ipaddr_ntoa(&netif->netmask));
        printf("IPv4 Gateway: %s\n", ipaddr_ntoa(&netif->gw));
    }
}

#if defined(NO_SYS) && !NO_SYS
void netif_update_link_status(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;
    uint32_t *msg;

    sys_mbox_new(&netif_status_mbox, SYS_LWIP_MBOX_SIZE);

    for (;;) {
        sys_arch_mbox_fetch(&netif_status_mbox, (void **)&msg, SYS_LWIP_TIMEOUT_FOREVER);

        if (*msg == enet_phy_link_up) {
            netif_set_link_up(netif);
        } else {
            netif_set_link_down(netif);
        }
    }
}
#endif