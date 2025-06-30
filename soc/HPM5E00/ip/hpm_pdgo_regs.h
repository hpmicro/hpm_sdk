/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_PDGO_H
#define HPM_PDGO_H

typedef struct {
    __W  uint32_t DGO_TURNOFF;                 /* 0x0: Trunoff Control Register */
    __RW uint32_t DGO_RC32K_CFG;               /* 0x4: RC32K CLOCK TRIM Register */
    __RW uint32_t DGO_CTRL;                    /* 0x8: Control Register */
    __R  uint8_t  RESERVED0[4];                /* 0xC - 0xF: Reserved */
    __RW uint32_t PIN_CTRL;                    /* 0x10: IO Control Register */
    __RW uint32_t PIN_STS;                     /* 0x14: IO Status Register */
    __RW uint32_t WAKEUP;                      /* 0x18: Wakeup Register */
    __RW uint32_t WKUP_CYC;                    /* 0x1C: Wakeup Cycle Register */
    __RW uint32_t WKUP_EN;                     /* 0x20: Wakeup Enable Register */
    __RW uint32_t WKUP_STS;                    /* 0x24: Wakeup Status Register */
    __R  uint8_t  RESERVED1[56];               /* 0x28 - 0x5F: Reserved */
    __RW uint32_t IRQ0_EN;                     /* 0x60: Interrupt 0 Enable Register */
    __RW uint32_t IRQ0_STS;                    /* 0x64: Interrupt 0 Status Register */
    __RW uint32_t IRQ1_EN;                     /* 0x68: Interrupt 1 Enable Register */
    __RW uint32_t IRQ1_STS;                    /* 0x6C: Interrupt 1 Status Register */
    __R  uint8_t  RESERVED2[16];               /* 0x70 - 0x7F: Reserved */
    __RW uint32_t GPR[8];                      /* 0x80 - 0x9C: General Purpose Register */
    __R  uint8_t  RESERVED3[96];               /* 0xA0 - 0xFF: Reserved */
    __RW uint32_t IOFILTER[4];                 /* 0x100 - 0x10C: WUIO Filter Register */
    __R  uint8_t  RESERVED4[240];              /* 0x110 - 0x1FF: Reserved */
    struct {
        __RW uint32_t CTRL;                    /* 0x200: Pulse Capture Control Register */
        __RW uint32_t CP_CFG;                  /* 0x204: Compare and  Period Configuration Register */
        __R  uint8_t  RESERVED0[8];            /* 0x208 - 0x20F: Reserved */
        __RW uint32_t COUNTER;                 /* 0x210: Counter Register */
        __RW uint32_t DUMP;                    /* 0x214: DUMP Register */
    } PCAP[1];
    __R  uint8_t  RESERVED5[104];              /* 0x218 - 0x27F: Reserved */
    struct {
        __RW uint32_t CNT_CFG;                 /* 0x280: Pulse Count Configuration Register */
        __RW uint32_t CALIB_CFG;               /* 0x284: Calibration Configuration Register */
        __RW uint32_t RESOLUTION;              /* 0x288: Resolution Configuration Register */
        __RW uint32_t HOMING_CFG;              /* 0x28C: Homing Configuration Register */
        __RW uint32_t ANALYSE_CFG;             /* 0x290: Analyse Configuration Register */
        __RW uint32_t DIRECTION;               /* 0x294: Direction Configuration Register */
        __RW uint32_t CMP0;                    /* 0x298: Compare0 Configuration Register */
        __RW uint32_t CMP1;                    /* 0x29C: Compare1 Configuration Register */
        __RW uint32_t CNT;                     /* 0x2A0: Counter Register */
        __RW uint32_t TACHO;                   /* 0x2A4: Tacho Register */
        __R  uint32_t DUMP;                    /* 0x2A8: DUMP Register */
        __RW uint32_t STS;                     /* 0x2AC: Status Register */
    } PCNT[1];
} PDGO_Type;


/* Bitfield definition for register: DGO_TURNOFF */
/*
 * COUNTER (WO)
 *
 * trunoff counter, counter stops when it counts down to 0, the trunoff occurs when the counter value is 1.
 */
#define PDGO_DGO_TURNOFF_COUNTER_MASK (0xFFFFFFFFUL)
#define PDGO_DGO_TURNOFF_COUNTER_SHIFT (0U)
#define PDGO_DGO_TURNOFF_COUNTER_SET(x) (((uint32_t)(x) << PDGO_DGO_TURNOFF_COUNTER_SHIFT) & PDGO_DGO_TURNOFF_COUNTER_MASK)
#define PDGO_DGO_TURNOFF_COUNTER_GET(x) (((uint32_t)(x) & PDGO_DGO_TURNOFF_COUNTER_MASK) >> PDGO_DGO_TURNOFF_COUNTER_SHIFT)

/* Bitfield definition for register: DGO_RC32K_CFG */
/*
 * COUNTER (RW)
 *
 * IRC32K trim happened, this bit set by hardware after trim value loaded, and stop load, write 0 will clear this bit and reload trim value
 * 0: irc is not trimmed
 * 1: irc is trimmed
 */
#define PDGO_DGO_RC32K_CFG_COUNTER_MASK (0x80000000UL)
#define PDGO_DGO_RC32K_CFG_COUNTER_SHIFT (31U)
#define PDGO_DGO_RC32K_CFG_COUNTER_SET(x) (((uint32_t)(x) << PDGO_DGO_RC32K_CFG_COUNTER_SHIFT) & PDGO_DGO_RC32K_CFG_COUNTER_MASK)
#define PDGO_DGO_RC32K_CFG_COUNTER_GET(x) (((uint32_t)(x) & PDGO_DGO_RC32K_CFG_COUNTER_MASK) >> PDGO_DGO_RC32K_CFG_COUNTER_SHIFT)

/*
 * CAPEX7_TRIM (RW)
 *
 * IRC32K bit 7
 */
#define PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_MASK (0x800000UL)
#define PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_SHIFT (23U)
#define PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_SET(x) (((uint32_t)(x) << PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_SHIFT) & PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_MASK)
#define PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_GET(x) (((uint32_t)(x) & PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_MASK) >> PDGO_DGO_RC32K_CFG_CAPEX7_TRIM_SHIFT)

/*
 * CAPEX6_TRIM (RW)
 *
 * IRC32K bit 6
 */
#define PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_MASK (0x400000UL)
#define PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_SHIFT (22U)
#define PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_SET(x) (((uint32_t)(x) << PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_SHIFT) & PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_MASK)
#define PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_GET(x) (((uint32_t)(x) & PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_MASK) >> PDGO_DGO_RC32K_CFG_CAPEX6_TRIM_SHIFT)

/*
 * CAP_TRIM (RW)
 *
 * capacitor trim bits
 */
#define PDGO_DGO_RC32K_CFG_CAP_TRIM_MASK (0x1FFU)
#define PDGO_DGO_RC32K_CFG_CAP_TRIM_SHIFT (0U)
#define PDGO_DGO_RC32K_CFG_CAP_TRIM_SET(x) (((uint32_t)(x) << PDGO_DGO_RC32K_CFG_CAP_TRIM_SHIFT) & PDGO_DGO_RC32K_CFG_CAP_TRIM_MASK)
#define PDGO_DGO_RC32K_CFG_CAP_TRIM_GET(x) (((uint32_t)(x) & PDGO_DGO_RC32K_CFG_CAP_TRIM_MASK) >> PDGO_DGO_RC32K_CFG_CAP_TRIM_SHIFT)

/* Bitfield definition for register: DGO_CTRL */
/*
 * RET (RW)
 *
 * Retention, retain contents in DGO.GPR[n] register after RSTN (When PMIC has power)
 * 0: Disable
 * 1: Enable
 */
#define PDGO_DGO_CTRL_RET_MASK (0x10000UL)
#define PDGO_DGO_CTRL_RET_SHIFT (16U)
#define PDGO_DGO_CTRL_RET_SET(x) (((uint32_t)(x) << PDGO_DGO_CTRL_RET_SHIFT) & PDGO_DGO_CTRL_RET_MASK)
#define PDGO_DGO_CTRL_RET_GET(x) (((uint32_t)(x) & PDGO_DGO_CTRL_RET_MASK) >> PDGO_DGO_CTRL_RET_SHIFT)

/*
 * LESP (RW)
 *
 * Bandgap works in power save mode, bandgap function normally
 * 0: bandgap works in high performance mode
 * 1: bandgap works in power saving mode
 */
#define PDGO_DGO_CTRL_LESP_MASK (0x2U)
#define PDGO_DGO_CTRL_LESP_SHIFT (1U)
#define PDGO_DGO_CTRL_LESP_SET(x) (((uint32_t)(x) << PDGO_DGO_CTRL_LESP_SHIFT) & PDGO_DGO_CTRL_LESP_MASK)
#define PDGO_DGO_CTRL_LESP_GET(x) (((uint32_t)(x) & PDGO_DGO_CTRL_LESP_MASK) >> PDGO_DGO_CTRL_LESP_SHIFT)

/*
 * LOWP (RW)
 *
 * Bandgap works in low power mode, bandgap function limited
 * 0: bandgap works in normal mode
 * 1: bandgap works in low power mode
 */
#define PDGO_DGO_CTRL_LOWP_MASK (0x1U)
#define PDGO_DGO_CTRL_LOWP_SHIFT (0U)
#define PDGO_DGO_CTRL_LOWP_SET(x) (((uint32_t)(x) << PDGO_DGO_CTRL_LOWP_SHIFT) & PDGO_DGO_CTRL_LOWP_MASK)
#define PDGO_DGO_CTRL_LOWP_GET(x) (((uint32_t)(x) & PDGO_DGO_CTRL_LOWP_MASK) >> PDGO_DGO_CTRL_LOWP_SHIFT)

/* Bitfield definition for register: PIN_CTRL */
/*
 * RSTN (RW)
 *
 * resetn pin pull up disable
 * 0: Enable Pull Up
 * 1: Disable Pull Up
 */
#define PDGO_PIN_CTRL_RSTN_MASK (0x1000000UL)
#define PDGO_PIN_CTRL_RSTN_SHIFT (24U)
#define PDGO_PIN_CTRL_RSTN_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_RSTN_SHIFT) & PDGO_PIN_CTRL_RSTN_MASK)
#define PDGO_PIN_CTRL_RSTN_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_RSTN_MASK) >> PDGO_PIN_CTRL_RSTN_SHIFT)

/*
 * WKUP (RW)
 *
 * wakeup pin pull down disable
 * 0: Enable Pull Down
 * 1: Disable Pull Down
 */
#define PDGO_PIN_CTRL_WKUP_MASK (0x10000UL)
#define PDGO_PIN_CTRL_WKUP_SHIFT (16U)
#define PDGO_PIN_CTRL_WKUP_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_WKUP_SHIFT) & PDGO_PIN_CTRL_WKUP_MASK)
#define PDGO_PIN_CTRL_WKUP_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_WKUP_MASK) >> PDGO_PIN_CTRL_WKUP_SHIFT)

/*
 * WUIO3 (RW)
 *
 * WUIO pin pull down disable
 * 0: Enable Pull Down
 * 1: Disable Pull Down
 */
