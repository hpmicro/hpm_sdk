/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_PDMLITE_H
#define HPM_PDMLITE_H

typedef struct {
    __RW uint32_t CTRL;                        /* 0x0: Control Register */
    __RW uint32_t CH_CTRL;                     /* 0x4: Channel Control Register */
    __W  uint32_t ST;                          /* 0x8: Status Register */
    __R  uint8_t  RESERVED0[4];                /* 0xC - 0xF: Reserved */
    __RW uint32_t CIC_CFG;                     /* 0x10: CIC configuration register */
    __R  uint8_t  RESERVED1[12];               /* 0x14 - 0x1F: Reserved */
    __RW uint32_t RUN;                         /* 0x20: Run Register */
} PDMLITE_Type;


/* Bitfield definition for register: CTRL */
/*
 * SFTRST (RW)
 *
 * software reset the module. Self-clear.
 */
#define PDMLITE_CTRL_SFTRST_MASK (0x80000000UL)
#define PDMLITE_CTRL_SFTRST_SHIFT (31U)
#define PDMLITE_CTRL_SFTRST_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_SFTRST_SHIFT) & PDMLITE_CTRL_SFTRST_MASK)
#define PDMLITE_CTRL_SFTRST_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_SFTRST_MASK) >> PDMLITE_CTRL_SFTRST_SHIFT)

/*
 * SOF_FEDGE (RW)
 *
 * asserted if the falling edge of the ref fclk from DAO is the start of a new frame. This is used to to align DAO feedback signal.
 */
#define PDMLITE_CTRL_SOF_FEDGE_MASK (0x800000UL)
#define PDMLITE_CTRL_SOF_FEDGE_SHIFT (23U)
#define PDMLITE_CTRL_SOF_FEDGE_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_SOF_FEDGE_SHIFT) & PDMLITE_CTRL_SOF_FEDGE_MASK)
#define PDMLITE_CTRL_SOF_FEDGE_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_SOF_FEDGE_MASK) >> PDMLITE_CTRL_SOF_FEDGE_SHIFT)

/*
 * OFIFO_OVFL_ERR_IE (RW)
 *
 * output fifo overflow error interrupt enable
 */
#define PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_MASK (0x40000UL)
#define PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_SHIFT (18U)
#define PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_SHIFT) & PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_MASK)
#define PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_MASK) >> PDMLITE_CTRL_OFIFO_OVFL_ERR_IE_SHIFT)

/*
 * CIC_OVLD_ERR_IE (RW)
 *
 * CIC overload error interrupt enable
 */
#define PDMLITE_CTRL_CIC_OVLD_ERR_IE_MASK (0x20000UL)
#define PDMLITE_CTRL_CIC_OVLD_ERR_IE_SHIFT (17U)
#define PDMLITE_CTRL_CIC_OVLD_ERR_IE_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_CIC_OVLD_ERR_IE_SHIFT) & PDMLITE_CTRL_CIC_OVLD_ERR_IE_MASK)
#define PDMLITE_CTRL_CIC_OVLD_ERR_IE_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_CIC_OVLD_ERR_IE_MASK) >> PDMLITE_CTRL_CIC_OVLD_ERR_IE_SHIFT)

/*
 * CIC_SAT_ERR_IE (RW)
 *
 * Error interrupt enable
 * This bit controls the generation of an interrupt when an error condition  (CIC saturation) occurs.
 * 0: Error interrupt is masked
 * 1: Error interrupt is enabled
 */
#define PDMLITE_CTRL_CIC_SAT_ERR_IE_MASK (0x10000UL)
#define PDMLITE_CTRL_CIC_SAT_ERR_IE_SHIFT (16U)
#define PDMLITE_CTRL_CIC_SAT_ERR_IE_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_CIC_SAT_ERR_IE_SHIFT) & PDMLITE_CTRL_CIC_SAT_ERR_IE_MASK)
#define PDMLITE_CTRL_CIC_SAT_ERR_IE_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_CIC_SAT_ERR_IE_MASK) >> PDMLITE_CTRL_CIC_SAT_ERR_IE_SHIFT)

/*
 * CAPT_DLY (RW)
 *
 * Capture cycle delay>=0, should be less than PDM_CLK_HFDIV
 */
#define PDMLITE_CTRL_CAPT_DLY_MASK (0x780U)
#define PDMLITE_CTRL_CAPT_DLY_SHIFT (7U)
#define PDMLITE_CTRL_CAPT_DLY_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_CAPT_DLY_SHIFT) & PDMLITE_CTRL_CAPT_DLY_MASK)
#define PDMLITE_CTRL_CAPT_DLY_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_CAPT_DLY_MASK) >> PDMLITE_CTRL_CAPT_DLY_SHIFT)

