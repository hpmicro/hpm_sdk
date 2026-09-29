/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef COMMON_LWIP_H
#define COMMON_LWIP_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "common_cfg.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "hpm_l1c_drv.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_phy_common.h"

/* ENET_TX_HW_CRC_MODE: enet_tx_hw_crc_append / replace / off (see hpm_enet_drv.h) */
#ifndef ENET_TX_HW_CRC_MODE
#define ENET_TX_HW_CRC_MODE enet_tx_hw_crc_replace
#endif

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "osal.h"
#define IDLE_TASK_PRIO      (RT_THREAD_PRIORITY_MAX - 1)
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

#define LWIP_NETIF_COUNT BOARD_ENET_COUNT

#ifndef ENET_TX_BUFF_COUNT
#define ENET_TX_BUFF_COUNT  (10U)
#endif

#ifndef ENET_RX_BUFF_COUNT
#define ENET_RX_BUFF_COUNT  (20U)
#endif

#ifndef ENET_TX_BUFF_SIZE
#define ENET_TX_BUFF_SIZE   (1536U)
#endif

#ifndef ENET_RX_BUFF_SIZE
#define ENET_RX_BUFF_SIZE   (1536U)
#endif

typedef ENET_Type enet_base_t;

typedef struct {
    int idx;
    enet_frame_t frame[ENET_RX_BUFF_COUNT];
    enet_desc_t *desc;
    ENET_Type *base;
} enet_netif_state_t;

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

extern struct netif gnetif[];
extern uint8_t mac[][ENET_MAC_SIZE];
extern enet_desc_t desc[];
extern enet_netif_state_t enet_netif_state[];

#if __ENABLE_ENET_RECEIVE_INTERRUPT
extern volatile bool rx_flag[];
#endif

hpm_stat_t enet_init(uint8_t idx);
int8_t enet_get_mac_address(uint8_t i, uint8_t *mac);
void enet_set_mac_address(void *config, uint8_t *mac);
bool enet_get_link_status(uint8_t i);
void enet_self_adaptive_port_speed(void);
void enet_services(struct netif *netif);
void enet_common_handler(struct netif *netif);

void enet_netif_state_bind(uint8_t idx, ENET_Type *base);
static inline enet_netif_state_t *enet_netif_get_state(struct netif *netif)
{
    return (enet_netif_state_t *)netif->state;
}

void enet_lwip_tx_release(enet_desc_t *desc);
err_t enet_lwip_output(ENET_Type *ptr, enet_desc_t *desc, struct pbuf *p);
void enet_lwip_dc_invalidate(uint32_t addr, uint32_t len);

typedef struct {
    struct pbuf_custom p;
    void *dma_descriptor;
} enet_lwip_rx_custom_pbuf_t;

typedef void *(*enet_lwip_rx_custom_alloc_fn)(void);
typedef void (*enet_lwip_rx_custom_free_fn)(struct pbuf *p);

struct pbuf *enet_lwip_input(enet_frame_t *frame, uint16_t rx_buff_size,
                                          enet_lwip_rx_custom_alloc_fn alloc_fn,
                                          enet_lwip_rx_custom_free_fn free_fn);

#if defined(LWIP_DHCP) && LWIP_DHCP
void enet_update_dhcp_state(struct netif *netif);
#endif

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer);
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
void timer_callback(void *parameter);
#endif
void log_send_message(const char *format, ...);
#else
void sys_timer_callback(void);
#endif /* defined(NO_SYS) && !NO_SYS */

#if defined __cplusplus
}
#endif /* __cplusplus */

#endif /* COMMON_LWIP_H */