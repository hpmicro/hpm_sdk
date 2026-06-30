/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_drv.h"
#include "hpm_tsw_jl1111_regs.h"
#include "hpm_tsw_jl1111.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool jl1111_check_id(TSW_Type *ptr, uint8_t port, uint32_t phy_addr)
{
    uint16_t id1, id2;

    tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_PHYID1, &id1);
    tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_PHYID2, &id2);

    if (JL1111_PHYID1_OUI_MSB_GET(id1) == JL1111_ID1 && JL1111_PHYID2_OUI_LSB_GET(id2) == JL1111_ID2) {
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
bool jl1111_reset(TSW_Type *ptr, uint8_t port, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = TSW_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_BMCR, JL1111_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_BMCR, &data);
    } while (JL1111_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void jl1111_basic_mode_default_config(TSW_Type *ptr, jl1111_config_t *config)
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

bool jl1111_basic_mode_init(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, jl1111_config_t *config)
{
    uint16_t data = 0;

    data |= JL1111_BMCR_RESET_SET(0)                        /* Normal operation */
         |  JL1111_BMCR_LOOPBACK_SET(config->loopback)      /* configure PCS loopback mode */
         |  JL1111_BMCR_ANE_SET(config->auto_negotiation)   /* configure Auto-Negotiation */
         |  JL1111_BMCR_PWD_SET(0)                          /* Normal operation */
         |  JL1111_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  JL1111_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  JL1111_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == 0) {
        data |= JL1111_BMCR_SPEED0_SET(config->speed);      /* Set port speed */
        data |= JL1111_BMCR_DUPLEX_SET(config->duplex);     /* Set duplex mode */
    }

    /* check the id of jl1111 */
    if (jl1111_check_id(ptr, port, phy_addr) == false) {
        return false;
    }

    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_BMCR, data);

    if (config->media_interface == tsw_port_phy_itf_rmii) {
        jl1111_set_itf(ptr, port, phy_addr, jl1111_config_rmii_itf);
        jl1111_set_rmii_skew(ptr, port, phy_addr, 0x0f, 0x0f);
        if (config->rmii_refclk_dir == jl1111_config_refclk_input) {
            jl1111_set_rmii_refclk_direction(ptr, port, phy_addr, jl1111_config_refclk_input);
        } else {
            jl1111_set_rmii_refclk_direction(ptr, port, phy_addr, jl1111_config_refclk_output);
        }
    } else if (config->media_interface == tsw_port_phy_itf_mii) {
        jl1111_set_itf(ptr, port, phy_addr, jl1111_config_mii_itf);
    }

    return true;
}

static void jl1111_set_speed_from_bmcr(uint16_t bmcr, tsw_phy_status_t *status)
{
    status->tsw_phy_speed = JL1111_BMCR_SPEED0_GET(bmcr) ? tsw_phy_port_speed_100mbps : tsw_phy_port_speed_10mbps;
    status->tsw_phy_duplex = JL1111_BMCR_DUPLEX_GET(bmcr);
    status->tsw_phy_speed_valid = 1U;
}

static void jl1111_set_speed_from_anar(uint16_t data, tsw_phy_status_t *status)
{
    if (JL1111_ANAR_100BASE_TX_FD_GET(data)) {
        status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
        status->tsw_phy_duplex = tsw_phy_duplex_full;
        status->tsw_phy_speed_valid = 1U;
    } else if (JL1111_ANAR_100BASE_TX_GET(data)) {
        status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
        status->tsw_phy_duplex = tsw_phy_duplex_half;
        status->tsw_phy_speed_valid = 1U;
    } else if (JL1111_ANAR_10BASE_T_FD_GET(data)) {
        status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
        status->tsw_phy_duplex = tsw_phy_duplex_full;
        status->tsw_phy_speed_valid = 1U;
    } else if (JL1111_ANAR_10BASE_T_GET(data)) {
        status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
        status->tsw_phy_duplex = tsw_phy_duplex_half;
        status->tsw_phy_speed_valid = 1U;
    }
}

