/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_ADAPTIVE_GLUE_H
#define HPM_TSW_PHY_ADAPTIVE_GLUE_H

#include <stdbool.h>
#include "hpm_tsw_phy.h"
#include "hpm_tsw_drv.h"

typedef struct tsw_phy_adaptive_binding tsw_phy_adaptive_binding_t;

typedef void (*tsw_phy_adaptive_notify_link_fn)(const tsw_phy_adaptive_binding_t *binding, bool link_up);

struct tsw_phy_adaptive_binding {
    tsw_phy_status_t *last;
    TSW_Type *tsw_base;
    uint8_t tsw_port;
    uint8_t frame_dst_port;
    const char *log_prefix;
    bool print_port_banner;
    bool notify_netif;
    uint8_t netif_idx;
    tsw_phy_adaptive_notify_link_fn notify_link;
    void *status_mbox;
    uint32_t *link_msg;
};

void tsw_phy_adaptive_binding_poll(const tsw_phy_adaptive_binding_t *binding,
                                   const tsw_phy_status_t *status);

#endif
