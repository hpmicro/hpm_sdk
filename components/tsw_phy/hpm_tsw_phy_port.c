/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_tsw_phy_port.h"
#include "hpm_tsw_phy_common.h"

static uint8_t hpm_tsw_phy_port_get_addr(hpm_tsw_phy_port_config_t *config)
{
    if (config->addr != HPM_TSW_PHY_USE_DEFAULT_ADDR) {
        return config->addr;
    }

    switch (config->type) {
    case hpm_tsw_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        return RTL8211_ADDR;
#else
        break;
#endif
    case hpm_tsw_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        return JL1111_ADDR;
#else
        break;
#endif
    default:
        break;
    }

    return HPM_TSW_PHY_USE_DEFAULT_ADDR;
}

hpm_stat_t hpm_tsw_phy_port_init(hpm_tsw_phy_port_config_t *config)
{
    bool phy_init_success = false;
    uint8_t phy_addr;
#if defined(__USE_RTL8211) && __USE_RTL8211
    rtl8211_config_t rtl8211_config;
#endif
#if defined(__USE_JL1111) && __USE_JL1111
    jl1111_config_t jl1111_config;
#endif

    if ((config == NULL) || (config->base == NULL)) {
        return status_invalid_argument;
    }

    phy_addr = hpm_tsw_phy_port_get_addr(config);
    if (phy_addr == HPM_TSW_PHY_USE_DEFAULT_ADDR) {
        return status_invalid_argument;
    }

    switch (config->type) {
    case hpm_tsw_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        rtl8211_reset(config->base, config->port, phy_addr);
        rtl8211_basic_mode_default_config(config->base, &rtl8211_config);
        phy_init_success = rtl8211_basic_mode_init(config->base, config->port, phy_addr, &rtl8211_config);
#endif
        break;
    case hpm_tsw_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        jl1111_reset(config->base, config->port, phy_addr);
        jl1111_basic_mode_default_config(config->base, &jl1111_config);
        jl1111_config.media_interface = config->interface;
        if (config->interface == HPM_TSW_PHY_ITF_RMII) {
            if (config->rmii_refclk_dir == tsw_phy_rmii_refclk_dir_in) {
                jl1111_config.rmii_refclk_dir = jl1111_config_refclk_input;
            } else {
                jl1111_config.rmii_refclk_dir = jl1111_config_refclk_output;
            }
        }
        phy_init_success = jl1111_basic_mode_init(config->base, config->port, phy_addr, &jl1111_config);
        if (phy_init_success && ((config->init_flags & HPM_TSW_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE) != 0U)) {
            jl1111_disable_broadcast_response(config->base, config->port, phy_addr);
        }
#endif
        break;
    default:
        return status_invalid_argument;
    }

    return phy_init_success ? status_success : status_fail;
}

hpm_stat_t hpm_tsw_phy_port_get_status(hpm_tsw_phy_port_config_t *config, tsw_phy_status_t *status)
{
    uint8_t phy_addr;

    if ((config == NULL) || (config->base == NULL) || (status == NULL)) {
        return status_invalid_argument;
    }

    phy_addr = hpm_tsw_phy_port_get_addr(config);
    if (phy_addr == HPM_TSW_PHY_USE_DEFAULT_ADDR) {
        return status_invalid_argument;
    }

    switch (config->type) {
    case hpm_tsw_phy_rtl8211:
#if defined(__USE_RTL8211) && __USE_RTL8211
        return rtl8211_get_phy_status(config->base, config->port, phy_addr, status);
#else
        return status_fail;
#endif
    case hpm_tsw_phy_jl1111:
#if defined(__USE_JL1111) && __USE_JL1111
        return jl1111_get_phy_status(config->base, config->port, phy_addr, status);
#else
        return status_fail;
#endif
    default:
        return status_invalid_argument;
    }
}
