/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NETCONF_H
#define NETCONF_H

/* Exported macro ------------------------------------------------------------*/
#define DHCP_START              1
#define DHCP_WAIT_ADDRESS       2
#define DHCP_ADDRESS_ASSIGNED   3
#define DHCP_TIMEOUT            4
#define DHCP_LINK_DOWN          5

/* MAC ADDRESS */
#define MAC_ADDR0   0x98
#define MAC_ADDR1   0x2C
#define MAC_ADDR2   0xBC
#define MAC_ADDR3   0xB1
#define MAC_ADDR4   0x9F
#define MAC_ADDR5   0x17

/* Static IP ADDRESS */
#define IP_ADDR0    10
#define IP_ADDR1    10
#define IP_ADDR2    10
#define IP_ADDR3    10

/* NETMASK */
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/* Gateway Address */
#define GW_ADDR0   10
#define GW_ADDR1   10
#define GW_ADDR2   10
#define GW_ADDR3   1

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables---------------------------------------------------------*/
extern struct netif gnetif;
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
void netif_config(void);
#ifdef USE_DHCP
void LwIP_DHCP_task(void *pvParameters);
#endif
void user_notification(struct netif *netif);
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