#define PDGO_PIN_CTRL_WUIO3_MASK (0x8U)
#define PDGO_PIN_CTRL_WUIO3_SHIFT (3U)
#define PDGO_PIN_CTRL_WUIO3_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_WUIO3_SHIFT) & PDGO_PIN_CTRL_WUIO3_MASK)
#define PDGO_PIN_CTRL_WUIO3_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_WUIO3_MASK) >> PDGO_PIN_CTRL_WUIO3_SHIFT)

/*
 * WUIO2 (RW)
 *
 * WUIO pin pull down disable
 * 0: Enable Pull Down
 * 1: Disable Pull Down
 */
#define PDGO_PIN_CTRL_WUIO2_MASK (0x4U)
#define PDGO_PIN_CTRL_WUIO2_SHIFT (2U)
#define PDGO_PIN_CTRL_WUIO2_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_WUIO2_SHIFT) & PDGO_PIN_CTRL_WUIO2_MASK)
#define PDGO_PIN_CTRL_WUIO2_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_WUIO2_MASK) >> PDGO_PIN_CTRL_WUIO2_SHIFT)

/*
 * WUIO1 (RW)
 *
 * WUIO pin pull down disable
 * 0: Enable Pull Down
 * 1: Disable Pull Down
 */
#define PDGO_PIN_CTRL_WUIO1_MASK (0x2U)
#define PDGO_PIN_CTRL_WUIO1_SHIFT (1U)
#define PDGO_PIN_CTRL_WUIO1_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_WUIO1_SHIFT) & PDGO_PIN_CTRL_WUIO1_MASK)
#define PDGO_PIN_CTRL_WUIO1_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_WUIO1_MASK) >> PDGO_PIN_CTRL_WUIO1_SHIFT)

/*
 * WUIO0 (RW)
 *
 * WUIO pin pull down disable
 * 0: Enable Pull Down
 * 1: Disable Pull Down
 */
#define PDGO_PIN_CTRL_WUIO0_MASK (0x1U)
#define PDGO_PIN_CTRL_WUIO0_SHIFT (0U)
#define PDGO_PIN_CTRL_WUIO0_SET(x) (((uint32_t)(x) << PDGO_PIN_CTRL_WUIO0_SHIFT) & PDGO_PIN_CTRL_WUIO0_MASK)
#define PDGO_PIN_CTRL_WUIO0_GET(x) (((uint32_t)(x) & PDGO_PIN_CTRL_WUIO0_MASK) >> PDGO_PIN_CTRL_WUIO0_SHIFT)

/* Bitfield definition for register: PIN_STS */
/*
 * TMOD (RO)
 *
 * TMOD Pin Status
 */
#define PDGO_PIN_STS_TMOD_MASK (0x80000000UL)
#define PDGO_PIN_STS_TMOD_SHIFT (31U)
#define PDGO_PIN_STS_TMOD_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_TMOD_MASK) >> PDGO_PIN_STS_TMOD_SHIFT)

/*
 * RSTN (RO)
 *
 * RSTN Pin Status
 */
#define PDGO_PIN_STS_RSTN_MASK (0x1000000UL)
#define PDGO_PIN_STS_RSTN_SHIFT (24U)
#define PDGO_PIN_STS_RSTN_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_RSTN_MASK) >> PDGO_PIN_STS_RSTN_SHIFT)

/*
 * WKUP (RO)
 *
 * WKUP pin status
 */
#define PDGO_PIN_STS_WKUP_MASK (0x10000UL)
#define PDGO_PIN_STS_WKUP_SHIFT (16U)
#define PDGO_PIN_STS_WKUP_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_WKUP_MASK) >> PDGO_PIN_STS_WKUP_SHIFT)

/*
 * WUIO3 (RO)
 *
 * WUIO3 pin status
 */
#define PDGO_PIN_STS_WUIO3_MASK (0x8U)
#define PDGO_PIN_STS_WUIO3_SHIFT (3U)
#define PDGO_PIN_STS_WUIO3_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_WUIO3_MASK) >> PDGO_PIN_STS_WUIO3_SHIFT)

/*
 * WUIO2 (RO)
 *
 * WUIO2 pin status
 */
#define PDGO_PIN_STS_WUIO2_MASK (0x4U)
#define PDGO_PIN_STS_WUIO2_SHIFT (2U)
#define PDGO_PIN_STS_WUIO2_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_WUIO2_MASK) >> PDGO_PIN_STS_WUIO2_SHIFT)

/*
 * WUIO1 (RO)
 *
 * WUIO1 pin status
 */
#define PDGO_PIN_STS_WUIO1_MASK (0x2U)
#define PDGO_PIN_STS_WUIO1_SHIFT (1U)
#define PDGO_PIN_STS_WUIO1_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_WUIO1_MASK) >> PDGO_PIN_STS_WUIO1_SHIFT)

/*
 * WUIO0 (RO)
 *
 * WUIO0 pin status
 */
#define PDGO_PIN_STS_WUIO0_MASK (0x1U)
#define PDGO_PIN_STS_WUIO0_SHIFT (0U)
#define PDGO_PIN_STS_WUIO0_GET(x) (((uint32_t)(x) & PDGO_PIN_STS_WUIO0_MASK) >> PDGO_PIN_STS_WUIO0_SHIFT)

/* Bitfield definition for register: WAKEUP */
/*
 * COUNTER (RW)
 *
 * software wake-up counter，counter stops when it counts down(base on 32KHz clock) to 0, the wakeup occurs when the counter value is 1.
 * wakeup once: write counter value here and set CYCLE register to be 0.
 * wakeup in cycle: write wakeup cycle in CYCLE register
 */
#define PDGO_WAKEUP_COUNTER_MASK (0xFFFFFFFFUL)
#define PDGO_WAKEUP_COUNTER_SHIFT (0U)
#define PDGO_WAKEUP_COUNTER_SET(x) (((uint32_t)(x) << PDGO_WAKEUP_COUNTER_SHIFT) & PDGO_WAKEUP_COUNTER_MASK)
#define PDGO_WAKEUP_COUNTER_GET(x) (((uint32_t)(x) & PDGO_WAKEUP_COUNTER_MASK) >> PDGO_WAKEUP_COUNTER_SHIFT)

/* Bitfield definition for register: WKUP_CYC */
/*
 * INTERVAL (RW)
 *
 * interval between wakeups, count by 32KHz clock cycles.
 */
#define PDGO_WKUP_CYC_INTERVAL_MASK (0xFFFFFFFFUL)
#define PDGO_WKUP_CYC_INTERVAL_SHIFT (0U)
#define PDGO_WKUP_CYC_INTERVAL_SET(x) (((uint32_t)(x) << PDGO_WKUP_CYC_INTERVAL_SHIFT) & PDGO_WKUP_CYC_INTERVAL_MASK)
#define PDGO_WKUP_CYC_INTERVAL_GET(x) (((uint32_t)(x) & PDGO_WKUP_CYC_INTERVAL_MASK) >> PDGO_WKUP_CYC_INTERVAL_SHIFT)

/* Bitfield definition for register: WKUP_EN */
/*
 * PCNT0 (RW)
 *
 * Allow wake-up via Pulse PCNT0 events.
 * 0：Disable
 * 1：Enable
 * Note: wake-up interrupt request shloud be enable at the same time.
 */
#define PDGO_WKUP_EN_PCNT0_MASK (0x100U)
#define PDGO_WKUP_EN_PCNT0_SHIFT (8U)
#define PDGO_WKUP_EN_PCNT0_SET(x) (((uint32_t)(x) << PDGO_WKUP_EN_PCNT0_SHIFT) & PDGO_WKUP_EN_PCNT0_MASK)
#define PDGO_WKUP_EN_PCNT0_GET(x) (((uint32_t)(x) & PDGO_WKUP_EN_PCNT0_MASK) >> PDGO_WKUP_EN_PCNT0_SHIFT)

/*
 * PCAP0 (RW)
 *
 * Allow wake-up via PCAP0 events.
 * 0：Disable
 * 1：Enable
 * Note: wake-up interrupt request shloud be enable at the same time.
 */
#define PDGO_WKUP_EN_PCAP0_MASK (0x10U)
#define PDGO_WKUP_EN_PCAP0_SHIFT (4U)
#define PDGO_WKUP_EN_PCAP0_SET(x) (((uint32_t)(x) << PDGO_WKUP_EN_PCAP0_SHIFT) & PDGO_WKUP_EN_PCAP0_MASK)
#define PDGO_WKUP_EN_PCAP0_GET(x) (((uint32_t)(x) & PDGO_WKUP_EN_PCAP0_MASK) >> PDGO_WKUP_EN_PCAP0_SHIFT)

/*
 * WUIO (RW)
 *
 * Allow wake-up via WU00/WU01/WU02/WU03 pins.
 * 0：Disable
 * 1：Enable
 * Note: wake-up interrupt request shloud be enable at the same time.
 */
#define PDGO_WKUP_EN_WUIO_MASK (0x4U)
#define PDGO_WKUP_EN_WUIO_SHIFT (2U)
#define PDGO_WKUP_EN_WUIO_SET(x) (((uint32_t)(x) << PDGO_WKUP_EN_WUIO_SHIFT) & PDGO_WKUP_EN_WUIO_MASK)
#define PDGO_WKUP_EN_WUIO_GET(x) (((uint32_t)(x) & PDGO_WKUP_EN_WUIO_MASK) >> PDGO_WKUP_EN_WUIO_SHIFT)

/*
 * CYC (RW)
 *
 * Allow software wake-up via counter cycle.
 * 0：Disable
 * 1：Enable
 * Note: wake-up interrupt request shloud be enable at the same time.
 */
#define PDGO_WKUP_EN_CYC_MASK (0x2U)
#define PDGO_WKUP_EN_CYC_SHIFT (1U)
#define PDGO_WKUP_EN_CYC_SET(x) (((uint32_t)(x) << PDGO_WKUP_EN_CYC_SHIFT) & PDGO_WKUP_EN_CYC_MASK)
#define PDGO_WKUP_EN_CYC_GET(x) (((uint32_t)(x) & PDGO_WKUP_EN_CYC_MASK) >> PDGO_WKUP_EN_CYC_SHIFT)

/*
 * WKUP (RW)
 *
 * Allow wake-up via the WKUP pin.
 * 0：Disable
 * 1：Enable
 * Note: wake-up interrupt request shloud be enable at the same time.
 */
#define PDGO_WKUP_EN_WKUP_MASK (0x1U)
#define PDGO_WKUP_EN_WKUP_SHIFT (0U)
#define PDGO_WKUP_EN_WKUP_SET(x) (((uint32_t)(x) << PDGO_WKUP_EN_WKUP_SHIFT) & PDGO_WKUP_EN_WKUP_MASK)
#define PDGO_WKUP_EN_WKUP_GET(x) (((uint32_t)(x) & PDGO_WKUP_EN_WKUP_MASK) >> PDGO_WKUP_EN_WKUP_SHIFT)

/* Bitfield definition for register: WKUP_STS */
/*
 * PCNT0 (W1C)
 *
 * PCNT0 events wake-up status.
 * 0: wake-up is invalid
 * 1: wake-up is valid
 */
