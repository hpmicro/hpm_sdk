/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "netconf.h"
#include "board.h"

/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TCPTimer;
uint32_t ARPTimer;
uint32_t IPaddress;

#ifdef __ENABLE_DHCP
uint32_t DHCPfineTimer;
uint32_t DHCPcoarseTimer;
__IO uint8_t DHCP_state = DHCP_START;
#endif

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

#ifdef __ENABLE_DHCP
void lwip_dhcp(void)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
    uint8_t iptab[4] = {0};
    uint8_t iptxt[20];
    uint8_t mactxt[40];

    for (;;) {
        switch (DHCP_state) {
        case DHCP_START: {
            DHCP_state = DHCP_WAIT_ADDRESS;
            dhcp_start(&gnetif0);
            gnetif0.ip_addr.addr = 0;
            /* IP address should be set to 0, and every time we want to assign a new DHCP address */
            IPaddress = 0;

            printf("    Looking for    \r\n");
            printf("    DHCP server    \r\n");
            printf("    Please wait... \r\n");
        }
        break;

        case DHCP_WAIT_ADDRESS: {
            /* Read the new IP address */
            IPaddress = gnetif0.ip_addr.addr;

            if (IPaddress != 0) {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;

                iptab[0] = (uint8_t)(IPaddress >> 24);
                iptab[1] = (uint8_t)(IPaddress >> 16);
                iptab[2] = (uint8_t)(IPaddress >> 8);
                iptab[3] = (uint8_t)(IPaddress);

                sprintf((char *)iptxt, " %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

                /* Display the IP address */
                printf("IP address assigned by a DHCP server:\r\n");
                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif0.ip_addr.addr), (uint8_t)(gnetif0.ip_addr.addr>>8), (uint8_t)(gnetif0.ip_addr.addr>>16), (uint8_t)(gnetif0.ip_addr.addr>>24));
                printf("IP addr: %s\r\n", iptxt);

                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif0.netmask.addr), (uint8_t)(gnetif0.netmask.addr>>8), (uint8_t)(gnetif0.netmask.addr>>16), (uint8_t)(gnetif0.netmask.addr>>24));
                printf("Netmask addr: %s\r\n", iptxt);

                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif0.gw.addr), (uint8_t)(gnetif0.gw.addr>>8), (uint8_t)(gnetif0.gw.addr>>16), (uint8_t)(gnetif0.gw.addr>>24));
                printf("GW addr: %s\r\n", iptxt);

                sprintf((char *)mactxt, " %02x-%02x-%02x-%02x-%02x-%02x", gnetif0.hwaddr[0], gnetif0.hwaddr[1], gnetif0.hwaddr[2], gnetif0.hwaddr[3], gnetif0.hwaddr[4], gnetif0.hwaddr[5]);
                printf("MAC addr: %s\r\n", mactxt);
            } else {
                struct dhcp *dhcp = netif_dhcp_data(&gnetif0);
                /* DHCP timeout */
                if (dhcp->tries > MAX_DHCP_TRIES) {
                    DHCP_state = DHCP_TIMEOUT;

                    /* Stop DHCP */
                    dhcp_stop(&gnetif0);

                    /* Static address used */
                    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(&gnetif0, &ipaddr, &netmask, &gw);

                    printf("    DHCP timeout    \r\n");

                    iptab[0] = IP_ADDR3;
                    iptab[1] = IP_ADDR2;
                    iptab[2] = IP_ADDR1;
                    iptab[3] = IP_ADDR0;

                    sprintf((char *)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

                    printf("  Static IP address   \r\n");
                    printf("%s\r\n", iptxt);
                }
            }
        }
        break;

        default:
        break;
        }
    }
}
#endif

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
