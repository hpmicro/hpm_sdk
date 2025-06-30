/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_OWR_H
#define HPM_OWR_H

typedef struct {
    __RW uint32_t CTRL;                        /* 0x0: Control Register */
    __RW uint32_t DIV_CFG;                     /* 0x4:  */
    __RW uint32_t RESET;                       /* 0x8:  */
    __RW uint32_t CMD;                         /* 0xC:  */
    __RW uint32_t DATA;                        /* 0x10:  */
    __R  uint8_t  RESERVED0[64];               /* 0x14 - 0x53: Reserved */
    __RW uint32_t RX_CNT;                      /* 0x54:  */
    __RW uint32_t GLB_CFG;                     /* 0x58:  */
    __R  uint8_t  RESERVED1[4];                /* 0x5C - 0x5F: Reserved */
    __RW uint32_t DMA_EN;                      /* 0x60:  */
    __R  uint8_t  RESERVED2[12];               /* 0x64 - 0x6F: Reserved */
    __W  uint32_t IRQ_STS;                     /* 0x70:  */
    __RW uint32_t IRQ_EN;                      /* 0x74:  */
} OWR_Type;


/* Bitfield definition for register: CTRL */
/*
 * RPPBIT (RW)
 *
 * Reset/Presence-detect Pulse. This bit is self-clearing and is cleared after the presence or absence of an
 * external device is determined.
 * writing 1 generate reset pulse and sample the bus for the presence pulse from the external device.
 * When reading:
 * 0 Reset pulse complete.
 * 1 Sequence not complete.
 */
#define OWR_CTRL_RPPBIT_MASK (0x80U)
#define OWR_CTRL_RPPBIT_SHIFT (7U)
#define OWR_CTRL_RPPBIT_SET(x) (((uint32_t)(x) << OWR_CTRL_RPPBIT_SHIFT) & OWR_CTRL_RPPBIT_MASK)
#define OWR_CTRL_RPPBIT_GET(x) (((uint32_t)(x) & OWR_CTRL_RPPBIT_MASK) >> OWR_CTRL_RPPBIT_SHIFT)

/*
 * PSTBIT (RO)
 *
 * Presence status bit,this bit is valid after the RPP bit is self-cleared.
 * 0 Device is not present.
 * 1 Device is present
 */
#define OWR_CTRL_PSTBIT_MASK (0x40U)
#define OWR_CTRL_PSTBIT_SHIFT (6U)
#define OWR_CTRL_PSTBIT_GET(x) (((uint32_t)(x) & OWR_CTRL_PSTBIT_MASK) >> OWR_CTRL_PSTBIT_SHIFT)

/* Bitfield definition for register: DIV_CFG */
/*
 * DIV_CFG (RW)
 *
 * for 1us clock divider;
 * 2; div3, 3: div4;   …    255: div256
 * 0 and 1 are un supported configuation
 */
#define OWR_DIV_CFG_DIV_CFG_MASK (0xFFU)
#define OWR_DIV_CFG_DIV_CFG_SHIFT (0U)
#define OWR_DIV_CFG_DIV_CFG_SET(x) (((uint32_t)(x) << OWR_DIV_CFG_DIV_CFG_SHIFT) & OWR_DIV_CFG_DIV_CFG_MASK)
#define OWR_DIV_CFG_DIV_CFG_GET(x) (((uint32_t)(x) & OWR_DIV_CFG_DIV_CFG_MASK) >> OWR_DIV_CFG_DIV_CFG_SHIFT)

/* Bitfield definition for register: RESET */
/*
 * SW_RESET (RW)
 *
 * set to reset register 0x00, 0x04,0x10,0x14, 0x18 and related logic.
 * Clr to exit software reset
 */
