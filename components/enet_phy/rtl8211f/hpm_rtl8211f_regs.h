/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * RTL8211F-CG register definitions based on Realtek RTL8211F(I)/RTL8211FD(I)
 * Integrated 10/100/1000M Ethernet Transceiver Datasheet
 */

#ifndef HPM_RTL8211F_REGS_H
#define HPM_RTL8211F_REGS_H

typedef enum {
    RTL8211F_BMCR                        = 0,   /* 0x0: Basic Mode Control Register */
    RTL8211F_BMSR                        = 1,   /* 0x1: Basic Mode Status Register */
    RTL8211F_PHYID1                      = 2,   /* 0x2: PHY Identifier Register 1 */
    RTL8211F_PHYID2                      = 3,   /* 0x3: PHY Identifier Register 2 */
    RTL8211F_PHYSR                       = 26,  /* 0x1A: PHY Specific Status Register (page 0xa43) */
    RTL8211F_PAGESEL                     = 31,  /* 0x1F: Page Select Register */
} RTL8211F_REG_Type;

#define RTL8211F_PAGE_IEEE (0x0A42U)  /* default page; IEEE Clause 22 registers 0-15 */
#define RTL8211F_PAGE_PHY  (0x0A43U)  /* PHYCR1/PHYCR2/PHYSR/INSR */


/* Bitfield definition for register: BMCR */
/*
 * RESET (RW/SC)
 *
 * Reset.
 * 1: PHY reset
 * 0: Normal operation
 * Register 0 (BMCR) and register 1 (BMSR) will return to default
 * values after a software reset (set Bit15 to 1).
 * This action may change the internal PHY state and the state of the
 * physical link associated with the PHY.
 */
#define RTL8211F_BMCR_RESET_MASK (0x8000U)
#define RTL8211F_BMCR_RESET_SHIFT (15U)
#define RTL8211F_BMCR_RESET_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_RESET_SHIFT) & RTL8211F_BMCR_RESET_MASK)
#define RTL8211F_BMCR_RESET_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_RESET_MASK) >> RTL8211F_BMCR_RESET_SHIFT)

/*
 * LOOPBACK (RW)
 *
 * Loopback Mode.
 * 1: Enable PCS loopback mode
 * 0: Disable PCS loopback mode
 */
#define RTL8211F_BMCR_LOOPBACK_MASK (0x4000U)
#define RTL8211F_BMCR_LOOPBACK_SHIFT (14U)
#define RTL8211F_BMCR_LOOPBACK_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_LOOPBACK_SHIFT) & RTL8211F_BMCR_LOOPBACK_MASK)
#define RTL8211F_BMCR_LOOPBACK_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_LOOPBACK_MASK) >> RTL8211F_BMCR_LOOPBACK_SHIFT)

/*
 * SPEED0 (RW)
 *
 * Speed Select (Bits 6, 13):
 * When auto-negotiation is disabled writing to these bits allows the port
 * speed to be selected.
 * 11 = Reserved
 * 10 = 1000 Mbps
 * 01 = 100 Mbps
 * 00 = 10 Mbps
 */
#define RTL8211F_BMCR_SPEED0_MASK (0x2000U)
#define RTL8211F_BMCR_SPEED0_SHIFT (13U)
#define RTL8211F_BMCR_SPEED0_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_SPEED0_SHIFT) & RTL8211F_BMCR_SPEED0_MASK)
#define RTL8211F_BMCR_SPEED0_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_SPEED0_MASK) >> RTL8211F_BMCR_SPEED0_SHIFT)

/*
 * ANE (RW)
 *
 * Auto-Negotiation Enable.
 * 1: Enable Auto-Negotiation
 * 0: Disable Auto-Negotiation
 */
#define RTL8211F_BMCR_ANE_MASK (0x1000U)
#define RTL8211F_BMCR_ANE_SHIFT (12U)
#define RTL8211F_BMCR_ANE_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_ANE_SHIFT) & RTL8211F_BMCR_ANE_MASK)
#define RTL8211F_BMCR_ANE_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_ANE_MASK) >> RTL8211F_BMCR_ANE_SHIFT)

/*
 * PWD (RW)
 *
 * Power Down.
 * 1: Power down (only Management Interface and logic are active; link
 * is down)
 * 0: Normal operation
 */
