/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   phydev.c
 * \author phil.seipt@ipms.fraunhofer.de
 * \date   08.06.2021
 * \brief  PHY devices
 *
 */

#include "phydev.h"
#include "mdiobus.h"
#include <kernel/base/kernel.h>
#include <kernel/base/klibc.h>
#include <kernel/driver/phy/phy.h>

// ----------------------------------------------------------------
static const char *PHY_CONTEXT = "PHY";

static uint32_t _phy_dev_cnt = 0;
static list_data_t *phy_list;
static TaskHandle_t phy_task;

static void _taskPhyMonitor(void *pvParameters);
static int32_t _phydev_mdiobus_probe_from(struct mdiobus_s *bus, uint32_t from, uint32_t *id);
static void _phydev_generic_aneg_enable(struct phydev_s *phy, int enable);
static void _phydev_generic_force_speed(struct phydev_s *phy, int speed);
static void _phydev_generic_restart(struct phydev_s *phy);
static void _phydev_generic_reg_dump(struct phydev_s *phy);
static void _phydev_generic_reg_modify(struct phydev_s *phy, enum phydev_reg_access access, uint32_t reg, uint32_t *value);

// ----------------------------------------------------------------
/**
 * \brief Initialize the PHY device system, is called from the kernel
 */
void phydev_init(void)
{
    phy_list = list_create();
    kassert(phy_list != NULL);

    /* create sys task */
    kTaskCreate_a("KERNEL", _taskPhyMonitor, "phy_task", configMINIMAL_STACK_SIZE, (void *)phy_list, KERNEL_PRIO_BG, &phy_task);
}

/**
 * \brief Allocate and register MDIO bus device driver
 *
 * @param netdev    ptr to netdev device
 * @param mdio      ptr to MDIO bus device driver
 *
 * @return          pointer to driver structure, NULL on error
 */
struct phydev_s *phydev_register(struct netdev_s *netdev, struct mdiobus_s *mdio)
{
    struct phydev_s *p;

    // alloc struct
    p = (struct phydev_s *)kallocz(sizeof(struct phydev_s));
    if (!p)
        return p;

    list_add(phy_list, p);

    netdev->phy = p;

    // set fields
    p->netdev = netdev;
    p->mdio = mdio;
    p->id = _phy_dev_cnt;
    _phy_dev_cnt++;
    p->aneg_enable = _phydev_generic_aneg_enable;
    p->force_speed = _phydev_generic_force_speed;
    p->restart = _phydev_generic_restart;
    p->reg_dump = _phydev_generic_reg_dump;
    p->reg_modify = _phydev_generic_reg_modify;
    p->open = NULL;

    return p;
}

/**
 * \brief Start PHY
 *
 * @param phy      	ptr to PHY device
 * @param phyad		PHY address, PHY_MAX_ADDR start automatic address search
 *
 */
void phydev_start(struct phydev_s *phy, uint32_t phyad)
{
    int32_t tmp;
    uint32_t phyid;

    if (phy->mdio == NULL) {
        logk(LOG_ALWAYS, PHY_CONTEXT, "NO MDIO bus device registered!\n");
    } else {
        if (phyad == PHY_MAX_ADDR) {
            logs(LOG_ALWAYS, PHY_CONTEXT, "Probe PHYs on mdio%d ... ", phy->mdio->id);
            tmp = _phydev_mdiobus_probe_from(phy->mdio, 0, &phyid);
            if (tmp < 0) {
                loge(LOG_ALWAYS, "NO PHY!\n");
            } else {
                loge(LOG_ALWAYS, "ad=%d id=%08X\n", tmp, phyid);
            }
            phy->phyad = tmp;
        } else {
            if (_phydev_mdiobus_probe_from(phy->mdio, phyad, &phyid) != phyad)
                logk(LOG_ALWAYS, PHY_CONTEXT, "NO PHY at %d!\n", phyad);

            phy->phyad = phyad;
        }
    }

    if (phy->open)
        phy->open(phy, NULL);
}

