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
#include "hpm_tsw_drv.h"

typedef struct {
    uint8_t phy_link;
    uint8_t phy_speed;
    uint8_t phy_duplex;
} tsn_phy_status_t;

/* Exported Macros------------------------------------------------------------*/
#define TSW_MAC_LEN (6U)
#define TSW_FRAME_BUFF_COUNT (16U)

/* Exported Variables ------------------------------------------------------*/
extern uint8_t mac[];
extern uint8_t send_buff[][TSW_SEND_BUFF_LEN];
extern uint8_t recv_buff[][TSW_RECV_BUFF_LEN];

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
extern volatile bool rx_flag;
extern tsw_frame_t frame[];
#endif

#endif /* LWIP_H */