#define PDGO_WKUP_STS_PCNT0_MASK (0x100U)
#define PDGO_WKUP_STS_PCNT0_SHIFT (8U)
#define PDGO_WKUP_STS_PCNT0_SET(x) (((uint32_t)(x) << PDGO_WKUP_STS_PCNT0_SHIFT) & PDGO_WKUP_STS_PCNT0_MASK)
#define PDGO_WKUP_STS_PCNT0_GET(x) (((uint32_t)(x) & PDGO_WKUP_STS_PCNT0_MASK) >> PDGO_WKUP_STS_PCNT0_SHIFT)

/*
 * PCAP0 (W1C)
 *
 * PCAP0 events wake-up status.
 * 0: wake-up is invalid
 * 1: wake-up is valid
 */
#define PDGO_WKUP_STS_PCAP0_MASK (0x10U)
#define PDGO_WKUP_STS_PCAP0_SHIFT (4U)
#define PDGO_WKUP_STS_PCAP0_SET(x) (((uint32_t)(x) << PDGO_WKUP_STS_PCAP0_SHIFT) & PDGO_WKUP_STS_PCAP0_MASK)
#define PDGO_WKUP_STS_PCAP0_GET(x) (((uint32_t)(x) & PDGO_WKUP_STS_PCAP0_MASK) >> PDGO_WKUP_STS_PCAP0_SHIFT)

/*
 * WUIO (W1C)
 *
 * WUIO0/WUIO1/WUIO2/WUIO3 pins wake-up status.
 * 0: wake-up is invalid
 * 1: wake-up is valid
 */
#define PDGO_WKUP_STS_WUIO_MASK (0x4U)
#define PDGO_WKUP_STS_WUIO_SHIFT (2U)
#define PDGO_WKUP_STS_WUIO_SET(x) (((uint32_t)(x) << PDGO_WKUP_STS_WUIO_SHIFT) & PDGO_WKUP_STS_WUIO_MASK)
#define PDGO_WKUP_STS_WUIO_GET(x) (((uint32_t)(x) & PDGO_WKUP_STS_WUIO_MASK) >> PDGO_WKUP_STS_WUIO_SHIFT)

/*
 * CYC (W1C)
 *
 * counter cycle software wake-up status.
 * 0: wake-up is invalid
 * 1: wake-up is valid
 */
#define PDGO_WKUP_STS_CYC_MASK (0x2U)
#define PDGO_WKUP_STS_CYC_SHIFT (1U)
#define PDGO_WKUP_STS_CYC_SET(x) (((uint32_t)(x) << PDGO_WKUP_STS_CYC_SHIFT) & PDGO_WKUP_STS_CYC_MASK)
#define PDGO_WKUP_STS_CYC_GET(x) (((uint32_t)(x) & PDGO_WKUP_STS_CYC_MASK) >> PDGO_WKUP_STS_CYC_SHIFT)

/*
 * WKUP (W1C)
 *
 * WKUP pin wake-up status.
 * 0: wake-up is invalid
 * 1: wake-up is valid
 */
#define PDGO_WKUP_STS_WKUP_MASK (0x1U)
#define PDGO_WKUP_STS_WKUP_SHIFT (0U)
#define PDGO_WKUP_STS_WKUP_SET(x) (((uint32_t)(x) << PDGO_WKUP_STS_WKUP_SHIFT) & PDGO_WKUP_STS_WKUP_MASK)
#define PDGO_WKUP_STS_WKUP_GET(x) (((uint32_t)(x) & PDGO_WKUP_STS_WKUP_MASK) >> PDGO_WKUP_STS_WKUP_SHIFT)

/* Bitfield definition for register: IRQ0_EN */
/*
 * CYC (RW)
 *
 * Allow software counter cycle wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_CYC_MASK (0x20000UL)
#define PDGO_IRQ0_EN_CYC_SHIFT (17U)
#define PDGO_IRQ0_EN_CYC_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_CYC_SHIFT) & PDGO_IRQ0_EN_CYC_MASK)
#define PDGO_IRQ0_EN_CYC_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_CYC_MASK) >> PDGO_IRQ0_EN_CYC_SHIFT)

/*
 * WKUP (RW)
 *
 * Allow WKUP Pin wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_WKUP_MASK (0x10000UL)
#define PDGO_IRQ0_EN_WKUP_SHIFT (16U)
#define PDGO_IRQ0_EN_WKUP_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_WKUP_SHIFT) & PDGO_IRQ0_EN_WKUP_MASK)
#define PDGO_IRQ0_EN_WKUP_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_WKUP_MASK) >> PDGO_IRQ0_EN_WKUP_SHIFT)

/*
 * WUIO3 (RW)
 *
 * Allow WUIO3 Pin wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_WUIO3_MASK (0x8U)
#define PDGO_IRQ0_EN_WUIO3_SHIFT (3U)
#define PDGO_IRQ0_EN_WUIO3_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_WUIO3_SHIFT) & PDGO_IRQ0_EN_WUIO3_MASK)
#define PDGO_IRQ0_EN_WUIO3_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_WUIO3_MASK) >> PDGO_IRQ0_EN_WUIO3_SHIFT)

/*
 * WUIO2 (RW)
 *
 * Allow WUIO2 wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_WUIO2_MASK (0x4U)
#define PDGO_IRQ0_EN_WUIO2_SHIFT (2U)
#define PDGO_IRQ0_EN_WUIO2_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_WUIO2_SHIFT) & PDGO_IRQ0_EN_WUIO2_MASK)
#define PDGO_IRQ0_EN_WUIO2_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_WUIO2_MASK) >> PDGO_IRQ0_EN_WUIO2_SHIFT)

/*
 * WUIO1 (RW)
 *
 * Allow WUIO1 wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_WUIO1_MASK (0x2U)
#define PDGO_IRQ0_EN_WUIO1_SHIFT (1U)
#define PDGO_IRQ0_EN_WUIO1_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_WUIO1_SHIFT) & PDGO_IRQ0_EN_WUIO1_MASK)
#define PDGO_IRQ0_EN_WUIO1_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_WUIO1_MASK) >> PDGO_IRQ0_EN_WUIO1_SHIFT)

/*
 * WUIO0 (RW)
 *
 * Allow WUIO0 wake-up interrupt request.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ0_EN_WUIO0_MASK (0x1U)
#define PDGO_IRQ0_EN_WUIO0_SHIFT (0U)
#define PDGO_IRQ0_EN_WUIO0_SET(x) (((uint32_t)(x) << PDGO_IRQ0_EN_WUIO0_SHIFT) & PDGO_IRQ0_EN_WUIO0_MASK)
#define PDGO_IRQ0_EN_WUIO0_GET(x) (((uint32_t)(x) & PDGO_IRQ0_EN_WUIO0_MASK) >> PDGO_IRQ0_EN_WUIO0_SHIFT)

/* Bitfield definition for register: IRQ0_STS */
/*
 * CYC (W1C)
 *
 * software counter cycle wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_CYC_MASK (0x20000UL)
#define PDGO_IRQ0_STS_CYC_SHIFT (17U)
#define PDGO_IRQ0_STS_CYC_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_CYC_SHIFT) & PDGO_IRQ0_STS_CYC_MASK)
#define PDGO_IRQ0_STS_CYC_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_CYC_MASK) >> PDGO_IRQ0_STS_CYC_SHIFT)

/*
 * WKUP (W1C)
 *
 * WKUP Pin wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_WKUP_MASK (0x10000UL)
#define PDGO_IRQ0_STS_WKUP_SHIFT (16U)
#define PDGO_IRQ0_STS_WKUP_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_WKUP_SHIFT) & PDGO_IRQ0_STS_WKUP_MASK)
#define PDGO_IRQ0_STS_WKUP_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_WKUP_MASK) >> PDGO_IRQ0_STS_WKUP_SHIFT)

/*
 * WUIO3 (W1C)
 *
 * WUIO3 Pin wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_WUIO3_MASK (0x8U)
#define PDGO_IRQ0_STS_WUIO3_SHIFT (3U)
#define PDGO_IRQ0_STS_WUIO3_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_WUIO3_SHIFT) & PDGO_IRQ0_STS_WUIO3_MASK)
#define PDGO_IRQ0_STS_WUIO3_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_WUIO3_MASK) >> PDGO_IRQ0_STS_WUIO3_SHIFT)

/*
 * WUIO2 (W1C)
 *
 * WUIO2 wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_WUIO2_MASK (0x4U)
#define PDGO_IRQ0_STS_WUIO2_SHIFT (2U)
#define PDGO_IRQ0_STS_WUIO2_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_WUIO2_SHIFT) & PDGO_IRQ0_STS_WUIO2_MASK)
#define PDGO_IRQ0_STS_WUIO2_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_WUIO2_MASK) >> PDGO_IRQ0_STS_WUIO2_SHIFT)

/*
 * WUIO1 (W1C)
 *
 * WUIO1 wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_WUIO1_MASK (0x2U)
#define PDGO_IRQ0_STS_WUIO1_SHIFT (1U)
#define PDGO_IRQ0_STS_WUIO1_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_WUIO1_SHIFT) & PDGO_IRQ0_STS_WUIO1_MASK)
#define PDGO_IRQ0_STS_WUIO1_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_WUIO1_MASK) >> PDGO_IRQ0_STS_WUIO1_SHIFT)

/*
 * WUIO0 (W1C)
 *
 * WUIO0 wake-up interrupt status.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ0_STS_WUIO0_MASK (0x1U)
#define PDGO_IRQ0_STS_WUIO0_SHIFT (0U)
#define PDGO_IRQ0_STS_WUIO0_SET(x) (((uint32_t)(x) << PDGO_IRQ0_STS_WUIO0_SHIFT) & PDGO_IRQ0_STS_WUIO0_MASK)
#define PDGO_IRQ0_STS_WUIO0_GET(x) (((uint32_t)(x) & PDGO_IRQ0_STS_WUIO0_MASK) >> PDGO_IRQ0_STS_WUIO0_SHIFT)

/* Bitfield definition for register: IRQ1_EN */
/*
 * PCNT0_SEQERR (RW)
 *
 * Allow pulse counter interrupt request when phase sequence error happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_SEQERR_MASK (0x800000UL)
#define PDGO_IRQ1_EN_PCNT0_SEQERR_SHIFT (23U)
#define PDGO_IRQ1_EN_PCNT0_SEQERR_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_SEQERR_SHIFT) & PDGO_IRQ1_EN_PCNT0_SEQERR_MASK)
#define PDGO_IRQ1_EN_PCNT0_SEQERR_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_SEQERR_MASK) >> PDGO_IRQ1_EN_PCNT0_SEQERR_SHIFT)

/*
 * PCNT0_DECERR (RW)
 *
 * Allow pulse counter interrupt request when decoder error happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_DECERR_MASK (0x400000UL)
#define PDGO_IRQ1_EN_PCNT0_DECERR_SHIFT (22U)
#define PDGO_IRQ1_EN_PCNT0_DECERR_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_DECERR_SHIFT) & PDGO_IRQ1_EN_PCNT0_DECERR_MASK)
#define PDGO_IRQ1_EN_PCNT0_DECERR_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_DECERR_MASK) >> PDGO_IRQ1_EN_PCNT0_DECERR_SHIFT)

/*
 * PCNT0_TACHO (RW)
 *
 * Allow pulse counter interrupt request when tacho done happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_TACHO_MASK (0x200000UL)
#define PDGO_IRQ1_EN_PCNT0_TACHO_SHIFT (21U)
#define PDGO_IRQ1_EN_PCNT0_TACHO_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_TACHO_SHIFT) & PDGO_IRQ1_EN_PCNT0_TACHO_MASK)
#define PDGO_IRQ1_EN_PCNT0_TACHO_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_TACHO_MASK) >> PDGO_IRQ1_EN_PCNT0_TACHO_SHIFT)

/*
 * PCNT0_MATCH0 (RW)
 *
 * Allow pulse counter interrupt request when match 0 events happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_MATCH0_MASK (0x80000UL)
#define PDGO_IRQ1_EN_PCNT0_MATCH0_SHIFT (19U)
#define PDGO_IRQ1_EN_PCNT0_MATCH0_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_MATCH0_SHIFT) & PDGO_IRQ1_EN_PCNT0_MATCH0_MASK)
#define PDGO_IRQ1_EN_PCNT0_MATCH0_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_MATCH0_MASK) >> PDGO_IRQ1_EN_PCNT0_MATCH0_SHIFT)

/*
 * PCNT0_HOME (RW)
 *
 * Allow pulse counter interrupt request when Phase h (HOME)  edges or level match events happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_HOME_MASK (0x40000UL)
#define PDGO_IRQ1_EN_PCNT0_HOME_SHIFT (18U)
#define PDGO_IRQ1_EN_PCNT0_HOME_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_HOME_SHIFT) & PDGO_IRQ1_EN_PCNT0_HOME_MASK)
#define PDGO_IRQ1_EN_PCNT0_HOME_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_HOME_MASK) >> PDGO_IRQ1_EN_PCNT0_HOME_SHIFT)

/*
 * PCNT0_Z (RW)
 *
 * Allow pulse counter interrupt request when Phase Z (INDEX)  edges or level match events happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_Z_MASK (0x20000UL)
#define PDGO_IRQ1_EN_PCNT0_Z_SHIFT (17U)
#define PDGO_IRQ1_EN_PCNT0_Z_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_Z_SHIFT) & PDGO_IRQ1_EN_PCNT0_Z_MASK)
#define PDGO_IRQ1_EN_PCNT0_Z_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_Z_MASK) >> PDGO_IRQ1_EN_PCNT0_Z_SHIFT)

/*
 * PCNT0_UPDATE (RW)
 *
 * Allow pulse counter interrupt request when counter updates.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCNT0_UPDATE_MASK (0x10000UL)
#define PDGO_IRQ1_EN_PCNT0_UPDATE_SHIFT (16U)
#define PDGO_IRQ1_EN_PCNT0_UPDATE_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCNT0_UPDATE_SHIFT) & PDGO_IRQ1_EN_PCNT0_UPDATE_MASK)
#define PDGO_IRQ1_EN_PCNT0_UPDATE_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCNT0_UPDATE_MASK) >> PDGO_IRQ1_EN_PCNT0_UPDATE_SHIFT)

/*
 * PCAP0_MATCH (RW)
 *
 * Allow PCAP0 interrupt request when match events happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCAP0_MATCH_MASK (0x4U)
#define PDGO_IRQ1_EN_PCAP0_MATCH_SHIFT (2U)
#define PDGO_IRQ1_EN_PCAP0_MATCH_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCAP0_MATCH_SHIFT) & PDGO_IRQ1_EN_PCAP0_MATCH_MASK)
#define PDGO_IRQ1_EN_PCAP0_MATCH_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCAP0_MATCH_MASK) >> PDGO_IRQ1_EN_PCAP0_MATCH_SHIFT)

/*
 * PCAP0_DUMP (RW)
 *
 * Allow PCAP0 interrupt request when dump events happens.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCAP0_DUMP_MASK (0x2U)
#define PDGO_IRQ1_EN_PCAP0_DUMP_SHIFT (1U)
#define PDGO_IRQ1_EN_PCAP0_DUMP_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCAP0_DUMP_SHIFT) & PDGO_IRQ1_EN_PCAP0_DUMP_MASK)
#define PDGO_IRQ1_EN_PCAP0_DUMP_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCAP0_DUMP_MASK) >> PDGO_IRQ1_EN_PCAP0_DUMP_SHIFT)

/*
 * PCAP0_PERIOD (RW)
 *
 * Allow PCAP0 interrupt request when period done.
 * 0：Disable
 * 1：Enable
 */
