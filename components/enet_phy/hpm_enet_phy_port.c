/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_enet_phy_port.h"
#include "hpm_enet_phy_common.h"

static hpm_stat_t hpm_enet_phy_port_get_addr(hpm_enet_phy_type_t type, uint8_t *addr)
{
    if (addr == NULL) {
        return status_invalid_argument;
    }

    switch (type) {
    case hpm_enet_phy_dp83867:
#if defined(__USE_DP83867) && __USE_DP83867
        *addr = DP83867_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    case hpm_enet_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        *addr = RTL8211_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    case hpm_enet_phy_dp83848:
#if defined(__USE_DP83848) && __USE_DP83848
        *addr = DP83848_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    case hpm_enet_phy_rtl8201:
#if defined(__USE_RTL8201) && __USE_RTL8201
        *addr = RTL8201_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    case hpm_enet_phy_lan8720:
#if defined(__USE_LAN8720) && __USE_LAN8720
        *addr = LAN8720_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    case hpm_enet_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        *addr = JL1111_ADDR;
        return status_success;
#else
        return status_fail;
#endif
    default:
        return status_invalid_argument;
    }
}

static hpm_stat_t hpm_enet_phy_port_validate_init_flags(hpm_enet_phy_type_t type, uint32_t flags)
{
    if ((flags & ~HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE) != 0U) {
        return status_invalid_argument;
    }

    if ((flags & HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE) != 0U) {
        if (type != hpm_enet_phy_jl1111) {
            return status_invalid_argument;
        }
    }

    return status_success;
}

hpm_stat_t hpm_enet_phy_port_init(hpm_enet_phy_port_config_t *config)
{
    bool phy_init_success = false;
    uint8_t phy_addr;
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_config_t dp83867_config;
#endif
#if defined(__USE_RTL8211) && __USE_RTL8211
    rtl8211_config_t rtl8211_config;
#endif
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_config_t dp83848_config;
#endif
#if defined(__USE_RTL8201) && __USE_RTL8201
    rtl8201_config_t rtl8201_config;
#endif
#if defined(__USE_LAN8720) && __USE_LAN8720
    lan8720_config_t lan8720_config;
#endif
#if defined(__USE_JL1111) && __USE_JL1111
    jl1111_config_t jl1111_config;
#endif

    if ((config == NULL) || (config->base == NULL)) {
        return status_invalid_argument;
    }

    if (hpm_enet_phy_port_validate_init_flags(config->type, config->init_flags) != status_success) {
        return status_invalid_argument;
    }

    if (config->addr == HPM_ENET_PHY_USE_DEFAULT_ADDR) {
        if (hpm_enet_phy_port_get_addr(config->type, &phy_addr) != status_success) {
            return status_fail;
        }
    } else {
        phy_addr = config->addr;
    }

    switch (config->type) {
    case hpm_enet_phy_dp83867:
#if defined(__USE_DP83867) && __USE_DP83867
        dp83867_reset(config->base, phy_addr);
#if defined(__DISABLE_AUTO_NEGO) && __DISABLE_AUTO_NEGO
        dp83867_set_mdi_crossover_mode(config->base, phy_addr, enet_phy_mdi_crossover_manual_mdix);
#endif
        dp83867_basic_mode_default_config(config->base, &dp83867_config);
        phy_init_success = dp83867_basic_mode_init(config->base, phy_addr, &dp83867_config);
#endif
        break;
    case hpm_enet_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        rtl8211_reset(config->base, phy_addr);
        rtl8211_basic_mode_default_config(config->base, &rtl8211_config);
        phy_init_success = rtl8211_basic_mode_init(config->base, phy_addr, &rtl8211_config);
#endif
        break;
    case hpm_enet_phy_dp83848:
#if defined(__USE_DP83848) && __USE_DP83848
        dp83848_reset(config->base, phy_addr);
        dp83848_basic_mode_default_config(config->base, &dp83848_config);
        phy_init_success = dp83848_basic_mode_init(config->base, phy_addr, &dp83848_config);
#endif
        break;
    case hpm_enet_phy_rtl8201:
#if defined(__USE_RTL8201) && __USE_RTL8201
        rtl8201_reset(config->base, phy_addr);
        rtl8201_basic_mode_default_config(config->base, &rtl8201_config);
        rtl8201_config.media_interface = config->interface;
        if (config->interface == HPM_ENET_PHY_ITF_RMII) {
            rtl8201_config.rmii_refclk_dir = config->rmii_refclk_dir;
        }
        phy_init_success = rtl8201_basic_mode_init(config->base, phy_addr, &rtl8201_config);
#endif
        break;
    case hpm_enet_phy_lan8720:
#if defined(__USE_LAN8720) && __USE_LAN8720
        lan8720_reset(config->base, phy_addr);
        lan8720_basic_mode_default_config(config->base, &lan8720_config);
        phy_init_success = lan8720_basic_mode_init(config->base, phy_addr, &lan8720_config);
#endif
        break;
    case hpm_enet_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        jl1111_reset(config->base, phy_addr);
        jl1111_basic_mode_default_config(config->base, &jl1111_config);
        jl1111_config.media_interface = config->interface;
        if (config->interface == HPM_ENET_PHY_ITF_RMII) {
            jl1111_config.rmii_refclk_dir = config->rmii_refclk_dir;
        }
        phy_init_success = jl1111_basic_mode_init(config->base, phy_addr, &jl1111_config);
        if (phy_init_success && ((config->init_flags & HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE) != 0U)) {
            jl1111_disable_broadcast_response(config->base, phy_addr);
        }
#endif
        break;
    default:
        return status_invalid_argument;
    }

    return phy_init_success ? status_success : status_fail;
}

hpm_stat_t hpm_enet_phy_port_get_status(hpm_enet_phy_port_config_t *config, enet_phy_status_t *status)
{
    uint8_t phy_addr;

    if ((config == NULL) || (config->base == NULL) || (status == NULL)) {
        return status_invalid_argument;
    }

    if (config->addr == HPM_ENET_PHY_USE_DEFAULT_ADDR) {
        if (hpm_enet_phy_port_get_addr(config->type, &phy_addr) != status_success) {
            return status_fail;
        }
    } else {
        phy_addr = config->addr;
    }

    switch (config->type) {
    case hpm_enet_phy_dp83867:
#if defined(__USE_DP83867) && __USE_DP83867
        return dp83867_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_enet_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        return rtl8211_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_enet_phy_dp83848:
#if defined(__USE_DP83848) && __USE_DP83848
        return dp83848_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_enet_phy_rtl8201:
#if defined(__USE_RTL8201) && __USE_RTL8201
        return rtl8201_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_enet_phy_lan8720:
#if defined(__USE_LAN8720) && __USE_LAN8720
        return lan8720_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_enet_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        return jl1111_get_phy_status(config->base, phy_addr, status);
#else
        return status_fail;
#endif
    default:
        return status_invalid_argument;
    }
}