#define RTL8211F_BMCR_PWD_MASK (0x800U)
#define RTL8211F_BMCR_PWD_SHIFT (11U)
#define RTL8211F_BMCR_PWD_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_PWD_SHIFT) & RTL8211F_BMCR_PWD_MASK)
#define RTL8211F_BMCR_PWD_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_PWD_MASK) >> RTL8211F_BMCR_PWD_SHIFT)

/*
 * ISOLATE (RW)
 *
 * Isolate.
 * 1: RGMII/GMII interface is isolated; the serial management interface
 * (MDC, MDIO) is still active. When this bit is asserted, the
 * RTL8211F(I)/RTL8211FD(I) ignores TXD[7:0], and TXCLT
 * inputs, and presents a high impedance on TXC, RXC, RXCLT,
 * RXD[7:0].
 * 0: Normal operation
 */
#define RTL8211F_BMCR_ISOLATE_MASK (0x400U)
#define RTL8211F_BMCR_ISOLATE_SHIFT (10U)
#define RTL8211F_BMCR_ISOLATE_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_ISOLATE_SHIFT) & RTL8211F_BMCR_ISOLATE_MASK)
#define RTL8211F_BMCR_ISOLATE_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_ISOLATE_MASK) >> RTL8211F_BMCR_ISOLATE_SHIFT)

/*
 * RESTART_AN (RW/SC)
 *
 * Restart Auto-Negotiation.
 * 1: Restart Auto-Negotiation
 * 0: Normal operation
 */
#define RTL8211F_BMCR_RESTART_AN_MASK (0x200U)
#define RTL8211F_BMCR_RESTART_AN_SHIFT (9U)
#define RTL8211F_BMCR_RESTART_AN_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_RESTART_AN_SHIFT) & RTL8211F_BMCR_RESTART_AN_MASK)
#define RTL8211F_BMCR_RESTART_AN_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_RESTART_AN_MASK) >> RTL8211F_BMCR_RESTART_AN_SHIFT)

/*
 * DUPLEX (RW)
 *
 * Duplex Mode.
 * 1: Full Duplex operation
 * 0: Half Duplex operation
 * This bit is valid only in force mode, i.e., NWay is disabled.
 */
#define RTL8211F_BMCR_DUPLEX_MASK (0x100U)
#define RTL8211F_BMCR_DUPLEX_SHIFT (8U)
#define RTL8211F_BMCR_DUPLEX_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_DUPLEX_SHIFT) & RTL8211F_BMCR_DUPLEX_MASK)
#define RTL8211F_BMCR_DUPLEX_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_DUPLEX_MASK) >> RTL8211F_BMCR_DUPLEX_SHIFT)

/*
 * COLLISION_TEST (RW)
 *
 * Collision Test.
 * 1: Collision test enabled
 * 0: Normal operation
 */
#define RTL8211F_BMCR_COLLISION_TEST_MASK (0x80U)
#define RTL8211F_BMCR_COLLISION_TEST_SHIFT (7U)
#define RTL8211F_BMCR_COLLISION_TEST_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_COLLISION_TEST_SHIFT) & RTL8211F_BMCR_COLLISION_TEST_MASK)
#define RTL8211F_BMCR_COLLISION_TEST_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_COLLISION_TEST_MASK) >> RTL8211F_BMCR_COLLISION_TEST_SHIFT)

/*
 * SPEED1 (RW)
 *
 * Speed Select Bit 1.
 * Refer to bit 13. Bits 6 and 13 together select speed:
 * 11=Reserved, 10=1000Mbps, 01=100Mbps, 00=10Mbps.
 */
#define RTL8211F_BMCR_SPEED1_MASK (0x40U)
#define RTL8211F_BMCR_SPEED1_SHIFT (6U)
#define RTL8211F_BMCR_SPEED1_SET(x) (((uint16_t)(x) << RTL8211F_BMCR_SPEED1_SHIFT) & RTL8211F_BMCR_SPEED1_MASK)
#define RTL8211F_BMCR_SPEED1_GET(x) (((uint16_t)(x) & RTL8211F_BMCR_SPEED1_MASK) >> RTL8211F_BMCR_SPEED1_SHIFT)

/* Bitfield definition for register: PHYID1 */
/*
 * OUI_MSB (RO)
 *
 * Organizationally Unique Identifier Bit 3:18.
 * Always 0000000000011100 (0x001C) for Realtek.
 */
#define RTL8211F_PHYID1_OUI_MSB_MASK (0xFFFFU)
#define RTL8211F_PHYID1_OUI_MSB_SHIFT (0U)
#define RTL8211F_PHYID1_OUI_MSB_GET(x) (((uint16_t)(x) & RTL8211F_PHYID1_OUI_MSB_MASK) >> RTL8211F_PHYID1_OUI_MSB_SHIFT)

