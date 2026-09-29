/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_PCFG_H
#define HPM_PCFG_H

typedef struct {
    __RW uint32_t BANDGAP;                     /* 0x0: BANGGAP control */
    __RW uint32_t LDO1P1;                      /* 0x4: 1V LDO config */
    __RW uint32_t LDO2P5;                      /* 0x8: 2.5V LDO config */
    __R  uint8_t  RESERVED0[56];               /* 0xC - 0x43: Reserved */
    __RW uint32_t WAKE_CAUSE;                  /* 0x44: Wake up source */
    __RW uint32_t WAKE_MASK;                   /* 0x48: Wake up mask */
    __RW uint32_t SCG_CTRL;                    /* 0x4C: Clock gate control in PMIC */
    __R  uint8_t  RESERVED1[16];               /* 0x50 - 0x5F: Reserved */
    __RW uint32_t RC24M;                       /* 0x60: RC 24M config */
    __RW uint32_t RC24M_TRACK;                 /* 0x64: RC 24M track mode */
    __RW uint32_t TRACK_TARGET;                /* 0x68: RC 24M track target */
    __R  uint32_t STATUS;                      /* 0x6C: RC 24M track status */
} PCFG_Type;


/* Bitfield definition for register: BANDGAP */
/*
 * VBG_TRIMMED (RW)
 *
 * Bandgap trim happened, this bit set by hardware after trim value loaded, and stop load, write 0 will clear this bit and reload trim value
 * 0: bandgap is not trimmed
 * 1: bandgap is trimmed
 */
#define PCFG_BANDGAP_VBG_TRIMMED_MASK (0x80000000UL)
#define PCFG_BANDGAP_VBG_TRIMMED_SHIFT (31U)
#define PCFG_BANDGAP_VBG_TRIMMED_SET(x) (((uint32_t)(x) << PCFG_BANDGAP_VBG_TRIMMED_SHIFT) & PCFG_BANDGAP_VBG_TRIMMED_MASK)
#define PCFG_BANDGAP_VBG_TRIMMED_GET(x) (((uint32_t)(x) & PCFG_BANDGAP_VBG_TRIMMED_MASK) >> PCFG_BANDGAP_VBG_TRIMMED_SHIFT)

/*
 * VBG_1P0_TRIM (RW)
 *
 * Banggap 1.0V output trim value
 */
#define PCFG_BANDGAP_VBG_1P0_TRIM_MASK (0x1F0000UL)
#define PCFG_BANDGAP_VBG_1P0_TRIM_SHIFT (16U)
#define PCFG_BANDGAP_VBG_1P0_TRIM_SET(x) (((uint32_t)(x) << PCFG_BANDGAP_VBG_1P0_TRIM_SHIFT) & PCFG_BANDGAP_VBG_1P0_TRIM_MASK)
#define PCFG_BANDGAP_VBG_1P0_TRIM_GET(x) (((uint32_t)(x) & PCFG_BANDGAP_VBG_1P0_TRIM_MASK) >> PCFG_BANDGAP_VBG_1P0_TRIM_SHIFT)

/*
 * VBG_P65_TRIM (RW)
 *
 * Banggap 1.0V output trim value
 */
#define PCFG_BANDGAP_VBG_P65_TRIM_MASK (0x1F00U)
#define PCFG_BANDGAP_VBG_P65_TRIM_SHIFT (8U)
#define PCFG_BANDGAP_VBG_P65_TRIM_SET(x) (((uint32_t)(x) << PCFG_BANDGAP_VBG_P65_TRIM_SHIFT) & PCFG_BANDGAP_VBG_P65_TRIM_MASK)
#define PCFG_BANDGAP_VBG_P65_TRIM_GET(x) (((uint32_t)(x) & PCFG_BANDGAP_VBG_P65_TRIM_MASK) >> PCFG_BANDGAP_VBG_P65_TRIM_SHIFT)

/*
 * VBG_P50_TRIM (RW)
 *
 * Banggap 1.0V output trim value
 */
