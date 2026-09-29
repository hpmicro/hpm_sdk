/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_GPTMRV2_H
#define HPM_GPTMRV2_H

typedef struct {
    struct {
        __RW uint32_t CR;                      /* 0x0: Control Register */
        __RW uint32_t CMP[2];                  /* 0x4 - 0x8: Comparator register 0 */
        __RW uint32_t RLD;                     /* 0xC: Reload register */
        __RW uint32_t CNTUPTVAL;               /* 0x10: Counter update value register */
        __RW uint32_t BURST_CFG;               /* 0x14: burst_cfg */
        __R  uint32_t BURST_COUNT;             /* 0x18: burst_count */
        __R  uint8_t  RESERVED0[4];            /* 0x1C - 0x1F: Reserved */
        __R  uint32_t CAPPOS;                  /* 0x20: Capture rising edge register */
        __R  uint32_t CAPNEG;                  /* 0x24: Capture falling edge register */
        __R  uint32_t CAPPRD;                  /* 0x28: PWM period measure register */
        __R  uint32_t CAPDTY;                  /* 0x2C: PWM duty cycle measure register */
        __R  uint32_t CNT;                     /* 0x30: Counter */
        __R  uint8_t  RESERVED1[12];           /* 0x34 - 0x3F: Reserved */
    } CHANNEL[4];
    __R  uint8_t  RESERVED0[256];              /* 0x100 - 0x1FF: Reserved */
    __RW uint32_t SR;                          /* 0x200: Status register */
    __RW uint32_t IRQEN;                       /* 0x204: Interrupt request enable register */
    __RW uint32_t GCR;                         /* 0x208: Global control register */
} GPTMRV2_Type;


/* Bitfield definition for register of struct array CHANNEL: CR */
/*
 * CNTUPT (WO)
 *
 * 1- update counter to new value as CNTUPTVAL
 * This bit will be auto cleared after 1 cycle
 */
#define GPTMRV2_CHANNEL_CR_CNTUPT_MASK (0x80000000UL)
#define GPTMRV2_CHANNEL_CR_CNTUPT_SHIFT (31U)
#define GPTMRV2_CHANNEL_CR_CNTUPT_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CNTUPT_SHIFT) & GPTMRV2_CHANNEL_CR_CNTUPT_MASK)
#define GPTMRV2_CHANNEL_CR_CNTUPT_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CNTUPT_MASK) >> GPTMRV2_CHANNEL_CR_CNTUPT_SHIFT)

/*
 * SHADOW_EN (RW)
 *
 * set to enable val0 and val1 shadow feature,
 * the value of val0 and val1 will be loaded as working register at reload point
 */
#define GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK (0x100000UL)
#define GPTMRV2_CHANNEL_CR_SHADOW_EN_SHIFT (20U)
#define GPTMRV2_CHANNEL_CR_SHADOW_EN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_SHADOW_EN_SHIFT) & GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK)
#define GPTMRV2_CHANNEL_CR_SHADOW_EN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK) >> GPTMRV2_CHANNEL_CR_SHADOW_EN_SHIFT)

/*
 * BURST_MODE (RW)
 *
 * set to enable burst mode, timer will reload configured times(burst_cfg), then stop.
 * user need clear CEN and set it to start timer agian.
 * NOTE: do not set burst_mode and opmode at same time
 */
#define GPTMRV2_CHANNEL_CR_BURST_MODE_MASK (0x80000UL)
#define GPTMRV2_CHANNEL_CR_BURST_MODE_SHIFT (19U)
#define GPTMRV2_CHANNEL_CR_BURST_MODE_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_BURST_MODE_SHIFT) & GPTMRV2_CHANNEL_CR_BURST_MODE_MASK)
#define GPTMRV2_CHANNEL_CR_BURST_MODE_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_BURST_MODE_MASK) >> GPTMRV2_CHANNEL_CR_BURST_MODE_SHIFT)

/*
 * CNT_MODE (RW)
 *
 * 0: internal counting mode, timer increase each gptmr clock cycle.
 * 1: external counting mode, timer increase at each input signal posedge,
 *     reload/compare feature can still work but change at input signal posedge.
 */
#define GPTMRV2_CHANNEL_CR_CNT_MODE_MASK (0x40000UL)
#define GPTMRV2_CHANNEL_CR_CNT_MODE_SHIFT (18U)
#define GPTMRV2_CHANNEL_CR_CNT_MODE_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CNT_MODE_SHIFT) & GPTMRV2_CHANNEL_CR_CNT_MODE_MASK)
#define GPTMRV2_CHANNEL_CR_CNT_MODE_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CNT_MODE_MASK) >> GPTMRV2_CHANNEL_CR_CNT_MODE_SHIFT)

/*
 * OPMODE (RW)
 *
 * 0:  round mode
 * 1:  one-shot mode, timer will stopped at reload point.user need clear CEN and set it to start timer agian.
 * NOTE: reload irq will be always set at one-shot mode at end
 */
#define GPTMRV2_CHANNEL_CR_OPMODE_MASK (0x20000UL)
#define GPTMRV2_CHANNEL_CR_OPMODE_SHIFT (17U)
#define GPTMRV2_CHANNEL_CR_OPMODE_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_OPMODE_SHIFT) & GPTMRV2_CHANNEL_CR_OPMODE_MASK)
#define GPTMRV2_CHANNEL_CR_OPMODE_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_OPMODE_MASK) >> GPTMRV2_CHANNEL_CR_OPMODE_SHIFT)

