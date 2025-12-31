/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ETHERNETIF_H
#define ETHERNETIF_H


#include "lwip/err.h"
#include "lwip/netif.h"
#ifdef __cplusplus
extern "C" {
#endif

void eoe_receive(uint16_t *pdata, uint16_t length);
void eoe_setip(uint16_t *mac_addr, uint16_t *ip_addr, uint16_t *subnet_mask, uint16_t *default_gateway, uint16_t *dns_ip);
void eoe_netif_config(struct netif *netif);

#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* ETHERNETIF_H */