#define PDGO_IRQ1_EN_PCAP0_PERIOD_MASK (0x1U)
#define PDGO_IRQ1_EN_PCAP0_PERIOD_SHIFT (0U)
#define PDGO_IRQ1_EN_PCAP0_PERIOD_SET(x) (((uint32_t)(x) << PDGO_IRQ1_EN_PCAP0_PERIOD_SHIFT) & PDGO_IRQ1_EN_PCAP0_PERIOD_MASK)
#define PDGO_IRQ1_EN_PCAP0_PERIOD_GET(x) (((uint32_t)(x) & PDGO_IRQ1_EN_PCAP0_PERIOD_MASK) >> PDGO_IRQ1_EN_PCAP0_PERIOD_SHIFT)

/* Bitfield definition for register: IRQ1_STS */
/*
 * PCNT0_SEQERR (W1C)
 *
 * pulse counter interrupt status when phase sequence error happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_SEQERR_MASK (0x800000UL)
#define PDGO_IRQ1_STS_PCNT0_SEQERR_SHIFT (23U)
#define PDGO_IRQ1_STS_PCNT0_SEQERR_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_SEQERR_SHIFT) & PDGO_IRQ1_STS_PCNT0_SEQERR_MASK)
#define PDGO_IRQ1_STS_PCNT0_SEQERR_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_SEQERR_MASK) >> PDGO_IRQ1_STS_PCNT0_SEQERR_SHIFT)

/*
 * PCNT0_DECERR (W1C)
 *
 * pulse counter interrupt status when decoder error happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_DECERR_MASK (0x400000UL)
#define PDGO_IRQ1_STS_PCNT0_DECERR_SHIFT (22U)
#define PDGO_IRQ1_STS_PCNT0_DECERR_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_DECERR_SHIFT) & PDGO_IRQ1_STS_PCNT0_DECERR_MASK)
#define PDGO_IRQ1_STS_PCNT0_DECERR_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_DECERR_MASK) >> PDGO_IRQ1_STS_PCNT0_DECERR_SHIFT)

/*
 * PCNT0_TACHO (W1C)
 *
 * pulse counter interrupt status when tacho done happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_TACHO_MASK (0x200000UL)
#define PDGO_IRQ1_STS_PCNT0_TACHO_SHIFT (21U)
#define PDGO_IRQ1_STS_PCNT0_TACHO_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_TACHO_SHIFT) & PDGO_IRQ1_STS_PCNT0_TACHO_MASK)
#define PDGO_IRQ1_STS_PCNT0_TACHO_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_TACHO_MASK) >> PDGO_IRQ1_STS_PCNT0_TACHO_SHIFT)

/*
 * PCNT0_MATCH0 (W1C)
 *
 * pulse counter interrupt status when match 0 events happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_MATCH0_MASK (0x80000UL)
#define PDGO_IRQ1_STS_PCNT0_MATCH0_SHIFT (19U)
#define PDGO_IRQ1_STS_PCNT0_MATCH0_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_MATCH0_SHIFT) & PDGO_IRQ1_STS_PCNT0_MATCH0_MASK)
#define PDGO_IRQ1_STS_PCNT0_MATCH0_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_MATCH0_MASK) >> PDGO_IRQ1_STS_PCNT0_MATCH0_SHIFT)

/*
 * PCNT0_HOME (W1C)
 *
 * pulse counter interrupt status when Phase H (HOME) edges or level match events happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_HOME_MASK (0x40000UL)
#define PDGO_IRQ1_STS_PCNT0_HOME_SHIFT (18U)
#define PDGO_IRQ1_STS_PCNT0_HOME_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_HOME_SHIFT) & PDGO_IRQ1_STS_PCNT0_HOME_MASK)
#define PDGO_IRQ1_STS_PCNT0_HOME_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_HOME_MASK) >> PDGO_IRQ1_STS_PCNT0_HOME_SHIFT)

/*
 * PCNT0_Z (W1C)
 *
 * pulse counter interrupt status when Phase Z (INDEX) edges or level match events happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_Z_MASK (0x20000UL)
#define PDGO_IRQ1_STS_PCNT0_Z_SHIFT (17U)
#define PDGO_IRQ1_STS_PCNT0_Z_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_Z_SHIFT) & PDGO_IRQ1_STS_PCNT0_Z_MASK)
#define PDGO_IRQ1_STS_PCNT0_Z_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_Z_MASK) >> PDGO_IRQ1_STS_PCNT0_Z_SHIFT)

/*
 * PCNT0_UPDATE (W1C)
 *
 * pulse counter interrupt status when counter updates.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCNT0_UPDATE_MASK (0x10000UL)
#define PDGO_IRQ1_STS_PCNT0_UPDATE_SHIFT (16U)
#define PDGO_IRQ1_STS_PCNT0_UPDATE_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCNT0_UPDATE_SHIFT) & PDGO_IRQ1_STS_PCNT0_UPDATE_MASK)
#define PDGO_IRQ1_STS_PCNT0_UPDATE_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCNT0_UPDATE_MASK) >> PDGO_IRQ1_STS_PCNT0_UPDATE_SHIFT)

/*
 * PCAP0_MATCH (W1C)
 *
 * PCAP0 interrupt status when match events happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCAP0_MATCH_MASK (0x4U)
#define PDGO_IRQ1_STS_PCAP0_MATCH_SHIFT (2U)
#define PDGO_IRQ1_STS_PCAP0_MATCH_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCAP0_MATCH_SHIFT) & PDGO_IRQ1_STS_PCAP0_MATCH_MASK)
#define PDGO_IRQ1_STS_PCAP0_MATCH_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCAP0_MATCH_MASK) >> PDGO_IRQ1_STS_PCAP0_MATCH_SHIFT)

/*
 * PCAP0_DUMP (W1C)
 *
 * PCAP0 interrupt status when dump events happens.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCAP0_DUMP_MASK (0x2U)
#define PDGO_IRQ1_STS_PCAP0_DUMP_SHIFT (1U)
#define PDGO_IRQ1_STS_PCAP0_DUMP_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCAP0_DUMP_SHIFT) & PDGO_IRQ1_STS_PCAP0_DUMP_MASK)
#define PDGO_IRQ1_STS_PCAP0_DUMP_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCAP0_DUMP_MASK) >> PDGO_IRQ1_STS_PCAP0_DUMP_SHIFT)

/*
 * PCAP0_PERIOD (W1C)
 *
 * PCAP0 interrupt status when period done.
 * 0： is invalid
 * 1： is valid
 */