/*
 * MONITOR_SEL (RW)
 *
 * set to monitor input signal high level time(chan_meas_high)
 * clr to monitor input signal period(chan_meas_prd)
 */
#define GPTMRV2_CHANNEL_CR_MONITOR_SEL_MASK (0x10000UL)
#define GPTMRV2_CHANNEL_CR_MONITOR_SEL_SHIFT (16U)
#define GPTMRV2_CHANNEL_CR_MONITOR_SEL_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_MONITOR_SEL_SHIFT) & GPTMRV2_CHANNEL_CR_MONITOR_SEL_MASK)
#define GPTMRV2_CHANNEL_CR_MONITOR_SEL_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_MONITOR_SEL_MASK) >> GPTMRV2_CHANNEL_CR_MONITOR_SEL_SHIFT)

/*
 * MONITOR_EN (RW)
 *
 * set to monitor input signal period or high level time.
 * When this bit is set, if detected period less than val_0 or more than val_1, will set related irq_sts
 * * only can be used when trig_mode is selected as measure mode(100)
 * * the time may not correct after reload, so monitor is disabled after reload point, and enabled again after two continul posedge.
 * if no posedge after reload for more than val_1, will also assert irq_capt
 */
#define GPTMRV2_CHANNEL_CR_MONITOR_EN_MASK (0x8000U)
#define GPTMRV2_CHANNEL_CR_MONITOR_EN_SHIFT (15U)
#define GPTMRV2_CHANNEL_CR_MONITOR_EN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_MONITOR_EN_SHIFT) & GPTMRV2_CHANNEL_CR_MONITOR_EN_MASK)
#define GPTMRV2_CHANNEL_CR_MONITOR_EN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_MONITOR_EN_MASK) >> GPTMRV2_CHANNEL_CR_MONITOR_EN_SHIFT)

/*
 * CNTRST (RW)
 *
 * 1- reset counter
 */
#define GPTMRV2_CHANNEL_CR_CNTRST_MASK (0x4000U)
#define GPTMRV2_CHANNEL_CR_CNTRST_SHIFT (14U)
#define GPTMRV2_CHANNEL_CR_CNTRST_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CNTRST_SHIFT) & GPTMRV2_CHANNEL_CR_CNTRST_MASK)
#define GPTMRV2_CHANNEL_CR_CNTRST_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CNTRST_MASK) >> GPTMRV2_CHANNEL_CR_CNTRST_SHIFT)

/*
 * SYNCFLW (RW)
 *
 * 1- enable this channel to reset counter to reload(RLD) together with its previous channel.
 * This bit is not valid for channel 0.
 */
#define GPTMRV2_CHANNEL_CR_SYNCFLW_MASK (0x2000U)
#define GPTMRV2_CHANNEL_CR_SYNCFLW_SHIFT (13U)
#define GPTMRV2_CHANNEL_CR_SYNCFLW_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_SYNCFLW_SHIFT) & GPTMRV2_CHANNEL_CR_SYNCFLW_MASK)
#define GPTMRV2_CHANNEL_CR_SYNCFLW_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_SYNCFLW_MASK) >> GPTMRV2_CHANNEL_CR_SYNCFLW_SHIFT)

/*
 * SYNCIFEN (RW)
 *
 * 1- SYNCI is valid on its falling edge
 */
#define GPTMRV2_CHANNEL_CR_SYNCIFEN_MASK (0x1000U)
#define GPTMRV2_CHANNEL_CR_SYNCIFEN_SHIFT (12U)
#define GPTMRV2_CHANNEL_CR_SYNCIFEN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_SYNCIFEN_SHIFT) & GPTMRV2_CHANNEL_CR_SYNCIFEN_MASK)
#define GPTMRV2_CHANNEL_CR_SYNCIFEN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_SYNCIFEN_MASK) >> GPTMRV2_CHANNEL_CR_SYNCIFEN_SHIFT)

/*
 * SYNCIREN (RW)
 *
 * 1- SYNCI is valid on its rising edge
 */
#define GPTMRV2_CHANNEL_CR_SYNCIREN_MASK (0x800U)
#define GPTMRV2_CHANNEL_CR_SYNCIREN_SHIFT (11U)
#define GPTMRV2_CHANNEL_CR_SYNCIREN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_SYNCIREN_SHIFT) & GPTMRV2_CHANNEL_CR_SYNCIREN_MASK)
#define GPTMRV2_CHANNEL_CR_SYNCIREN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_SYNCIREN_MASK) >> GPTMRV2_CHANNEL_CR_SYNCIREN_SHIFT)

/*
 * CEN (RW)
 *
 * 1- counter enable
 */
#define GPTMRV2_CHANNEL_CR_CEN_MASK (0x400U)
#define GPTMRV2_CHANNEL_CR_CEN_SHIFT (10U)
#define GPTMRV2_CHANNEL_CR_CEN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CEN_SHIFT) & GPTMRV2_CHANNEL_CR_CEN_MASK)
#define GPTMRV2_CHANNEL_CR_CEN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CEN_MASK) >> GPTMRV2_CHANNEL_CR_CEN_SHIFT)

