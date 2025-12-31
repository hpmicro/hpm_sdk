/**
 * @file hpm_dp83867.h
 * @brief DP83867 Ethernet PHY driver header file
 * @details This file provides the API definitions for the DP83867 Ethernet PHY chip driver.
 *          The DP83867 is a single-port 10/100/1000-Mbps Ethernet physical layer transceiver.
 */

/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_DP83867_H
#define HPM_DP83867_H

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
#ifndef DP83867_ADDR
/**
 * @brief Default PHY address for DP83867
 */
#define DP83867_ADDR (0U)
#endif

/**
 * @brief DP83867 PHY ID register 1 value (OUI MSB)
 */
#define DP83867_ID1  (0x2000U)
/**
 * @brief DP83867 PHY ID register 2 value (OUI LSB)
 */
#define DP83867_ID2  (0x28U)

/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief DP83867 PHY configuration structure
 */
typedef struct {
    bool loopback;              /**< Enable/disable PCS loopback mode */
    uint8_t speed;              /**< Port speed: @ref enet_phy_port_speed_t */
    bool auto_negotiation;      /**< Enable/disable auto-negotiation */
    uint8_t duplex;             /**< Duplex mode: @ref enet_phy_duplex_mode_t */
} dp83867_config_t;

/**
 * @brief DP83867 RGMII RX delay configuration
 * @details This enumeration defines the available RX delay values for RGMII interface timing adjustment.
 */
typedef enum {
    DP83867_RX_DELAY_0P25_NS = 0,  /**< RX delay: 0.25 ns */
    DP83867_RX_DELAY_0P50_NS,      /**< RX delay: 0.50 ns */
    DP83867_RX_DELAY_0P75_NS,      /**< RX delay: 0.75 ns */
    DP83867_RX_DELAY_1P00_NS,      /**< RX delay: 1.00 ns */
    DP83867_RX_DELAY_1P25_NS,      /**< RX delay: 1.25 ns */
    DP83867_RX_DELAY_1P50_NS,      /**< RX delay: 1.50 ns */
    DP83867_RX_DELAY_1P75_NS,      /**< RX delay: 1.75 ns */
    DP83867_RX_DELAY_2P00_NS,      /**< RX delay: 2.00 ns */
    DP83867_RX_DELAY_2P25_NS,      /**< RX delay: 2.25 ns */
    DP83867_RX_DELAY_2P50_NS,      /**< RX delay: 2.50 ns */
    DP83867_RX_DELAY_2P75_NS,      /**< RX delay: 2.75 ns */
    DP83867_RX_DELAY_3P00_NS,      /**< RX delay: 3.00 ns */
    DP83867_RX_DELAY_3P25_NS,      /**< RX delay: 3.25 ns */
    DP83867_RX_DELAY_3P50_NS,      /**< RX delay: 3.50 ns */
    DP83867_RX_DELAY_3P75_NS,      /**< RX delay: 3.75 ns */
    DP83867_RX_DELAY_4P00_NS       /**< RX delay: 4.00 ns */
} dp83867_rgmii_rx_delay_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
/**
 * @brief Reset DP83867 PHY chip
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @retval true Reset operation completed successfully
 * @retval false Reset operation failed (timeout)
 */
bool dp83867_reset(ENET_Type *ptr, uint32_t phy_addr);

/**
 * @brief Set default configuration for DP83867 PHY basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[out] config Pointer to the configuration structure to be initialized
 * @details This function initializes the configuration structure with default values:
 *          - Loopback: disabled
 *          - Auto-negotiation: enabled (unless __DISABLE_AUTO_NEGO is defined)
 *          - Speed: 1000 Mbps (when auto-negotiation is disabled)
 *          - Duplex: full duplex (when auto-negotiation is disabled)
 */
void dp83867_basic_mode_default_config(ENET_Type *ptr, dp83867_config_t *config);

/**
 * @brief Initialize DP83867 PHY chip in basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] config Pointer to the PHY configuration structure
 * @retval true Initialization completed successfully
 * @retval false Initialization failed (PHY ID check failed)
 * @details This function performs the following operations:
 *          - Verifies the PHY ID to ensure it's a DP83867 chip
 *          - Configures the PHY according to the provided configuration
 *          - Sets up loopback mode, auto-negotiation, speed, and duplex settings
 */
bool dp83867_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, dp83867_config_t *config);

/**
 * @brief Get DP83867 PHY status
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[out] status Pointer to the status structure to store PHY status
 * @details This function reads the PHY status register and extracts:
 *          - Link status (up/down)
 *          - Port speed (10 Mbps, 100 Mbps, or 1000 Mbps)
 *          - Duplex mode (half or full)
 */
void dp83867_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status);

/**
 * @brief Set MDI crossover mode for DP83867 PHY
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] mode Crossover mode: @ref enet_phy_crossover_mode_t
 * @details This function configures the MDI/MDIX crossover mode for the PHY.
 *          The crossover mode determines how the PHY handles cable connections.
 */
void dp83867_set_mdi_crossover_mode(ENET_Type *ptr, uint32_t phy_addr, enet_phy_crossover_mode_t mode);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_DP83867_H */