#define PDGO_IRQ1_STS_PCAP0_PERIOD_MASK (0x1U)
#define PDGO_IRQ1_STS_PCAP0_PERIOD_SHIFT (0U)
#define PDGO_IRQ1_STS_PCAP0_PERIOD_SET(x) (((uint32_t)(x) << PDGO_IRQ1_STS_PCAP0_PERIOD_SHIFT) & PDGO_IRQ1_STS_PCAP0_PERIOD_MASK)
#define PDGO_IRQ1_STS_PCAP0_PERIOD_GET(x) (((uint32_t)(x) & PDGO_IRQ1_STS_PCAP0_PERIOD_MASK) >> PDGO_IRQ1_STS_PCAP0_PERIOD_SHIFT)

/* Bitfield definition for register array: GPR */
/*
 * VALUE (RW)
 *
 * Value
 */
#define PDGO_GPR_VALUE_MASK (0xFFFFFFFFUL)
#define PDGO_GPR_VALUE_SHIFT (0U)
#define PDGO_GPR_VALUE_SET(x) (((uint32_t)(x) << PDGO_GPR_VALUE_SHIFT) & PDGO_GPR_VALUE_MASK)
#define PDGO_GPR_VALUE_GET(x) (((uint32_t)(x) & PDGO_GPR_VALUE_MASK) >> PDGO_GPR_VALUE_SHIFT)

/* Bitfield definition for register array: IOFILTER */
/*
 * EN (RW)
 *
 * Allow this IO filter channel output
 * 0： Disable
 * 1： Enable
 */
#define PDGO_IOFILTER_EN_MASK (0x80000000UL)
#define PDGO_IOFILTER_EN_SHIFT (31U)
#define PDGO_IOFILTER_EN_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_EN_SHIFT) & PDGO_IOFILTER_EN_MASK)
#define PDGO_IOFILTER_EN_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_EN_MASK) >> PDGO_IOFILTER_EN_SHIFT)

/*
 * IRQMODE (RW)
 *
 * Filter IRQ Mode:
 * 0： Disable
 * 1： Level Mode
 * 2： Rising Edge
 * 3： Rising Edge or Falling Edge
 */
#define PDGO_IOFILTER_IRQMODE_MASK (0x3000000UL)
#define PDGO_IOFILTER_IRQMODE_SHIFT (24U)
#define PDGO_IOFILTER_IRQMODE_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_IRQMODE_SHIFT) & PDGO_IOFILTER_IRQMODE_MASK)
#define PDGO_IOFILTER_IRQMODE_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_IRQMODE_MASK) >> PDGO_IOFILTER_IRQMODE_SHIFT)

/*
 * NORM (RW)
 *
 * Initial output value
 */
#define PDGO_IOFILTER_NORM_MASK (0x200000UL)
#define PDGO_IOFILTER_NORM_SHIFT (21U)
#define PDGO_IOFILTER_NORM_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_NORM_SHIFT) & PDGO_IOFILTER_NORM_MASK)
#define PDGO_IOFILTER_NORM_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_NORM_MASK) >> PDGO_IOFILTER_NORM_SHIFT)

/*
 * INV (RW)
 *
 * Invert Filter output result.
 */
#define PDGO_IOFILTER_INV_MASK (0x100000UL)
#define PDGO_IOFILTER_INV_SHIFT (20U)
#define PDGO_IOFILTER_INV_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_INV_SHIFT) & PDGO_IOFILTER_INV_MASK)
#define PDGO_IOFILTER_INV_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_INV_MASK) >> PDGO_IOFILTER_INV_SHIFT)

/*
 * BYPASS (RW)
 *
 * Bypass Mode
 * 0： Disable Bypass Mode
 * 1： Enable Bypass Mode, use filter input as result output
 */
#define PDGO_IOFILTER_BYPASS_MASK (0x40000UL)
#define PDGO_IOFILTER_BYPASS_SHIFT (18U)
#define PDGO_IOFILTER_BYPASS_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_BYPASS_SHIFT) & PDGO_IOFILTER_BYPASS_MASK)
#define PDGO_IOFILTER_BYPASS_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_BYPASS_MASK) >> PDGO_IOFILTER_BYPASS_SHIFT)

/*
 * MODE (RW)
 *
 * Filter Mode:
 * 0： Rapid change mode
 * 1： Delay filter mode
 * 2： Stalbe low mode
 * 3： Stable high mode
 */
#define PDGO_IOFILTER_MODE_MASK (0x30000UL)
#define PDGO_IOFILTER_MODE_SHIFT (16U)
#define PDGO_IOFILTER_MODE_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_MODE_SHIFT) & PDGO_IOFILTER_MODE_MASK)
#define PDGO_IOFILTER_MODE_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_MODE_MASK) >> PDGO_IOFILTER_MODE_SHIFT)

/*
 * FILTER_LEN (RW)
 *
 * Filter Length(Base on 32K clock)
 */
#define PDGO_IOFILTER_FILTER_LEN_MASK (0x3FFU)
#define PDGO_IOFILTER_FILTER_LEN_SHIFT (0U)
#define PDGO_IOFILTER_FILTER_LEN_SET(x) (((uint32_t)(x) << PDGO_IOFILTER_FILTER_LEN_SHIFT) & PDGO_IOFILTER_FILTER_LEN_MASK)
#define PDGO_IOFILTER_FILTER_LEN_GET(x) (((uint32_t)(x) & PDGO_IOFILTER_FILTER_LEN_MASK) >> PDGO_IOFILTER_FILTER_LEN_SHIFT)

/* Bitfield definition for register of struct array PCAP: CTRL */
/*
 * EN (RW)
 *
 * Enable PCAP counter
 * 0： Disable
 * 1： Enable
 * Note:  Other registers should be configured before enable this bit.
 */
#define PDGO_PCAP_CTRL_EN_MASK (0x80000000UL)
#define PDGO_PCAP_CTRL_EN_SHIFT (31U)
#define PDGO_PCAP_CTRL_EN_SET(x) (((uint32_t)(x) << PDGO_PCAP_CTRL_EN_SHIFT) & PDGO_PCAP_CTRL_EN_MASK)
#define PDGO_PCAP_CTRL_EN_GET(x) (((uint32_t)(x) & PDGO_PCAP_CTRL_EN_MASK) >> PDGO_PCAP_CTRL_EN_SHIFT)

/*
 * SELECT (RW)
 *
 * Select Input for counter:
 * 0:  Use WUIO0 as input
 * 1:  Use WUIO1 as input
 * 2:  Use WUIO2 as input
 * 3:  Use WUIO3 as input
 */
#define PDGO_PCAP_CTRL_SELECT_MASK (0x3000000UL)
#define PDGO_PCAP_CTRL_SELECT_SHIFT (24U)
#define PDGO_PCAP_CTRL_SELECT_SET(x) (((uint32_t)(x) << PDGO_PCAP_CTRL_SELECT_SHIFT) & PDGO_PCAP_CTRL_SELECT_MASK)
#define PDGO_PCAP_CTRL_SELECT_GET(x) (((uint32_t)(x) & PDGO_PCAP_CTRL_SELECT_MASK) >> PDGO_PCAP_CTRL_SELECT_SHIFT)

/*
 * MATCH_MODE (RW)
 *
 * Match Mode Selection. Generate Match Pulse when :
 * 0   : Disable Match Mode;
 * 1   : Reserved;
 * 2   : COUNTER >= COMPARE;
 * 3   : COUNTER <= COMPARE;
 * 4   : HIGH_DUMP >= COMPARE;
 * 5   : HIGH_DUMP <= COMPARE;
 * 6   : LOW_DUMP >= COMPARE;
 * 7   : LOW_DUMP <= COMPARE;
 * 8   : (HIGH_DUMP  + LOW_DUMP) >= COMPARE;
 * 9   : (HIGH_DUMP  + LOW_DUMP) <= COMPARE;
 * 10 :  | HIGH_DUMP  - LOW_DUMP | >= COMPARE;
 * 11 :  | HIGH_DUMP  - LOW_DUMP | <= COMPARE;
 * 12 :  | HIGH_DUMP  - LOW_DUMP | >= COMPARE and HIGH_DUMP >= LOW_DUMP;
 * 13 :  | HIGH_DUMP  - LOW_DUMP | <= COMPARE and LOW_DUMP >= HIGH_DUMP;
 * 14 :  | HIGH_DUMP  - LOW_DUMP | >= COMPARE and HIGH_DUMP <= LOW_DUMP;
 * 15 :  | HIGH_DUMP  - LOW_DUMP | <= COMPARE and LOW_DUMP <= HIGH_DUMP;
 */
#define PDGO_PCAP_CTRL_MATCH_MODE_MASK (0xF0000UL)
#define PDGO_PCAP_CTRL_MATCH_MODE_SHIFT (16U)
#define PDGO_PCAP_CTRL_MATCH_MODE_SET(x) (((uint32_t)(x) << PDGO_PCAP_CTRL_MATCH_MODE_SHIFT) & PDGO_PCAP_CTRL_MATCH_MODE_MASK)
#define PDGO_PCAP_CTRL_MATCH_MODE_GET(x) (((uint32_t)(x) & PDGO_PCAP_CTRL_MATCH_MODE_MASK) >> PDGO_PCAP_CTRL_MATCH_MODE_SHIFT)

/*
 * DUMP_MODE (RW)
 *
 * Dump Mode, select the time using to update LOW_DUMP(input signal's low level time) and HIGH_DUMP(input signal's high level time):
 * 00: Disable Dump
 * 01: Update at rising edge. (input selection via SELECT bit field)
 * 10: Update at falling edge.  (input selected via SELECT bit field)
 * 11: Update when COUNTER counts period done.
 */
#define PDGO_PCAP_CTRL_DUMP_MODE_MASK (0x300U)
#define PDGO_PCAP_CTRL_DUMP_MODE_SHIFT (8U)
#define PDGO_PCAP_CTRL_DUMP_MODE_SET(x) (((uint32_t)(x) << PDGO_PCAP_CTRL_DUMP_MODE_SHIFT) & PDGO_PCAP_CTRL_DUMP_MODE_MASK)
#define PDGO_PCAP_CTRL_DUMP_MODE_GET(x) (((uint32_t)(x) & PDGO_PCAP_CTRL_DUMP_MODE_MASK) >> PDGO_PCAP_CTRL_DUMP_MODE_SHIFT)

/*
 * COUNT_MODE (RW)
 *
 * Count Mode
 * - when Bit[0]:
 * 0: Count up
 * 1: Count down
 * - when Bit[1]:
 * 0: Oneshot mode
 * 1: Period Mode
 * - when Bit[3:2]:
 * 00: Count by internal 32KHz clock
 * 01: Count updates when rising edge
 * 10: Count updates when falling edge
 */
#define PDGO_PCAP_CTRL_COUNT_MODE_MASK (0xFU)
#define PDGO_PCAP_CTRL_COUNT_MODE_SHIFT (0U)
#define PDGO_PCAP_CTRL_COUNT_MODE_SET(x) (((uint32_t)(x) << PDGO_PCAP_CTRL_COUNT_MODE_SHIFT) & PDGO_PCAP_CTRL_COUNT_MODE_MASK)
#define PDGO_PCAP_CTRL_COUNT_MODE_GET(x) (((uint32_t)(x) & PDGO_PCAP_CTRL_COUNT_MODE_MASK) >> PDGO_PCAP_CTRL_COUNT_MODE_SHIFT)

