/**
 * @file hpm_jl1111.h
 * @brief JL1111 TSW PHY driver header file
 * @details This file provides the API definitions for the JL1111 TSW PHY chip driver.
 *          The JL1111 is a single-port 10/100-Mbps Ethernet physical layer transceiver
 *          for Time-Sensitive Networking (TSW) applications.
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
#include "hpm_tsw_phy.h"
#include "hpm_common.h"
#include "hpm_tsw_regs.h"
/*---------------------------------------------------------------------
 *  Macro Const Definitions
 *---------------------------------------------------------------------
 */
#ifndef JL1111_ADDR
/**
 * @brief Default PHY address for JL1111
 */
#define JL1111_ADDR (0U)
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
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
/**
 * @brief JL1111 PHY configuration structure
 */
typedef struct {
    bool loopback;              /**< Enable/disable PCS loopback mode */
    uint8_t speed;              /**< Port speed: @ref tsw_phy_port_speed_t */
    bool auto_negotiation;      /**< Enable/disable auto-negotiation */
    uint8_t duplex;             /**< Duplex mode: @ref tsw_phy_duplex_mode_t */
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
 * @param[in] ptr Pointer to the TSW peripheral base address
 * @param[in] port Port number
 * @details This function performs a software reset on the JL1111 PHY chip.
 */
void jl1111_reset(TSW_Type *ptr, uint8_t port);

/**
 * @brief Set default configuration for JL1111 PHY basic mode
 * @param[in] ptr Pointer to the TSW peripheral base address
 * @param[out] config Pointer to the configuration structure to be initialized
 * @details This function initializes the configuration structure with default values:
 *          - Loopback: disabled
 *          - Auto-negotiation: enabled (unless __DISABLE_AUTO_NEGO is defined)
 *          - Speed: 100 Mbps (when auto-negotiation is disabled)
 *          - Duplex: full duplex (when auto-negotiation is disabled)
 */
void jl1111_basic_mode_default_config(TSW_Type *ptr, jl1111_config_t *config);

/**
 * @brief Initialize JL1111 PHY chip in basic mode
 * @param[in] ptr Pointer to the TSW peripheral base address
 * @param[in] port Port number
 * @param[in] config Pointer to the PHY configuration structure
 * @retval true Initialization completed successfully
 * @retval false Initialization failed (PHY ID check failed)
 * @details This function performs the following operations:
 *          - Verifies the PHY ID to ensure it's a JL1111 chip
 *          - Configures the PHY according to the provided configuration
 *          - Sets up loopback mode, auto-negotiation, speed, and duplex settings
 */
bool jl1111_basic_mode_init(TSW_Type *ptr, uint8_t port, jl1111_config_t *config);

/**
 * @brief Get JL1111 PHY status
 * @param[in] ptr Pointer to the TSW peripheral base address
 * @param[in] port Port number
 * @param[out] status Pointer to the status structure to store PHY status
 * @details This function reads the PHY status register and extracts:
 *          - Link status (up/down)
 *          - Port speed (10 Mbps or 100 Mbps)
 *          - Duplex mode (half or full)
 */
void jl1111_get_phy_status(TSW_Type *ptr, uint8_t port, tsw_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_JL1111_H */