#define OWR_RESET_SW_RESET_MASK (0x1U)
#define OWR_RESET_SW_RESET_SHIFT (0U)
#define OWR_RESET_SW_RESET_SET(x) (((uint32_t)(x) << OWR_RESET_SW_RESET_SHIFT) & OWR_RESET_SW_RESET_MASK)
#define OWR_RESET_SW_RESET_GET(x) (((uint32_t)(x) & OWR_RESET_SW_RESET_MASK) >> OWR_RESET_SW_RESET_SHIFT)

/* Bitfield definition for register: CMD */
/*
 * CMD (RW)
 *
 * Search ROM Accelerator(srabit). This bit is cleared when the reset-presence-pulse bit rppbit is set.
 * 0 Deactivate the search ROM accelerator.
 * 1 Switch to search ROM accelerator mode.
 */
#define OWR_CMD_CMD_MASK (0x2U)
#define OWR_CMD_CMD_SHIFT (1U)
#define OWR_CMD_CMD_SET(x) (((uint32_t)(x) << OWR_CMD_CMD_SHIFT) & OWR_CMD_CMD_MASK)
#define OWR_CMD_CMD_GET(x) (((uint32_t)(x) & OWR_CMD_CMD_MASK) >> OWR_CMD_CMD_SHIFT)

/* Bitfield definition for register: DATA */
/*
 * TXRX_DATA (RW)
 *
 * When writing:
 * The data byte is written to the transmit buffer.
 * When reading:
 * A data byte is read from the receive buffer. The data is valid only when irq_sts.rbf is set.
 */
#define OWR_DATA_TXRX_DATA_MASK (0xFFU)
#define OWR_DATA_TXRX_DATA_SHIFT (0U)
#define OWR_DATA_TXRX_DATA_SET(x) (((uint32_t)(x) << OWR_DATA_TXRX_DATA_SHIFT) & OWR_DATA_TXRX_DATA_MASK)
#define OWR_DATA_TXRX_DATA_GET(x) (((uint32_t)(x) & OWR_DATA_TXRX_DATA_MASK) >> OWR_DATA_TXRX_DATA_SHIFT)

/* Bitfield definition for register: RX_CNT */
/*
 * RX_CNT (RW)
 *
 * define RX byte number.
 * After one byte is received and read, if value is not one, hardware will trigger new read process
 * (write 0xFF to txrx_data, after txrx_data is read, or negedge rbf).  It's used together with dma for multiple receive bytes
 * NOTE, In SRA mode,  must set rx_cnt to 1. txdma will send data to start receive process.
 */
#define OWR_RX_CNT_RX_CNT_MASK (0xFFU)
#define OWR_RX_CNT_RX_CNT_SHIFT (0U)
#define OWR_RX_CNT_RX_CNT_SET(x) (((uint32_t)(x) << OWR_RX_CNT_RX_CNT_SHIFT) & OWR_RX_CNT_RX_CNT_MASK)
#define OWR_RX_CNT_RX_CNT_GET(x) (((uint32_t)(x) & OWR_RX_CNT_RX_CNT_MASK) >> OWR_RX_CNT_RX_CNT_SHIFT)

/* Bitfield definition for register: GLB_CFG */
/*
 * FORCE_CLK_ON (RW)
 *
 * force enable 1us clock
 */
#define OWR_GLB_CFG_FORCE_CLK_ON_MASK (0x2U)
#define OWR_GLB_CFG_FORCE_CLK_ON_SHIFT (1U)
#define OWR_GLB_CFG_FORCE_CLK_ON_SET(x) (((uint32_t)(x) << OWR_GLB_CFG_FORCE_CLK_ON_SHIFT) & OWR_GLB_CFG_FORCE_CLK_ON_MASK)
#define OWR_GLB_CFG_FORCE_CLK_ON_GET(x) (((uint32_t)(x) & OWR_GLB_CFG_FORCE_CLK_ON_MASK) >> OWR_GLB_CFG_FORCE_CLK_ON_SHIFT)

/* Bitfield definition for register: DMA_EN */
/*
 * TX_DMA_EN (RW)
 *
 */