#define PCFG_BANDGAP_VBG_P50_TRIM_MASK (0x1FU)
#define PCFG_BANDGAP_VBG_P50_TRIM_SHIFT (0U)
#define PCFG_BANDGAP_VBG_P50_TRIM_SET(x) (((uint32_t)(x) << PCFG_BANDGAP_VBG_P50_TRIM_SHIFT) & PCFG_BANDGAP_VBG_P50_TRIM_MASK)
#define PCFG_BANDGAP_VBG_P50_TRIM_GET(x) (((uint32_t)(x) & PCFG_BANDGAP_VBG_P50_TRIM_MASK) >> PCFG_BANDGAP_VBG_P50_TRIM_SHIFT)

/* Bitfield definition for register: LDO1P1 */
/*
 * ENABLE (RW)
 *
 * LDO enable
 * 0: turn off LDO
 * 1: turn on LDO
 */
#define PCFG_LDO1P1_ENABLE_MASK (0x10000UL)
#define PCFG_LDO1P1_ENABLE_SHIFT (16U)
#define PCFG_LDO1P1_ENABLE_SET(x) (((uint32_t)(x) << PCFG_LDO1P1_ENABLE_SHIFT) & PCFG_LDO1P1_ENABLE_MASK)
#define PCFG_LDO1P1_ENABLE_GET(x) (((uint32_t)(x) & PCFG_LDO1P1_ENABLE_MASK) >> PCFG_LDO1P1_ENABLE_SHIFT)

/*
 * VOLT (RW)
 *
 * LDO output voltage in mV,  value valid through 700-1320, , step 20mV.  Hardware select voltage no less than target if not on valid steps, with maximum 1320mV.
 * 700: 700mV
 * 720: 720mV
 * . . .
 * 1320:1320mV
 */
#define PCFG_LDO1P1_VOLT_MASK (0xFFFU)
#define PCFG_LDO1P1_VOLT_SHIFT (0U)
#define PCFG_LDO1P1_VOLT_SET(x) (((uint32_t)(x) << PCFG_LDO1P1_VOLT_SHIFT) & PCFG_LDO1P1_VOLT_MASK)
#define PCFG_LDO1P1_VOLT_GET(x) (((uint32_t)(x) & PCFG_LDO1P1_VOLT_MASK) >> PCFG_LDO1P1_VOLT_SHIFT)

/* Bitfield definition for register: LDO2P5 */
/*
 * READY (RO)
 *
 * Ready flag, will set 1ms after enabled or voltage change
 * 0: LDO is not ready for use
 * 1: LDO is ready
 */
#define PCFG_LDO2P5_READY_MASK (0x8000000UL)
#define PCFG_LDO2P5_READY_SHIFT (27U)
#define PCFG_LDO2P5_READY_GET(x) (((uint32_t)(x) & PCFG_LDO2P5_READY_MASK) >> PCFG_LDO2P5_READY_SHIFT)

/*
 * MUX (RW)
 *
 * LDO mux
 * 0: select capless LDO
 * 1: select cap LDO
 */
#define PCFG_LDO2P5_MUX_MASK (0x20000UL)
#define PCFG_LDO2P5_MUX_SHIFT (17U)
#define PCFG_LDO2P5_MUX_SET(x) (((uint32_t)(x) << PCFG_LDO2P5_MUX_SHIFT) & PCFG_LDO2P5_MUX_MASK)
#define PCFG_LDO2P5_MUX_GET(x) (((uint32_t)(x) & PCFG_LDO2P5_MUX_MASK) >> PCFG_LDO2P5_MUX_SHIFT)

/*
 * ENABLE (RW)
 *
 * LDO enable
 * 0: turn off LDO
 * 1: turn on LDO
 */
#define PCFG_LDO2P5_ENABLE_MASK (0x10000UL)
#define PCFG_LDO2P5_ENABLE_SHIFT (16U)
#define PCFG_LDO2P5_ENABLE_SET(x) (((uint32_t)(x) << PCFG_LDO2P5_ENABLE_SHIFT) & PCFG_LDO2P5_ENABLE_MASK)
#define PCFG_LDO2P5_ENABLE_GET(x) (((uint32_t)(x) & PCFG_LDO2P5_ENABLE_MASK) >> PCFG_LDO2P5_ENABLE_SHIFT)

/*
 * VOLT (RW)
 *
 * LDO output voltage in mV,  value valid through 2125-2900, step 25mV.  Hardware select voltage no less than target if not on valid steps, with maximum 2900mV.
 * 2125: 2125mV
 * 2150: 2150mV
 * . . .
 * 2900:2900mV
 */
