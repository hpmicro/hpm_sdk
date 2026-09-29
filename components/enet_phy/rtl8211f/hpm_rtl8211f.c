/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * RTL8211F-CG 10/100/1000M Ethernet PHY driver
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_drv.h"
#include "hpm_rtl8211f_regs.h"
#include "hpm_rtl8211f.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool rtl8211f_check_id(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t id1, id2;

    if (enet_read_phy(ptr, phy_addr, RTL8211F_PHYID1, &id1) != status_success ||
        enet_read_phy(ptr, phy_addr, RTL8211F_PHYID2, &id2) != status_success) {
        return false;
    }

    if (RTL8211F_PHYID1_OUI_MSB_GET(id1) == RTL8211F_ID1 && id2 == RTL8211F_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
bool rtl8211f_reset(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = ENET_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    enet_write_phy(ptr, phy_addr, RTL8211F_BMCR, RTL8211F_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        if (enet_read_phy(ptr, phy_addr, RTL8211F_BMCR, &data) != status_success) {
            return false;
        }
    } while (RTL8211F_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void rtl8211f_basic_mode_default_config(ENET_Type *ptr, rtl8211f_config_t *config)
{
    (void)ptr;

    config->loopback         = false;                        /* Disable PCS loopback mode */
    #if defined(__DISABLE_AUTO_NEGO) && (__DISABLE_AUTO_NEGO)
    config->auto_negotiation = false;                        /* Disable Auto-Negotiation */
    config->speed            = enet_phy_port_speed_1000mbps;
    config->duplex           = enet_phy_duplex_full;
    #else
    config->auto_negotiation = true;                         /* Enable Auto-Negotiation */
    #endif
}

bool rtl8211f_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, rtl8211f_config_t *config)
{
    uint16_t data = 0;

    data |= RTL8211F_BMCR_RESET_SET(0)                        /* Normal operation */
         |  RTL8211F_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  RTL8211F_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  RTL8211F_BMCR_PWD_SET(0)                          /* Normal operation */
         |  RTL8211F_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  RTL8211F_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  RTL8211F_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == 0) {
        data |= RTL8211F_BMCR_SPEED0_SET(config->speed) | RTL8211F_BMCR_SPEED1_SET(config->speed >> 1);   /* Set port speed */
        data |= RTL8211F_BMCR_DUPLEX_SET(config->duplex);                                                /* Set duplex mode */
    }

    /* check the id of rtl8211f */
    if (rtl8211f_check_id(ptr, phy_addr) == false) {
        return false;
    }

    enet_write_phy(ptr, phy_addr, RTL8211F_BMCR, data);

    return true;
}

hpm_stat_t rtl8211f_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t bmcr, bmsr, physr;
    uint8_t speed_val;
    hpm_stat_t stat, restore_stat;

    if (status == NULL) {
        return status_invalid_argument;
    }

    status->enet_phy_speed_valid = 0U;

    stat = enet_read_phy(ptr, phy_addr, RTL8211F_BMCR, &bmcr);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }

    if (RTL8211F_BMCR_ANE_GET(bmcr) != 0U) {
        stat = enet_read_phy(ptr, phy_addr, RTL8211F_BMSR, &bmsr);
        if (stat != status_success) {
            status->enet_phy_link = enet_phy_link_unknown;
            return stat;
        }
    }

    stat = enet_write_phy(ptr, phy_addr, RTL8211F_PAGESEL, RTL8211F_PAGE_PHY);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    stat = enet_read_phy(ptr, phy_addr, RTL8211F_PHYSR, &physr);
    restore_stat = enet_write_phy(ptr, phy_addr, RTL8211F_PAGESEL, RTL8211F_PAGE_IEEE);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    if (restore_stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return restore_stat;
    }

    status->enet_phy_link = RTL8211F_PHYSR_LINK_REAL_TIME_GET(physr);
    if (status->enet_phy_link == 0U) {
        return status_success;
    }

    if (RTL8211F_BMCR_ANE_GET(bmcr) != 0U) {
        if (RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_GET(bmsr) == 0U) {
            return status_success;
        }
    }

    speed_val = RTL8211F_PHYSR_SPEED_GET(physr);
    status->enet_phy_speed = speed_val == 0 ? enet_phy_port_speed_10mbps : speed_val == 1 ? enet_phy_port_speed_100mbps : enet_phy_port_speed_1000mbps;
    status->enet_phy_duplex = RTL8211F_PHYSR_DUPLEX_GET(physr);
    status->enet_phy_speed_valid = 1U;

    return status_success;
}
