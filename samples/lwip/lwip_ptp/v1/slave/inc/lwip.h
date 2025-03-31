/*
 * Copyright (c) 2021-2025 HPMicro
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

/* Exported Macros------------------------------------------------------------*/
#if defined(RGMII) && RGMII
#define ENET_INF_TYPE       enet_inf_rgmii
#define ENET                BOARD_ENET_RGMII
#define ENET_PTP_CLK        BOARD_ENET_RGMII_PTP_CLOCK
#define ENET_PPS_PINOUT     BOARD_ENET_RGMII_PPS0_PINOUT
#elif defined(RMII) && RMII
#define ENET_INF_TYPE       enet_inf_rmii
#define ENET                BOARD_ENET_RMII
#define ENET_PTP_CLK        BOARD_ENET_RMII_PTP_CLOCK
#define ENET_PPS_PINOUT     BOARD_ENET_RMII_PPS0_PINOUT
#endif

#define ENET_TX_BUFF_COUNT  (10U)
#define ENET_RX_BUFF_COUNT  (20U)
#define ENET_TX_BUFF_SIZE   (1536U)
#define ENET_RX_BUFF_SIZE   (1536U)

/* Exported Variables ------------------------------------------------------*/
extern enet_desc_t desc;
extern uint8_t mac[];
#endif /* LWIP_H */
