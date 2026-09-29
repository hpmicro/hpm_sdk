/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "common_lwip.h"


void netif_config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    enet_netif_state_bind(0, ENET);
    netif_add(&gnetif, &ipaddr, &netmask, &gw, &enet_netif_state[0], &ethernetif_init, &ethernet_input);

    netif_set_default(&gnetif);

    /*
     * Administrative and physical link states are independent in lwIP.
     * The PHY adaptive handler updates the latter after auto-negotiation.
     */
    netif_set_up(&gnetif);
}

void netif_show_ip_info(struct netif *netif)
{
    printf("IPv4 Address: %s\n", ipaddr_ntoa(&netif->ip_addr));
    printf("IPv4 Netmask: %s\n", ipaddr_ntoa(&netif->netmask));
    printf("IPv4 Gateway: %s\n", ipaddr_ntoa(&netif->gw));
}

void user_notification(struct netif *netif)
{
    if (netif_is_up(netif)) {
        netif_show_ip_info(netif);
    } else {
        printf("The network interface card is not ready!\n");
    }
}