#define OWR_DMA_EN_TX_DMA_EN_MASK (0x2U)
#define OWR_DMA_EN_TX_DMA_EN_SHIFT (1U)
#define OWR_DMA_EN_TX_DMA_EN_SET(x) (((uint32_t)(x) << OWR_DMA_EN_TX_DMA_EN_SHIFT) & OWR_DMA_EN_TX_DMA_EN_MASK)
#define OWR_DMA_EN_TX_DMA_EN_GET(x) (((uint32_t)(x) & OWR_DMA_EN_TX_DMA_EN_MASK) >> OWR_DMA_EN_TX_DMA_EN_SHIFT)

/*
 * RX_DMA_EN (RW)
 *
 */
#define OWR_DMA_EN_RX_DMA_EN_MASK (0x1U)
#define OWR_DMA_EN_RX_DMA_EN_SHIFT (0U)
#define OWR_DMA_EN_RX_DMA_EN_SET(x) (((uint32_t)(x) << OWR_DMA_EN_RX_DMA_EN_SHIFT) & OWR_DMA_EN_RX_DMA_EN_MASK)
#define OWR_DMA_EN_RX_DMA_EN_GET(x) (((uint32_t)(x) & OWR_DMA_EN_RX_DMA_EN_MASK) >> OWR_DMA_EN_RX_DMA_EN_SHIFT)

/* Bitfield definition for register: IRQ_STS */
/*
 * RSRF (W1C)
 *
 * Receive Shift Register Full.
 */
#define OWR_IRQ_STS_RSRF_MASK (0x20U)
#define OWR_IRQ_STS_RSRF_SHIFT (5U)
#define OWR_IRQ_STS_RSRF_SET(x) (((uint32_t)(x) << OWR_IRQ_STS_RSRF_SHIFT) & OWR_IRQ_STS_RSRF_MASK)
#define OWR_IRQ_STS_RSRF_GET(x) (((uint32_t)(x) & OWR_IRQ_STS_RSRF_MASK) >> OWR_IRQ_STS_RSRF_SHIFT)

/*
 * RBF (W1C)
 *
 * Receive Buffer Full. This flag prevents new data from being shifted into the receive buffer from the receive shift register.
 * 0 No new data
 * 1 A byte is waiting to be read from the TX/RX register
 */
#define OWR_IRQ_STS_RBF_MASK (0x10U)
#define OWR_IRQ_STS_RBF_SHIFT (4U)
#define OWR_IRQ_STS_RBF_SET(x) (((uint32_t)(x) << OWR_IRQ_STS_RBF_SHIFT) & OWR_IRQ_STS_RBF_MASK)
#define OWR_IRQ_STS_RBF_GET(x) (((uint32_t)(x) & OWR_IRQ_STS_RBF_MASK) >> OWR_IRQ_STS_RBF_SHIFT)

/*
 * TEMT (W1C)
 *
 * Transmit Shift Register Empty.
 */
#define OWR_IRQ_STS_TEMT_MASK (0x8U)
#define OWR_IRQ_STS_TEMT_SHIFT (3U)
#define OWR_IRQ_STS_TEMT_SET(x) (((uint32_t)(x) << OWR_IRQ_STS_TEMT_SHIFT) & OWR_IRQ_STS_TEMT_MASK)
#define OWR_IRQ_STS_TEMT_GET(x) (((uint32_t)(x) & OWR_IRQ_STS_TEMT_MASK) >> OWR_IRQ_STS_TEMT_SHIFT)

/*
 * TBE (W1C)
 *
 * Transmit Buffer Empty.
 */
#define OWR_IRQ_STS_TBE_MASK (0x4U)
#define OWR_IRQ_STS_TBE_SHIFT (2U)
#define OWR_IRQ_STS_TBE_SET(x) (((uint32_t)(x) << OWR_IRQ_STS_TBE_SHIFT) & OWR_IRQ_STS_TBE_MASK)
#define OWR_IRQ_STS_TBE_GET(x) (((uint32_t)(x) & OWR_IRQ_STS_TBE_MASK) >> OWR_IRQ_STS_TBE_SHIFT)

