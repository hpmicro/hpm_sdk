/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_PPI_H
#define _HPM_PPI_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_ppi_drv.h"

/**
 * @brief ppi dq pins enum
 *
 */
typedef enum {
    ppi_dq_pins_0_7,
    ppi_dq_pins_8_15,
    ppi_dq_pins_16_23,
    ppi_dq_pins_24_31,
} ppi_dq_pins_t;

/**
 * @brief ppi async sram config structure
 *
 */
typedef struct {
    uint32_t base_address;              /**< external SRAM base address, should be 1MB aligned */
    uint32_t size_in_byte;              /**< external SRAM size in byte */
    ppi_port_size_t port_size;          /**< port size */
    bool ad_mux_mode;                   /**< addr and data mux mode */
    bool cs_valid_polarity;             /**< cs valid polarity */
    bool dm_valid_polarity;             /**< dm valid polarity */
    bool addr_valid_polarity;           /**< addr valid polarity */
    uint8_t adv_ctrl_pin;               /**< adv ctrl pin number, 0 - 7 */
    uint8_t rel_ctrl_pin;               /**< rel ctrl pin number, 0 - 7 */
    uint8_t wel_ctrl_pin;               /**< wel ctrl pin number, 0 - 7 */
    uint16_t as_in_ns;                  /**< address setup time */
    uint16_t ah_in_ns;                  /**< address hold time */
    uint16_t rel_in_ns;                 /**< RE low time */
    uint16_t reh_in_ns;                 /**< RE high time */
    uint16_t wel_in_ns;                 /**< WE low time */
    uint16_t weh_in_ns;                 /**< WE high time */
    ppi_dq_pins_t dq_sig_sel[4];        /**< dq signal selection.
                                         *   dq_sig_sel[0] is signal dq0-7 select ppi dq pins,
                                         *   dq_sig_sel[1] is signal dq8-15 select ppi dq pins,
                                         *   dq_sig_sel[2] is signal dq16-23 select ppi dq pins,
                                         *   dq_sig_sel[3] is signal dq24-31 select ppi dq pins.
                                         */
} ppi_async_sram_config_t;


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief get async sram default config
 *
 * @param[in] ppi PPI base address
 * @param[in] config async sram default config structure pointer, @ref ppi_async_sram_config_t
 */
void ppi_get_async_sram_defconfig(PPI_Type *ppi, ppi_async_sram_config_t *config);

/**
 * @brief config async sram
 *
 * @param[in] ppi PPI base address
 * @param[in] cs_index cs index, value: 0 - 3
 * @param[in] cmd_start_index cmd start index, should be a multiple of 8, such as 0, 8, 16, 24 ...
 * @param[in] config async sram config structure pointer, @ref ppi_async_sram_config_t
 */
void ppi_config_async_sram(PPI_Type *ppi, uint8_t cs_index, uint8_t cmd_start_index, ppi_async_sram_config_t *config);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_PPI_H */