/*
 * PDM_CLK_HFDIV (RW)
 *
 * The clock divider will work at least 4.
 * 0: div-by-2,
 * 1: div-by-4
 * . . .
 * n: div-by-2*(n+1)
 */
#define PDMLITE_CTRL_PDM_CLK_HFDIV_MASK (0x78U)
#define PDMLITE_CTRL_PDM_CLK_HFDIV_SHIFT (3U)
#define PDMLITE_CTRL_PDM_CLK_HFDIV_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_PDM_CLK_HFDIV_SHIFT) & PDMLITE_CTRL_PDM_CLK_HFDIV_MASK)
#define PDMLITE_CTRL_PDM_CLK_HFDIV_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_PDM_CLK_HFDIV_MASK) >> PDMLITE_CTRL_PDM_CLK_HFDIV_SHIFT)

/*
 * PDM_CLK_DIV_BYPASS (RW)
 *
 * asserted to bypass the pdm clock divider
 */
#define PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_MASK (0x4U)
#define PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SHIFT (2U)
#define PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SHIFT) & PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_MASK)
#define PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_MASK) >> PDMLITE_CTRL_PDM_CLK_DIV_BYPASS_SHIFT)

/*
 * PDM_CLK_OE (RW)
 *
 * pdm_clk_output_en
 */
#define PDMLITE_CTRL_PDM_CLK_OE_MASK (0x2U)
#define PDMLITE_CTRL_PDM_CLK_OE_SHIFT (1U)
#define PDMLITE_CTRL_PDM_CLK_OE_SET(x) (((uint32_t)(x) << PDMLITE_CTRL_PDM_CLK_OE_SHIFT) & PDMLITE_CTRL_PDM_CLK_OE_MASK)
#define PDMLITE_CTRL_PDM_CLK_OE_GET(x) (((uint32_t)(x) & PDMLITE_CTRL_PDM_CLK_OE_MASK) >> PDMLITE_CTRL_PDM_CLK_OE_SHIFT)

/* Bitfield definition for register: CH_CTRL */
/*
 * CH_POL (RW)
 *
 * Asserted to select PDM_CLK high level captured, otherwise to select PDM_CLK low level captured.
 */
#define PDMLITE_CH_CTRL_CH_POL_MASK (0xFF0000UL)
#define PDMLITE_CH_CTRL_CH_POL_SHIFT (16U)
#define PDMLITE_CH_CTRL_CH_POL_SET(x) (((uint32_t)(x) << PDMLITE_CH_CTRL_CH_POL_SHIFT) & PDMLITE_CH_CTRL_CH_POL_MASK)
#define PDMLITE_CH_CTRL_CH_POL_GET(x) (((uint32_t)(x) & PDMLITE_CH_CTRL_CH_POL_MASK) >> PDMLITE_CH_CTRL_CH_POL_SHIFT)

/*
 * CH_EN (RW)
 *
 * Asserted to enable the channel.
 * Ch8 & 9 are refs.
 * Ch0-7 are pdm mics.
 */
#define PDMLITE_CH_CTRL_CH_EN_MASK (0x3FFU)
#define PDMLITE_CH_CTRL_CH_EN_SHIFT (0U)
#define PDMLITE_CH_CTRL_CH_EN_SET(x) (((uint32_t)(x) << PDMLITE_CH_CTRL_CH_EN_SHIFT) & PDMLITE_CH_CTRL_CH_EN_MASK)
#define PDMLITE_CH_CTRL_CH_EN_GET(x) (((uint32_t)(x) & PDMLITE_CH_CTRL_CH_EN_MASK) >> PDMLITE_CH_CTRL_CH_EN_SHIFT)

/* Bitfield definition for register: ST */
/*
 * OFIFO_OVFL_ERR (W1C)
 *
 * output fifo overflow error. The reason may be sampling frequency mismatch, either fast or slow.
 */
#define PDMLITE_ST_OFIFO_OVFL_ERR_MASK (0x4U)
#define PDMLITE_ST_OFIFO_OVFL_ERR_SHIFT (2U)
#define PDMLITE_ST_OFIFO_OVFL_ERR_SET(x) (((uint32_t)(x) << PDMLITE_ST_OFIFO_OVFL_ERR_SHIFT) & PDMLITE_ST_OFIFO_OVFL_ERR_MASK)
#define PDMLITE_ST_OFIFO_OVFL_ERR_GET(x) (((uint32_t)(x) & PDMLITE_ST_OFIFO_OVFL_ERR_MASK) >> PDMLITE_ST_OFIFO_OVFL_ERR_SHIFT)

/*
 * CIC_OVLD_ERR (W1C)
 *
 * CIC overload error. write 1 clear
 */
