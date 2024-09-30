/*
 * Copyright (c) 2023-2024 HPMicro
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
    {IP0_ADDR0, IP0_ADDR1, IP0_ADDR2, IP0_ADDR3, NETMASK0_ADDR0, NETMASK0_ADDR1, NETMASK0_ADDR2, NETMASK0_ADDR3, GW0_ADDR0, GW0_ADDR1, GW0_ADDR2, GW0_ADDR3},
    {IP1_ADDR0, IP1_ADDR1, IP1_ADDR2, IP1_ADDR3, NETMASK1_ADDR0, NETMASK1_ADDR1, NETMASK1_ADDR2, NETMASK1_ADDR3, GW1_ADDR0, GW1_ADDR1, GW1_ADDR2, GW1_ADDR3}
};

mac_init_t mac_init[BOARD_ENET_COUNT] = {
    {MAC0_ADDR0, MAC0_ADDR1, MAC0_ADDR2, MAC0_ADDR3, MAC0_ADDR4, MAC0_ADDR5},
    {MAC1_ADDR0, MAC1_ADDR1, MAC1_ADDR2, MAC1_ADDR3, MAC1_ADDR4, MAC1_ADDR5}
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
    ip_addr_t ip_addr, netmask, gw;

    IP_ADDR4(&ip_addr, ip_init[i].ip_addr0, ip_init[i].ip_addr1, ip_init[i].ip_addr2, ip_init[i].ip_addr3);
    IP_ADDR4(&netmask, ip_init[i].netmask0, ip_init[i].netmask1, ip_init[i].netmask2, ip_init[i].netmask3);
    IP_ADDR4(&gw,      ip_init[i].gw0,      ip_init[i].gw1,      ip_init[i].gw2,      ip_init[i].gw3);

#if defined(NO_SYS) && NO_SYS
    netif_add(netif, &ip_addr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
    netif_set_up(netif);
    netif_set_default(netif);
#else
    netifapi_netif_add(netif, &ip_addr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
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