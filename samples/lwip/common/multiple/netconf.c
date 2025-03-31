/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "board.h"
#include "netconf.h"
#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "common.h"

#if defined(LWIP_DHCP) && LWIP_DHCP
#include "lwip/dhcp.h"
#endif

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "rtthread.h"
#endif

#if defined(NO_SYS) && !NO_SYS
sys_mbox_t netif_status_mbox[BOARD_ENET_COUNT];
#endif

ip_init_t ip_init[BOARD_ENET_COUNT] = {
    {HPM_STRINGIFY(IP0_CONFIG), HPM_STRINGIFY(NETMASK0_CONFIG), HPM_STRINGIFY(GW0_CONFIG)},
    {HPM_STRINGIFY(IP1_CONFIG), HPM_STRINGIFY(NETMASK1_CONFIG), HPM_STRINGIFY(GW1_CONFIG)}
};

mac_init_t mac_init[BOARD_ENET_COUNT] = {
    {HPM_STRINGIFY(MAC0_CONFIG)},
    {HPM_STRINGIFY(MAC1_CONFIG)}
};


#if defined(LWIP_DHCP) && LWIP_DHCP
/**
* @brief  LwIP_DHCP_Process_Handle
* @param  parameter the parameter of thread enter function
* @retval None
*/
void LwIP_DHCP_task(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;

    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        dhcp_start(&netif[i]);
    }

    for (;;) {
        for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
            enet_update_dhcp_state(&netif[i]);
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
            vTaskDelay(5);
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
            rt_thread_mdelay(5);
#endif
        }
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

void netif_config(struct netif *netif, uint8_t i)
{
    ip_addr_t ipaddr, netmask, gw;

    ip4addr_aton(ip_init[i].ip_addr, &ipaddr);
    ip4addr_aton(ip_init[i].netmask, &netmask);
    ip4addr_aton(ip_init[i].gw, &gw);

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
        printf("================ Network Interface %d ================\n", netif->num);
        printf("IPv4 Address: %s\n", ipaddr_ntoa(&netif->ip_addr));
        printf("IPv4 Netmask: %s\n", ipaddr_ntoa(&netif->netmask));
        printf("IPv4 Gateway: %s\n", ipaddr_ntoa(&netif->gw));
    }
}

#if defined(NO_SYS) && !NO_SYS
void netif0_update_link_status(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;
    uint32_t *msg;

    sys_mbox_new(&netif_status_mbox[0], SYS_LWIP_MBOX_SIZE);

    for (;;) {
        sys_arch_mbox_fetch(&netif_status_mbox[0], (void **)&msg, SYS_LWIP_TIMEOUT_FOREVER);
        if (*msg == enet_phy_link_up) {
            netif_set_link_up(netif);
        } else {
            netif_set_link_down(netif);
        }
    }
}

void netif1_update_link_status(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;
    uint32_t *msg;

    sys_mbox_new(&netif_status_mbox[1], SYS_LWIP_MBOX_SIZE);

    for (;;) {
        sys_arch_mbox_fetch(&netif_status_mbox[1], (void **)&msg, SYS_LWIP_TIMEOUT_FOREVER);
        if (*msg == enet_phy_link_up) {
            netif_set_link_up(netif);
        } else {
            netif_set_link_down(netif);
        }
    }
}
#endif