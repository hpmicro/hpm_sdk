/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_ENET_PHY_ADAPTIVE_GLUE_H
#define HPM_ENET_PHY_ADAPTIVE_GLUE_H

#include <stdbool.h>
#include "hpm_enet_phy.h"
#include "hpm_enet_drv.h"

typedef struct enet_phy_adaptive_binding enet_phy_adaptive_binding_t;

typedef void (*enet_phy_adaptive_notify_link_fn)(const enet_phy_adaptive_binding_t *binding, bool link_up);

struct enet_phy_adaptive_binding {
    enet_phy_status_t *last;
    ENET_Type *enet_base;
    uint8_t phy_port;
    const char *log_prefix;
    bool print_port_banner;
    bool notify_netif;
    uint8_t netif_idx;
    enet_phy_adaptive_notify_link_fn notify_link;
    void *status_mbox;
    uint32_t *link_msg;
};

void enet_phy_adaptive_binding_poll(const enet_phy_adaptive_binding_t *binding,
                                    const enet_phy_status_t *status);

#endif
