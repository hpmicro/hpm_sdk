/*
 * Copyright (c) 2021 hpmicro
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
#include <stdio.h>
#include "hpm_common.h"

/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES        4

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0;
uint32_t IPaddress = 0;

#ifdef USE_DHCP
uint32_t DHCPfineTimer = 0;
uint32_t DHCPcoarseTimer = 0;
__IO uint8_t DHCP_state = DHCP_START;
#endif
extern __IO uint32_t  EthStatus;
struct netif gnetif;

#ifdef USE_DHCP
void lwip_dhcp(void)
{
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  uint8_t iptab[4] = {0};
  uint8_t iptxt[20];
  uint8_t mactxt[40];

  for (;;)
  {
	  switch (DHCP_state)
	  {
	  case DHCP_START:
	    {
	      DHCP_state = DHCP_WAIT_ADDRESS;
	      dhcp_start(&gnetif);
        gnetif.ip_addr.addr = 0;
	      /* IP address should be set to 0
	         every time we want to assign a new DHCP address */
	      IPaddress = 0;

	      printf("     Looking for    \r\n");
	      printf("     DHCP server    \r\n");
	      printf("     please wait... \r\n");
	    }
	    break;

	  case DHCP_WAIT_ADDRESS:
	    {
	      /* Read the new IP address */
	      IPaddress = gnetif.ip_addr.addr;

	      if (IPaddress!=0)
	      {
	        DHCP_state = DHCP_ADDRESS_ASSIGNED;

	        iptab[0] = (uint8_t)(IPaddress >> 24);
	        iptab[1] = (uint8_t)(IPaddress >> 16);
	        iptab[2] = (uint8_t)(IPaddress >> 8);
	        iptab[3] = (uint8_t)(IPaddress);

	        sprintf((char*)iptxt, " %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

	        /* Display the IP address */
	        printf("IP address assigned by a DHCP server:\r\n");
          sprintf((char*)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.ip_addr.addr), (uint8_t)(gnetif.ip_addr.addr>>8), (uint8_t)(gnetif.ip_addr.addr>>16), (uint8_t)(gnetif.ip_addr.addr>>24));
          printf("IP addr: %s\r\n", iptxt);

          sprintf((char*)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.netmask.addr), (uint8_t)(gnetif.netmask.addr>>8), (uint8_t)(gnetif.netmask.addr>>16), (uint8_t)(gnetif.netmask.addr>>24));
          printf("Netmask addr: %s\r\n", iptxt);

          sprintf((char*)iptxt, " %d.%d.%d.%d", (uint8_t)(gnetif.gw.addr), (uint8_t)(gnetif.gw.addr>>8), (uint8_t)(gnetif.gw.addr>>16), (uint8_t)(gnetif.gw.addr>>24));
          printf("GW addr: %s\r\n", iptxt);

          sprintf((char*)mactxt, " %02x-%02x-%02x-%02x-%02x-%02x", gnetif.hwaddr[0], gnetif.hwaddr[1], gnetif.hwaddr[2], gnetif.hwaddr[3], gnetif.hwaddr[4], gnetif.hwaddr[5]);
          printf("MAC addr: %s\r\n", mactxt);
          break;
	      }
	      else
	      {
		    struct dhcp *dhcp = netif_dhcp_data(&gnetif);
	        /* DHCP timeout */
	        if (dhcp->tries > MAX_DHCP_TRIES)
	        {
	          DHCP_state = DHCP_TIMEOUT;

	          /* Stop DHCP */
	          dhcp_stop(&gnetif);

	          /* Static address used */
	          IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
	          IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	          IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	          netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);

	          printf("    DHCP timeout    \r\n");

	          iptab[0] = IP_ADDR3;
	          iptab[1] = IP_ADDR2;
	          iptab[2] = IP_ADDR1;
	          iptab[3] = IP_ADDR0;

	          sprintf((char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

	          printf("  Static IP address   \r\n");
	          printf("%s\r\n", iptxt);
	        }
	      }
	    }
	    break;

	  default: break;
	  }
   }
}
#endif

void netif_config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);

  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    netif_set_up(&gnetif);
  }
  else
  {
    netif_set_down(&gnetif);
  }
}

void user_notification(struct netif *netif)
{
  if (netif_is_up(netif))
  {
    printf("Static IP: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
    printf("NETMASK  : %d.%d.%d.%d\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
    printf("Gateway  : %d.%d.%d.%d\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
  }
  else
  {
    printf ("The network cable is not connected \n");
  }
}
