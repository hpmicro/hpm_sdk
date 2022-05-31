/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LWIP_H
#define LWIP_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"

/* Exported Macros------------------------------------------------------------*/
#if RGMII
#define ENET_INF_TYPE       enet_inf_rgmii
#define ENET                BOARD_ENET_RGMII
#define ENET_PTP_CLK        BOARD_ENET_RGMII_PTP_CLOCK
#else
#define ENET_INF_TYPE       enet_inf_rmii
#define ENET                BOARD_ENET_RMII
#define ENET_PTP_CLK        BOARD_ENET_RMII_PTP_CLOCK
#endif

#define ENET_TX_BUFF_COUNT  (10U)
#define ENET_RX_BUFF_COUNT  (20U)
#define ENET_RX_BUFF_SIZE   ENET_MAX_FRAME_SIZE
#define ENET_TX_BUFF_SIZE   ENET_MAX_FRAME_SIZE

/* Exported Variables ------------------------------------------------------*/
extern enet_desc_t desc;

#endif /* LWIP_H */
