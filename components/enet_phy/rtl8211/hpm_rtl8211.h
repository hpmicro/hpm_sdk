/**
 * @file hpm_rtl8211.h
 * @brief RTL8211 Ethernet PHY driver header file
 * @details This file provides the API definitions for the RTL8211 Ethernet PHY chip driver.
 *          The RTL8211 is a single-port 10/100-Mbps Ethernet physical layer transceiver.
 */

/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_RTL8211_H
#define HPM_RTL8211_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_phy.h"
#include "hpm_common.h"
#include "hpm_enet_regs.h"
/*---------------------------------------------------------------------
 *
 *  Macro Const Definitions
 *---------------------------------------------------------------------
 */
#ifndef RTL8211_ADDR
/**
 * @brief Default PHY address for RTL8211
 */
#define RTL8211_ADDR (2U)
#endif

/**
 * @brief RTL8211 PHY ID register 1 value (OUI MSB)
 */
#define RTL8211_ID1  (0x001CU)
/**
 * @brief RTL8211 PHY ID register 2 value (OUI LSB)
 */
#define RTL8211_ID2  (0x32U)

/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief RTL8211 PHY configuration structure
 */
typedef struct {
    bool loopback;              /**< Enable/disable PCS loopback mode */
    uint8_t speed;              /**< Port speed: @ref enet_phy_port_speed_t */
    bool auto_negotiation;      /**< Enable/disable auto-negotiation */
    uint8_t duplex;             /**< Duplex mode: @ref enet_phy_duplex_mode_t */
} rtl8211_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
/**
 * @brief Reset RTL8211 PHY chip
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @retval true Reset operation completed successfully
 * @retval false Reset operation failed (timeout)
 */
bool rtl8211_reset(ENET_Type *ptr, uint32_t phy_addr);

/**
 * @brief Set default configuration for RTL8211 PHY basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[out] config Pointer to the configuration structure to be initialized
 * @details This function initializes the configuration structure with default values:
 *          - Loopback: disabled
 *          - Auto-negotiation: enabled (unless __DISABLE_AUTO_NEGO is defined)
 *          - Speed: 100 Mbps (when auto-negotiation is disabled)
 *          - Duplex: full duplex (when auto-negotiation is disabled)
 */
void rtl8211_basic_mode_default_config(ENET_Type *ptr, rtl8211_config_t *config);

/**
 * @brief Initialize RTL8211 PHY chip in basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] config Pointer to the PHY configuration structure
 * @retval true Initialization completed successfully
 * @retval false Initialization failed (PHY ID check failed)
 * @details This function performs the following operations:
 *          - Verifies the PHY ID to ensure it's a RTL8211 chip
 *          - Configures the PHY according to the provided configuration
 *          - Sets up loopback mode, auto-negotiation, speed, and duplex settings
 */
bool rtl8211_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, rtl8211_config_t *config);

/**
 * @brief Get RTL8211 PHY status
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[out] status Pointer to the status structure to store PHY status
 * @details This function reads the PHY status register and extracts:
 *          - Link status (up/down)
 *          - Port speed (10 Mbps, 100 Mbps, or 1000 Mbps)
 *          - Duplex mode (half or full)
 */
void rtl8211_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_RTL8211_H */
