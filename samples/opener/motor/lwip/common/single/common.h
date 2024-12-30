/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef COMMON_LWIP_H
#define COMMON_LWIP_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "common_cfg.h"
#include "lwip/netif.h"
#include "hpm_enet_phy_common.h"

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#else
#include "osal.h"
#endif
#endif /* defined(NO_SYS) && !NO_SYS */

/* Exported Macros------------------------------------------------------------*/
typedef enum {
    ENET_MAC_ADDR_PARA_ERROR    = -1,
    ENET_MAC_ADDR_FROM_OTP_MAC,
    ENET_MAC_ADDR_FROM_OTP_UUID,
    ENET_MAC_ADDR_FROM_MACRO
} enet_mac_addr_t;

#define IS_UUID_INVALID(UUID)  (UUID[0] == 0 && \
                                UUID[1] == 0 && \
                                UUID[2] == 0 && \
                                UUID[3] == 0)

#define IS_MAC_INVALID(MAC) (MAC[0] == 0 && \
                             MAC[1] == 0 && \
                             MAC[2] == 0 && \
                             MAC[3] == 0 && \
                             MAC[4] == 0 && \
                             MAC[5] == 0)

#ifndef LWIP_APP_TIMER_INTERVAL
#define LWIP_APP_TIMER_INTERVAL (2 * 1000U)  /* 2 * 1000 ms */
#endif /* LWIP_APP_TIMER_INTERVAL  */

#define LWIP_NETIF_IDX  (1)

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t enet_get_mac_address(uint8_t *mac);
bool enet_get_link_status(void);
void enet_self_adaptive_port_speed(void);
void enet_services(struct netif *netif);
void enet_common_handler(struct netif *netif);

#if defined(LWIP_DHCP) && LWIP_DHCP
void enet_update_dhcp_state(struct netif *netif);
#endif

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer);
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
void timer_callback(void *parameter);
#endif

#else
void sys_timer_callback(void);
#endif /* defined(NO_SYS) && !NO_SYS */

#if defined __cplusplus
}
#endif /* __cplusplus */

#endif /* COMMON_LWIP_H */