/* Bitfield definition for register of struct array PCAP: CP_CFG */
/*
 * COMPARE (RW)
 *
 * Compare Value
 * Note: This is a SYNC register, and it won't be valid immediately after change. It can be change only after last configuration valid.
 */
#define PDGO_PCAP_CP_CFG_COMPARE_MASK (0xFFFF0000UL)
#define PDGO_PCAP_CP_CFG_COMPARE_SHIFT (16U)
#define PDGO_PCAP_CP_CFG_COMPARE_SET(x) (((uint32_t)(x) << PDGO_PCAP_CP_CFG_COMPARE_SHIFT) & PDGO_PCAP_CP_CFG_COMPARE_MASK)
#define PDGO_PCAP_CP_CFG_COMPARE_GET(x) (((uint32_t)(x) & PDGO_PCAP_CP_CFG_COMPARE_MASK) >> PDGO_PCAP_CP_CFG_COMPARE_SHIFT)

/*
 * PERIOD (RW)
 *
 * Period Value
 * Note: This is a SYNC register, and it won't be valid immediately after change. It can be change only after last configuration valid.
 */
#define PDGO_PCAP_CP_CFG_PERIOD_MASK (0xFFFFU)
#define PDGO_PCAP_CP_CFG_PERIOD_SHIFT (0U)
#define PDGO_PCAP_CP_CFG_PERIOD_SET(x) (((uint32_t)(x) << PDGO_PCAP_CP_CFG_PERIOD_SHIFT) & PDGO_PCAP_CP_CFG_PERIOD_MASK)
#define PDGO_PCAP_CP_CFG_PERIOD_GET(x) (((uint32_t)(x) & PDGO_PCAP_CP_CFG_PERIOD_MASK) >> PDGO_PCAP_CP_CFG_PERIOD_SHIFT)

/* Bitfield definition for register of struct array PCAP: COUNTER */
/*
 * COUNTER (RW)
 *
 * Counter Value
 */
#define PDGO_PCAP_COUNTER_COUNTER_MASK (0xFFFFU)
#define PDGO_PCAP_COUNTER_COUNTER_SHIFT (0U)
#define PDGO_PCAP_COUNTER_COUNTER_SET(x) (((uint32_t)(x) << PDGO_PCAP_COUNTER_COUNTER_SHIFT) & PDGO_PCAP_COUNTER_COUNTER_MASK)
#define PDGO_PCAP_COUNTER_COUNTER_GET(x) (((uint32_t)(x) & PDGO_PCAP_COUNTER_COUNTER_MASK) >> PDGO_PCAP_COUNTER_COUNTER_SHIFT)

/* Bitfield definition for register of struct array PCAP: DUMP */
/*
 * LOW_DUMP (RW)
 *
 * Dump Value: Low Level Time(Base on 32K Clock)
 */
#define PDGO_PCAP_DUMP_LOW_DUMP_MASK (0xFFFF0000UL)
#define PDGO_PCAP_DUMP_LOW_DUMP_SHIFT (16U)
#define PDGO_PCAP_DUMP_LOW_DUMP_SET(x) (((uint32_t)(x) << PDGO_PCAP_DUMP_LOW_DUMP_SHIFT) & PDGO_PCAP_DUMP_LOW_DUMP_MASK)
#define PDGO_PCAP_DUMP_LOW_DUMP_GET(x) (((uint32_t)(x) & PDGO_PCAP_DUMP_LOW_DUMP_MASK) >> PDGO_PCAP_DUMP_LOW_DUMP_SHIFT)

/*
 * HIGH_DUMP (RW)
 *
 * Dump Value: High Level Time(Base on 32K Clock)
 */
#define PDGO_PCAP_DUMP_HIGH_DUMP_MASK (0xFFFFU)
#define PDGO_PCAP_DUMP_HIGH_DUMP_SHIFT (0U)
#define PDGO_PCAP_DUMP_HIGH_DUMP_SET(x) (((uint32_t)(x) << PDGO_PCAP_DUMP_HIGH_DUMP_SHIFT) & PDGO_PCAP_DUMP_HIGH_DUMP_MASK)
#define PDGO_PCAP_DUMP_HIGH_DUMP_GET(x) (((uint32_t)(x) & PDGO_PCAP_DUMP_HIGH_DUMP_MASK) >> PDGO_PCAP_DUMP_HIGH_DUMP_SHIFT)

/* Bitfield definition for register of struct array PCNT: CNT_CFG */
/*
 * EN (RW)
 *
 * Enable Pulse Counter
 * 0： Disable
 * 1： Enable
 * Note:  Other registers should be configured before enable this bit.
 */
#define PDGO_PCNT_CNT_CFG_EN_MASK (0x80000000UL)
#define PDGO_PCNT_CNT_CFG_EN_SHIFT (31U)
#define PDGO_PCNT_CNT_CFG_EN_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_CFG_EN_SHIFT) & PDGO_PCNT_CNT_CFG_EN_MASK)
#define PDGO_PCNT_CNT_CFG_EN_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_CFG_EN_MASK) >> PDGO_PCNT_CNT_CFG_EN_SHIFT)

/*
 * OVRD (RW)
 *
 * Software Override Direction
 * 0： Disable
 * 1： Enable
 */
#define PDGO_PCNT_CNT_CFG_OVRD_MASK (0x1000U)
#define PDGO_PCNT_CNT_CFG_OVRD_SHIFT (12U)
#define PDGO_PCNT_CNT_CFG_OVRD_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_CFG_OVRD_SHIFT) & PDGO_PCNT_CNT_CFG_OVRD_MASK)
#define PDGO_PCNT_CNT_CFG_OVRD_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_CFG_OVRD_MASK) >> PDGO_PCNT_CNT_CFG_OVRD_SHIFT)

/*
 * SEQ (RW)
 *
 * Change the direction of decoder
 * 0： Disable
 * 1： Enable
 * If enable：
 * In PD Mode：modify the positive direction by using high level in D channel as forward .
 * In UD Mode：modify the positive direction by using pulse in U channel as forward, pulse in D channel as reverse.
 * In ABZ Mode：modify the positive direction by using B channel's phase lead over A channel as forward.
 * In UVW Mode：modify the positive direction by using W channel lead as forward.
 */
#define PDGO_PCNT_CNT_CFG_SEQ_MASK (0x100U)
#define PDGO_PCNT_CNT_CFG_SEQ_SHIFT (8U)
#define PDGO_PCNT_CNT_CFG_SEQ_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_CFG_SEQ_SHIFT) & PDGO_PCNT_CNT_CFG_SEQ_MASK)
#define PDGO_PCNT_CNT_CFG_SEQ_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_CFG_SEQ_MASK) >> PDGO_PCNT_CNT_CFG_SEQ_SHIFT)

/*
 * BIAS (RW)
 *
 * Bias compensation, compensation for decoded phase
 */
#define PDGO_PCNT_CNT_CFG_BIAS_MASK (0x70U)
#define PDGO_PCNT_CNT_CFG_BIAS_SHIFT (4U)
#define PDGO_PCNT_CNT_CFG_BIAS_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_CFG_BIAS_SHIFT) & PDGO_PCNT_CNT_CFG_BIAS_MASK)
#define PDGO_PCNT_CNT_CFG_BIAS_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_CFG_BIAS_MASK) >> PDGO_PCNT_CNT_CFG_BIAS_SHIFT)

/*
 * DECODE_MODE (RW)
 *
 * Decode Mode:
 * 0: MODE_AB_4X
 * 1: MODE_AB_2X
 * 2: MODE_AB_1X
 * 3: Reserved
 * 4: MODE_PD_1X
 * 5: MODE_PDF1X（Disable checking of direction change when input pulse is valid）
 * 6: MODE_PD_2X
 * 7: MODE_PDF2X（Disable checking of direction change when input pulse is valid）
 * 8: MODE_UD_1X
 * 9: MODE_UDF1X（Disable checking if both channel U and D are valid at the same time）
 * 10: MODE_UD_2X
 * 11: MODE_UDF2X（Disable checking if both channel U and D are valid at the same time）
 * 12: MODE_UVW6X (120°)
 * 13: MODE_UVW6Y (  60°)
 * 14: Reserved
 * 15: Reserved
 */
#define PDGO_PCNT_CNT_CFG_DECODE_MODE_MASK (0xFU)
#define PDGO_PCNT_CNT_CFG_DECODE_MODE_SHIFT (0U)
#define PDGO_PCNT_CNT_CFG_DECODE_MODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_CFG_DECODE_MODE_SHIFT) & PDGO_PCNT_CNT_CFG_DECODE_MODE_MASK)
#define PDGO_PCNT_CNT_CFG_DECODE_MODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_CFG_DECODE_MODE_MASK) >> PDGO_PCNT_CNT_CFG_DECODE_MODE_SHIFT)

/* Bitfield definition for register of struct array PCNT: CALIB_CFG */
/*
 * ZMODE (RW)
 *
 * Phase Z Mode Select
 * 0：Disable
 * 1：Edge Mode(Use Edge in ZSTATE Register)
 * 2：Level Mode(Match Phase A,B and Z)
 * 3：Level Mode(Only Match Phase Z)
 */
#define PDGO_PCNT_CALIB_CFG_ZMODE_MASK (0x300000UL)
#define PDGO_PCNT_CALIB_CFG_ZMODE_SHIFT (20U)
#define PDGO_PCNT_CALIB_CFG_ZMODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_ZMODE_SHIFT) & PDGO_PCNT_CALIB_CFG_ZMODE_MASK)
#define PDGO_PCNT_CALIB_CFG_ZMODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_ZMODE_MASK) >> PDGO_PCNT_CALIB_CFG_ZMODE_SHIFT)

/*
 * ZSTATE (RW)
 *
 * Phase Z Matching Status
 * -When in level mode:
 * A bit2 of 1 indicates at the rising edge of the Z phase, and 0 at the falling edge.
 * The remaining bits correspond to the level state of the HZBA phase.
 * -When edge mode:
 * If bit3 is 1: Match Z-phase rising edge, reverse
 * If bit2 is 1: Match Z-phase falling edge, forward
 * If bit1 is 1: Match Z-phase falling edge, reverse
 * If bit0 is 1: Match Z-phase rising edge, forward.
 */
#define PDGO_PCNT_CALIB_CFG_ZSTATE_MASK (0xF0000UL)
#define PDGO_PCNT_CALIB_CFG_ZSTATE_SHIFT (16U)
#define PDGO_PCNT_CALIB_CFG_ZSTATE_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_ZSTATE_SHIFT) & PDGO_PCNT_CALIB_CFG_ZSTATE_MASK)
#define PDGO_PCNT_CALIB_CFG_ZSTATE_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_ZSTATE_MASK) >> PDGO_PCNT_CALIB_CFG_ZSTATE_SHIFT)

/*
 * H_SEL (RW)
 *
 * Select Input for Phase H：
 * 0：WUIO0
 * 1：WUIO1
 * 2：WUIO2
 * 3：WUIO3
 */
