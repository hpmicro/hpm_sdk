/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_ADAPTIVE_H
#define HPM_TSW_PHY_ADAPTIVE_H

#include "hpm_tsw_phy.h"

typedef void (*tsw_phy_adaptive_link_down_cb)(const tsw_phy_status_t *status, void *ctx);
typedef void (*tsw_phy_adaptive_mac_apply_cb)(const tsw_phy_status_t *status, void *ctx);

typedef struct {
    tsw_phy_status_t *last;
    tsw_phy_adaptive_link_down_cb on_link_down;
    tsw_phy_adaptive_mac_apply_cb on_mac_apply;
    void *ctx;
} tsw_phy_adaptive_port_t;

void tsw_phy_adaptive_poll(const tsw_phy_status_t *status, tsw_phy_adaptive_port_t *port);

#endif
