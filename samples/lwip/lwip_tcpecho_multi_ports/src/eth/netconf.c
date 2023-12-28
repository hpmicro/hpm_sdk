/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "board.h"
#include "netconf.h"
#include "netif/etharp.h"
#include "ethernetif.h"


static struct netif gnetif0, gnetif1;
struct netif *gnetif[] = {&gnetif0, &gnetif1};

ip_init_t ip_init[BOARD_ENET_COUNT] = {
    {IP0_ADDR0, IP0_ADDR1, IP0_ADDR2, IP0_ADDR3, NETMASK0_ADDR0, NETMASK0_ADDR1, NETMASK0_ADDR2, NETMASK0_ADDR3, GW0_ADDR0, GW0_ADDR1, GW0_ADDR2, GW0_ADDR3},
    {IP1_ADDR0, IP1_ADDR1, IP1_ADDR2, IP1_ADDR3, NETMASK1_ADDR0, NETMASK1_ADDR1, NETMASK1_ADDR2, NETMASK1_ADDR3, GW1_ADDR0, GW1_ADDR1, GW1_ADDR2, GW1_ADDR3}
};

mac_init_t mac_init[BOARD_ENET_COUNT] = {
    {MAC0_ADDR0, MAC0_ADDR1, MAC0_ADDR2, MAC0_ADDR3, MAC0_ADDR4, MAC0_ADDR5},
    {MAC1_ADDR0, MAC1_ADDR1, MAC1_ADDR2, MAC1_ADDR3, MAC1_ADDR4, MAC1_ADDR5}
};

void netif_config(uint8_t i)
{
    ip_addr_t ip_addr, netmask, gw;

    IP_ADDR4(&ip_addr, ip_init[i].ip_addr0, ip_init[i].ip_addr1, ip_init[i].ip_addr2, ip_init[i].ip_addr3);
    IP_ADDR4(&netmask, ip_init[i].netmask0, ip_init[i].netmask1, ip_init[i].netmask2, ip_init[i].netmask3);
    IP_ADDR4(&gw,      ip_init[i].gw0,      ip_init[i].gw1,      ip_init[i].gw2,      ip_init[i].gw3);
    netif_add(gnetif[i], &ip_addr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    if (netif_is_link_up(gnetif[i])) {
        netif_set_up(gnetif[i]);
    } else {
        netif_set_down(gnetif[i]);
    }

    netif_set_default(gnetif[i]);
}

void user_notification(struct netif *netif)
{
    if (netif_is_up(netif)) {
        printf("================ Network Interface %d ================\n", netif->num);
        printf("Static IP:  %d.%d.%d.%d\n", (uint8_t)(netif->ip_addr.addr), (uint8_t)(netif->ip_addr.addr >> 8), (uint8_t)(netif->ip_addr.addr >> 16), (uint8_t)(netif->ip_addr.addr >> 24));
        printf("Netmask  : %d.%d.%d.%d\n",  (uint8_t)(netif->netmask.addr), (uint8_t)(netif->netmask.addr >> 8), (uint8_t)(netif->netmask.addr >> 16), (uint8_t)(netif->netmask.addr >> 24));
        printf("Gateway  : %d.%d.%d.%d\n",  (uint8_t)(netif->gw.addr),      (uint8_t)(netif->gw.addr >> 8),      (uint8_t)(netif->gw.addr >> 16),      (uint8_t)(netif->gw.addr >> 24));
    } else {
        printf("The network interface card is not ready!\n");
    }
}
