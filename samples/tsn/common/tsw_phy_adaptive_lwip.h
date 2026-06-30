/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TSW_PHY_ADAPTIVE_LWIP_H
#define TSW_PHY_ADAPTIVE_LWIP_H

#include "hpm_tsw_phy_adaptive_glue.h"

typedef tsw_phy_adaptive_binding_t lwip_tsw_phy_adaptive_binding_t;

void lwip_tsw_phy_adaptive_poll(const lwip_tsw_phy_adaptive_binding_t *binding);

#endif
