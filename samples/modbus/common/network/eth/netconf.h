/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NETCONF_H
#define NETCONF_H

#include "stdint.h"

/* MAC ADDRESS */
#define MAC_ADDR0   0x98
#define MAC_ADDR1   0x2C
#define MAC_ADDR2   0xBC
#define MAC_ADDR3   0xB1
#define MAC_ADDR4   0x9F
#define MAC_ADDR5   0x17

/* Static IP ADDRESS */
#define IP_ADDR0    192
#define IP_ADDR1    168
#define IP_ADDR2    100
#ifdef MODBUS_TCP_SLAVE
#define IP_ADDR3    10
#else
#define IP_ADDR3    11
#endif
/* NETMASK */
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/* Gateway Address */
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   100
#define GW_ADDR3   1

/* Remote IP Address */
#define REMOTE_IP_ADDR0 192
#define REMOTE_IP_ADDR1 168
#define REMOTE_IP_ADDR2 100
#define REMOTE_IP_ADDR3 10

#define TCP_SERVER_PORT          (5001U)
#define NETWORK_BUFFER_SIZE      (4096U)

typedef struct {
    uint32_t front_index;
    uint32_t rear_index;
} network_rx_t;

#ifdef __cplusplus
extern "C" {
#endif
/* Exported variables---------------------------------------------------------*/
extern struct netif gnetif;
/* Exported functions ------------------------------------------------------- */
void netif_config(void);
void user_notification(struct netif *netif);
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* NETCONF_H */