/*
 * PST_DET (W1C)
 *
 * Presence Detect. After an 1-Wire reset has been issued, this flag is set after the appropriate amount of time(tpst) for a presence detect pulse to have occurred.
 */
#define OWR_IRQ_STS_PST_DET_MASK (0x1U)
#define OWR_IRQ_STS_PST_DET_SHIFT (0U)
#define OWR_IRQ_STS_PST_DET_SET(x) (((uint32_t)(x) << OWR_IRQ_STS_PST_DET_SHIFT) & OWR_IRQ_STS_PST_DET_MASK)
#define OWR_IRQ_STS_PST_DET_GET(x) (((uint32_t)(x) & OWR_IRQ_STS_PST_DET_MASK) >> OWR_IRQ_STS_PST_DET_SHIFT)

/* Bitfield definition for register: IRQ_EN */
/*
 * ERSF (RW)
 *
 */
#define OWR_IRQ_EN_ERSF_MASK (0x20U)
#define OWR_IRQ_EN_ERSF_SHIFT (5U)
#define OWR_IRQ_EN_ERSF_SET(x) (((uint32_t)(x) << OWR_IRQ_EN_ERSF_SHIFT) & OWR_IRQ_EN_ERSF_MASK)
#define OWR_IRQ_EN_ERSF_GET(x) (((uint32_t)(x) & OWR_IRQ_EN_ERSF_MASK) >> OWR_IRQ_EN_ERSF_SHIFT)

/*
 * ERBF (RW)
 *
 */
#define OWR_IRQ_EN_ERBF_MASK (0x10U)
#define OWR_IRQ_EN_ERBF_SHIFT (4U)
#define OWR_IRQ_EN_ERBF_SET(x) (((uint32_t)(x) << OWR_IRQ_EN_ERBF_SHIFT) & OWR_IRQ_EN_ERBF_MASK)
#define OWR_IRQ_EN_ERBF_GET(x) (((uint32_t)(x) & OWR_IRQ_EN_ERBF_MASK) >> OWR_IRQ_EN_ERBF_SHIFT)

/*
 * ETMT (RW)
 *
 */
#define OWR_IRQ_EN_ETMT_MASK (0x8U)
#define OWR_IRQ_EN_ETMT_SHIFT (3U)
#define OWR_IRQ_EN_ETMT_SET(x) (((uint32_t)(x) << OWR_IRQ_EN_ETMT_SHIFT) & OWR_IRQ_EN_ETMT_MASK)
#define OWR_IRQ_EN_ETMT_GET(x) (((uint32_t)(x) & OWR_IRQ_EN_ETMT_MASK) >> OWR_IRQ_EN_ETMT_SHIFT)

/*
 * ETBE (RW)
 *
 */
#define OWR_IRQ_EN_ETBE_MASK (0x4U)
#define OWR_IRQ_EN_ETBE_SHIFT (2U)
#define OWR_IRQ_EN_ETBE_SET(x) (((uint32_t)(x) << OWR_IRQ_EN_ETBE_SHIFT) & OWR_IRQ_EN_ETBE_MASK)
#define OWR_IRQ_EN_ETBE_GET(x) (((uint32_t)(x) & OWR_IRQ_EN_ETBE_MASK) >> OWR_IRQ_EN_ETBE_SHIFT)

/*
 * EPD (RW)
 *
 */
#define OWR_IRQ_EN_EPD_MASK (0x1U)
#define OWR_IRQ_EN_EPD_SHIFT (0U)
#define OWR_IRQ_EN_EPD_SET(x) (((uint32_t)(x) << OWR_IRQ_EN_EPD_SHIFT) & OWR_IRQ_EN_EPD_MASK)
#define OWR_IRQ_EN_EPD_GET(x) (((uint32_t)(x) & OWR_IRQ_EN_EPD_MASK) >> OWR_IRQ_EN_EPD_SHIFT)




#endif /* HPM_OWR_H */
