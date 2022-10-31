#ifndef COMMON_H
#define COMMON_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "common_cfg.h"
#include "lwip/netif.h"

/* Exported Macros------------------------------------------------------------*/
#ifndef LWIP_APP_TIMER_INTERVAL
#define LWIP_APP_TIMER_INTERVAL (2 * 1000U)  /* 2 * 1000 ms */
#endif

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

bool enet_get_link_status(void);
void enet_self_adaptive_port_speed(void);
void enet_services(struct netif *netif);
void enet_common_handler(struct netif *netif);

#if defined __cplusplus
}
#endif /* __cplusplus */

#endif