#define PDMLITE_ST_CIC_OVLD_ERR_MASK (0x2U)
#define PDMLITE_ST_CIC_OVLD_ERR_SHIFT (1U)
#define PDMLITE_ST_CIC_OVLD_ERR_SET(x) (((uint32_t)(x) << PDMLITE_ST_CIC_OVLD_ERR_SHIFT) & PDMLITE_ST_CIC_OVLD_ERR_MASK)
#define PDMLITE_ST_CIC_OVLD_ERR_GET(x) (((uint32_t)(x) & PDMLITE_ST_CIC_OVLD_ERR_MASK) >> PDMLITE_ST_CIC_OVLD_ERR_SHIFT)

/*
 * CIC_SAT_ERR (W1C)
 *
 * CIC saturation. Write 1 clear
 */
#define PDMLITE_ST_CIC_SAT_ERR_MASK (0x1U)
#define PDMLITE_ST_CIC_SAT_ERR_SHIFT (0U)
#define PDMLITE_ST_CIC_SAT_ERR_SET(x) (((uint32_t)(x) << PDMLITE_ST_CIC_SAT_ERR_SHIFT) & PDMLITE_ST_CIC_SAT_ERR_MASK)
#define PDMLITE_ST_CIC_SAT_ERR_GET(x) (((uint32_t)(x) & PDMLITE_ST_CIC_SAT_ERR_MASK) >> PDMLITE_ST_CIC_SAT_ERR_SHIFT)

/* Bitfield definition for register: CIC_CFG */
/*
 * POST_SCALE (RW)
 *
 * the shift value after CIC results.
 */
#define PDMLITE_CIC_CFG_POST_SCALE_MASK (0xFC00U)
#define PDMLITE_CIC_CFG_POST_SCALE_SHIFT (10U)
#define PDMLITE_CIC_CFG_POST_SCALE_SET(x) (((uint32_t)(x) << PDMLITE_CIC_CFG_POST_SCALE_SHIFT) & PDMLITE_CIC_CFG_POST_SCALE_MASK)
#define PDMLITE_CIC_CFG_POST_SCALE_GET(x) (((uint32_t)(x) & PDMLITE_CIC_CFG_POST_SCALE_MASK) >> PDMLITE_CIC_CFG_POST_SCALE_SHIFT)

/*
 * SGD (RW)
 *
 * Sigma_delta_order[1:0]
 * 2'b00: 7
 * 2'b01: 6
 * 2'b10: 5
 * Others: unused
 */
#define PDMLITE_CIC_CFG_SGD_MASK (0x300U)
#define PDMLITE_CIC_CFG_SGD_SHIFT (8U)
#define PDMLITE_CIC_CFG_SGD_SET(x) (((uint32_t)(x) << PDMLITE_CIC_CFG_SGD_SHIFT) & PDMLITE_CIC_CFG_SGD_MASK)
#define PDMLITE_CIC_CFG_SGD_GET(x) (((uint32_t)(x) & PDMLITE_CIC_CFG_SGD_MASK) >> PDMLITE_CIC_CFG_SGD_SHIFT)

/*
 * CIC_DEC_RATIO (RW)
 *
 * CIC decimation factor
 */
#define PDMLITE_CIC_CFG_CIC_DEC_RATIO_MASK (0xFFU)
#define PDMLITE_CIC_CFG_CIC_DEC_RATIO_SHIFT (0U)
#define PDMLITE_CIC_CFG_CIC_DEC_RATIO_SET(x) (((uint32_t)(x) << PDMLITE_CIC_CFG_CIC_DEC_RATIO_SHIFT) & PDMLITE_CIC_CFG_CIC_DEC_RATIO_MASK)
#define PDMLITE_CIC_CFG_CIC_DEC_RATIO_GET(x) (((uint32_t)(x) & PDMLITE_CIC_CFG_CIC_DEC_RATIO_MASK) >> PDMLITE_CIC_CFG_CIC_DEC_RATIO_SHIFT)

/* Bitfield definition for register: RUN */
/*
 * PDM_EN (RW)
 *
 * Asserted to enable the module
 */
#define PDMLITE_RUN_PDM_EN_MASK (0x1U)
#define PDMLITE_RUN_PDM_EN_SHIFT (0U)
#define PDMLITE_RUN_PDM_EN_SET(x) (((uint32_t)(x) << PDMLITE_RUN_PDM_EN_SHIFT) & PDMLITE_RUN_PDM_EN_MASK)
#define PDMLITE_RUN_PDM_EN_GET(x) (((uint32_t)(x) & PDMLITE_RUN_PDM_EN_MASK) >> PDMLITE_RUN_PDM_EN_SHIFT)




#endif /* HPM_PDMLITE_H */
