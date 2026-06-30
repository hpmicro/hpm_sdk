/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_drv.h"
#include "hpm_rtl8201_regs.h"
#include "hpm_rtl8201.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool rtl8201_check_id(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t id1, id2;

    if (enet_read_phy(ptr, phy_addr, RTL8201_PHYID1, &id1) != status_success ||
        enet_read_phy(ptr, phy_addr, RTL8201_PHYID2, &id2) != status_success) {
        return false;
    }

    if (RTL8201_PHYID1_OUI_MSB_GET(id1) == RTL8201_ID1 && RTL8201_PHYID2_OUI_LSB_GET(id2) == RTL8201_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
bool rtl8201_reset(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = ENET_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    enet_write_phy(ptr, phy_addr, RTL8201_BMCR, RTL8201_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        if (enet_read_phy(ptr, phy_addr, RTL8201_BMCR, &data) != status_success) {
            return false;
        }
    } while (RTL8201_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void rtl8201_basic_mode_default_config(ENET_Type *ptr, rtl8201_config_t *config)
{
    (void)ptr;

    config->loopback         = false;                        /* Disable PCS loopback mode */
    #if defined(__DISABLE_AUTO_NEGO) && (__DISABLE_AUTO_NEGO)
    config->auto_negotiation = false;                        /* Disable Auto-Negotiation */
    config->speed            = enet_phy_port_speed_100mbps;
    config->duplex           = enet_phy_duplex_full;
    #else
    config->auto_negotiation = true;                         /* Enable Auto-Negotiation */
    #endif
}

bool rtl8201_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, rtl8201_config_t *config)
{
    uint16_t data = 0;

    data |= RTL8201_BMCR_RESET_SET(0)                        /* Normal operation */
         |  RTL8201_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  RTL8201_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  RTL8201_BMCR_PWD_SET(0)                          /* Normal operation */
         |  RTL8201_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  RTL8201_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  RTL8201_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == 0) {
        data |= RTL8201_BMCR_SPEED0_SET(config->speed);      /* Set port speed */
        data |= RTL8201_BMCR_DUPLEX_SET(config->duplex);     /* Set duplex mode */
    }

    /* check the id of rtl8201 */
    if (rtl8201_check_id(ptr, phy_addr) == false) {
        return false;
    }

    enet_write_phy(ptr, phy_addr, RTL8201_BMCR, data);

    if (config->media_interface == enet_inf_rmii) {
        if (config->rmii_refclk_dir == enet_phy_rmii_refclk_dir_in) {
            rtl8201_set_rmii_refclk_direction(ptr, phy_addr, rtl8201_config_refclk_input);
        } else {
            rtl8201_set_rmii_refclk_direction(ptr, phy_addr, rtl8201_config_refclk_output);
        }
    }

    return true;
}

static void rtl8201_set_speed_from_bmcr(uint16_t bmcr, enet_phy_status_t *status)
{
    status->enet_phy_speed = RTL8201_BMCR_SPEED0_GET(bmcr) ? enet_phy_port_speed_100mbps : enet_phy_port_speed_10mbps;
    status->enet_phy_duplex = RTL8201_BMCR_DUPLEX_GET(bmcr);
    status->enet_phy_speed_valid = 1U;
}

static void rtl8201_set_speed_from_anar(uint16_t data, enet_phy_status_t *status)
{
    if (RTL8201_ANAR_100BASE_TX_FD_GET(data)) {
        status->enet_phy_speed = enet_phy_port_speed_100mbps;
        status->enet_phy_duplex = enet_phy_duplex_full;
        status->enet_phy_speed_valid = 1U;
    } else if (RTL8201_ANAR_100BASE_TX_GET(data)) {
        status->enet_phy_speed = enet_phy_port_speed_100mbps;
        status->enet_phy_duplex = enet_phy_duplex_half;
        status->enet_phy_speed_valid = 1U;
    } else if (RTL8201_ANAR_10BASE_T_FD_GET(data)) {
        status->enet_phy_speed = enet_phy_port_speed_10mbps;
        status->enet_phy_duplex = enet_phy_duplex_full;
        status->enet_phy_speed_valid = 1U;
    } else if (RTL8201_ANAR_10BASE_T_GET(data)) {
        status->enet_phy_speed = enet_phy_port_speed_10mbps;
        status->enet_phy_duplex = enet_phy_duplex_half;
        status->enet_phy_speed_valid = 1U;
    }
}

hpm_stat_t rtl8201_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t bmsr, bmcr, anar, anlpar;
    hpm_stat_t stat;

    if (status == NULL) {
        return status_invalid_argument;
    }

    status->enet_phy_speed_valid = 0U;

    /* Page 0: Reg1 is BMSR. Some chips may still return the same BMSR when reading Reg1 on other pages,
     * but keep PAGESEL=0 for IEEE Clause 22 and in case other registers differ by page (e.g. BMCR).
     */
    stat = enet_write_phy(ptr, phy_addr, RTL8201_PAGESEL, 0);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }

    /* BMSR link bit is latched; second read is current state per IEEE 802.3 */
    stat = enet_read_phy(ptr, phy_addr, RTL8201_BMSR, &bmsr);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    stat = enet_read_phy(ptr, phy_addr, RTL8201_BMSR, &bmsr);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    status->enet_phy_link = RTL8201_BMSR_LINK_STATUS_GET(bmsr);

    if (status->enet_phy_link == 0U) {
        return status_success;
    }

    stat = enet_read_phy(ptr, phy_addr, RTL8201_BMCR, &bmcr);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    if (RTL8201_BMCR_ANE_GET(bmcr) == 0U) {
        rtl8201_set_speed_from_bmcr(bmcr, status);
        return status_success;
    }

    if (RTL8201_BMSR_AUTO_NEGOTIATION_COMPLETE_GET(bmsr) == 0U) {
        return status_success;
    }

    stat = enet_read_phy(ptr, phy_addr, RTL8201_ANAR, &anar);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    stat = enet_read_phy(ptr, phy_addr, RTL8201_ANLPAR, &anlpar);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    rtl8201_set_speed_from_anar(anar & anlpar, status);

    return status_success;
}

void rtl8201_set_rmii_refclk_direction(ENET_Type *ptr, uint32_t phy_addr, uint8_t dir)
{
    uint16_t data = 0;

    /* select page 7 */
    enet_write_phy(ptr, phy_addr, RTL8201_PAGESEL, 7);

    /* read register RMSR */
    enet_read_phy(ptr, phy_addr, RTL8201_RMSR_P7, &data);

    /* set tx clock direction */
    if (dir == rtl8201_config_refclk_output) {
        data &= ~BIT12_MASK; /* PHY output 50MHz */
    } else {
        data |= BIT12_MASK;  /* PHY input 50MHz */
    }

    /* write register RMSR */
    enet_write_phy(ptr, phy_addr, RTL8201_RMSR_P7, data);

    /* return to IEEE register map so later BMCR/BMSR MDIO reads are valid */
    enet_write_phy(ptr, phy_addr, RTL8201_PAGESEL, 0);
}
