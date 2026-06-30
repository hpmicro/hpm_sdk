/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef ENET_PHY_ADAPTIVE_LWIP_H
#define ENET_PHY_ADAPTIVE_LWIP_H

#include "hpm_enet_phy_adaptive_glue.h"

typedef enet_phy_adaptive_binding_t lwip_enet_phy_adaptive_binding_t;

void lwip_enet_phy_adaptive_poll(const lwip_enet_phy_adaptive_binding_t *binding);

#endif
