/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_DHC_DRV_H
#define HPM_DHC_DRV_H

#include "hpm_common.h"
#include "hpm_soc_feature.h"
#include "hpm_dhc_regs.h"

/**
 * @file hpm_dhc_drv.h
 * @brief DHC driver APIs
 *
 * The DHC is a flexible serial data transfer peripheral.
 * It supports configurable data line count, clock polarity and phase, chip select
 * control, DMA transfers, and burst transactions with programmable burst length or continuous mode.
 *
 * @defgroup dhc_interface DHC driver APIs
 * @ingroup dhc_interfaces
 * @{
 */

/**
 * @brief DHC data format select
 *
 * Defines the data width per transfer unit on the data lines.
 * @note Value 2 is reserved and must not be used.
 */
typedef enum {
    dhc_data_1_byte = 0, /**< 1-byte (8-bit) data format */
    dhc_data_2_byte = 1, /**< 2-byte (16-bit) data format */
    dhc_data_4_byte = 3, /**< 4-byte (32-bit) data format */
} dhc_data_format_t;

/**
 * @brief DHC interrupt mask
 *
 * Selects which interrupt sources to enable or check.
 */
typedef enum {
    dhc_irq_wakeup = DHC_IRQ_EN_WAKEUP_EN_MASK,   /**< CS wakeup detection interrupt */
    dhc_irq_trans_end = DHC_IRQ_EN_TRANS_END_EN_MASK, /**< Transfer end interrupt */
} dhc_irq_mask_t;

/**
 * @brief DHC data line configuration structure
 *
 * The hardware defaults to big endian (MSB first per byte) for both TX and RX.
 * Set tx_msb_first / rx_msb_first to false for little endian (bit 0 first per byte).
 */
typedef struct {
    uint32_t data_line_en;      /**< Data line enable mask: bit0 for line0, bit1 for line1, etc. */
    uint32_t data_dir_output;   /**< Data direction mask: bit0 for line0, bit1 for line1, etc. 1 = output, 0 = input */
    uint32_t data_invert;       /**< Data polarity invert mask: bit0 for line0, bit1 for line1, etc. */
    bool tx_msb_first;          /**< TX data bit order per byte: true = MSB first (big endian, hardware default), false = LSB first */
    bool rx_msb_first;          /**< RX data bit order per byte: true = MSB first (big endian, hardware default), false = LSB first */
} dhc_data_config_t;

/**
 * @brief DHC DMA configuration structure
 *
 */
typedef struct {
    bool tx_dma_en;                 /**< Enable TX DMA transfer */
    bool rx_dma_en;                 /**< Enable RX DMA transfer */
    dhc_data_format_t tx_data_format; /**< TX data width per transfer unit */
    dhc_data_format_t rx_data_format; /**< RX data width per transfer unit */
} dhc_dma_config_t;

/**
 * @brief DHC clock configuration structure
 *
 * All values are specified in their natural units; the driver handles the hardware register offset-by-one
 * encoding (where HW 0 means 1 cycle, 1 means 2 cycles, etc.) internally.
 *
 * - clk_div: output clock = bus_clk / clk_div.  clk_div = 1 means bus clock directly (no division).
 * - clk_high_cycle / clk_low_cycle: clock waveform duty cycle in units of the divided output clock.
 *   1 = 1 cycle high/low, N = N cycles.
 */
typedef struct {
    bool clk_output_en;     /**< Enable clock pad output */
    bool clk_invert;        /**< Invert clock output polarity */
    uint8_t clk_div;        /**< Clock divider (>= 1): clk_out = bus_clk / clk_div. 1 = bus clock (no division). 0 is invalid. */
    uint8_t clk_high_cycle; /**< Clock high duration in divided clock cycles (>= 1). 1 = 1 cycle, N = N cycles. 0 is invalid. */
    uint8_t clk_low_cycle;  /**< Clock low duration in divided clock cycles (>= 1). 1 = 1 cycle, N = N cycles. 0 is invalid. */
} dhc_clk_config_t;

/**
 * @brief DHC chip select configuration structure
 *
 */
