/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LWIP_H
#define LWIP_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"
#include "hpm_l1c_drv.h"

typedef ENET_Type enet_base_t;

/* Exported Macros------------------------------------------------------------*/
#if defined(RGMII) && RGMII
#define ENET_INF_TYPE       enet_inf_rgmii
#define ENET                BOARD_ENET_RGMII
#else
#define ENET_INF_TYPE       enet_inf_rmii
#define ENET                BOARD_ENET_RMII
#endif

#define ENET_TX_BUFF_COUNT  (10U)
#define ENET_RX_BUFF_COUNT  (20U)
#define ENET_TX_BUFF_SIZE   (1536U)
#define ENET_RX_BUFF_SIZE   (1536U)

/* Exported Variables ------------------------------------------------------*/
extern enet_desc_t desc[];
extern uint8_t mac[][ENET_MAC];

#endif /* LWIP_H */