/*
 * CMPINIT (RW)
 *
 * Output compare initial poliarity
 * 1- The channel output initial level is high
 * 0- The channel output initial level is low
 * User should set this bit before set CMPEN to 1.
 */
#define GPTMRV2_CHANNEL_CR_CMPINIT_MASK (0x200U)
#define GPTMRV2_CHANNEL_CR_CMPINIT_SHIFT (9U)
#define GPTMRV2_CHANNEL_CR_CMPINIT_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CMPINIT_SHIFT) & GPTMRV2_CHANNEL_CR_CMPINIT_MASK)
#define GPTMRV2_CHANNEL_CR_CMPINIT_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CMPINIT_MASK) >> GPTMRV2_CHANNEL_CR_CMPINIT_SHIFT)

/*
 * CMPEN (RW)
 *
 * 1- Enable the channel output compare function. The output signal can be generated per comparator (CMPx) settings.
 */
#define GPTMRV2_CHANNEL_CR_CMPEN_MASK (0x100U)
#define GPTMRV2_CHANNEL_CR_CMPEN_SHIFT (8U)
#define GPTMRV2_CHANNEL_CR_CMPEN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CMPEN_SHIFT) & GPTMRV2_CHANNEL_CR_CMPEN_MASK)
#define GPTMRV2_CHANNEL_CR_CMPEN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CMPEN_MASK) >> GPTMRV2_CHANNEL_CR_CMPEN_SHIFT)

/*
 * DMASEL (RW)
 *
 * select one of DMA request:
 * 00- CMP0 flag
 * 01- CMP1 flag
 * 10- Input signal toggle captured
 * 11- RLD flag, counter reload;
 */
#define GPTMRV2_CHANNEL_CR_DMASEL_MASK (0xC0U)
#define GPTMRV2_CHANNEL_CR_DMASEL_SHIFT (6U)
#define GPTMRV2_CHANNEL_CR_DMASEL_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_DMASEL_SHIFT) & GPTMRV2_CHANNEL_CR_DMASEL_MASK)
#define GPTMRV2_CHANNEL_CR_DMASEL_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_DMASEL_MASK) >> GPTMRV2_CHANNEL_CR_DMASEL_SHIFT)

/*
 * DMAEN (RW)
 *
 * 1- enable dma
 */
#define GPTMRV2_CHANNEL_CR_DMAEN_MASK (0x20U)
#define GPTMRV2_CHANNEL_CR_DMAEN_SHIFT (5U)
#define GPTMRV2_CHANNEL_CR_DMAEN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_DMAEN_SHIFT) & GPTMRV2_CHANNEL_CR_DMAEN_MASK)
#define GPTMRV2_CHANNEL_CR_DMAEN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_DMAEN_MASK) >> GPTMRV2_CHANNEL_CR_DMAEN_SHIFT)

/*
 * SWSYNCIEN (RW)
 *
 * 1- enable software sync. When this bit is set, counter will reset to RLD when swsynct bit is set
 */
#define GPTMRV2_CHANNEL_CR_SWSYNCIEN_MASK (0x10U)
#define GPTMRV2_CHANNEL_CR_SWSYNCIEN_SHIFT (4U)
#define GPTMRV2_CHANNEL_CR_SWSYNCIEN_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_SWSYNCIEN_SHIFT) & GPTMRV2_CHANNEL_CR_SWSYNCIEN_MASK)
#define GPTMRV2_CHANNEL_CR_SWSYNCIEN_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_SWSYNCIEN_MASK) >> GPTMRV2_CHANNEL_CR_SWSYNCIEN_SHIFT)

/*
 * DBGPAUSE (RW)
 *
 * 1- counter will pause if chip is in debug mode
 */
#define GPTMRV2_CHANNEL_CR_DBGPAUSE_MASK (0x8U)
#define GPTMRV2_CHANNEL_CR_DBGPAUSE_SHIFT (3U)
#define GPTMRV2_CHANNEL_CR_DBGPAUSE_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_DBGPAUSE_SHIFT) & GPTMRV2_CHANNEL_CR_DBGPAUSE_MASK)
#define GPTMRV2_CHANNEL_CR_DBGPAUSE_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_DBGPAUSE_MASK) >> GPTMRV2_CHANNEL_CR_DBGPAUSE_SHIFT)

/*
 * CAPMODE (RW)
 *
 * This bitfield define the input capture mode
 * 100:  width measure mode, timer will calculate the input signal period and duty cycle
 * 011:  capture at both rising edge and falling edge
 * 010:  capture at falling edge
 * 001:  capture at rising edge
 * 000:  No capture
 */
#define GPTMRV2_CHANNEL_CR_CAPMODE_MASK (0x7U)
#define GPTMRV2_CHANNEL_CR_CAPMODE_SHIFT (0U)
#define GPTMRV2_CHANNEL_CR_CAPMODE_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CR_CAPMODE_SHIFT) & GPTMRV2_CHANNEL_CR_CAPMODE_MASK)
#define GPTMRV2_CHANNEL_CR_CAPMODE_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CR_CAPMODE_MASK) >> GPTMRV2_CHANNEL_CR_CAPMODE_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CMP0 */
/*
 * CMP (RW)
 *
 * compare value 0
 */
