/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include "lwip/pbuf.h"
#include "lwip/netif.h"

typedef struct my_custom_pbuf {
   struct pbuf_custom p;
   void *dma_descriptor;
} my_custom_pbuf_t;

#ifdef __cplusplus
extern "C" {
#endif
/* Exported functions---------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
#if defined(NO_SYS) && !NO_SYS
void ethernetif_input(void *pvParameters);
#else
err_t ethernetif_input(struct netif *netif);
#endif
#ifdef __cplusplus /* __cplusplus */
}
#endif

#endif /* ETHERNETIF_H */
