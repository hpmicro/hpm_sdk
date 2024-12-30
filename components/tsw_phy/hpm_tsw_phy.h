/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_H
#define HPM_TSW_PHY_H
#include <stdint.h>

typedef enum {
  tsw_phy_link_down = 0,
  tsw_phy_link_up,
  tsw_phy_link_unknown
} tsw_phy_link_status_t;

typedef enum {
    tsw_phy_port_speed_10mbps = 0,
    tsw_phy_port_speed_100mbps,
    tsw_phy_port_speed_1000mbps
} tsw_phy_port_speed_t;

typedef enum {
    tsw_phy_duplex_half = 0,
    tsw_phy_duplex_full
} tsw_phy_duplex_mode_t;

typedef enum {
    tsw_phy_mdi_crossover_manual_mdi = 0,
    tsw_phy_mdi_crossover_manual_mdix,
    tsw_phy_mdi_crossover_automatic
} tsw_phy_crossover_mode_t;

typedef struct {
    uint8_t tsw_phy_link;
    uint8_t tsw_phy_speed;
    uint8_t tsw_phy_duplex;
} tsw_phy_status_t;

#endif