#define GPTMRV2_CHANNEL_CMP_CMP_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CMP_CMP_SHIFT (0U)
#define GPTMRV2_CHANNEL_CMP_CMP_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CMP_CMP_SHIFT) & GPTMRV2_CHANNEL_CMP_CMP_MASK)
#define GPTMRV2_CHANNEL_CMP_CMP_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CMP_CMP_MASK) >> GPTMRV2_CHANNEL_CMP_CMP_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: RLD */
/*
 * RLD (RW)
 *
 * reload value
 */
#define GPTMRV2_CHANNEL_RLD_RLD_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_RLD_RLD_SHIFT (0U)
#define GPTMRV2_CHANNEL_RLD_RLD_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_RLD_RLD_SHIFT) & GPTMRV2_CHANNEL_RLD_RLD_MASK)
#define GPTMRV2_CHANNEL_RLD_RLD_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_RLD_RLD_MASK) >> GPTMRV2_CHANNEL_RLD_RLD_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CNTUPTVAL */
/*
 * CNTUPTVAL (RW)
 *
 * counter will be set to this value when software write cntupt bit in CR
 */
#define GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_SHIFT (0U)
#define GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_SHIFT) & GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_MASK)
#define GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_MASK) >> GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: BURST_CFG */
/*
 * PRE_DIV (RW)
 *
 * 0 for no pre divider
 */
#define GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_MASK (0xFFFF0000UL)
#define GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SHIFT (16U)
#define GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SHIFT) & GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_MASK)
#define GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_MASK) >> GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SHIFT)

/*
 * BURST_CFG (RW)
 *
 * Burst configuration register: The counter stops after completing this number of cycles, and can be used to output a specified number of pulses.
 */
#define GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_MASK (0xFFFFU)
#define GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_SHIFT (0U)
#define GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_SET(x) (((uint32_t)(x) << GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_SHIFT) & GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_MASK)
#define GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_MASK) >> GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: BURST_COUNT */
/*
 * BURST_COUNT (RO)
 *
 * Current burst count: Increments by 1 every time the counter hits the reload value.
 */
#define GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_MASK (0xFFFFU)
#define GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_SHIFT (0U)
#define GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_MASK) >> GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CAPPOS */
/*
 * CAPPOS (RO)
 *
 * This register contains the counter value captured at input signal rising edge
 */
#define GPTMRV2_CHANNEL_CAPPOS_CAPPOS_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CAPPOS_CAPPOS_SHIFT (0U)
#define GPTMRV2_CHANNEL_CAPPOS_CAPPOS_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CAPPOS_CAPPOS_MASK) >> GPTMRV2_CHANNEL_CAPPOS_CAPPOS_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CAPNEG */
/*
 * CAPNEG (RO)
 *
 * This register contains the counter value captured at input signal falling edge
 */
#define GPTMRV2_CHANNEL_CAPNEG_CAPNEG_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CAPNEG_CAPNEG_SHIFT (0U)
#define GPTMRV2_CHANNEL_CAPNEG_CAPNEG_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CAPNEG_CAPNEG_MASK) >> GPTMRV2_CHANNEL_CAPNEG_CAPNEG_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CAPPRD */
/*
 * CAPPRD (RO)
 *
 * This register contains the input signal period when channel is configured to input capture measure mode.
 */
#define GPTMRV2_CHANNEL_CAPPRD_CAPPRD_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CAPPRD_CAPPRD_SHIFT (0U)
#define GPTMRV2_CHANNEL_CAPPRD_CAPPRD_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CAPPRD_CAPPRD_MASK) >> GPTMRV2_CHANNEL_CAPPRD_CAPPRD_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CAPDTY */
/*
 * MEAS_HIGH (RO)
 *
 * This register contains the input signal duty cycle when channel is configured to input capture measure mode.
 */
#define GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_SHIFT (0U)
#define GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_MASK) >> GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_SHIFT)

/* Bitfield definition for register of struct array CHANNEL: CNT */
/*
 * COUNTER (RO)
 *
 * 32 bit counter value
 */
#define GPTMRV2_CHANNEL_CNT_COUNTER_MASK (0xFFFFFFUL)
#define GPTMRV2_CHANNEL_CNT_COUNTER_SHIFT (0U)
#define GPTMRV2_CHANNEL_CNT_COUNTER_GET(x) (((uint32_t)(x) & GPTMRV2_CHANNEL_CNT_COUNTER_MASK) >> GPTMRV2_CHANNEL_CNT_COUNTER_SHIFT)

/* Bitfield definition for register: SR */
/*
 * CH3CMP1F (W1C)
 *
 * channel 3 compare value 1 match flag
 */
#define GPTMRV2_SR_CH3CMP1F_MASK (0x8000U)
#define GPTMRV2_SR_CH3CMP1F_SHIFT (15U)
#define GPTMRV2_SR_CH3CMP1F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH3CMP1F_SHIFT) & GPTMRV2_SR_CH3CMP1F_MASK)
#define GPTMRV2_SR_CH3CMP1F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH3CMP1F_MASK) >> GPTMRV2_SR_CH3CMP1F_SHIFT)

