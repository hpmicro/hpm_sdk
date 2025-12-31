/**
 * @file hpm_jl1111.h
 * @brief JL1111 Ethernet PHY driver header file
 * @details This file provides the API definitions for the JL1111 Ethernet PHY chip driver.
 *          The JL1111 is a single-port 10/100-Mbps Ethernet physical layer transceiver.
 */

/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_JL1111_H
#define HPM_JL1111_H

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
#ifndef JL1111_ADDR
/**
 * @brief Default PHY address for JL1111
 */
#define JL1111_ADDR (2U)
#endif

/**
 * @brief JL1111 PHY ID register 1 value (OUI MSB)
 */
#define JL1111_ID1  (0x937CU)
/**
 * @brief JL1111 PHY ID register 2 value (OUI LSB)
 */
#define JL1111_ID2  (0x10U)

/*---------------------------------------------------------------------
 *  Typedef Enum Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief JL1111 interface configuration
 */
typedef enum {
    jl1111_config_mii_itf = 0,   /**< MII interface mode */
    jl1111_config_rmii_itf        /**< RMII interface mode */
} jl1111_config_interface_t;

/**
 * @brief JL1111 reference clock direction configuration
 */
typedef enum {
    jl1111_config_refclk_output = 0, /**< Reference clock output mode */
    jl1111_config_refclk_input        /**< Reference clock input mode */
} jl1111_config_refclk_direction_t;
/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief JL1111 PHY configuration structure
 */
typedef struct {
    bool loopback;              /**< Enable/disable PCS loopback mode */
    uint8_t speed;              /**< Port speed: @ref enet_phy_port_speed_t */
    bool auto_negotiation;      /**< Enable/disable auto-negotiation */
    uint8_t duplex;             /**< Duplex mode: @ref enet_phy_duplex_mode_t */
    uint8_t rmii_refclk_dir;    /**< RMII reference clock direction: @ref jl1111_config_refclk_direction_t */
} jl1111_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
/**
 * @brief Reset JL1111 PHY chip
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @retval true Reset operation completed successfully
 * @retval false Reset operation failed (timeout)
 */
bool jl1111_reset(ENET_Type *ptr, uint32_t phy_addr);

/**
 * @brief Set default configuration for JL1111 PHY basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[out] config Pointer to the configuration structure to be initialized
 * @details This function initializes the configuration structure with default values:
 *          - Loopback: disabled
 *          - Auto-negotiation: enabled (unless __DISABLE_AUTO_NEGO is defined)
 *          - Speed: 100 Mbps (when auto-negotiation is disabled)
 *          - Duplex: full duplex (when auto-negotiation is disabled)
 */
void jl1111_basic_mode_default_config(ENET_Type *ptr, jl1111_config_t *config);

/**
 * @brief Initialize JL1111 PHY chip in basic mode
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] config Pointer to the PHY configuration structure
 * @retval true Initialization completed successfully
 * @retval false Initialization failed (PHY ID check failed)
 * @details This function performs the following operations:
 *          - Verifies the PHY ID to ensure it's a JL1111 chip
 *          - Configures the PHY according to the provided configuration
 *          - Sets up loopback mode, auto-negotiation, speed, and duplex settings
 */
bool jl1111_basic_mode_init(ENET_Type *ptr, uint32_t phy_addr, jl1111_config_t *config);

/**
 * @brief Get JL1111 PHY status
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[out] status Pointer to the status structure to store PHY status
 * @details This function reads the PHY status register and extracts:
 *          - Link status (up/down)
 *          - Port speed (10 Mbps or 100 Mbps)
 *          - Duplex mode (half or full)
 */
void jl1111_get_phy_status(ENET_Type *ptr, uint32_t phy_addr, enet_phy_status_t *status);

/**
 * @brief Set interface mode for JL1111 PHY
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] itf Interface mode: @ref jl1111_config_interface_t
 * @details This function configures the PHY interface mode (MII or RMII).
 */
void jl1111_set_itf(ENET_Type *ptr, uint32_t phy_addr, uint8_t itf);

/**
 * @brief Set RMII reference clock direction for JL1111 PHY
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] dir Reference clock direction: @ref jl1111_config_refclk_direction_t
 * @details This function configures the RMII reference clock direction (input or output).
 */
void jl1111_set_rmii_refclk_direction(ENET_Type *ptr, uint32_t phy_addr, uint8_t dir);

/**
 * @brief Set RMII skew timing for JL1111 PHY
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @param[in] tx_skew TX skew value
 * @param[in] rx_skew RX skew value
 * @details This function configures the RMII TX and RX skew timing values for signal alignment.
 */
void jl1111_set_rmii_skew(ENET_Type *ptr, uint32_t phy_addr, uint8_t tx_skew, uint8_t rx_skew);

/**
 * @brief Disable broadcast response for JL1111 PHY
 * @param[in] ptr Pointer to the ENET peripheral base address
 * @param[in] phy_addr PHY address
 * @details This function disables the PHY's response to broadcast frames.
 */
void jl1111_disable_broadcast_response(ENET_Type *ptr, uint32_t phy_addr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_JL1111_H */
