/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LWIP_H
#define LWIP_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"

typedef ENET_Type enet_base_t;

/* Exported Macros------------------------------------------------------------*/
#define ENET_TX_BUFF_COUNT  (32U)
#define ENET_RX_BUFF_COUNT  (50U)
#define ENET_TX_BUFF_SIZE   ENET_MAX_BUFF_SIZE
#define ENET_RX_BUFF_SIZE   ENET_MAX_BUFF_SIZE

/* Exported Variables ------------------------------------------------------*/
extern enet_desc_t desc[];
extern uint8_t mac[BOARD_ENET_COUNT][ENET_MAC];

#endif /* LWIP_H */