/*
 * CH3CMP0F (W1C)
 *
 * channel 3 compare value 1 match flag
 */
#define GPTMRV2_SR_CH3CMP0F_MASK (0x4000U)
#define GPTMRV2_SR_CH3CMP0F_SHIFT (14U)
#define GPTMRV2_SR_CH3CMP0F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH3CMP0F_SHIFT) & GPTMRV2_SR_CH3CMP0F_MASK)
#define GPTMRV2_SR_CH3CMP0F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH3CMP0F_MASK) >> GPTMRV2_SR_CH3CMP0F_SHIFT)

/*
 * CH3CAPF (W1C)
 *
 * channel 3 capture flag, the flag will be set at the valid capture edge per CAPMODE setting.
 * If the capture channel is set to measure mode, the flag will be set at rising edge.
 */
#define GPTMRV2_SR_CH3CAPF_MASK (0x2000U)
#define GPTMRV2_SR_CH3CAPF_SHIFT (13U)
#define GPTMRV2_SR_CH3CAPF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH3CAPF_SHIFT) & GPTMRV2_SR_CH3CAPF_MASK)
#define GPTMRV2_SR_CH3CAPF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH3CAPF_MASK) >> GPTMRV2_SR_CH3CAPF_SHIFT)

/*
 * CH3RLDF (W1C)
 *
 * channel 3 counter reload flag
 */
#define GPTMRV2_SR_CH3RLDF_MASK (0x1000U)
#define GPTMRV2_SR_CH3RLDF_SHIFT (12U)
#define GPTMRV2_SR_CH3RLDF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH3RLDF_SHIFT) & GPTMRV2_SR_CH3RLDF_MASK)
#define GPTMRV2_SR_CH3RLDF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH3RLDF_MASK) >> GPTMRV2_SR_CH3RLDF_SHIFT)

/*
 * CH2CMP1F (W1C)
 *
 * channel 2 compare value 1 match flag
 */
#define GPTMRV2_SR_CH2CMP1F_MASK (0x800U)
#define GPTMRV2_SR_CH2CMP1F_SHIFT (11U)
#define GPTMRV2_SR_CH2CMP1F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH2CMP1F_SHIFT) & GPTMRV2_SR_CH2CMP1F_MASK)
#define GPTMRV2_SR_CH2CMP1F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH2CMP1F_MASK) >> GPTMRV2_SR_CH2CMP1F_SHIFT)

/*
 * CH2CMP0F (W1C)
 *
 * channel 2 compare value 1 match flag
 */
#define GPTMRV2_SR_CH2CMP0F_MASK (0x400U)
#define GPTMRV2_SR_CH2CMP0F_SHIFT (10U)
#define GPTMRV2_SR_CH2CMP0F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH2CMP0F_SHIFT) & GPTMRV2_SR_CH2CMP0F_MASK)
#define GPTMRV2_SR_CH2CMP0F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH2CMP0F_MASK) >> GPTMRV2_SR_CH2CMP0F_SHIFT)

/*
 * CH2CAPF (W1C)
 *
 * channel 2 capture flag, the flag will be set at the valid capture edge per CAPMODE setting.
 * If the capture channel is set to measure mode, the flag will be set at rising edge.
 */
#define GPTMRV2_SR_CH2CAPF_MASK (0x200U)
#define GPTMRV2_SR_CH2CAPF_SHIFT (9U)
#define GPTMRV2_SR_CH2CAPF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH2CAPF_SHIFT) & GPTMRV2_SR_CH2CAPF_MASK)
#define GPTMRV2_SR_CH2CAPF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH2CAPF_MASK) >> GPTMRV2_SR_CH2CAPF_SHIFT)

/*
 * CH2RLDF (W1C)
 *
 * channel 2 counter reload flag
 */
#define GPTMRV2_SR_CH2RLDF_MASK (0x100U)
#define GPTMRV2_SR_CH2RLDF_SHIFT (8U)
#define GPTMRV2_SR_CH2RLDF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH2RLDF_SHIFT) & GPTMRV2_SR_CH2RLDF_MASK)
#define GPTMRV2_SR_CH2RLDF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH2RLDF_MASK) >> GPTMRV2_SR_CH2RLDF_SHIFT)

/*
 * CH1CMP1F (W1C)
 *
 * channel 1 compare value 1 match flag
 */
#define GPTMRV2_SR_CH1CMP1F_MASK (0x80U)
#define GPTMRV2_SR_CH1CMP1F_SHIFT (7U)
#define GPTMRV2_SR_CH1CMP1F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH1CMP1F_SHIFT) & GPTMRV2_SR_CH1CMP1F_MASK)
#define GPTMRV2_SR_CH1CMP1F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH1CMP1F_MASK) >> GPTMRV2_SR_CH1CMP1F_SHIFT)

/*
 * CH1CMP0F (W1C)
 *
 * channel 1 compare value 1 match flag
 */
#define GPTMRV2_SR_CH1CMP0F_MASK (0x40U)
#define GPTMRV2_SR_CH1CMP0F_SHIFT (6U)
#define GPTMRV2_SR_CH1CMP0F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH1CMP0F_SHIFT) & GPTMRV2_SR_CH1CMP0F_MASK)
#define GPTMRV2_SR_CH1CMP0F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH1CMP0F_MASK) >> GPTMRV2_SR_CH1CMP0F_SHIFT)