typedef struct {
    bool cs_output_enable;          /**< Enable CS pad output */
    uint8_t cs_output_disable_point; /**< CS output disable point in output clock cycles. Non-zero disables CS OE after the configured cycles, then re-enables after link_num*8 cycles. For hall test mode. */
    uint8_t cs_recover_link_num;    /**< Number of hall cells per data line. CS OE recovers after link_num*8 output clock cycles. For hall test mode. */
    uint8_t cs_wakeup_len;          /**< CS wakeup detection length in output clock cycles. Non-zero enables wakeup detect on the CS pin; a CS low longer than this value triggers a wakeup interrupt. */
    uint32_t cs_sel_dataline;       /**< CS line selection from d00-d31 by bit mask: bit0 for line0, bit1 for line1, etc. */
} dhc_cs_config_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DHC reset transfer state
 *
 * Clears DMA requests, internal FIFOs, and counters. Bit auto-clears after reset completes.
 *
 * @param[in] dhc DHC base address
 */
static inline void dhc_trans_reset(DHC_Type *dhc)
{
    dhc->GLB_CFG0 |= DHC_GLB_CFG0_TRANS_RESET_MASK;
}

/**
 * @brief DHC software-triggered transfer start
 *
 * @param[in] dhc DHC base address
 * @param[in] clk_cnt Burst configuration: 0xFFFFFF for continuous transfer, otherwise the total bit count to transfer
 *            before the transfer ends automatically. The bit count must be a multiple of 8.
 */
static inline void dhc_trans_sw_start(DHC_Type *dhc, uint32_t clk_cnt)
{
    assert(clk_cnt == 0xFFFFFF || (clk_cnt & 0x7) == 0);
    dhc->TRANS_CFG = (dhc->TRANS_CFG & ~DHC_TRANS_CFG_BURST_CFG_MASK) | DHC_TRANS_CFG_BURST_CFG_SET(clk_cnt)
                    | DHC_TRANS_CFG_SW_START_MASK;
}

/**
 * @brief DHC enable or disable transfer trigger
 *
 * @param[in] dhc DHC base address
 * @param[in] enable true - enable trigger; false - disable trigger
 */
static inline void dhc_set_trans_trig_en(DHC_Type *dhc, bool enable)
{
    dhc->TRANS_CFG = (dhc->TRANS_CFG & ~DHC_TRANS_CFG_TRIGGER_ENABLE_MASK) | DHC_TRANS_CFG_TRIGGER_ENABLE_SET(enable);
}

/**
 * @brief DHC set transfer burst count
 *
 * Configures the burst length without triggering a transfer. Call @ref dhc_trans_sw_start to begin.
 *
 * @param[in] dhc DHC base address
 * @param[in] clk_cnt Burst configuration: 0xFFFFFF for continuous transfer, otherwise the total bit count to transfer
 *            before the transfer ends automatically. The bit count must be a multiple of 8.
 */
static inline void dhc_set_trans_burst(DHC_Type *dhc, uint32_t clk_cnt)
{
    assert(clk_cnt == 0xFFFFFF || (clk_cnt & 0x7) == 0);
    dhc->TRANS_CFG = (dhc->TRANS_CFG & ~DHC_TRANS_CFG_BURST_CFG_MASK) | DHC_TRANS_CFG_BURST_CFG_SET(clk_cnt);
}

/**
 * @brief DHC force clock output to a specific level
 *
 * Bypasses normal clock generation to manually drive the clock line.
 * Used for hall sensor low power mode.
 *
 * @param[in] dhc DHC base address
 * @param[in] force_en true - force clock output; false - normal clock operation
 * @param[in] clk_value true - force high level; false - force low level
 */
static inline void dhc_force_clk_output(DHC_Type *dhc, bool force_en, bool clk_value)
{
    dhc->CLK_CFG = (dhc->CLK_CFG & ~(DHC_CLK_CFG_FORCE_CLK_MASK | DHC_CLK_CFG_CLK_VALUE_MASK)) |
                   DHC_CLK_CFG_FORCE_CLK_SET(force_en) | DHC_CLK_CFG_CLK_VALUE_SET(clk_value);
}

/**
 * @brief DHC set clock polarity
 *
 * @param[in] dhc DHC base address
 * @param[in] revert true - invert clock polarity; false - normal polarity
 */
static inline void dhc_set_clk_revert(DHC_Type *dhc, bool revert)
{
    dhc->CLK_CFG = (dhc->CLK_CFG & ~(DHC_CLK_CFG_CLKO_POL_MASK)) | DHC_CLK_CFG_CLKO_POL_SET(revert);
}

