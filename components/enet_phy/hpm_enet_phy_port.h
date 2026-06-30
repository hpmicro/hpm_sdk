/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ENET_PHY_PORT_H
#define HPM_ENET_PHY_PORT_H

#include "hpm_common.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_phy.h"

#define HPM_ENET_PHY_USE_DEFAULT_ADDR (0xFFU)
#define HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE (1UL << 0)

/** @brief PHY port media interface (maps to @ref enet_inf_type_t) */
#define HPM_ENET_PHY_ITF_MII    enet_inf_mii
#define HPM_ENET_PHY_ITF_RMII   enet_inf_rmii
#define HPM_ENET_PHY_ITF_RGMII  enet_inf_rgmii

typedef enum {
    hpm_enet_phy_dp83867,
    hpm_enet_phy_rtl8211,
    hpm_enet_phy_dp83848,
    hpm_enet_phy_rtl8201,
    hpm_enet_phy_lan8720,
    hpm_enet_phy_jl1111,
} hpm_enet_phy_type_t;

typedef struct {
    ENET_Type *base;
    hpm_enet_phy_type_t type;
    uint8_t addr;
    uint8_t interface;
    uint8_t rmii_refclk_dir;
    uint32_t init_flags;
} hpm_enet_phy_port_config_t;

/**
 * @brief Whether RMII refclk direction must be supplied in port config
 * @note RMII interface only; individual PHY drivers apply it when supported
 */
static inline bool hpm_enet_phy_port_needs_rmii_refclk(uint8_t interface)
{
    return (interface == HPM_ENET_PHY_ITF_RMII);
}

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

hpm_stat_t hpm_enet_phy_port_init(hpm_enet_phy_port_config_t *config);
hpm_stat_t hpm_enet_phy_port_get_status(hpm_enet_phy_port_config_t *config, enet_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* HPM_ENET_PHY_PORT_H */
