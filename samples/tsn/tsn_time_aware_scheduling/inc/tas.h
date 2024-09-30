/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TAS_H
#define TAS_H

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "hpm_tsw_drv.h"
#include "hpm_tsw_phy_common.h"

typedef struct {
    uint8_t phy_link;
    uint8_t phy_speed;
    uint8_t phy_duplex;
} tsn_phy_status_t;

/* Exported Macros------------------------------------------------------------*/
#define TSW_MAC_LEN (6U)
#define TSW_APP_TIMER_INTERVAL (2000) /* 2 ms*/

/* Exported Variables ------------------------------------------------------*/
extern uint8_t mac[];
extern uint8_t send_buff[][TSW_SEND_BUFF_LEN];
extern uint8_t recv_buff[][TSW_RECV_BUFF_LEN];

#endif /* TAS_H */