#define PCFG_LDO2P5_VOLT_MASK (0xFFFU)
#define PCFG_LDO2P5_VOLT_SHIFT (0U)
#define PCFG_LDO2P5_VOLT_SET(x) (((uint32_t)(x) << PCFG_LDO2P5_VOLT_SHIFT) & PCFG_LDO2P5_VOLT_MASK)
#define PCFG_LDO2P5_VOLT_GET(x) (((uint32_t)(x) & PCFG_LDO2P5_VOLT_MASK) >> PCFG_LDO2P5_VOLT_SHIFT)

/* Bitfield definition for register: WAKE_CAUSE */
/*
 * CAUSE (RW)
 *
 * wake up cause, each bit represents one wake up source, write 1 to clear the register bit
 * 0: wake up source is not active during last wakeup
 * 1: wake up source is active furing last wakeup
 * bit 0: pmic_enable
 * bit 7: UART interrupt
 * bit 8: TMR interrupt
 * bit 9: WDG interrupt
 * bit10: GPIO in PMIC interrupt
 * bit16: batt security interrupt
 * bit17:batt gpio interrupt
 * bit19:rtc interrupt
 * bit31: pin wakeup
 */
#define PCFG_WAKE_CAUSE_CAUSE_MASK (0xFFFFFFFFUL)
#define PCFG_WAKE_CAUSE_CAUSE_SHIFT (0U)
#define PCFG_WAKE_CAUSE_CAUSE_SET(x) (((uint32_t)(x) << PCFG_WAKE_CAUSE_CAUSE_SHIFT) & PCFG_WAKE_CAUSE_CAUSE_MASK)
#define PCFG_WAKE_CAUSE_CAUSE_GET(x) (((uint32_t)(x) & PCFG_WAKE_CAUSE_CAUSE_MASK) >> PCFG_WAKE_CAUSE_CAUSE_SHIFT)

/* Bitfield definition for register: WAKE_MASK */
/*
 * MASK (RW)
 *
 * mask for wake up sources, each bit represents one wakeup source
 * 0: allow source to wake up system
 * 1: disallow source to wakeup system
 * bit 0: pmic_enable
 * bit 7: UART interrupt
 * bit 8: TMR interrupt
 * bit 9: WDG interrupt
 * bit10: GPIO in PMIC interrupt
 * bit16: batt security interrupt
 * bit17:batt gpio interrupt
 * bit19:rtc interrupt
 * bit31: pin wakeup
 */
#define PCFG_WAKE_MASK_MASK_MASK (0xFFFFFFFFUL)
#define PCFG_WAKE_MASK_MASK_SHIFT (0U)
#define PCFG_WAKE_MASK_MASK_SET(x) (((uint32_t)(x) << PCFG_WAKE_MASK_MASK_SHIFT) & PCFG_WAKE_MASK_MASK_MASK)
#define PCFG_WAKE_MASK_MASK_GET(x) (((uint32_t)(x) & PCFG_WAKE_MASK_MASK_MASK) >> PCFG_WAKE_MASK_MASK_SHIFT)

/* Bitfield definition for register: SCG_CTRL */
/*
 * SCG (RW)
 *
 * control whether clock being gated during PMIC low power flow, 2 bits for each peripheral
 * 00,01: reserved
 * 10: clock is always off
 * 11: clock is always on
 * bit6-7:gpio
 * bit8-9:ioc
 * bit10-11: timer
 * bit12-13:wdog
 * bit14-15:uart
 */
#define PCFG_SCG_CTRL_SCG_MASK (0xFFFFFFFFUL)
#define PCFG_SCG_CTRL_SCG_SHIFT (0U)
#define PCFG_SCG_CTRL_SCG_SET(x) (((uint32_t)(x) << PCFG_SCG_CTRL_SCG_SHIFT) & PCFG_SCG_CTRL_SCG_MASK)
#define PCFG_SCG_CTRL_SCG_GET(x) (((uint32_t)(x) & PCFG_SCG_CTRL_SCG_MASK) >> PCFG_SCG_CTRL_SCG_SHIFT)

/* Bitfield definition for register: RC24M */
/*
 * RC_TRIMMED (RW)
 *
 * RC24M trim happened, this bit set by hardware after trim value loaded, and stop load, write 0 will clear this bit and reload trim value
 * 0: RC is not trimmed
 * 1: RC is trimmed
 */