/**
 * @brief DHC set data line polarity
 *
 * @param[in] dhc DHC base address
 * @param[in] bitmask data line selection by bit mask
 * @param[in] revert true - invert data polarity; false - normal polarity
 */
static inline void dhc_set_data_revert(DHC_Type *dhc, uint32_t bitmask, bool revert)
{
    dhc->DATA_POL = revert ? (dhc->DATA_POL | bitmask) : (dhc->DATA_POL & ~bitmask);
}

/**
 * @brief DHC set data line enable
 *
 * @param[in] dhc DHC base address
 * @param[in] bitmask data line selection by bit mask
 * @param[in] enable true - enable data line; false - disable data line
 */
static inline void dhc_set_data_enable(DHC_Type *dhc, uint32_t bitmask, bool enable)
{
    dhc->DATA_EN = enable ? (dhc->DATA_EN | bitmask) : (dhc->DATA_EN & ~bitmask);
}

/**
 * @brief DHC set data line direction
 *
 * @param[in] dhc DHC base address
 * @param[in] bitmask data line selection by bit mask
 * @param[in] output true - output; false - input
 */
static inline void dhc_set_data_dir(DHC_Type *dhc, uint32_t bitmask, bool output)
{
    dhc->DATA_OE = output ? (dhc->DATA_OE | bitmask) : (dhc->DATA_OE & ~bitmask);
}

/**
 * @brief DHC select CS signal source from data lines d00-d31
 *
 * Set non-zero bits to use the corresponding data line as the CS signal source.
 *
 * @note For hall test mode, configure the selected data line with data_en high and data_oe low
 *       so that CS activity on that data line can be received as data.
 *
 * @param[in] dhc DHC base address
 * @param[in] bitmask CS line selection from d00-d31 by bit mask
 */
static inline void dhc_select_cs_line(DHC_Type *dhc, uint32_t bitmask)
{
    dhc->CS_SEL = bitmask;
}

/**
 * @brief DHC set CS wakeup detection length
 *
 * When set to a non-zero value, the DHC monitors the CS pin and triggers a wakeup interrupt
 * if CS stays low for more than the configured number of output clock cycles.
 * Set to 0 to disable wakeup detection.
 *
 * @param[in] dhc DHC base address
 * @param[in] wakeup_len Wakeup detection threshold in output clock cycles. 0 disables wakeup detection.
 *            @note After entering low power mode, disable CS output (pad_oe_cs = 0) and enable the CS pull-up.
 */
static inline void dhc_set_cs_wakeup_len(DHC_Type *dhc, uint8_t wakeup_len)
{
    dhc->CS_CFG = (dhc->CS_CFG & ~DHC_CS_CFG_WAKEUP_LEN_MASK)
                  | DHC_CS_CFG_WAKEUP_LEN_SET(wakeup_len);
}

/**
 * @brief DHC set CS output enable or disable
 *
 * @param[in] dhc DHC base address
 * @param[in] enable true - drive CS pin as output; false - CS pin controlled by pad_oe
 */
static inline void dhc_set_cs_output_enable(DHC_Type *dhc, bool enable)
{
    dhc->CS_CFG = (dhc->CS_CFG & ~DHC_CS_CFG_PAD_OE_CS_MASK)
                  | DHC_CS_CFG_PAD_OE_CS_SET(enable);
}

/**
 * @brief DHC set CS output disable point and recover link number for hall test mode
 *
 * When disable_point is non-zero, CS output is disabled after the configured number of output clock cycles,
 * then re-enabled after recover_link_num * 8 output clock cycles. The CS pin pull-up and loopback must be
 * enabled in the pad controller for this mode.
 *
 * @param[in] dhc DHC base address
 * @param[in] disable_point CS output disable point in output clock cycles
 * @param[in] recover_link_num Number of hall cells per data line. CS OE recovery delay = recover_link_num * 8 output clock cycles
 */
static inline void dhc_set_cs_output_disable_recover_point(DHC_Type *dhc, uint8_t disable_point, uint8_t recover_link_num)
{
    dhc->CS_CFG = (dhc->CS_CFG & ~(DHC_CS_CFG_OE_DIS_POINT_MASK | DHC_CS_CFG_LINK_NUM_MASK))
                  | DHC_CS_CFG_OE_DIS_POINT_SET(disable_point)
                  | DHC_CS_CFG_LINK_NUM_SET(recover_link_num);
}

