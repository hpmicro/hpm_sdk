/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_EUI_DRV_H
#define HPM_EUI_DRV_H

#include "hpm_common.h"
#include "hpm_soc_ip_feature.h"
#include "hpm_eui_regs.h"

/**
 * @brief EUI driver APIs
 * @defgroup eui_interface EUI driver APIs
 * @ingroup eui_interface
 * @{
 */

/**
 * @brief eui work mode selection
 *
 */
typedef enum {
    eui_work_mode_8x8 = 0,
    eui_work_mode_16x4
} eui_work_mode_t;    /**< eui_work_mode_t */

/**
 * @brief eui disp control index
 *
 */
typedef enum {
    eui_disp_ctrl_idx_ab = 0,
    eui_disp_ctrl_idx_cd,
    eui_disp_ctrl_idx_ef,
    eui_disp_ctrl_idx_gh,
} eui_disp_ctrl_idx_t;    /**< eui_disp_ctrl_idx_t */

/**
 * @brief eui b disp date format
 *
 */
typedef enum {
    eui_b_disp_b = 0,
    eui_b_disp_a_and_b,
    eui_b_disp_a_or_b,
    eui_b_disp_a_xor_b,
} eui_b_disp_format_t;    /**< eui_b_disp_format_t */

/**
 * @brief eui disp data index
 *
 */
typedef enum {
    eui_disp_data_idx_a = 0,
    eui_disp_data_idx_b,
    eui_disp_data_idx_c,
    eui_disp_data_idx_d,
    eui_disp_data_idx_e,
    eui_disp_data_idx_f,
    eui_disp_data_idx_g,
    eui_disp_data_idx_h,
} eui_disp_data_idx_t;    /**< eui_disp_data_idx_t */

/**
 * @brief eui time id
 *
 */
typedef enum {
    eui_clko_time = 0,
    eui_slot_time,
    eui_hold_time,
    eui_disp_time,
    eui_filter_time,
} eui_time_id_t;    /**< eui_time_id_t */

/**
 * @brief irq mask
 *
 */
typedef enum {
    eui_irq_area_mask = EUI_IRQ_EN_IRQ_AREA_EN_MASK,
    eui_irq_single_mask = EUI_IRQ_EN_IRQ_SINGLE_EN_MASK,
} eui_irq_mask_t;    /**< eui_irq_mask_t */

/**
 * @brief Number of dedicated output lines
 *
 */
typedef enum {
    eui_dedicate_output_0_line = 0,
    eui_dedicate_output_1_line,
    eui_dedicate_output_2_lines,
    eui_dedicate_output_3_lines,
    eui_dedicate_output_4_lines,
    eui_dedicate_output_5_lines,
    eui_dedicate_output_6_lines,
    eui_dedicate_output_24_lines,
} eui_dedicate_output_lines_t;    /**< eui_dedicate_output_lines_t */

/**
 * @brief eui ctrl config structure
 *
 */
typedef struct {
    eui_work_mode_t work_mode;
    bool colum_loop_mode;
    uint16_t clko_freq_khz;
    uint8_t hold_tm_cnt;
    uint16_t key_filter_ms;
    bool di_invert;
    bool do_invert;
    bool clk_invert;
    bool sh_invert;
    uint16_t disp_data_invert;    /* set by bit */
    uint8_t scan_invert;          /* set by bit */
    eui_dedicate_output_lines_t dedicate_out_cfg;
    uint16_t dedicate_in_cfg;     /* set by bit */
} eui_ctrl_config_t;    /**< eui_ctrl_config_t */

/**
 * @brief eui disp config structure
 *
 */
typedef struct {
    uint16_t disp_a_time_ms;
    uint16_t disp_b_time_ms;
    uint8_t ab_loop_cnt;
    eui_b_disp_format_t b_data_format;
} eui_disp_config_t;    /**< eui_disp_config_t */

/**
 * @brief eui scan disp data structure
 *
 */
typedef union {
    uint8_t data_8x8[8];
    uint16_t data_16x4[4];
    uint8_t data_u8[8];
    uint32_t data_u32[2];
} eui_scan_disp_data_t;    /**< eui_scan_disp_data_t */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief set enable
 *
 * @param[in] eui EUI base address
 * @param[in] enable true - enable, false - disable
 */
static inline void eui_set_enable(EUI_Type *eui, bool enable)
{
    eui->CTRL = (eui->CTRL & ~EUI_CTRL_ENABLE_MASK) | EUI_CTRL_ENABLE_SET(enable);
}

/**
 * @brief set work mode
 *
 * @param[in] eui EUI base address
 * @param[in] mode EUI work mode, @ref eui_work_mode_t
 */
static inline void eui_set_work_mode(EUI_Type *eui, eui_work_mode_t mode)
{
    eui->CTRL = (eui->CTRL & ~EUI_CTRL_DISP_MODE_MASK) | EUI_CTRL_DISP_MODE_SET(mode);
}

/**
 * @brief get work mode
 *
 * @param[in] eui EUI base address
 *
 * @return EUI work mode, @ref eui_work_mode_t
 */
static inline eui_work_mode_t eui_get_work_mode(EUI_Type *eui)
{
    return (eui_work_mode_t)EUI_CTRL_DISP_MODE_GET(eui->CTRL);
}

/**
 * @brief clear output data
 *
 * @param[in] eui EUI base address
 */
static inline void eui_clear_output_data(EUI_Type *eui)
{
    eui->CTRL |= EUI_CTRL_CLR_OUTPUT_MASK;
}

/**
 * @brief clear input data
 *
 * @param[in] eui EUI base address
 */
static inline void eui_clear_intput_data(EUI_Type *eui)
{
    eui->CTRL |= EUI_CTRL_CLR_DATA_IN_MASK;
}

