/*
 * Copyright (c) 2024,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_drv.h"
#include "hpm_tsw_rtl8211_regs.h"
#include "hpm_tsw_rtl8211.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool rtl8211_check_id(TSW_Type *ptr, uint8_t port, uint32_t phy_addr)
{
    uint16_t id1, id2;

    tsw_ep_mdio_read(ptr, port, phy_addr, RTL8211_PHYID1, &id1);
    tsw_ep_mdio_read(ptr, port, phy_addr, RTL8211_PHYID2, &id2);

    if (RTL8211_PHYID1_OUI_MSB_GET(id1) == RTL8211_ID1 && RTL8211_PHYID2_OUI_LSB_GET(id2) == RTL8211_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
void rtl8211_reset(TSW_Type *ptr, uint8_t port, uint32_t phy_addr)
{
    uint16_t data;

    /* PHY reset */
    tsw_ep_mdio_write(ptr, port, phy_addr, RTL8211_BMCR, RTL8211_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        tsw_ep_mdio_read(ptr, port, phy_addr, RTL8211_BMCR, &data);
    } while (RTL8211_BMCR_RESET_GET(data));
}

void rtl8211_basic_mode_default_config(TSW_Type *ptr, rtl8211_config_t *config)
{
    (void)ptr;

    config->loopback         = false;                        /* Disable PCS loopback mode */
    #if defined(__DISABLE_AUTO_NEGO) && (__DISABLE_AUTO_NEGO)
    config->auto_negotiation = false;                        /* Disable Auto-Negotiation */
    config->speed            = tsw_phy_port_speed_100mbps;
    config->duplex           = tsw_phy_duplex_full;
    #else
    config->auto_negotiation = true;                         /* Enable Auto-Negotiation */
    #endif
}

bool rtl8211_basic_mode_init(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, rtl8211_config_t *config)
{
    uint16_t data = 0;

    data |= RTL8211_BMCR_RESET_SET(0)                        /* Normal operation */
         |  RTL8211_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  RTL8211_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  RTL8211_BMCR_PWD_SET(0)                          /* Normal operation */
         |  RTL8211_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  RTL8211_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  RTL8211_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == 0) {
        data |= RTL8211_BMCR_SPEED0_SET(config->speed) | RTL8211_BMCR_SPEED1_SET(config->speed >> 1);   /* Set port speed */
        data |= RTL8211_BMCR_DUPLEX_SET(config->duplex);                                                /* Set duplex mode */
    }

    /* check the id of rtl8211 */
    if (rtl8211_check_id(ptr, port, phy_addr) == false) {
        return false;
    }

    tsw_ep_mdio_write(ptr, port, phy_addr, RTL8211_BMCR, data);

    return true;
}


hpm_stat_t rtl8211_get_phy_status(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, tsw_phy_status_t *status)
{
    uint16_t data;
    hpm_stat_t stat;

    if (status == NULL) {
        return status_invalid_argument;
    }

    status->tsw_phy_speed_valid = 0U;

    stat = tsw_ep_mdio_read(ptr, port, phy_addr, RTL8211_PHYSR, &data);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    status->tsw_phy_link = RTL8211_PHYSR_LINK_REAL_TIME_GET(data);

    if (status->tsw_phy_link == 0U) {
        return status_success;
    }

    if (RTL8211_PHYSR_SPEED_AND_DUPLEX_RESOLVED_GET(data) == 0U) {
        return status_success;
    }

    status->tsw_phy_speed = RTL8211_PHYSR_SPEED_GET(data) == 0 ? tsw_phy_port_speed_10mbps : RTL8211_PHYSR_SPEED_GET(data) == 1 ? tsw_phy_port_speed_100mbps : tsw_phy_port_speed_1000mbps;
    status->tsw_phy_duplex = RTL8211_PHYSR_DUPLEX_GET(data);
    status->tsw_phy_speed_valid = 1U;

    return status_success;
}
