/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <assert.h>
#include "hpm_dhc_drv.h"

void dhc_config_data_lines(DHC_Type *dhc, dhc_data_config_t *config)
{
    dhc->DATA_CFG = (dhc->DATA_CFG & ~(DHC_DATA_CFG_RXBIGENDIAN_MASK | DHC_DATA_CFG_TXBIGENDIAN_MASK))
                    | DHC_DATA_CFG_RXBIGENDIAN_SET(config->rx_msb_first)
                    | DHC_DATA_CFG_TXBIGENDIAN_SET(config->tx_msb_first);
    dhc->DATA_OE = config->data_dir_output;
    dhc->DATA_POL = config->data_invert;
    dhc->DATA_EN = config->data_line_en;
}

void dhc_config_clk_line(DHC_Type *dhc, dhc_clk_config_t *config)
{
    uint8_t clk_div = config->clk_div;
    uint8_t clk_high_cycle = config->clk_high_cycle;
    uint8_t clk_low_cycle = config->clk_low_cycle;

    if (clk_div > 0) {
        clk_div--;
    }
    if (clk_high_cycle > 0) {
        clk_high_cycle--;
    }
    if (clk_low_cycle > 0) {
        clk_low_cycle--;
    }

    dhc->CLK_CFG = (dhc->CLK_CFG & ~(DHC_CLK_CFG_CLK_LOW_MASK | DHC_CLK_CFG_CLK_HIG_MASK | DHC_CLK_CFG_PREDIV_MASK
                                   | DHC_CLK_CFG_CLKO_POL_MASK | DHC_CLK_CFG_PAD_OE_CLK_MASK))
                   | DHC_CLK_CFG_CLK_LOW_SET(clk_low_cycle)
                   | DHC_CLK_CFG_CLK_HIG_SET(clk_high_cycle)
                   | DHC_CLK_CFG_PREDIV_SET(clk_div)
                   | DHC_CLK_CFG_CLKO_POL_SET(config->clk_invert)
                   | DHC_CLK_CFG_PAD_OE_CLK_SET(config->clk_output_en);
}

void dhc_config_cs_line(DHC_Type *dhc, dhc_cs_config_t *config)
{
    dhc->CS_SEL = config->cs_sel_dataline;
    dhc->CS_CFG = (dhc->CS_CFG & ~(DHC_CS_CFG_OE_DIS_POINT_MASK | DHC_CS_CFG_PAD_OE_CS_MASK
                                 | DHC_CS_CFG_WAKEUP_LEN_MASK | DHC_CS_CFG_LINK_NUM_MASK))
                  | DHC_CS_CFG_WAKEUP_LEN_SET(config->cs_wakeup_len)
                  | DHC_CS_CFG_PAD_OE_CS_SET(config->cs_output_enable)
                  | DHC_CS_CFG_OE_DIS_POINT_SET(config->cs_output_disable_point)
                  | DHC_CS_CFG_LINK_NUM_SET(config->cs_recover_link_num);
}

void dhc_config_dma(DHC_Type *dhc, dhc_dma_config_t *config)
{
    dhc->DATA_CFG = (dhc->DATA_CFG & ~(DHC_DATA_CFG_RXD_FORMAT_MASK | DHC_DATA_CFG_TXD_FORMAT_MASK))
                    | DHC_DATA_CFG_RXD_FORMAT_SET(config->rx_data_format)
                    | DHC_DATA_CFG_TXD_FORMAT_SET(config->tx_data_format);
    dhc->GLB_CFG0 = (dhc->GLB_CFG0 & ~(DHC_GLB_CFG0_RX_DMA_EN_MASK | DHC_GLB_CFG0_TX_DMA_EN_MASK))
                   | DHC_GLB_CFG0_RX_DMA_EN_SET(config->rx_dma_en)
                   | DHC_GLB_CFG0_TX_DMA_EN_SET(config->tx_dma_en);
}
