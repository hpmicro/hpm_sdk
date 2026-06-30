/*
 * Copyright (c) 2024,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_H
#define HPM_TSW_PHY_H
#include <stdbool.h>
#include <stdint.h>

#ifndef TSW_PHY_SW_RESET_RETRY_CNT
#define TSW_PHY_SW_RESET_RETRY_CNT (100000UL)
#endif

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

typedef enum {
    tsw_phy_rmii_refclk_dir_out = 0,
    tsw_phy_rmii_refclk_dir_in
} tsw_phy_rmii_refclk_dir_t;

typedef struct {
    uint8_t tsw_phy_link;
    uint8_t tsw_phy_speed;
    uint8_t tsw_phy_duplex;
    uint8_t tsw_phy_speed_valid;
} tsw_phy_status_t;

static inline bool tsw_phy_link_up_with_valid_speed(const tsw_phy_status_t *status)
{
    return (status->tsw_phy_link == tsw_phy_link_up) &&
           (status->tsw_phy_speed_valid != 0U);
}

static inline bool tsw_phy_link_down_entered(const tsw_phy_status_t *last,
                                             const tsw_phy_status_t *cur)
{
    return cur->tsw_phy_link == tsw_phy_link_down &&
           last->tsw_phy_link != tsw_phy_link_down;
}

static inline void tsw_phy_status_note_link_down(tsw_phy_status_t *last)
{
    last->tsw_phy_link = tsw_phy_link_down;
    last->tsw_phy_speed_valid = 0U;
}

static inline bool tsw_phy_status_changed(const tsw_phy_status_t *last,
                                          const tsw_phy_status_t *cur)
{
    return (last->tsw_phy_link != tsw_phy_link_up) ||
           (last->tsw_phy_speed_valid == 0U) ||
           (last->tsw_phy_speed != cur->tsw_phy_speed) ||
           (last->tsw_phy_duplex != cur->tsw_phy_duplex);
}

#endif