/**
 * \brief Update link status of PHY
 *
 * @param phy       pointer to device driver
 */
void phydev_update_link(struct phydev_s *phy)
{
    if (!phy)
        return;
    if (phy->update_link)
        phy->update_link(phy);
}

/**
 * \brief Restart auto-negotiation of PHY
 *
 * @param phy       pointer to device driver
 * @param enable	enable/disable autonegotiation
 */
void phydev_aneg_enable(struct phydev_s *phy, int enable)
{
    if (!phy)
        return;
    if (phy->aneg_enable)
        phy->aneg_enable(phy, enable);
}

/**
 * \brief Force PHY to given link speed
 *
 * @param phy       pointer to device driver
 * @param speed     speed (10/100/1000)
 */
void phydev_force_speed(struct phydev_s *phy, int speed)
{
    if (!phy)
        return;
    if (phy->force_speed)
        phy->force_speed(phy, speed);
}

/**
 * \brief PHY software reset
 *
 * @param phy       pointer to device driver
 */
void phydev_reset(struct phydev_s *phy)
{
    phydev_modify(phy, PHY_REG_BMCR, 0, BMCR_RESET);
}

/**
 * \brief Force PHY to given link speed
 *
 * @param phy       pointer to device driver
 */
void phydev_reg_dump(struct phydev_s *phy)
{
    if (!phy)
        return;
    if (phy->reg_dump)
        phy->reg_dump(phy);
}

int phydev_rreg(struct phydev_s *phy, uint32_t reg)
{
    uint32_t value = -1;
    if (!phy)
        return value;
    if (phy->reg_modify)
        phy->reg_modify(phy, PHYDEV_ACCESS_GET, reg, &value);
    return value;
}

void phydev_wreg(struct phydev_s *phy, uint32_t reg, uint32_t value)
{
    if (!phy)
        return;
    if (phy->reg_modify)
        phy->reg_modify(phy, PHYDEV_ACCESS_SET, reg, &value);
}

/**
 * \brief Force PHY to restart
 *
 * @param phy       pointer to device driver
 */
void phydev_restart(struct phydev_s *phy)
{
    if (!phy)
        return;
    if (phy->reg_dump)
        phy->restart(phy);
}

/**
 * \brief Read-Modify-Write PHY register
 *
 * @param phy       pointer to device driver
 * @param regad     PHY register address
 * @param clrmask   clear bit vector of read value
 * @param setmask   set bit vector to value before write-back
 * @return
 */
int32_t phydev_modify(struct phydev_s *phy, uint32_t regad, uint16_t clrmask, uint16_t setmask)
{
    int32_t value;
    value = mdiobus_read(phy->mdio, phy->phyad, regad);
    if (value < 0)
        return value;
    value &= ~clrmask;
    value |= setmask;
    return mdiobus_write(phy->mdio, phy->phyad, regad, value);
}

// ----------------------------------------------------------------
static void _taskPhyMonitor(void *pvParameters)
{
    list_data_t *_phy_list = (list_data_t *)pvParameters;
    list_data_t *el;
    struct phydev_s *phy;
    int lastspeed;

    while (1) {
        el = _phy_list;
        vTaskDelay(PHY_UPDATE_DELAY_MS);

        do {
            if (el->data != NULL) {
                phy = (struct phydev_s *)el->data;

                lastspeed = phy->speed;
                phydev_update_link(phy);
                if (lastspeed != phy->speed) {
                    // print information
                    logk(LOG_ALWAYS, PHY_CONTEXT, "speed changed on %s = %d (fd=%d)\n", phy->netdev->name, phy->speed, phy->fullduplex);

                    // update netdev
                    netdev_adjust_link(phy->netdev, phy->speed);
                }
            }
            el = list_next(_phy_list, el);
        } while (!list_end(_phy_list, el));
    }
}