#define PCFG_RC24M_RC_TRIMMED_MASK (0x80000000UL)
#define PCFG_RC24M_RC_TRIMMED_SHIFT (31U)
#define PCFG_RC24M_RC_TRIMMED_SET(x) (((uint32_t)(x) << PCFG_RC24M_RC_TRIMMED_SHIFT) & PCFG_RC24M_RC_TRIMMED_MASK)
#define PCFG_RC24M_RC_TRIMMED_GET(x) (((uint32_t)(x) & PCFG_RC24M_RC_TRIMMED_MASK) >> PCFG_RC24M_RC_TRIMMED_SHIFT)

/*
 * TRIM_C (RW)
 *
 * Coarse trim for RC24M, bigger value means faster
 */
#define PCFG_RC24M_TRIM_C_MASK (0x700U)
#define PCFG_RC24M_TRIM_C_SHIFT (8U)
#define PCFG_RC24M_TRIM_C_SET(x) (((uint32_t)(x) << PCFG_RC24M_TRIM_C_SHIFT) & PCFG_RC24M_TRIM_C_MASK)
#define PCFG_RC24M_TRIM_C_GET(x) (((uint32_t)(x) & PCFG_RC24M_TRIM_C_MASK) >> PCFG_RC24M_TRIM_C_SHIFT)

/*
 * TRIM_F (RW)
 *
 * Fine trim for RC24M, bigger value means faster
 */
#define PCFG_RC24M_TRIM_F_MASK (0x1FU)
#define PCFG_RC24M_TRIM_F_SHIFT (0U)
#define PCFG_RC24M_TRIM_F_SET(x) (((uint32_t)(x) << PCFG_RC24M_TRIM_F_SHIFT) & PCFG_RC24M_TRIM_F_MASK)
#define PCFG_RC24M_TRIM_F_GET(x) (((uint32_t)(x) & PCFG_RC24M_TRIM_F_MASK) >> PCFG_RC24M_TRIM_F_SHIFT)

/* Bitfield definition for register: RC24M_TRACK */
/*
 * SEL24M (RW)
 *
 * Select track reference
 * 0: select 32K as reference
 * 1: select 24M XTAL as reference
 */
#define PCFG_RC24M_TRACK_SEL24M_MASK (0x10000UL)
#define PCFG_RC24M_TRACK_SEL24M_SHIFT (16U)
#define PCFG_RC24M_TRACK_SEL24M_SET(x) (((uint32_t)(x) << PCFG_RC24M_TRACK_SEL24M_SHIFT) & PCFG_RC24M_TRACK_SEL24M_MASK)
#define PCFG_RC24M_TRACK_SEL24M_GET(x) (((uint32_t)(x) & PCFG_RC24M_TRACK_SEL24M_MASK) >> PCFG_RC24M_TRACK_SEL24M_SHIFT)

/*
 * RETURN (RW)
 *
 * Retrun default value when XTAL loss
 * 0: remain last tracking value
 * 1: switch to default value
 */
#define PCFG_RC24M_TRACK_RETURN_MASK (0x10U)
#define PCFG_RC24M_TRACK_RETURN_SHIFT (4U)
#define PCFG_RC24M_TRACK_RETURN_SET(x) (((uint32_t)(x) << PCFG_RC24M_TRACK_RETURN_SHIFT) & PCFG_RC24M_TRACK_RETURN_MASK)
#define PCFG_RC24M_TRACK_RETURN_GET(x) (((uint32_t)(x) & PCFG_RC24M_TRACK_RETURN_MASK) >> PCFG_RC24M_TRACK_RETURN_SHIFT)

/*
 * TRACK (RW)
 *
 * track mode
 * 0: RC24M free running
 * 1: track RC24M to external XTAL
 */
#define PCFG_RC24M_TRACK_TRACK_MASK (0x1U)
#define PCFG_RC24M_TRACK_TRACK_SHIFT (0U)
#define PCFG_RC24M_TRACK_TRACK_SET(x) (((uint32_t)(x) << PCFG_RC24M_TRACK_TRACK_SHIFT) & PCFG_RC24M_TRACK_TRACK_MASK)
#define PCFG_RC24M_TRACK_TRACK_GET(x) (((uint32_t)(x) & PCFG_RC24M_TRACK_TRACK_MASK) >> PCFG_RC24M_TRACK_TRACK_SHIFT)

