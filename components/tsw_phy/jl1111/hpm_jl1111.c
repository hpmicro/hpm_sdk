/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_drv.h"
#include "hpm_jl1111_regs.h"
#include "hpm_jl1111.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool jl1111_check_id(TSW_Type *ptr, uint8_t port)
{
    uint16_t id1, id2;

    tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_PHYID1, &id1);
    tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_PHYID2, &id2);

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
void jl1111_reset(TSW_Type *ptr, uint8_t port)
{
    uint16_t data;

    /* PHY reset */
    tsw_ep_mdio_write(ptr, port, JL1111_ADDR, JL1111_BMCR, JL1111_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_BMCR, &data);
    } while (JL1111_BMCR_RESET_GET(data));
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

bool jl1111_basic_mode_init(TSW_Type *ptr, uint8_t port, jl1111_config_t *config)
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
    if (jl1111_check_id(ptr, port) == false) {
        return false;
    }

    tsw_ep_mdio_write(ptr, port, JL1111_ADDR, JL1111_BMCR, data);

    return true;
}

void jl1111_get_phy_status(TSW_Type *ptr, uint8_t port, tsw_phy_status_t *status)
{
    uint16_t data, anar, anlpar;

    tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_BMSR, &data);
    status->tsw_phy_link = JL1111_BMSR_LINK_STATUS_GET(data);

    tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_ANAR, &anar);
    tsw_ep_mdio_read(ptr, port, JL1111_ADDR, JL1111_ANLPAR, &anlpar);
    data = anar & anlpar;

    if (JL1111_ANAR_100BASE_TX_GET(data)) {
        if (JL1111_ANAR_100BASE_TX_FD_GET(data)) {
            status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_full;
        } else {
            status->tsw_phy_speed = tsw_phy_port_speed_100mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_half;
        }
    } else if (JL1111_ANAR_10BASE_T_GET(data)) {
        if (JL1111_ANAR_10BASE_T_FD_GET(data)) {
            status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_full;
        } else {
            status->tsw_phy_speed = tsw_phy_port_speed_10mbps;
            status->tsw_phy_duplex = tsw_phy_duplex_half;
        }
    } else {

    }
}
