/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   phydev.h
 * \author phil.seipt@ipms.fraunhofer.de
 * \date   08.06.2021
 * \brief  PHY devices
 *
 */

#ifndef PHYDEV_H_
#define PHYDEV_H_

#include <stdint.h>
//#include <kernel/net/phy.h>
#include <kernel/net/netdev.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

#define PHY_MAX_ADDR        32
#define PHY_UPDATE_DELAY_MS 2000

/**
 * \brief phydev register access
 */
enum phydev_reg_access {
    PHYDEV_ACCESS_SET, //!< set register
    PHYDEV_ACCESS_GET, //!< get register
};

/**
 * \brief PHY device driver
 */
struct phydev_s {
    uint32_t id;                                           //!< device driver id
    void *prv;                                             //!< pointer to private data
    struct netdev_s *netdev;                               //!< pointer to netdevice
    struct mdiobus_s *mdio;                                //!< pointer to MDIO bus driver
    uint32_t phyad;                                        //!< PHY address at MDIO bus
    int speed;                                             //!< last known speed
    int aneg_on;                                           //!< autonegotiation enabled
    uint32_t fullduplex;                                   //!< last known fullduplex status
    void (*open)(struct phydev_s *phy, void *config);      //!< function pointer, bring up the PHY device
    void (*update_link)(struct phydev_s *phy);             //!< function pointer, update link status (speed, duplex)
    void (*aneg_enable)(struct phydev_s *phy, int enable); //!< function pointer, enable/disable auto negotiation
    void (*force_speed)(struct phydev_s *phy, int speed);  //!< function pointer, force PHY to speed
    void (*reg_dump)(struct phydev_s *phy);                //!< function pointer, print PHY registers
    void (*restart)(struct phydev_s *phy);                 //!< function pointer, restart PHY
    void (*reg_modify)(struct phydev_s *phy, enum phydev_reg_access access, uint32_t reg, uint32_t *value); //!< function pointer, PHY register modify
};

void phydev_init(void);
struct phydev_s *phydev_register(struct netdev_s *netdev, struct mdiobus_s *mdio);
void phydev_start(struct phydev_s *phy, uint32_t phyad);
void phydev_update_link(struct phydev_s *phy);
void phydev_aneg_enable(struct phydev_s *phy, int enable);
void phydev_force_speed(struct phydev_s *phy, int speed);
void phydev_reset(struct phydev_s *phy);
void phydev_reg_dump(struct phydev_s *phy);
int phydev_rreg(struct phydev_s *phy, uint32_t reg);
void phydev_wreg(struct phydev_s *phy, uint32_t reg, uint32_t value);
void phydev_restart(struct phydev_s *phy);
int32_t phydev_modify(struct phydev_s *phy, uint32_t regad, uint16_t clrmask, uint16_t setmask);

#endif /* PHYDEV_H_ */
