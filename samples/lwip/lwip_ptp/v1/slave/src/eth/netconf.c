/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"
#include "netif/etharp.h"
#include "ethernetif.h"

struct netif gnetif;

void netif_config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif)) {
        netif_set_up(&gnetif);
    } else {
        netif_set_down(&gnetif);
    }
}

void user_notification(struct netif *netif)
{
    if (netif_is_up(netif)) {
        printf("Static IP: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        printf("Netmask  : %d.%d.%d.%d\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        printf("Gateway  : %d.%d.%d.%d\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    } else {
        printf("The network interface card is not ready!\n");
    }
}