/*
 * CH1CAPF (W1C)
 *
 * channel 1 capture flag, the flag will be set at the valid capture edge per CAPMODE setting.
 * If the capture channel is set to measure mode, the flag will be set at rising edge.
 */
#define GPTMRV2_SR_CH1CAPF_MASK (0x20U)
#define GPTMRV2_SR_CH1CAPF_SHIFT (5U)
#define GPTMRV2_SR_CH1CAPF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH1CAPF_SHIFT) & GPTMRV2_SR_CH1CAPF_MASK)
#define GPTMRV2_SR_CH1CAPF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH1CAPF_MASK) >> GPTMRV2_SR_CH1CAPF_SHIFT)

/*
 * CH1RLDF (W1C)
 *
 * channel 1 counter reload flag
 */
#define GPTMRV2_SR_CH1RLDF_MASK (0x10U)
#define GPTMRV2_SR_CH1RLDF_SHIFT (4U)
#define GPTMRV2_SR_CH1RLDF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH1RLDF_SHIFT) & GPTMRV2_SR_CH1RLDF_MASK)
#define GPTMRV2_SR_CH1RLDF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH1RLDF_MASK) >> GPTMRV2_SR_CH1RLDF_SHIFT)

/*
 * CH0CMP1F (W1C)
 *
 * channel 1 compare value 1 match flag
 */
#define GPTMRV2_SR_CH0CMP1F_MASK (0x8U)
#define GPTMRV2_SR_CH0CMP1F_SHIFT (3U)
#define GPTMRV2_SR_CH0CMP1F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH0CMP1F_SHIFT) & GPTMRV2_SR_CH0CMP1F_MASK)
#define GPTMRV2_SR_CH0CMP1F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH0CMP1F_MASK) >> GPTMRV2_SR_CH0CMP1F_SHIFT)

/*
 * CH0CMP0F (W1C)
 *
 * channel 1 compare value 1 match flag
 */
#define GPTMRV2_SR_CH0CMP0F_MASK (0x4U)
#define GPTMRV2_SR_CH0CMP0F_SHIFT (2U)
#define GPTMRV2_SR_CH0CMP0F_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH0CMP0F_SHIFT) & GPTMRV2_SR_CH0CMP0F_MASK)
#define GPTMRV2_SR_CH0CMP0F_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH0CMP0F_MASK) >> GPTMRV2_SR_CH0CMP0F_SHIFT)

/*
 * CH0CAPF (W1C)
 *
 * channel 1 capture flag, the flag will be set at the valid capture edge per CAPMODE setting.
 * If the capture channel is set to measure mode, the flag will be set at rising edge.
 */
#define GPTMRV2_SR_CH0CAPF_MASK (0x2U)
#define GPTMRV2_SR_CH0CAPF_SHIFT (1U)
#define GPTMRV2_SR_CH0CAPF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH0CAPF_SHIFT) & GPTMRV2_SR_CH0CAPF_MASK)
#define GPTMRV2_SR_CH0CAPF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH0CAPF_MASK) >> GPTMRV2_SR_CH0CAPF_SHIFT)

/*
 * CH0RLDF (W1C)
 *
 * channel 1 counter reload flag
 */
#define GPTMRV2_SR_CH0RLDF_MASK (0x1U)
#define GPTMRV2_SR_CH0RLDF_SHIFT (0U)
#define GPTMRV2_SR_CH0RLDF_SET(x) (((uint32_t)(x) << GPTMRV2_SR_CH0RLDF_SHIFT) & GPTMRV2_SR_CH0RLDF_MASK)
#define GPTMRV2_SR_CH0RLDF_GET(x) (((uint32_t)(x) & GPTMRV2_SR_CH0RLDF_MASK) >> GPTMRV2_SR_CH0RLDF_SHIFT)

/* Bitfield definition for register: IRQEN */
/*
 * CH3CMP1EN (RW)
 *
 * 1- generate interrupt request when ch3cmp1f flag is set
 */
#define GPTMRV2_IRQEN_CH3CMP1EN_MASK (0x8000U)
#define GPTMRV2_IRQEN_CH3CMP1EN_SHIFT (15U)
#define GPTMRV2_IRQEN_CH3CMP1EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH3CMP1EN_SHIFT) & GPTMRV2_IRQEN_CH3CMP1EN_MASK)
#define GPTMRV2_IRQEN_CH3CMP1EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH3CMP1EN_MASK) >> GPTMRV2_IRQEN_CH3CMP1EN_SHIFT)

/*
 * CH3CMP0EN (RW)
 *
 * 1- generate interrupt request when ch3cmp0f flag is set
 */
#define GPTMRV2_IRQEN_CH3CMP0EN_MASK (0x4000U)
#define GPTMRV2_IRQEN_CH3CMP0EN_SHIFT (14U)
#define GPTMRV2_IRQEN_CH3CMP0EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH3CMP0EN_SHIFT) & GPTMRV2_IRQEN_CH3CMP0EN_MASK)
#define GPTMRV2_IRQEN_CH3CMP0EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH3CMP0EN_MASK) >> GPTMRV2_IRQEN_CH3CMP0EN_SHIFT)