/* Bitfield definition for register: TRACK_TARGET */
/*
 * PRE_DIV (RW)
 *
 * Divider for reference source
 */
#define PCFG_TRACK_TARGET_PRE_DIV_MASK (0xFFFF0000UL)
#define PCFG_TRACK_TARGET_PRE_DIV_SHIFT (16U)
#define PCFG_TRACK_TARGET_PRE_DIV_SET(x) (((uint32_t)(x) << PCFG_TRACK_TARGET_PRE_DIV_SHIFT) & PCFG_TRACK_TARGET_PRE_DIV_MASK)
#define PCFG_TRACK_TARGET_PRE_DIV_GET(x) (((uint32_t)(x) & PCFG_TRACK_TARGET_PRE_DIV_MASK) >> PCFG_TRACK_TARGET_PRE_DIV_SHIFT)

/*
 * TARGET (RW)
 *
 * Target frequency multiplier of divided source
 */
#define PCFG_TRACK_TARGET_TARGET_MASK (0xFFFFU)
#define PCFG_TRACK_TARGET_TARGET_SHIFT (0U)
#define PCFG_TRACK_TARGET_TARGET_SET(x) (((uint32_t)(x) << PCFG_TRACK_TARGET_TARGET_SHIFT) & PCFG_TRACK_TARGET_TARGET_MASK)
#define PCFG_TRACK_TARGET_TARGET_GET(x) (((uint32_t)(x) & PCFG_TRACK_TARGET_TARGET_MASK) >> PCFG_TRACK_TARGET_TARGET_SHIFT)

/* Bitfield definition for register: STATUS */
/*
 * SEL32K (RO)
 *
 * track is using XTAL32K
 * 0: track is not using XTAL32K
 * 1: track is using XTAL32K
 */
#define PCFG_STATUS_SEL32K_MASK (0x100000UL)
#define PCFG_STATUS_SEL32K_SHIFT (20U)
#define PCFG_STATUS_SEL32K_GET(x) (((uint32_t)(x) & PCFG_STATUS_SEL32K_MASK) >> PCFG_STATUS_SEL32K_SHIFT)

/*
 * SEL24M (RO)
 *
 * track is using XTAL24M
 * 0: track is not using XTAL24M
 * 1: track is using XTAL24M
 */
#define PCFG_STATUS_SEL24M_MASK (0x10000UL)
#define PCFG_STATUS_SEL24M_SHIFT (16U)
#define PCFG_STATUS_SEL24M_GET(x) (((uint32_t)(x) & PCFG_STATUS_SEL24M_MASK) >> PCFG_STATUS_SEL24M_SHIFT)

/*
 * EN_TRIM (RO)
 *
 * default value takes effect
 * 0: default value is invalid
 * 1: default value is valid
 */
#define PCFG_STATUS_EN_TRIM_MASK (0x8000U)
#define PCFG_STATUS_EN_TRIM_SHIFT (15U)
#define PCFG_STATUS_EN_TRIM_GET(x) (((uint32_t)(x) & PCFG_STATUS_EN_TRIM_MASK) >> PCFG_STATUS_EN_TRIM_SHIFT)

/*
 * TRIM_C (RO)
 *
 * default coarse trim value
 */
#define PCFG_STATUS_TRIM_C_MASK (0x700U)
#define PCFG_STATUS_TRIM_C_SHIFT (8U)
#define PCFG_STATUS_TRIM_C_GET(x) (((uint32_t)(x) & PCFG_STATUS_TRIM_C_MASK) >> PCFG_STATUS_TRIM_C_SHIFT)

/*
 * TRIM_F (RO)
 *
 * default fine trim value
 */
#define PCFG_STATUS_TRIM_F_MASK (0x1FU)
#define PCFG_STATUS_TRIM_F_SHIFT (0U)
#define PCFG_STATUS_TRIM_F_GET(x) (((uint32_t)(x) & PCFG_STATUS_TRIM_F_MASK) >> PCFG_STATUS_TRIM_F_SHIFT)




#endif /* HPM_PCFG_H */
