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
#include "hpm_dp83867_regs.h"
#include "hpm_dp83867.h"

/*---------------------------------------------------------------------
 * Internal API
 *---------------------------------------------------------------------
 */
static bool dp83867_check_id(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t id1, id2;

    if (enet_read_phy(ptr, phy_addr, DP83867_PHYIDR1, &id1) != status_success ||
        enet_read_phy(ptr, phy_addr, DP83867_PHYIDR2, &id2) != status_success) {
        return false;
    }

    if (DP83867_PHYIDR1_OUI_MSB_GET(id1) == DP83867_ID1 && DP83867_PHYIDR2_OUI_LSB_GET(id2) == DP83867_ID2) {
        return true;
    } else {
        return false;
    }
}

static hpm_stat_t dp83867_write_phy_ext(ENET_Type *ptr, uint32_t phy_addr, uint32_t addr, uint32_t data)
{
    hpm_stat_t stat;

    /* set the control register for register address */
    stat = enet_write_phy(ptr, phy_addr, DP83867_REGCR, DP83867_REGCR_FUNCTION_SET(0) | DP83867_REGCR_DEVAD_SET(0x1f));
    if (stat != status_success) {
        return stat;
    }

    /* write the specified register address */
    stat = enet_write_phy(ptr, phy_addr, DP83867_ADDAR, addr);
    if (stat != status_success) {
        return stat;
    }

    /* set the control register for register data */
    stat = enet_write_phy(ptr, phy_addr, DP83867_REGCR, DP83867_REGCR_FUNCTION_SET(1) | DP83867_REGCR_DEVAD_SET(0x1f));
    if (stat != status_success) {
        return stat;
    }

    /* write the specified register data */
    return enet_write_phy(ptr, phy_addr, DP83867_ADDAR, data);
}

static hpm_stat_t dp83867_read_phy_ext(ENET_Type *ptr, uint32_t phy_addr, uint32_t addr, uint16_t *data)
{
    hpm_stat_t stat;

    /* set the control register for register address */
    stat = enet_write_phy(ptr, phy_addr, DP83867_REGCR, DP83867_REGCR_FUNCTION_SET(0) | DP83867_REGCR_DEVAD_SET(0x1f));
    if (stat != status_success) {
        return stat;
    }

    /* write the specified register address */
    stat = enet_write_phy(ptr, phy_addr, DP83867_ADDAR, addr);
    if (stat != status_success) {
        return stat;
    }

    /* set the control register for register data */
    stat = enet_write_phy(ptr, phy_addr, DP83867_REGCR, DP83867_REGCR_FUNCTION_SET(1) | DP83867_REGCR_DEVAD_SET(0x1f));
    if (stat != status_success) {
        return stat;
    }

    /* read the specified register data */
    return enet_read_phy(ptr, phy_addr, DP83867_ADDAR, data);
}
/*---------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------
 */
bool dp83867_reset(ENET_Type *ptr, uint32_t phy_addr)
{
    uint16_t data;
    uint32_t retry_cnt = ENET_PHY_SW_RESET_RETRY_CNT;

    /* PHY reset */
    enet_write_phy(ptr, phy_addr, DP83867_BMCR, DP83867_BMCR_RESET_SET(1));

    /* wait until the reset is completed */
    do {
        if (enet_read_phy(ptr, phy_addr, DP83867_BMCR, &data) != status_success) {
            return false;
        }
    } while (DP83867_BMCR_RESET_GET(data) && --retry_cnt);

    return retry_cnt > 0 ? true : false;
}

void dp83867_basic_mode_default_config(ENET_Type *ptr, dp83867_config_t *config)
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

bool dp83867_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, dp83867_config_t *config)
{
    uint16_t data = 0;

    data |= DP83867_BMCR_RESET_SET(0)                        /* Normal operation */
         |  DP83867_BMCR_LOOPBACK_SET(config->loopback)      /* Configure PCS loopback mode */
         |  DP83867_BMCR_ANE_SET(config->auto_negotiation)   /* Configure Auto-Negotiation */
         |  DP83867_BMCR_PWD_SET(0)                          /* Normal operation */
         |  DP83867_BMCR_ISOLATE_SET(0)                      /* Normal operation */
         |  DP83867_BMCR_RESTART_AN_SET(0)                   /* Normal operation (ignored when Auto-Negotiation is disabled) */
         |  DP83867_BMCR_COLLISION_TEST_SET(0);              /* Normal operation */

    if (config->auto_negotiation == false) {
        data |= DP83867_BMCR_SPEED0_SET(config->speed) | DP83867_BMCR_SPEED1_SET(config->speed >> 1); /* Set port speed */
        data |= DP83867_BMCR_DUPLEX_SET(config->duplex);                                              /* Set duplex mode */
    }

    /* check the id of dp83867 */
    if (dp83867_check_id(ptr, phy_addr) == false) {
        return false;
    }

    enet_write_phy(ptr, phy_addr, DP83867_BMCR, data);
    enet_read_phy(ptr, phy_addr, DP83867_BMCR, &data);

    return true;
}

hpm_stat_t dp83867_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status)
{
    uint16_t data, bmcr;
    uint8_t speed;
    hpm_stat_t stat;

    if (status == NULL) {
        return status_invalid_argument;
    }

    status->enet_phy_speed_valid = 0U;

    stat = enet_read_phy(ptr, phy_addr, DP83867_PHYSTS, &data);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    status->enet_phy_link = DP83867_PHYSTS_LINK_STATUS_GET(data);

    if (status->enet_phy_link == 0U) {
        return status_success;
    }

    stat = enet_read_phy(ptr, phy_addr, DP83867_BMCR, &bmcr);
    if (stat != status_success) {
        status->enet_phy_link = enet_phy_link_unknown;
        return stat;
    }
    if (DP83867_BMCR_ANE_GET(bmcr) == 0U) {
        speed = (uint8_t)((DP83867_BMCR_SPEED1_GET(bmcr) << 1U) | DP83867_BMCR_SPEED0_GET(bmcr));
        status->enet_phy_speed = speed == 0U ? enet_phy_port_speed_10mbps : speed == 1U ? enet_phy_port_speed_100mbps : enet_phy_port_speed_1000mbps;
        status->enet_phy_duplex = DP83867_BMCR_DUPLEX_GET(bmcr);
        status->enet_phy_speed_valid = 1U;
        return status_success;
    }

    if (DP83867_PHYSTS_SPEED_DUPLEX_RESOLVED_GET(data) == 0U) {
        return status_success;
    }

    status->enet_phy_speed = DP83867_PHYSTS_SPEED_SELECTION_GET(data) == 0 ? enet_phy_port_speed_10mbps : DP83867_PHYSTS_SPEED_SELECTION_GET(data) == 1 ? enet_phy_port_speed_100mbps : enet_phy_port_speed_1000mbps;
    status->enet_phy_duplex = DP83867_PHYSTS_DUPLEX_MODE_GET(data);
    status->enet_phy_speed_valid = 1U;

    return status_success;
}

void dp83867_set_mdi_crossover_mode(ENET_Type *ptr, uint32_t phy_addr, enet_phy_crossover_mode_t mode)
{
    uint16_t data;

    if (dp83867_read_phy_ext(ptr, phy_addr, DP83867_PHYCR, &data) != status_success) {
        return;
    }
    data &= ~DP83867_PHYCR_MDI_CROSSOVER_MASK;
    data |= DP83867_PHYCR_MDI_CROSSOVER_SET(mode);
    (void)dp83867_write_phy_ext(ptr, phy_addr, DP83867_PHYCR, data);
}
