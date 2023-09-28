/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"
#include "hpm_common.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "ethernetif.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TCPTimer;
uint32_t ARPTimer;
uint32_t IPaddress;

#if __ENABLE_DHCP
uint32_t DHCPfineTimer;
uint32_t DHCPcoarseTimer;
__IO uint8_t DHCP_state = DHCP_START;
#endif
extern __IO uint32_t  EthStatus;
struct netif gnetif;

/**
* @brief  Initializes the lwIP stack
* @param  None
* @retval None
*/
void LwIP_Init(void)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

#if __ENABLE_DHCP
    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

    netifapi_netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netifapi_netif_set_default(&gnetif);
    netifapi_netif_set_up(&gnetif);

#if __ENABLE_DHCP
    DHCP_state = DHCP_START;
#else
    printf("\r\n	Static IP address	\r\n");
    printf("IP: %d.%d.%d.%d\r\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    printf("NETMASK: %d.%d.%d.%d\r\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    printf("Gateway: %d.%d.%d.%d\r\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* __ENABLE_DHCP */
}

#if __ENABLE_DHCP
/**
* @brief  LwIP_DHCP_Process_Handle
* @param  None
* @retval None
*/
void LwIP_DHCP_task(void *pvParameters)
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
            dhcp_start(&gnetif);
            gnetif.ip_addr.addr = 0;
            /* IP address should be set to 0, and every time we want to assign a new DHCP address */
            IPaddress = 0;

            printf("    Looking for    \r\n");
            printf("    DHCP server    \r\n");
            printf("    Please wait... \r\n");
        }
        break;

        case DHCP_WAIT_ADDRESS: {
            /* Read the new IP address */
            IPaddress = gnetif.ip_addr.addr;

            if (IPaddress != 0) {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;

                iptab[0] = (uint8_t)(IPaddress >> 24);
                iptab[1] = (uint8_t)(IPaddress >> 16);
                iptab[2] = (uint8_t)(IPaddress >> 8);
                iptab[3] = (uint8_t)(IPaddress);

                sprintf((char *)iptxt, " %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

                /* Display the IP address */
                printf("IP address assigned by a DHCP server:\r\n");
                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.ip_addr.addr), (uint8_t)(gnetif.ip_addr.addr>>8), (uint8_t)(gnetif.ip_addr.addr>>16), (uint8_t)(gnetif.ip_addr.addr>>24));
                printf("IP addr: %s\r\n", iptxt);

                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.netmask.addr), (uint8_t)(gnetif.netmask.addr>>8), (uint8_t)(gnetif.netmask.addr>>16), (uint8_t)(gnetif.netmask.addr>>24));
                printf("Netmask addr: %s\r\n", iptxt);

                sprintf((char *)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.gw.addr), (uint8_t)(gnetif.gw.addr>>8), (uint8_t)(gnetif.gw.addr>>16), (uint8_t)(gnetif.gw.addr>>24));
                printf("GW addr: %s\r\n", iptxt);

                sprintf((char *)mactxt, " %02x-%02x-%02x-%02x-%02x-%02x", gnetif.hwaddr[0], gnetif.hwaddr[1], gnetif.hwaddr[2], gnetif.hwaddr[3], gnetif.hwaddr[4], gnetif.hwaddr[5]);
                printf("MAC addr: %s\r\n", mactxt);
            } else {
                struct dhcp *dhcp = netif_dhcp_data(&gnetif);
                /* DHCP timeout */
                if (dhcp->tries > MAX_DHCP_TRIES) {
                    DHCP_state = DHCP_TIMEOUT;

                    /* Stop DHCP */
                    dhcp_stop(&gnetif);

                    /* Static address used */
                    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);

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
        vTaskDelay(250);
    }
}
#endif

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
        printf("NETMASK  : %d.%d.%d.%d\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        printf("Gateway  : %d.%d.%d.%d\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    } else {
        printf("The network interface card is not ready!\n");
    }
}
