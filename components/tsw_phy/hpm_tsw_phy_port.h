/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TSW_PHY_PORT_H
#define HPM_TSW_PHY_PORT_H

#include "hpm_common.h"
#include "hpm_tsw_drv.h"
#include "hpm_tsw_phy.h"

#define HPM_TSW_PHY_USE_DEFAULT_ADDR (0xFFU)
#define HPM_TSW_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE (1UL << 0)

/** @brief TSW PHY port media interface (maps to @ref tsw_port_phy_itf_t) */
#define HPM_TSW_PHY_ITF_MII    tsw_port_phy_itf_mii
#define HPM_TSW_PHY_ITF_RMII   tsw_port_phy_itf_rmii
#define HPM_TSW_PHY_ITF_RGMII  tsw_port_phy_itf_rgmii

typedef enum {
    hpm_tsw_phy_rtl8211,
    hpm_tsw_phy_jl1111,
} hpm_tsw_phy_type_t;

typedef struct {
    TSW_Type *base;
    hpm_tsw_phy_type_t type;
    uint8_t addr;
    uint8_t port;
    uint8_t interface;
    uint8_t rmii_refclk_dir;
    uint32_t init_flags;
} hpm_tsw_phy_port_config_t;

/**
 * @brief Whether RMII refclk direction must be supplied in port config
 * @note RMII interface only; PHY drivers that support refclk use it inside RMII init paths
 */
static inline bool hpm_tsw_phy_port_needs_rmii_refclk(uint8_t interface)
{
    return (interface == HPM_TSW_PHY_ITF_RMII);
}

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

hpm_stat_t hpm_tsw_phy_port_init(hpm_tsw_phy_port_config_t *config);
hpm_stat_t hpm_tsw_phy_port_get_status(hpm_tsw_phy_port_config_t *config, tsw_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* HPM_TSW_PHY_PORT_H */
