/**
 * @file hpm_lan8720.h
 * @brief LAN8720 Ethernet PHY driver header file
 * @details This file provides the API definitions for the LAN8720 Ethernet PHY chip driver.
 *          The LAN8720 is a single-port 10/100-Mbps Ethernet physical layer transceiver.
 */

/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_LAN8720_H
#define HPM_LAN8720_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_enet_phy.h"
#include "hpm_common.h"
#include "hpm_enet_regs.h"
/*---------------------------------------------------------------------
 *  Macro Const Definitions
 *---------------------------------------------------------------------
 */
#ifndef LAN8720_ADDR
/**
 * @brief Default PHY address for LAN8720
 */
#define LAN8720_ADDR (0U)
#endif

/**
 * @brief LAN8720 PHY ID register 1 value (OUI MSB)
 */
#define LAN8720_ID1  (0x0007U)
/**
 * @brief LAN8720 PHY ID register 2 value (OUI LSB)
 */
#define LAN8720_ID2  (0x30U)

/*---------------------------------------------------------------------
 *  Typedef Enum Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief LAN8720 reference clock direction configuration
 */
typedef enum {
    lan8720_config_refclk_output = 0,  /**< Reference clock output mode */
    lan8720_config_refclk_input        /**< Reference clock input mode */
} lan8720_config_refclk_direction_t;
/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief LAN8720 PHY configuration structure
 */
typedef struct {
    bool loopback;              /**< Enable/disable PCS loopback mode */
    uint8_t speed;              /**< Port speed: @ref enet_phy_port_speed_t */
    bool auto_negotiation;      /**< Enable/disable auto-negotiation */
    uint8_t duplex;             /**< Duplex mode: @ref enet_phy_duplex_mode_t */
} lan8720_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
/**
 * @brief Reset LAN8720 PHY chip
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @retval true Reset operation completed successfully
 * @retval false Reset operation failed (timeout)
 */
bool lan8720_reset(ENET_Type *ptr, uint32_t phy_addr);

/**
 * @brief Set default configuration for LAN8720 PHY basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[out] config Pointer to the configuration structure to be initialized
 * @details This function initializes the configuration structure with default values:
 *          - Loopback: disabled
 *          - Auto-negotiation: enabled (unless __DISABLE_AUTO_NEGO is defined)
 *          - Speed: 100 Mbps (when auto-negotiation is disabled)
 *          - Duplex: full duplex (when auto-negotiation is disabled)
 */
void lan8720_basic_mode_default_config(ENET_Type *ptr, lan8720_config_t *config);

/**
 * @brief Initialize LAN8720 PHY chip in basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] config Pointer to the PHY configuration structure
 * @retval true Initialization completed successfully
 * @retval false Initialization failed (PHY ID check failed)
 * @details This function performs the following operations:
 *          - Verifies the PHY ID to ensure it's a LAN8720 chip
 *          - Configures the PHY according to the provided configuration
 *          - Sets up loopback mode, auto-negotiation, speed, and duplex settings
 */
bool lan8720_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, lan8720_config_t *config);

/**
 * @brief Get LAN8720 PHY status
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[out] status Pointer to the status structure to store PHY status
 * @details This function reads the PHY status register and extracts:
 *          - Link status (up/down)
 *          - Port speed (10 Mbps or 100 Mbps)
 *          - Duplex mode (half or full)
 */
void lan8720_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_LAN8720_H */