hpm_stat_t jl1111_get_phy_status(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, tsw_phy_status_t *status)
{
    uint16_t bmsr, bmcr, anar, anlpar;
    hpm_stat_t stat;

    if (status == NULL) {
        return status_invalid_argument;
    }

    status->tsw_phy_speed_valid = 0U;

    /* Page 0: Reg1 is BMSR. Some chips may still return the same BMSR when reading Reg1 on other pages,
     * but keep PAGESEL=0 for IEEE Clause 22 and in case other registers differ by page (e.g. BMCR).
     */
    stat = tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 0);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }

    /* BMSR link bit is latched; second read is current state per IEEE 802.3 */
    stat = tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_BMSR, &bmsr);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    stat = tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_BMSR, &bmsr);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    status->tsw_phy_link = JL1111_BMSR_LINK_STATUS_GET(bmsr);

    if (status->tsw_phy_link == 0U) {
        return status_success;
    }

    stat = tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_BMCR, &bmcr);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    if (JL1111_BMCR_ANE_GET(bmcr) == 0U) {
        jl1111_set_speed_from_bmcr(bmcr, status);
        return status_success;
    }

    if (JL1111_BMSR_AUTO_NEGOTIATION_COMPLETE_GET(bmsr) == 0U) {
        return status_success;
    }

    stat = tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_ANAR, &anar);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    stat = tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_ANLPAR, &anlpar);
    if (stat != status_success) {
        status->tsw_phy_link = tsw_phy_link_unknown;
        return stat;
    }
    jl1111_set_speed_from_anar(anar & anlpar, status);

    return status_success;
}

void jl1111_set_itf(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint8_t itf)
{
    uint16_t data = 0;

    /* select page 7 */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_RMSR_P7, &data);

    /* set interface */
    if (itf == jl1111_config_mii_itf) {
        data &= ~BIT3_MASK;   /* MII interface */
    } else {
        data |= BIT3_MASK;    /* RMII interface */
    }

    /* write register RMSR */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_RMSR_P7, data);

    /* return to IEEE register map so later BMCR/BMSR MDIO reads are valid */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 0);
}

void jl1111_set_rmii_refclk_direction(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint8_t dir)
{
    uint16_t data = 0;

    /* select page 7 */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_RMSR_P7, &data);

    /* set tx clock direction */
    if (dir == jl1111_config_refclk_output) {
        data &= ~BIT12_MASK; /* PHY output 50MHz */
    } else {
        data |= BIT12_MASK;  /* PHY input 50MHz */
    }

    /* write register RMSR */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_RMSR_P7, data);

    /* return to IEEE register map so later BMCR/BMSR MDIO reads are valid */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 0);
}

void jl1111_set_rmii_skew(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint8_t tx_skew, uint8_t rx_skew)
{
    uint16_t data = 0;

    /* select page 7 */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    tsw_ep_mdio_read(ptr, port, phy_addr, JL1111_RMSR_P7, &data);

    /* set TX skew */
    data = (data & ~(0xf << 8)) | (tx_skew & 0xf) << 8;

    /* set RX skew */
    data = (data & ~(0xf << 4)) | (rx_skew & 0xf) << 4;

    /* write register RMSR */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_RMSR_P7, data);

    /* return to IEEE register map so later BMCR/BMSR MDIO reads are valid */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 0);
}

void jl1111_disable_broadcast_response(TSW_Type *ptr, uint8_t port, uint32_t phy_addr)
{
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 128);
    tsw_ep_mdio_write(ptr, port, phy_addr, 19, (phy_addr << 5) | 0x1f);

    /* return to IEEE register map so later BMCR/BMSR MDIO reads are valid */
    tsw_ep_mdio_write(ptr, port, phy_addr, JL1111_PAGESEL, 0);
}