/**
 * \brief Probe for PHY, start probing from given PHY address
 *
 * @param bus       pointer to MDIO bus device driver
 * @param from      starting PHY address
 * @param id        pointer to found PHY ID (reg 2/3 of PHY)
 * @return          >= 0 found PHY address, <0 otherwise
 */
static int32_t _phydev_mdiobus_probe_from(struct mdiobus_s *bus, uint32_t from, uint32_t *id)
{
    uint32_t phyid;
    uint32_t phyad;
    int32_t value;

    for (phyad = from; phyad < PHY_MAX_ADDR; ++phyad) {
        value = bus->read(bus, phyad, PHY_REG_ID1);
        if (value < 0)
            continue;
        if (value != 0xFFFF) {
            // store phyID1
            phyid = (value << 16);
            // read phyID2
            value = bus->read(bus, phyad, PHY_REG_ID2);
            phyid |= (value & 0xFFFF);
            if (id)
                *id = phyid;
            return phyad;
        }
    }

    return -1;
}

static void _phydev_generic_aneg_enable(struct phydev_s *phy, int enable)
{
    if (enable)
        phydev_modify(phy, PHY_REG_BMCR, 0, BMCR_ANENABLE | BMCR_ANRESTART);
    else
        phydev_modify(phy, PHY_REG_BMCR, BMCR_ANENABLE, 0);

    phy->aneg_on = enable;
}

static void _phydev_generic_force_speed(struct phydev_s *phy, int speed)
{
    uint16_t ctl = 0;

    if (speed == 1000) {
        ctl = BMCR_SPEED1000;
    } else if (speed == 100) {
        ctl = BMCR_SPEED100;
    }
    // always force to full duplex
    ctl |= BMCR_FULLDPLX;
    // clear ALL but loopback
    phydev_modify(phy, PHY_REG_BMCR, ~(BMCR_LOOPBACK), ctl);
    phy->aneg_on = 0;
}

static void _phydev_generic_restart(struct phydev_s *phy)
{
    // clear ALL but loopback
    phydev_modify(phy, PHY_REG_BMCR, 0, BMCR_RESET);
    phy->aneg_on = 1;

    if (phy->open)
        phy->open(phy, NULL);
}

static void _phydev_generic_reg_dump(struct phydev_s *phy)
{
    logk(LOG_ALWAYS, PHY_CONTEXT, "%02u = 0x%04X\n", PHY_REG_BMCR, mdiobus_read(phy->mdio, phy->phyad, PHY_REG_BMCR));
    logk(LOG_ALWAYS, PHY_CONTEXT, "%02u = 0x%04X\n", PHY_REG_BMSR, mdiobus_read(phy->mdio, phy->phyad, PHY_REG_BMSR));
    logk(LOG_ALWAYS, PHY_CONTEXT, "%02u = 0x%04X\n", PHY_REG_ID1, mdiobus_read(phy->mdio, phy->phyad, PHY_REG_ID1));
    logk(LOG_ALWAYS, PHY_CONTEXT, "%02u = 0x%04X\n", PHY_REG_ID2, mdiobus_read(phy->mdio, phy->phyad, PHY_REG_ID2));
    logk(LOG_ALWAYS, PHY_CONTEXT, "%02u = 0x%04X\n", PHY_REG_ADV, mdiobus_read(phy->mdio, phy->phyad, PHY_REG_ADV));
}

static void _phydev_generic_reg_modify(struct phydev_s *phy, enum phydev_reg_access access, uint32_t reg, uint32_t *value)
{
    if (access == PHYDEV_ACCESS_GET) {
        *value = mdiobus_read(phy->mdio, phy->phyad, reg);
    } else if (access == PHYDEV_ACCESS_SET) {
        mdiobus_write(phy->mdio, phy->phyad, reg, *value);
    }
}
