/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_ADAPTIVE_UTIL_H
#define HPM_TSW_PHY_ADAPTIVE_UTIL_H

#include <stdbool.h>
#include "hpm_tsw_phy.h"
#include "hpm_tsw_drv.h"

void tsw_phy_adaptive_print_link_down(const char *log_prefix);
void tsw_phy_adaptive_print_port_banner(uint8_t tsw_port);
void tsw_phy_adaptive_print_link_up(const char *log_prefix, const tsw_phy_status_t *status);
void tsw_phy_adaptive_clear_port_on_down(TSW_Type *tsw_base, uint8_t frame_dst_port);
bool tsw_phy_adaptive_apply_port_on_up(TSW_Type *tsw_base, uint8_t tsw_port, uint8_t frame_dst_port,
                                      const tsw_phy_status_t *status);

#if defined(CONFIG_ENET_PHY) && CONFIG_ENET_PHY
#include "hpm_enet_phy.h"

static inline void tsw_phy_adaptive_enet_to_tsw_status(const enet_phy_status_t *enet,
                                                       tsw_phy_status_t *tsw)
{
    tsw->tsw_phy_link = enet->enet_phy_link;
    tsw->tsw_phy_speed = enet->enet_phy_speed;
    tsw->tsw_phy_duplex = enet->enet_phy_duplex;
    tsw->tsw_phy_speed_valid = enet->enet_phy_speed_valid;
}
#endif

#endif
