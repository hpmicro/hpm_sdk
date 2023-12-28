/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NETCONF_H
#define NETCONF_H

/* Exported typedef ------------------------------------------------------------*/
typedef struct {
    uint32_t ip_addr0;
    uint32_t ip_addr1;
    uint32_t ip_addr2;
    uint32_t ip_addr3;
    uint32_t netmask0;
    uint32_t netmask1;
    uint32_t netmask2;
    uint32_t netmask3;
    uint32_t gw0;
    uint32_t gw1;
    uint32_t gw2;
    uint32_t gw3;
} ip_init_t;
typedef struct {
    uint8_t mac_addr0;
    uint8_t mac_addr1;
    uint8_t mac_addr2;
    uint8_t mac_addr3;
    uint8_t mac_addr4;
    uint8_t mac_addr5;
} mac_init_t;

/* Exported macro ------------------------------------------------------------*/
#define DHCP_START              1
#define DHCP_WAIT_ADDRESS       2
#define DHCP_ADDRESS_ASSIGNED   3
#define DHCP_TIMEOUT            4
#define DHCP_LINK_DOWN          5

/* MAC ADDRESS */
#define MAC0_ADDR0   0x98
#define MAC0_ADDR1   0x2C
#define MAC0_ADDR2   0xBC
#define MAC0_ADDR3   0xB1
#define MAC0_ADDR4   0x9F
#define MAC0_ADDR5   0x17

#define MAC1_ADDR0   0x98
#define MAC1_ADDR1   0x2C
#define MAC1_ADDR2   0xBC
#define MAC1_ADDR3   0xB1
#define MAC1_ADDR4   0x9F
#define MAC1_ADDR5   0x27

/* Static IP ADDRESS */
#define IP0_ADDR0    192
#define IP0_ADDR1    168
#define IP0_ADDR2    100
#define IP0_ADDR3    10

#define IP1_ADDR0    192
#define IP1_ADDR1    168
#define IP1_ADDR2    200
#define IP1_ADDR3    10

/* NETMASK */
#define NETMASK0_ADDR0   255
#define NETMASK0_ADDR1   255
#define NETMASK0_ADDR2   255
#define NETMASK0_ADDR3   0

#define NETMASK1_ADDR0   255
#define NETMASK1_ADDR1   255
#define NETMASK1_ADDR2   255
#define NETMASK1_ADDR3   0

/* Gateway Address */
#define GW0_ADDR0   192
#define GW0_ADDR1   168
#define GW0_ADDR2   100
#define GW0_ADDR3   1

#define GW1_ADDR0   192
#define GW1_ADDR1   168
#define GW1_ADDR2   200
#define GW1_ADDR3   1

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables---------------------------------------------------------*/
extern struct netif *gnetif[];
extern mac_init_t mac_init[];
/* Exported functions ------------------------------------------------------- */
void netif_config(uint8_t i);
void user_notification(struct netif *netif);
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
