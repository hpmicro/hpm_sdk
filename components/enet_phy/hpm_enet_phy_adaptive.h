/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_ENET_PHY_ADAPTIVE_H
#define HPM_ENET_PHY_ADAPTIVE_H

#include "hpm_enet_phy.h"

typedef void (*enet_phy_adaptive_link_down_cb)(const enet_phy_status_t *status, void *ctx);
typedef void (*enet_phy_adaptive_mac_apply_cb)(const enet_phy_status_t *status, void *ctx);

typedef struct {
    enet_phy_status_t *last;
    enet_phy_adaptive_link_down_cb on_link_down;
    enet_phy_adaptive_mac_apply_cb on_mac_apply;
    void *ctx;
} enet_phy_adaptive_port_t;

void enet_phy_adaptive_poll(const enet_phy_status_t *status, enet_phy_adaptive_port_t *port);

#endif