/*
 * CH3CAPEN (RW)
 *
 * 1- generate interrupt request when ch3capf flag is set
 */
#define GPTMRV2_IRQEN_CH3CAPEN_MASK (0x2000U)
#define GPTMRV2_IRQEN_CH3CAPEN_SHIFT (13U)
#define GPTMRV2_IRQEN_CH3CAPEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH3CAPEN_SHIFT) & GPTMRV2_IRQEN_CH3CAPEN_MASK)
#define GPTMRV2_IRQEN_CH3CAPEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH3CAPEN_MASK) >> GPTMRV2_IRQEN_CH3CAPEN_SHIFT)

/*
 * CH3RLDEN (RW)
 *
 * 1- generate interrupt request when ch3rldf flag is set
 */
#define GPTMRV2_IRQEN_CH3RLDEN_MASK (0x1000U)
#define GPTMRV2_IRQEN_CH3RLDEN_SHIFT (12U)
#define GPTMRV2_IRQEN_CH3RLDEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH3RLDEN_SHIFT) & GPTMRV2_IRQEN_CH3RLDEN_MASK)
#define GPTMRV2_IRQEN_CH3RLDEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH3RLDEN_MASK) >> GPTMRV2_IRQEN_CH3RLDEN_SHIFT)

/*
 * CH2CMP1EN (RW)
 *
 * 1- generate interrupt request when ch2cmp1f flag is set
 */
#define GPTMRV2_IRQEN_CH2CMP1EN_MASK (0x800U)
#define GPTMRV2_IRQEN_CH2CMP1EN_SHIFT (11U)
#define GPTMRV2_IRQEN_CH2CMP1EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH2CMP1EN_SHIFT) & GPTMRV2_IRQEN_CH2CMP1EN_MASK)
#define GPTMRV2_IRQEN_CH2CMP1EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH2CMP1EN_MASK) >> GPTMRV2_IRQEN_CH2CMP1EN_SHIFT)

/*
 * CH2CMP0EN (RW)
 *
 * 1- generate interrupt request when ch2cmp0f flag is set
 */
#define GPTMRV2_IRQEN_CH2CMP0EN_MASK (0x400U)
#define GPTMRV2_IRQEN_CH2CMP0EN_SHIFT (10U)
#define GPTMRV2_IRQEN_CH2CMP0EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH2CMP0EN_SHIFT) & GPTMRV2_IRQEN_CH2CMP0EN_MASK)
#define GPTMRV2_IRQEN_CH2CMP0EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH2CMP0EN_MASK) >> GPTMRV2_IRQEN_CH2CMP0EN_SHIFT)

/*
 * CH2CAPEN (RW)
 *
 * 1- generate interrupt request when ch2capf flag is set
 */
#define GPTMRV2_IRQEN_CH2CAPEN_MASK (0x200U)
#define GPTMRV2_IRQEN_CH2CAPEN_SHIFT (9U)
#define GPTMRV2_IRQEN_CH2CAPEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH2CAPEN_SHIFT) & GPTMRV2_IRQEN_CH2CAPEN_MASK)
#define GPTMRV2_IRQEN_CH2CAPEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH2CAPEN_MASK) >> GPTMRV2_IRQEN_CH2CAPEN_SHIFT)

/*
 * CH2RLDEN (RW)
 *
 * 1- generate interrupt request when ch2rldf flag is set
 */
#define GPTMRV2_IRQEN_CH2RLDEN_MASK (0x100U)
#define GPTMRV2_IRQEN_CH2RLDEN_SHIFT (8U)
#define GPTMRV2_IRQEN_CH2RLDEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH2RLDEN_SHIFT) & GPTMRV2_IRQEN_CH2RLDEN_MASK)
#define GPTMRV2_IRQEN_CH2RLDEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH2RLDEN_MASK) >> GPTMRV2_IRQEN_CH2RLDEN_SHIFT)

/*
 * CH1CMP1EN (RW)
 *
 * 1- generate interrupt request when ch1cmp1f flag is set
 */
#define GPTMRV2_IRQEN_CH1CMP1EN_MASK (0x80U)
#define GPTMRV2_IRQEN_CH1CMP1EN_SHIFT (7U)
#define GPTMRV2_IRQEN_CH1CMP1EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH1CMP1EN_SHIFT) & GPTMRV2_IRQEN_CH1CMP1EN_MASK)
#define GPTMRV2_IRQEN_CH1CMP1EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH1CMP1EN_MASK) >> GPTMRV2_IRQEN_CH1CMP1EN_SHIFT)

/*
 * CH1CMP0EN (RW)
 *
 * 1- generate interrupt request when ch1cmp0f flag is set
 */
#define GPTMRV2_IRQEN_CH1CMP0EN_MASK (0x40U)
#define GPTMRV2_IRQEN_CH1CMP0EN_SHIFT (6U)
#define GPTMRV2_IRQEN_CH1CMP0EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH1CMP0EN_SHIFT) & GPTMRV2_IRQEN_CH1CMP0EN_MASK)
#define GPTMRV2_IRQEN_CH1CMP0EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH1CMP0EN_MASK) >> GPTMRV2_IRQEN_CH1CMP0EN_SHIFT)

/*
 * CH1CAPEN (RW)
 *
 * 1- generate interrupt request when ch1capf flag is set
 */
