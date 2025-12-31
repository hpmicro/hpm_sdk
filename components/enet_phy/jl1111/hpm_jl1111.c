/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_drv.h"
#include "hpm_jl1111_regs.h"
#include "hpm_jl1111.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool jl1111_check_id(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t id1, id2;

    id1 = enet_read_phy(ptr, phy_addr, JL1111_PHYID1);
    id2 = enet_read_phy(ptr, phy_addr, JL1111_PHYID2);

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
bool jl1111_reset(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = ENET_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    enet_write_phy(ptr, phy_addr, JL1111_BMCR, JL1111_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        data = enet_read_phy(ptr, phy_addr, JL1111_BMCR);
    } while (JL1111_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void jl1111_basic_mode_default_config(ENET_Type *ptr, jl1111_config_t *config)
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

bool jl1111_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, jl1111_config_t *config)
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
    if (jl1111_check_id(ptr, phy_addr) == false) {
        return false;
    }

    enet_write_phy(ptr, phy_addr, JL1111_BMCR, data);

#if defined(RMII) && RMII
    jl1111_set_itf(ptr, phy_addr, jl1111_config_rmii_itf);
    jl1111_set_rmii_skew(ptr, phy_addr, 0x0f, 0x0f);
    if (config->rmii_refclk_dir == enet_phy_rmii_refclk_dir_in) {
        jl1111_set_rmii_refclk_direction(ptr, phy_addr, jl1111_config_refclk_input);
    } else {
        jl1111_set_rmii_refclk_direction(ptr, phy_addr, jl1111_config_refclk_output);
    }
#elif defined(MII) && MII
    jl1111_set_itf(ptr, phy_addr, jl1111_config_mii_itf);
#endif

    return true;
}

void jl1111_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t data, anar, anlpar;

    data = enet_read_phy(ptr, phy_addr, JL1111_BMSR);
    status->enet_phy_link = JL1111_BMSR_LINK_STATUS_GET(data);

    anar = enet_read_phy(ptr, phy_addr, JL1111_ANAR);
    anlpar = enet_read_phy(ptr, phy_addr, JL1111_ANLPAR);
    data = anar & anlpar;

    if (JL1111_ANAR_100BASE_TX_GET(data)) {
        if (JL1111_ANAR_100BASE_TX_FD_GET(data)) {
            status->enet_phy_speed = enet_phy_port_speed_100mbps;
            status->enet_phy_duplex = enet_phy_duplex_full;
        } else {
            status->enet_phy_speed = enet_phy_port_speed_100mbps;
            status->enet_phy_duplex = enet_phy_duplex_half;
        }
    } else if (JL1111_ANAR_10BASE_T_GET(data)) {
        if (JL1111_ANAR_10BASE_T_FD_GET(data)) {
            status->enet_phy_speed = enet_phy_port_speed_10mbps;
            status->enet_phy_duplex = enet_phy_duplex_full;
        } else {
            status->enet_phy_speed = enet_phy_port_speed_10mbps;
            status->enet_phy_duplex = enet_phy_duplex_half;
        }
    } else {

    }
}

void jl1111_set_itf(ENET_Type *ptr, uint32_t phy_addr, uint8_t itf)
{
    uint16_t data = 0;

    /* select page 7 */
    enet_write_phy(ptr, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    data = enet_read_phy(ptr, phy_addr, JL1111_RMSR_P7);

    /* set interface */
    if (itf == jl1111_config_mii_itf) {
        data &= ~BIT3_MASK;   /* MII interface */
    } else {
        data |= BIT3_MASK;    /* RMII interface */
    }

    /* write register RMSR */
    enet_write_phy(ptr, phy_addr, JL1111_RMSR_P7, data);
}

void jl1111_set_rmii_refclk_direction(ENET_Type *ptr, uint32_t phy_addr, uint8_t dir)
{
    uint16_t data = 0;

    /* select page 7 */
    enet_write_phy(ptr, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    data = enet_read_phy(ptr, phy_addr, JL1111_RMSR_P7);

    /* set tx clock direction */
    if (dir == jl1111_config_refclk_output) {
        data &= ~BIT12_MASK; /* PHY output 50MHz */
    } else {
        data |= BIT12_MASK;  /* PHY input 50MHz */
    }

    /* write register RMSR */
    enet_write_phy(ptr, phy_addr, JL1111_RMSR_P7, data);
}

void jl1111_set_rmii_skew(ENET_Type *ptr, uint32_t phy_addr, uint8_t tx_skew, uint8_t rx_skew)
{
    uint16_t data = 0;

    /* select page 7 */
    enet_write_phy(ptr, phy_addr, JL1111_PAGESEL, 7);

    /* read register RMSR */
    data = enet_read_phy(ptr, phy_addr, JL1111_RMSR_P7);

    /* set TX skew */
    data = (data & ~(0xf << 8)) | (tx_skew & 0xf) << 8;

    /* set RX skew */
    data = (data & ~(0xf << 4)) | (rx_skew & 0xf) << 4;

    /* write register RMSR */
    enet_write_phy(ptr, phy_addr, JL1111_RMSR_P7, data);
}

void jl1111_disable_broadcast_response(ENET_Type *ptr, uint32_t phy_addr)
{
    enet_write_phy(ptr, phy_addr, JL1111_PAGESEL, 128);
    enet_write_phy(ptr, phy_addr, 19, (phy_addr << 5) | 0x1f);
}