/* Bitfield definition for register: PHYID2 */
/*
 * OUI_LSB (RO)
 *
 * Organizationally Unique Identifier Bit 19:24.
 * Part of PHY ID for RTL8211F identification.
 */
#define RTL8211F_PHYID2_OUI_LSB_MASK (0xFC00U)
#define RTL8211F_PHYID2_OUI_LSB_SHIFT (10U)
#define RTL8211F_PHYID2_OUI_LSB_GET(x) (((uint16_t)(x) & RTL8211F_PHYID2_OUI_LSB_MASK) >> RTL8211F_PHYID2_OUI_LSB_SHIFT)

/*
 * MODEL_NUMBER (RO)
 *
 * Manufacture's Model Number.
 * RTL8211F model number is 0x11 (bits 9:4 of PHYID2).
 */
#define RTL8211F_PHYID2_MODEL_NUMBER_MASK (0x3F0U)
#define RTL8211F_PHYID2_MODEL_NUMBER_SHIFT (4U)
#define RTL8211F_PHYID2_MODEL_NUMBER_GET(x) (((uint16_t)(x) & RTL8211F_PHYID2_MODEL_NUMBER_MASK) >> RTL8211F_PHYID2_MODEL_NUMBER_SHIFT)

/*
 * REVISION_NUMBER (RO)
 *
 * Revision Number.
 * Revision number of the RTL8211F device.
 */
#define RTL8211F_PHYID2_REVISION_NUMBER_MASK (0xFU)
#define RTL8211F_PHYID2_REVISION_NUMBER_SHIFT (0U)
#define RTL8211F_PHYID2_REVISION_NUMBER_GET(x) (((uint16_t)(x) & RTL8211F_PHYID2_REVISION_NUMBER_MASK) >> RTL8211F_PHYID2_REVISION_NUMBER_SHIFT)

/* Bitfield definition for register: BMSR */
/*
 * AUTO_NEGOTIATION_COMPLETE (RO)
 *
 * Auto-Negotiation Complete.
 * 1: Auto-Negotiation process complete, and contents of registers
 * 5, 6, 8, and 10 are valid
 * 0: Auto-Negotiation process not complete
 */
#define RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_MASK (0x20U)
#define RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_SHIFT (5U)
#define RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_GET(x) (((uint16_t)(x) & RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_MASK) >> RTL8211F_BMSR_AUTO_NEGOTIATION_COMPLETE_SHIFT)

/* Bitfield definition for register: PHYSR (page 0xa43) */
/*
 * SPEED (RO) - Bits 5:4
 *
 * Link Speed.
 * 11: Reserved  10: 1000Mbps
 * 01: 100Mbps  00: 10Mbps
 */
#define RTL8211F_PHYSR_SPEED_MASK (0x0030U)
#define RTL8211F_PHYSR_SPEED_SHIFT (4U)
#define RTL8211F_PHYSR_SPEED_GET(x) (((uint16_t)(x) & RTL8211F_PHYSR_SPEED_MASK) >> RTL8211F_PHYSR_SPEED_SHIFT)

/*
 * DUPLEX (RO) - Bit 3
 *
 * Full/Half Duplex Mode.
 * 1: Full duplex  0: Half duplex
 */
#define RTL8211F_PHYSR_DUPLEX_MASK (0x0008U)
#define RTL8211F_PHYSR_DUPLEX_SHIFT (3U)
#define RTL8211F_PHYSR_DUPLEX_GET(x) (((uint16_t)(x) & RTL8211F_PHYSR_DUPLEX_MASK) >> RTL8211F_PHYSR_DUPLEX_SHIFT)

/*
 * LINK_REAL_TIME (RO) - Bit 2
 *
 * Real Time Link Status.
 * 1: Link OK  0: Link not OK
 */
#define RTL8211F_PHYSR_LINK_REAL_TIME_MASK (0x0004U)
#define RTL8211F_PHYSR_LINK_REAL_TIME_SHIFT (2U)
#define RTL8211F_PHYSR_LINK_REAL_TIME_GET(x) (((uint16_t)(x) & RTL8211F_PHYSR_LINK_REAL_TIME_MASK) >> RTL8211F_PHYSR_LINK_REAL_TIME_SHIFT)


#endif /* HPM_RTL8211F_REGS_H */
