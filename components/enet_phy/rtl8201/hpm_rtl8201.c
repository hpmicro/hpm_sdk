/*
 * Copyright (c) 2021-2025 HPMicro
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

    id1 = enet_read_phy(ptr, phy_addr, RTL8201_PHYID1);
    id2 = enet_read_phy(ptr, phy_addr, RTL8201_PHYID2);

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
        data = enet_read_phy(ptr, phy_addr, RTL8201_BMCR);
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

#if defined(RMII) && RMII
    if (config->rmii_refclk_dir == enet_phy_rmii_refclk_dir_in) {
        rtl8201_set_rmii_refclk_direction(ptr, phy_addr, rtl8201_config_refclk_input);
    } else {
        rtl8201_set_rmii_refclk_direction(ptr, phy_addr, rtl8201_config_refclk_output);
    }
#endif

    return true;
}

void rtl8201_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t data;

    data = enet_read_phy(ptr, phy_addr, RTL8201_BMSR);
    status->enet_phy_link = RTL8201_BMSR_LINK_STATUS_GET(data);

    data = enet_read_phy(ptr, phy_addr, RTL8201_BMCR);
    status->enet_phy_speed = RTL8201_BMCR_SPEED0_GET(data) == 0 ? enet_phy_port_speed_10mbps : enet_phy_port_speed_100mbps;
    status->enet_phy_duplex = RTL8201_BMCR_DUPLEX_GET(data);
}

void rtl8201_set_rmii_refclk_direction(ENET_Type *ptr, uint32_t phy_addr, uint8_t dir)
{
    uint16_t data = 0;

    /* select page 7 */
    enet_write_phy(ptr, phy_addr, RTL8201_PAGESEL, 7);

    /* read register RMSR */
    data = enet_read_phy(ptr, phy_addr, RTL8201_RMSR_P7);

    /* set tx clock direction */
    if (dir == rtl8201_config_refclk_output) {
        data &= ~BIT12_MASK; /* PHY output 50MHz */
    } else {
        data |= BIT12_MASK;  /* PHY input 50MHz */
    }

    /* write register RMSR */
    enet_write_phy(ptr, phy_addr, RTL8201_RMSR_P7, data);
}