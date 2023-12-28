/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "netconf.h"

#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include "netif/etharp.h"
#include "ethernetif.h"

#if 0
#include "FreeRTOS.h"
#include "task.h"
#else
#include "rtthread.h"
#endif

/* Private macro -------------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private variables ---------------------------------------------------------*/
struct netif gnetif;

#if LWIP_DHCP
uint8_t DHCP_state = DHCP_START;
#endif

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

#if LWIP_DHCP
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

#if LWIP_DHCP
    DHCP_state = DHCP_START;
#else
    printf("\r\nStatic IP address\r\n");
    printf("IP       : %d.%d.%d.%d\r\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    printf("Netmask  : %d.%d.%d.%d\r\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    printf("Gateway  : %d.%d.%d.%d\r\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* LWIP_DHCP */
}

#if LWIP_DHCP
/**
* @brief  LwIP_DHCP_Process_Handle
* @param  None
* @retval None
*/
void LwIP_DHCP_task(void *pvParameters)
{
    (void)pvParameters;

    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    for (;;) {
        switch (DHCP_state) {
        case DHCP_START: {
            DHCP_state = DHCP_WAIT_ADDRESS;
            dhcp_start(&gnetif);
            gnetif.ip_addr.addr = 0;

            printf("\r\nLooking for DHCP server... \r\n");
            printf("Please wait... \r\n");
        }
        break;

        case DHCP_WAIT_ADDRESS: {
            /* Read the new IP address */
            if (gnetif.ip_addr.addr != 0) {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;

                /* Dynamic IP */
                printf("Dynamic IP address\r\n");
                printf("IPv4 Address     : %s\n", ipaddr_ntoa(&gnetif.ip_addr));
                printf("IPv4 Subnet mask : %s\n", ipaddr_ntoa(&gnetif.netmask));
                printf("IPv4 Gateway     : %s\n\n", ipaddr_ntoa(&gnetif.gw));

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
                    netifapi_netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);

                    printf("DHCP timeout    \r\n");

                    /* Static IP */
                    printf("Static IP address   \r\n");
                    printf("IPv4 Address     : %s\n", ipaddr_ntoa(&gnetif.ip_addr));
                    printf("IPv4 Subnet mask : %s\n", ipaddr_ntoa(&gnetif.netmask));
                    printf("IPv4 Gateway     : %s\n\n", ipaddr_ntoa(&gnetif.gw));
                }
            }
        }
        break;

        default:
        break;
        }
        rt_thread_mdelay(250);
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
        printf("Netmask  : %d.%d.%d.%d\n", NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        printf("Gateway  : %d.%d.%d.%d\n", GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    } else {
        printf("The network interface card is not ready!\n");
    }
}