#define PDGO_PCNT_CALIB_CFG_H_SEL_MASK (0x3000U)
#define PDGO_PCNT_CALIB_CFG_H_SEL_SHIFT (12U)
#define PDGO_PCNT_CALIB_CFG_H_SEL_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_H_SEL_SHIFT) & PDGO_PCNT_CALIB_CFG_H_SEL_MASK)
#define PDGO_PCNT_CALIB_CFG_H_SEL_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_H_SEL_MASK) >> PDGO_PCNT_CALIB_CFG_H_SEL_SHIFT)

/*
 * Z_SEL (RW)
 *
 * Select Input for Phase Z：
 * 0：WUIO0
 * 1：WUIO1
 * 2：WUIO2
 * 3：WUIO3
 */
#define PDGO_PCNT_CALIB_CFG_Z_SEL_MASK (0x300U)
#define PDGO_PCNT_CALIB_CFG_Z_SEL_SHIFT (8U)
#define PDGO_PCNT_CALIB_CFG_Z_SEL_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_Z_SEL_SHIFT) & PDGO_PCNT_CALIB_CFG_Z_SEL_MASK)
#define PDGO_PCNT_CALIB_CFG_Z_SEL_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_Z_SEL_MASK) >> PDGO_PCNT_CALIB_CFG_Z_SEL_SHIFT)

/*
 * B_SEL (RW)
 *
 * Select Input for Phase B：
 * 0：WUIO0
 * 1：WUIO1
 * 2：WUIO2
 * 3：WUIO3
 */
#define PDGO_PCNT_CALIB_CFG_B_SEL_MASK (0x30U)
#define PDGO_PCNT_CALIB_CFG_B_SEL_SHIFT (4U)
#define PDGO_PCNT_CALIB_CFG_B_SEL_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_B_SEL_SHIFT) & PDGO_PCNT_CALIB_CFG_B_SEL_MASK)
#define PDGO_PCNT_CALIB_CFG_B_SEL_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_B_SEL_MASK) >> PDGO_PCNT_CALIB_CFG_B_SEL_SHIFT)

/*
 * A_SEL (RW)
 *
 * Select Input for Phase A：
 * 0：WUIO0
 * 1：WUIO1
 * 2：WUIO2
 * 3：WUIO3
 */
#define PDGO_PCNT_CALIB_CFG_A_SEL_MASK (0x3U)
#define PDGO_PCNT_CALIB_CFG_A_SEL_SHIFT (0U)
#define PDGO_PCNT_CALIB_CFG_A_SEL_SET(x) (((uint32_t)(x) << PDGO_PCNT_CALIB_CFG_A_SEL_SHIFT) & PDGO_PCNT_CALIB_CFG_A_SEL_MASK)
#define PDGO_PCNT_CALIB_CFG_A_SEL_GET(x) (((uint32_t)(x) & PDGO_PCNT_CALIB_CFG_A_SEL_MASK) >> PDGO_PCNT_CALIB_CFG_A_SEL_SHIFT)

/* Bitfield definition for register of struct array PCNT: RESOLUTION */
/*
 * RESOLUTION (RW)
 *
 * Resolution Value
 * Note：The Field in this register is the same as COUNTER register, so if target resolution value is TARGET_LINES , it should be shifted according to the current DECODE_MODE, and then decrease 1 because it counts from 0, it means when:
 * MODE_AB_4X: RESOLUTION = (TARGET_LINES << 2) - 1;
 * MODE_AB_2X: RESOLUTION = (TARGET_LINES << 1) - 1;
 * MODE_AB_1X: RESOLUTION = (TARGET_LINES << 0) - 1;
 * MODE_PD_1X: RESOLUTION = (TARGET_LINES << 0) - 1;
 * MODE_PDF1X: RESOLUTION = (TARGET_LINES << 0) - 1;
 * MODE_PD_2X: RESOLUTION = (TARGET_LINES << 1) - 1;
 * MODE_PDF2X: RESOLUTION = (TARGET_LINES << 1) - 1;
 * MODE_UD_1X: RESOLUTION = (TARGET_LINES << 0) - 1;
 * MODE_UDF1X: RESOLUTION = (TARGET_LINES << 0) - 1;
 * MODE_UD_2X: RESOLUTION = (TARGET_LINES << 1) - 1;
 * MODE_UDF2X: RESOLUTION = (TARGET_LINES << 1) - 1;
 * MODE_UVW6X: RESOLUTION = (TARGET_LINES << 3) - 1;
 * MODE_UVW6Y: RESOLUTION = (TARGET_LINES << 3) - 1;
 */
#define PDGO_PCNT_RESOLUTION_RESOLUTION_MASK (0xFFFFFFFFUL)
#define PDGO_PCNT_RESOLUTION_RESOLUTION_SHIFT (0U)
#define PDGO_PCNT_RESOLUTION_RESOLUTION_SET(x) (((uint32_t)(x) << PDGO_PCNT_RESOLUTION_RESOLUTION_SHIFT) & PDGO_PCNT_RESOLUTION_RESOLUTION_MASK)
#define PDGO_PCNT_RESOLUTION_RESOLUTION_GET(x) (((uint32_t)(x) & PDGO_PCNT_RESOLUTION_RESOLUTION_MASK) >> PDGO_PCNT_RESOLUTION_RESOLUTION_SHIFT)

/* Bitfield definition for register of struct array PCNT: HOMING_CFG */
/*
 * RNG_CMP_SRC (RW)
 *
 * Select Compare Source for Comparator 0 in Range Match mode ( Mode1 )：
 * 0: Compare value from counter (Line+Phase), compare when counter updates
 * 1: Compare value from counter (Revolution+Line+Phase),  compare when counter updates
 * 2: Compare value from tacho_line(16bits), compare after each tacho
 * 3: Compare value from tacho_time(16bits), compare after each tacho
 */
#define PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_MASK (0x300000UL)
#define PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_SHIFT (20U)
#define PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_SHIFT) & PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_MASK)
#define PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_MASK) >> PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_SHIFT)

/*
 * CMP0_MODE (RW)
 *
 * Match Mode for Comparator 0:
 * Comparison point 0 matches mode:
 * Mode0 : Disable.
 * Mode1 : Range Match:
 * If  CMP0 <= CMP1, match valid when CMP0 <= RNG_CMP_SRC <= CMP1;
 * If CMP0 > CMP1, match valid when CMP0 <= RNG_CMP_SRC or RNG_CMP_SRC <= CMP1;
 * Note: The interrupt will be only available when entry and exit compare, i.e., it is generated only twice, but you can check whether it matches or not through the status bit in register.
 * Mode2 : when the COUNTER value matches the CMP0;
 * Mode3 : when the COUNTER value matches the CMP0; and the direction update value should be the same as the current direction;
 * Mode4: when counting up, the COUNTER value matches the CMP0;
 * Mode5: when counting up, the COUNTER value matches the CMP0; and the direction update value should be the same as the current direction;
 * Mode6: when counting down, the COUNTER value matches the CMP0;
 * Mode7: when counting down, the COUNTER value matches the CMP0; and the direction update value should be the same as the current direction;
 * Direction update value: direction decoded by the decoder according to the Phase (e.g. Phase 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 0 ...... mean forward.
 * Current Direction: direction decoded by the comparator based on the line field in counter.
 */
#define PDGO_PCNT_HOMING_CFG_CMP0_MODE_MASK (0x70000UL)
#define PDGO_PCNT_HOMING_CFG_CMP0_MODE_SHIFT (16U)
#define PDGO_PCNT_HOMING_CFG_CMP0_MODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_CMP0_MODE_SHIFT) & PDGO_PCNT_HOMING_CFG_CMP0_MODE_MASK)
#define PDGO_PCNT_HOMING_CFG_CMP0_MODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_CMP0_MODE_MASK) >> PDGO_PCNT_HOMING_CFG_CMP0_MODE_SHIFT)

/*
 * DMODE (RW)
 *
 * Dump Mode Selection
 * 0：Disable
 * 1：Edge Mode(Use Edge in DSTATE Register)
 * 2：Level Mode(Match Phase A,B and H)
 * 3：Level Mode(Only Match Phase H)
 */
#define PDGO_PCNT_HOMING_CFG_DMODE_MASK (0x3000U)
#define PDGO_PCNT_HOMING_CFG_DMODE_SHIFT (12U)
#define PDGO_PCNT_HOMING_CFG_DMODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_DMODE_SHIFT) & PDGO_PCNT_HOMING_CFG_DMODE_MASK)
#define PDGO_PCNT_HOMING_CFG_DMODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_DMODE_MASK) >> PDGO_PCNT_HOMING_CFG_DMODE_SHIFT)

/*
 * DSTATE (RW)
 *
 * Dump Match State
 * When level mode:
 * The remaining bits correspond to the level state of the HZBA phase, respectively.
 * When edge mode:
 * Bit3: H-phase rising edge, reverse
 * Bit2: H-phase falling edge, forward
 * Bit1: H-phase falling edge, reverse
 * Bit0: H-phase rising edge, forward.
 */
#define PDGO_PCNT_HOMING_CFG_DSTATE_MASK (0xF00U)
#define PDGO_PCNT_HOMING_CFG_DSTATE_SHIFT (8U)
#define PDGO_PCNT_HOMING_CFG_DSTATE_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_DSTATE_SHIFT) & PDGO_PCNT_HOMING_CFG_DSTATE_MASK)
#define PDGO_PCNT_HOMING_CFG_DSTATE_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_DSTATE_MASK) >> PDGO_PCNT_HOMING_CFG_DSTATE_SHIFT)

/*
 * HMODE (RW)
 *
 * Phase H Mode Selection
 * 0：Disable
 * 1：Edge Mode(Use Edge in HSTATE Register)
 * 2：Level Mode(Match Phase A,B and H)
 * 3：Level Mode(Only Match Phase H)
 */
#define PDGO_PCNT_HOMING_CFG_HMODE_MASK (0x30U)
#define PDGO_PCNT_HOMING_CFG_HMODE_SHIFT (4U)
#define PDGO_PCNT_HOMING_CFG_HMODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_HMODE_SHIFT) & PDGO_PCNT_HOMING_CFG_HMODE_MASK)
#define PDGO_PCNT_HOMING_CFG_HMODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_HMODE_MASK) >> PDGO_PCNT_HOMING_CFG_HMODE_SHIFT)

/*
 * HSTATE (RW)
 *
 * H-Phase Match State
 * When level mode:
 * Each bit correspond to the level state of the HZBA phase, respectively.
 * When edge mode:
 * If bit3 is 1: Match H-phase rising edge, reverse
 * If bit2 is 1: Match H-phase falling edge, forward
 * If bit1 is 1: Match H-phase falling edge, reverse
 * If bit0 is 1: Match H-phase rising edge, forward.
 */
#define PDGO_PCNT_HOMING_CFG_HSTATE_MASK (0xFU)
#define PDGO_PCNT_HOMING_CFG_HSTATE_SHIFT (0U)
#define PDGO_PCNT_HOMING_CFG_HSTATE_SET(x) (((uint32_t)(x) << PDGO_PCNT_HOMING_CFG_HSTATE_SHIFT) & PDGO_PCNT_HOMING_CFG_HSTATE_MASK)
#define PDGO_PCNT_HOMING_CFG_HSTATE_GET(x) (((uint32_t)(x) & PDGO_PCNT_HOMING_CFG_HSTATE_MASK) >> PDGO_PCNT_HOMING_CFG_HSTATE_SHIFT)