/**
 * @brief set data polarity and scan polarity
 *
 * @param[in] eui EUI base address
 * @param[in] disp_data_invert 16 bits disp data invert bit mask, 1 - invert, 0 - not invert
 * @param[in] scan_invert 8 bits scan invert bit mask, 1 - invert, 0 - not invert
 */
static inline void eui_set_data_polarity(EUI_Type *eui, uint16_t disp_data_invert, uint8_t scan_invert)
{
    eui->POL_CFG = (eui->POL_CFG & ~EUI_POL_CFG_DATA_POL_MASK) | EUI_POL_CFG_DATA_POL_SET((disp_data_invert << 8) | scan_invert);
}

/**
 * @brief set dedicate disp data
 *
 * @param[in] eui EUI base address
 * @param[in] data 24 bits dedicate disp data
 */
static inline void eui_set_dedicate_disp_data(EUI_Type *eui, uint32_t data)
{
    eui->OUT_CFG = (eui->OUT_CFG & ~EUI_OUT_CFG_DO_VAL_MASK) | EUI_OUT_CFG_DO_VAL_SET(data);
}

/**
 * @brief get dedicate input data
 *
 * @param[in] eui EUI base address

 * @return uint16_t 16 bits dedicate input data
 */
static inline uint16_t eui_get_dedicate_input_data(EUI_Type *eui)
{
    return EUI_IN_CFG_DI_VAL_GET(eui->IN_CFG);
}

/**
 * @brief set irq enable
 *
 * @param[in] eui EUI base address
 * @param[in] mask irq mask, @ref eui_irq_mask_t
 */
static inline void eui_set_irq_enable(EUI_Type *eui, uint32_t mask)
{
    eui->IRQ_EN |= mask;
}

/**
 * @brief set irq disable
 *
 * @param[in] eui EUI base address
 * @param[in] mask irq mask, @ref eui_irq_mask_t
 */
static inline void eui_set_irq_disable(EUI_Type *eui, uint32_t mask)
{
    eui->IRQ_EN &= ~mask;
}

/**
 * @brief get irq enable status
 *
 * @param[in] eui EUI base address
 * @retval irq enable status, @ref eui_irq_mask_t
 */
static inline uint32_t eui_get_irq_enable_status(EUI_Type *eui)
{
    return eui->IRQ_EN;
}

/**
 * @brief get irq status
 *
 * @param[in] eui EUI base address
 * @retval irq status, @ref eui_irq_mask_t
 */
static inline uint32_t eui_get_irq_status(EUI_Type *eui)
{
    return eui->IRQ_STS;
}

/**
 * @brief clear irq flag
 *
 * @param[in] eui EUI base address
 * @param[in] mask irq mask, @ref eui_irq_mask_t
 */
static inline void eui_clear_irq_flag(EUI_Type *eui, uint32_t mask)
{
    eui->IRQ_STS = mask;
}

/**
 * @brief get default control config structure
 *
 * @param[in] eui EUI base address
 * @param[out] config control config structure pointer, @ref eui_ctrl_config_t
 */
void eui_get_default_ctrl_config(EUI_Type *eui, eui_ctrl_config_t *config);

/**
 * @brief config control structure
 *
 * @param[in] eui EUI base address
 * @param[in] eui_clk_freq EUI clock frequency
 * @param[in] config control config structure pointer, @ref eui_ctrl_config_t
 */
void eui_config_ctrl(EUI_Type *eui, uint32_t eui_clk_freq, eui_ctrl_config_t *config);

/**
 * @brief config disp structure
 *
 * @param[in] eui EUI base address
 * @param[in] eui_clk_freq EUI clock frequency
 * @param[in] idx disp ctrl index, @ref eui_disp_ctrl_idx_t
 * @param[in] config disp config structure pointer, @ref eui_disp_config_t
 */
void eui_config_disp(EUI_Type *eui, uint32_t eui_clk_freq, eui_disp_ctrl_idx_t idx, eui_disp_config_t *config);

/**
 * @brief set eui scan disp data
 *
 * @param[in] eui EUI base address
 * @param[in] idx disp data index, @ref eui_disp_data_idx_t
 * @param[in] data scan disp data for 8x8 or 16x4, @ref eui_scan_disp_data_t
 */
void eui_set_scan_disp_data(EUI_Type *eui, eui_disp_data_idx_t idx, eui_scan_disp_data_t *data);

/**
 * @brief set eui scan disp data
 *
 * @param[in] eui EUI base address
 * @param[in] idx disp data index, @ref eui_disp_data_idx_t
 * @param[out] data0 disp data index 00
 * @param[out] data1 disp data index 01
 */
void eui_get_scan_disp_data(EUI_Type *eui, eui_disp_data_idx_t idx, uint32_t *data0, uint32_t *data1);

/**
 * @brief get eui scan key value by row
 *
 * @param[in] eui EUI base address
 * @param[in] row row index for scan input data

 * @return uint16_t 16 bits scan input data
 */
uint16_t eui_get_scan_key_by_row(EUI_Type *eui, uint8_t row);

/**
 * @brief get eui scan key value by row and column
 *
 * @param[in] eui EUI base address
 * @param[in] row row index for scan input data
 * @param[in] col column index for scan input data

 * @return bool 1 bit scan input data
 */
bool eui_get_scan_key_by_row_col(EUI_Type *eui, uint8_t row, uint8_t col);

/**
 * @brief get time value, unit: us
 *
 * @param[in] eui EUI base address
 * @param[in] eui_clk_freq EUI clock frequency
 * @param[in] id time id, @ref eui_time_id_t
 */
uint32_t eui_get_time(EUI_Type *eui, uint32_t eui_clk_freq, eui_time_id_t id);


#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif /* HPM_EUI_DRV_H */
