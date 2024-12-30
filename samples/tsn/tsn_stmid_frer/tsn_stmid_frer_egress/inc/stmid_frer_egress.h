/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef STMID_FRER_EGRESS_H
#define STMID_FRER_EGRESS_H

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
#define TSW_CBS_ENTRY_COUNT (1)
#define TSW_CBS_TQ_COUNT (2)
#define TSW_CBS_TQ_IDX   (0)
/* Exported Variables ------------------------------------------------------*/
extern uint8_t mac[];
extern uint8_t send_buff[][TSW_SEND_BUFF_LEN];
extern uint8_t recv_buff[][TSW_RECV_BUFF_LEN];

#endif /* STMID_FRER_EGRESS_H */