#define GPTMRV2_IRQEN_CH1CAPEN_MASK (0x20U)
#define GPTMRV2_IRQEN_CH1CAPEN_SHIFT (5U)
#define GPTMRV2_IRQEN_CH1CAPEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH1CAPEN_SHIFT) & GPTMRV2_IRQEN_CH1CAPEN_MASK)
#define GPTMRV2_IRQEN_CH1CAPEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH1CAPEN_MASK) >> GPTMRV2_IRQEN_CH1CAPEN_SHIFT)

/*
 * CH1RLDEN (RW)
 *
 * 1- generate interrupt request when ch1rldf flag is set
 */
#define GPTMRV2_IRQEN_CH1RLDEN_MASK (0x10U)
#define GPTMRV2_IRQEN_CH1RLDEN_SHIFT (4U)
#define GPTMRV2_IRQEN_CH1RLDEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH1RLDEN_SHIFT) & GPTMRV2_IRQEN_CH1RLDEN_MASK)
#define GPTMRV2_IRQEN_CH1RLDEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH1RLDEN_MASK) >> GPTMRV2_IRQEN_CH1RLDEN_SHIFT)

/*
 * CH0CMP1EN (RW)
 *
 * 1- generate interrupt request when ch0cmp1f flag is set
 */
#define GPTMRV2_IRQEN_CH0CMP1EN_MASK (0x8U)
#define GPTMRV2_IRQEN_CH0CMP1EN_SHIFT (3U)
#define GPTMRV2_IRQEN_CH0CMP1EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH0CMP1EN_SHIFT) & GPTMRV2_IRQEN_CH0CMP1EN_MASK)
#define GPTMRV2_IRQEN_CH0CMP1EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH0CMP1EN_MASK) >> GPTMRV2_IRQEN_CH0CMP1EN_SHIFT)

/*
 * CH0CMP0EN (RW)
 *
 * 1- generate interrupt request when ch0cmp0f flag is set
 */
#define GPTMRV2_IRQEN_CH0CMP0EN_MASK (0x4U)
#define GPTMRV2_IRQEN_CH0CMP0EN_SHIFT (2U)
#define GPTMRV2_IRQEN_CH0CMP0EN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH0CMP0EN_SHIFT) & GPTMRV2_IRQEN_CH0CMP0EN_MASK)
#define GPTMRV2_IRQEN_CH0CMP0EN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH0CMP0EN_MASK) >> GPTMRV2_IRQEN_CH0CMP0EN_SHIFT)

/*
 * CH0CAPEN (RW)
 *
 * 1- generate interrupt request when ch0capf flag is set
 */
#define GPTMRV2_IRQEN_CH0CAPEN_MASK (0x2U)
#define GPTMRV2_IRQEN_CH0CAPEN_SHIFT (1U)
#define GPTMRV2_IRQEN_CH0CAPEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH0CAPEN_SHIFT) & GPTMRV2_IRQEN_CH0CAPEN_MASK)
#define GPTMRV2_IRQEN_CH0CAPEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH0CAPEN_MASK) >> GPTMRV2_IRQEN_CH0CAPEN_SHIFT)

/*
 * CH0RLDEN (RW)
 *
 * 1- generate interrupt request when ch0rldf flag is set
 */
#define GPTMRV2_IRQEN_CH0RLDEN_MASK (0x1U)
#define GPTMRV2_IRQEN_CH0RLDEN_SHIFT (0U)
#define GPTMRV2_IRQEN_CH0RLDEN_SET(x) (((uint32_t)(x) << GPTMRV2_IRQEN_CH0RLDEN_SHIFT) & GPTMRV2_IRQEN_CH0RLDEN_MASK)
#define GPTMRV2_IRQEN_CH0RLDEN_GET(x) (((uint32_t)(x) & GPTMRV2_IRQEN_CH0RLDEN_MASK) >> GPTMRV2_IRQEN_CH0RLDEN_SHIFT)

/* Bitfield definition for register: GCR */
/*
 * SWSYNCT (RW1C)
 *
 * set this bitfield to trigger software counter sync event
 */
#define GPTMRV2_GCR_SWSYNCT_MASK (0xFU)
#define GPTMRV2_GCR_SWSYNCT_SHIFT (0U)
#define GPTMRV2_GCR_SWSYNCT_SET(x) (((uint32_t)(x) << GPTMRV2_GCR_SWSYNCT_SHIFT) & GPTMRV2_GCR_SWSYNCT_MASK)
#define GPTMRV2_GCR_SWSYNCT_GET(x) (((uint32_t)(x) & GPTMRV2_GCR_SWSYNCT_MASK) >> GPTMRV2_GCR_SWSYNCT_SHIFT)



/* CMP register group index macro definition */
#define GPTMRV2_CHANNEL_CMP_CMP0 (0UL)
#define GPTMRV2_CHANNEL_CMP_CMP1 (1UL)

/* CHANNEL register group index macro definition */
#define GPTMRV2_CHANNEL_CH0 (0UL)
#define GPTMRV2_CHANNEL_CH1 (1UL)
#define GPTMRV2_CHANNEL_CH2 (2UL)
#define GPTMRV2_CHANNEL_CH3 (3UL)


#endif /* HPM_GPTMRV2_H */