/* Bitfield definition for register of struct array PCNT: ANALYSE_CFG */
/*
 * ERS (RW)
 *
 * Whether to discard this result and automatically restart the tacho immediately after a Dec_Error or Seq_Error is detected.
 */
#define PDGO_PCNT_ANALYSE_CFG_ERS_MASK (0x40000UL)
#define PDGO_PCNT_ANALYSE_CFG_ERS_SHIFT (18U)
#define PDGO_PCNT_ANALYSE_CFG_ERS_SET(x) (((uint32_t)(x) << PDGO_PCNT_ANALYSE_CFG_ERS_SHIFT) & PDGO_PCNT_ANALYSE_CFG_ERS_MASK)
#define PDGO_PCNT_ANALYSE_CFG_ERS_GET(x) (((uint32_t)(x) & PDGO_PCNT_ANALYSE_CFG_ERS_MASK) >> PDGO_PCNT_ANALYSE_CFG_ERS_SHIFT)

/*
 * TACHO_MODE (RW)
 *
 * Tacho Mode Selection
 * 0: Disable tacho
 * 1: Period Mode, specify the length of time, measure the number of complete lines traveled within the length of time, and record the actual time spent.
 * 2: Travel Mode, specify the number of lines, measure the time taken to reach the number of lines.
 */
#define PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_MASK (0x30000UL)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_SHIFT (16U)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_SET(x) (((uint32_t)(x) << PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_SHIFT) & PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_MASK)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_GET(x) (((uint32_t)(x) & PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_MASK) >> PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_SHIFT)

/*
 * TACHO_LEN (RW)
 *
 * Tacho Length
 * Period Mode：specify the length of time for each tacho.
 * Travel  Mode：specify the number of lines for each tacho.
 * Note: This is a SYNC register, and it won't be valid immediately after change. It can be change only after last configuration valid.
 */
#define PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_MASK (0xFFFFU)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_SHIFT (0U)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_SET(x) (((uint32_t)(x) << PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_SHIFT) & PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_MASK)
#define PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_GET(x) (((uint32_t)(x) & PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_MASK) >> PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_SHIFT)

/* Bitfield definition for register of struct array PCNT: DIRECTION */
/*
 * DIR (RW)
 *
 * Software Override Direction Value
 * 0： Forward
 * 1： Reverse
 */
#define PDGO_PCNT_DIRECTION_DIR_MASK (0x1U)
#define PDGO_PCNT_DIRECTION_DIR_SHIFT (0U)
#define PDGO_PCNT_DIRECTION_DIR_SET(x) (((uint32_t)(x) << PDGO_PCNT_DIRECTION_DIR_SHIFT) & PDGO_PCNT_DIRECTION_DIR_MASK)
#define PDGO_PCNT_DIRECTION_DIR_GET(x) (((uint32_t)(x) & PDGO_PCNT_DIRECTION_DIR_MASK) >> PDGO_PCNT_DIRECTION_DIR_SHIFT)

/* Bitfield definition for register of struct array PCNT: CMP0 */
/*
 * CMP0 (RW)
 *
 * Comnpare Point 0
 * Note: This is a SYNC register, and it won't be valid immediately after change. It can be change only after last configuration valid.
 */
#define PDGO_PCNT_CMP0_CMP0_MASK (0xFFFFFFFFUL)
#define PDGO_PCNT_CMP0_CMP0_SHIFT (0U)
#define PDGO_PCNT_CMP0_CMP0_SET(x) (((uint32_t)(x) << PDGO_PCNT_CMP0_CMP0_SHIFT) & PDGO_PCNT_CMP0_CMP0_MASK)
#define PDGO_PCNT_CMP0_CMP0_GET(x) (((uint32_t)(x) & PDGO_PCNT_CMP0_CMP0_MASK) >> PDGO_PCNT_CMP0_CMP0_SHIFT)

/* Bitfield definition for register of struct array PCNT: CMP1 */
/*
 * CMP1 (RW)
 *
 * Comnpare Point 1
 * Note: This is a SYNC register, and it won't be valid immediately after change. It can be change only after last configuration valid.
 */
#define PDGO_PCNT_CMP1_CMP1_MASK (0xFFFFFFFFUL)
#define PDGO_PCNT_CMP1_CMP1_SHIFT (0U)
#define PDGO_PCNT_CMP1_CMP1_SET(x) (((uint32_t)(x) << PDGO_PCNT_CMP1_CMP1_SHIFT) & PDGO_PCNT_CMP1_CMP1_MASK)
#define PDGO_PCNT_CMP1_CMP1_GET(x) (((uint32_t)(x) & PDGO_PCNT_CMP1_CMP1_MASK) >> PDGO_PCNT_CMP1_CMP1_SHIFT)

/* Bitfield definition for register of struct array PCNT: CNT */
/*
 * COUNTER (RW)
 *
 * Pulse Counter Value
 * Note:
 * 1. Pulse Counter Value update when:
 * - Software Update: According to the current DECODE_MODE, the software directly writes the target value to the Revolution field and Line field of this register;
 * - Phase Z (Index) match: When Phase Z matches, clear Line field; Revolution field will be increased by 1 if forward, Revolution field remains unchanged if reverse, and the Phase field will be updated by decoder.
 * - Phase H (Home) match: when Phase H matches, clear Revolution field; and the Line field and Phase field will be updated by decoder.
 * - Decoder update: Revolution field, Line field and Phase field will be updated by decoder.
 * 2. This is a SYNC register, and it won't be valid immediately after software write. It can be change only after last configuration valid.
 * 3. Pulse Counter Value include：
 * - Revolution field;
 * - Line field，the width of this bit field depends on RESOLUTION register;
 * - Phase field，the width of this bit field depends on DECODE_MODE register, when:
 * ·  MODE_AB_4X: 2 Bits，Bit[1:0];
 * ·  MODE_AB_2X: 1 Bit，Bit[0];
 * ·  MODE_AB_1X: 0 Bit，no Phase field;
 * ·  MODE_PD_1X: 0 Bit，no Phase field;
 * ·  MODE_PDF1X: 0 Bit，no Phase field;
 * ·  MODE_PD_2X: 1 Bit，Bit[0]；
 * ·  MODE_PDF2X: 1 Bit，Bit[0];
 * ·  MODE_UD_1X: 0 Bit，no Phase field;
 * ·  MODE_UDF1X: 0 Bit，no Phase field;
 * ·  MODE_UD_2X: 1 Bit，Bit[0];
 * ·  MODE_UDF2X: 1 Bit，Bit[0];
 * ·  MODE_UVW6X: 3 Bits，Bit[2:0];
 * ·  MODE_UVW6Y: 3 Bits，Bit[2:0];
 */
#define PDGO_PCNT_CNT_COUNTER_MASK (0xFFFFFFFFUL)
#define PDGO_PCNT_CNT_COUNTER_SHIFT (0U)
#define PDGO_PCNT_CNT_COUNTER_SET(x) (((uint32_t)(x) << PDGO_PCNT_CNT_COUNTER_SHIFT) & PDGO_PCNT_CNT_COUNTER_MASK)
#define PDGO_PCNT_CNT_COUNTER_GET(x) (((uint32_t)(x) & PDGO_PCNT_CNT_COUNTER_MASK) >> PDGO_PCNT_CNT_COUNTER_SHIFT)

/* Bitfield definition for register of struct array PCNT: TACHO */
/*
 * LINE (RW)
 *
 * Line Result
 * Period Mode：count up from 0 when forward; count down from 0 when forward.
 * Travel  Mode：count up from 0 when forward; count down from 0xFFFF when forward.
 */
#define PDGO_PCNT_TACHO_LINE_MASK (0xFFFF0000UL)
#define PDGO_PCNT_TACHO_LINE_SHIFT (16U)
#define PDGO_PCNT_TACHO_LINE_SET(x) (((uint32_t)(x) << PDGO_PCNT_TACHO_LINE_SHIFT) & PDGO_PCNT_TACHO_LINE_MASK)
#define PDGO_PCNT_TACHO_LINE_GET(x) (((uint32_t)(x) & PDGO_PCNT_TACHO_LINE_MASK) >> PDGO_PCNT_TACHO_LINE_SHIFT)

/*
 * TIME (RW)
 *
 * Time Result
 * Period Mode: The actual time taken to complete the complete Lines in a specified time.
 * Travel Mode: The actual time taken to complete the specified Lines.
 */
#define PDGO_PCNT_TACHO_TIME_MASK (0xFFFFU)
#define PDGO_PCNT_TACHO_TIME_SHIFT (0U)
#define PDGO_PCNT_TACHO_TIME_SET(x) (((uint32_t)(x) << PDGO_PCNT_TACHO_TIME_SHIFT) & PDGO_PCNT_TACHO_TIME_MASK)
#define PDGO_PCNT_TACHO_TIME_GET(x) (((uint32_t)(x) & PDGO_PCNT_TACHO_TIME_MASK) >> PDGO_PCNT_TACHO_TIME_SHIFT)

/* Bitfield definition for register of struct array PCNT: DUMP */
/*
 * DUMP (RO)
 *
 * Dump Result:  Save COUNTER value when DUMP match.
 */
#define PDGO_PCNT_DUMP_DUMP_MASK (0xFFFFFFFFUL)
#define PDGO_PCNT_DUMP_DUMP_SHIFT (0U)
#define PDGO_PCNT_DUMP_DUMP_GET(x) (((uint32_t)(x) & PDGO_PCNT_DUMP_DUMP_MASK) >> PDGO_PCNT_DUMP_DUMP_SHIFT)

/* Bitfield definition for register of struct array PCNT: STS */
/*
 * MATCH0 (RO)
 *
 * MATCH0 Status
 * 0:invalid
 * 1:valid
 */
#define PDGO_PCNT_STS_MATCH0_MASK (0x1U)
#define PDGO_PCNT_STS_MATCH0_SHIFT (0U)
#define PDGO_PCNT_STS_MATCH0_GET(x) (((uint32_t)(x) & PDGO_PCNT_STS_MATCH0_MASK) >> PDGO_PCNT_STS_MATCH0_SHIFT)



/* GPR register group index macro definition */
#define PDGO_GPR_0 (0UL)
#define PDGO_GPR_1 (1UL)
#define PDGO_GPR_2 (2UL)
#define PDGO_GPR_3 (3UL)
#define PDGO_GPR_4 (4UL)
#define PDGO_GPR_5 (5UL)
#define PDGO_GPR_6 (6UL)
#define PDGO_GPR_7 (7UL)

/* IOFILTER register group index macro definition */
#define PDGO_IOFILTER_0 (0UL)
#define PDGO_IOFILTER_1 (1UL)
#define PDGO_IOFILTER_2 (2UL)
#define PDGO_IOFILTER_3 (3UL)

/* PCAP register group index macro definition */
#define PDGO_PCAP_0 (0UL)

/* PCNT register group index macro definition */
#define PDGO_PCNT_0 (0UL)


#endif /* HPM_PDGO_H */
