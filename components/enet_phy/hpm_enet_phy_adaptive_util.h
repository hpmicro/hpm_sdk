/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_ENET_PHY_ADAPTIVE_UTIL_H
#define HPM_ENET_PHY_ADAPTIVE_UTIL_H

#include "hpm_enet_phy.h"
#include "hpm_enet_drv.h"

void enet_phy_adaptive_print_link_down(const char *log_prefix);
void enet_phy_adaptive_print_port_banner(uint8_t netif_idx);
void enet_phy_adaptive_print_link_up(const char *log_prefix, const enet_phy_status_t *status);
void enet_phy_adaptive_apply_mac(ENET_Type *enet_base, const enet_phy_status_t *status);

#endif