/**
 * @brief DHC enable interrupts
 *
 * @param[in] dhc DHC base address
 * @param[in] mask IRQ mask (OR of @ref dhc_irq_mask_t values)
 */
static inline void dhc_set_irq_enable(DHC_Type *dhc, uint32_t mask)
{
    dhc->IRQ_EN |= mask;
}

/**
 * @brief DHC disable interrupts
 *
 * @param[in] dhc DHC base address
 * @param[in] mask IRQ mask (OR of @ref dhc_irq_mask_t values)
 */
static inline void dhc_set_irq_disable(DHC_Type *dhc, uint32_t mask)
{
    dhc->IRQ_EN &= ~mask;
}

/**
 * @brief DHC get interrupt enable status
 *
 * @param[in] dhc DHC base address
 * @retval Interrupt enable mask (bits as @ref dhc_irq_mask_t)
 */
static inline uint32_t dhc_get_irq_enable_status(DHC_Type *dhc)
{
    return dhc->IRQ_EN;
}

/**
 * @brief DHC get interrupt status flags
 *
 * @param[in] dhc DHC base address
 * @retval Interrupt status flags (bits as @ref dhc_irq_mask_t)
 */
static inline uint32_t dhc_get_irq_status(DHC_Type *dhc)
{
    return dhc->IRQ_STS;
}

/**
 * @brief DHC clear interrupt flags
 *
 * Write 1 to clear the corresponding interrupt flag.
 *
 * @param[in] dhc DHC base address
 * @param[in] mask IRQ mask (OR of @ref dhc_irq_mask_t values)
 */
static inline void dhc_clear_irq_flag(DHC_Type *dhc, uint32_t mask)
{
    dhc->IRQ_STS = mask;
}

/**
 * @brief DHC write TX data register
 *
 * @param[in] dhc DHC base address
 * @param[in] txdata Data word to transmit
 */
static inline void dhc_set_txdata(DHC_Type *dhc, uint32_t txdata)
{
    dhc->TXDATA = txdata;
}

/**
 * @brief DHC read RX data register
 *
 * @param[in] dhc DHC base address
 * @return Received data word
 */
static inline uint32_t dhc_get_rxdata(DHC_Type *dhc)
{
    return dhc->RXDATA;
}

/**
 * @brief DHC get TX data register address (for DMA use)
 *
 * @param[in] dhc DHC base address
 * @return Address of the TX data register
 */
static inline uint32_t dhc_get_txdata_addr(DHC_Type *dhc)
{
    return (uint32_t)&dhc->TXDATA;
}

/**
 * @brief DHC get RX data register address (for DMA use)
 *
 * @param[in] dhc DHC base address
 * @return Address of the RX data register
 */
static inline uint32_t dhc_get_rxdata_addr(DHC_Type *dhc)
{
    return (uint32_t)&dhc->RXDATA;
}

/**
 * @brief DHC configure data lines
 *
 * Configures data line enable, direction, polarity, and bit order for all data lines.
 *
 * @param[in] dhc DHC base address
 * @param[in] config Pointer to data line configuration structure, @ref dhc_data_config_t
 */
void dhc_config_data_lines(DHC_Type *dhc, dhc_data_config_t *config);

/**
 * @brief DHC configure clock line
 *
 * Configures clock output, polarity, divider, and duty cycle.
 *
 * @param[in] dhc DHC base address
 * @param[in] config Pointer to clock configuration structure, @ref dhc_clk_config_t
 */
void dhc_config_clk_line(DHC_Type *dhc, dhc_clk_config_t *config);

/**
 * @brief DHC configure chip select line
 *
 * Configures CS output enable, disable point, recover link number, wakeup length, and data line selection.
 *
 * @param[in] dhc DHC base address
 * @param[in] config Pointer to CS configuration structure, @ref dhc_cs_config_t
 */
void dhc_config_cs_line(DHC_Type *dhc, dhc_cs_config_t *config);

/**
 * @brief DHC configure DMA
 *
 * Configures TX/RX DMA enable and data format for DMA transfers.
 *
 * @param[in] dhc DHC base address
 * @param[in] config Pointer to DMA configuration structure, @ref dhc_dma_config_t
 */
void dhc_config_dma(DHC_Type *dhc, dhc_dma_config_t *config);


#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif /* HPM_DHC_DRV_H */
