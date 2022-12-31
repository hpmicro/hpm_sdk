/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef COMMON_H
#define COMMON_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "common_cfg.h"
#include "lwip/netif.h"
#include "hpm_enet_phy_common.h"

/* Exported Macros------------------------------------------------------------*/
#ifndef LWIP_APP_TIMER_INTERVAL
#define LWIP_APP_TIMER_INTERVAL (2 * 1000U)  /* 2 * 1000 ms */
#endif

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

void enet_get_mac_address(uint8_t *mac);
bool enet_get_link_status(void);
void enet_self_adaptive_port_speed(void);
void enet_services(struct netif *netif);
void enet_common_handler(struct netif *netif);

#if defined __cplusplus
}
#endif /* __cplusplus */

#endif