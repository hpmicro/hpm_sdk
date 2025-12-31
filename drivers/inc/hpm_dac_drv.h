/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_DAC_DRV_H
#define HPM_DAC_DRV_H

#include "hpm_common.h"
#include "hpm_dac_regs.h"
#include "hpm_soc_feature.h"

/**
 * @brief DAC driver APIs
 * @defgroup dac_interface DAC driver APIs
 * @{
 */

/** @brief The range of DAC output value setting is from 0.0 to 10000.0, which is mapped to 0 to 100 in percentage. */
#define DAC_OUTPUT(PERCENT) (PERCENT / 10000.0f * DAC_SOC_MAX_DATA)

/** @brief DAC AHB error event */
#define DAC_AHB_ERROR_EVENT     DAC_IRQ_EN_AHB_ERROR_MASK
/** @brief DAC FIFO empty event */
#define DAC_FIFO_EMPTY_EVENT    DAC_IRQ_EN_FIFO_EMPTY_MASK
/** @brief DAC buffer 1 complete event */
#define DAC_BUF1_COMPLETE_EVENT DAC_IRQ_EN_BUF1_CMPT_MASK
/** @brief DAC buffer 0 complete event */
#define DAC_BUF0_COMPLETE_EVENT DAC_IRQ_EN_BUF0_CMPT_MASK

/** @brief DAC operation mode */
typedef enum {
    dac_mode_direct = 0,  /**< Direct mode: output data directly */
    dac_mode_step,        /**< Step mode: output data in steps */
    dac_mode_buffer,      /**< Buffer mode: output data from buffer */
    dac_mode_trig         /**< Trigger mode: output data on trigger */
} dac_mode_t;

/** @brief DAC analog divider */
typedef enum {
    dac_ana_div_2 = 0,  /**< Divide by 2 */
    dac_ana_div_4,      /**< Divide by 4 */
    dac_ana_div_6,      /**< Divide by 6 */
    dac_ana_div_8       /**< Divide by 8 */
} dac_ana_div_t;

/** @brief DAC configuration structure */
typedef struct {
    bool sync_mode;     /**< Synchronization mode enable flag */
    uint8_t dac_mode;   /**< DAC operation mode, see @ref dac_mode_t */
    uint8_t ana_div;    /**< Analog divider, see @ref dac_ana_div_t */
} dac_config_t;

/** @brief DAC step direction */
typedef enum {
    dac_step_up = 0,    /**< Step up: increase output value */
    dac_step_down       /**< Step down: decrease output value */
} dac_step_direction_t;

/** @brief DAC round mode */
typedef enum {
    dac_round_mode_oneshot = 0,  /**< Oneshot mode: execute once and stop */
    dac_round_mode_loop          /**< Loop mode: execute repeatedly */
} dac_round_mode_t;

/** @brief DAC step configuration structure */
typedef struct {
    uint16_t start_point;  /**< Start point value for step mode */
    uint16_t end_point;   /**< End point value for step mode */
    uint8_t round_mode;   /**< Round mode, see @ref dac_round_mode_t */
    uint8_t up_down;      /**< Step direction, see @ref dac_step_direction_t */
    uint8_t step_num;     /**< Number of steps */
} dac_step_config_t;

/** @brief DAC data structure type */
typedef enum {
    dac_data_stru_2_point = 0,  /**< 2 point data structure: contains data0 and data1 */
    dac_data_stru_1_point       /**< 1 point data structure: contains single data point */
} dac_data_structure_t;

/** @brief DAC burst type */
typedef enum {
    dac_burst_single = 0,  /**< Single burst: transfer one data at a time */
    dac_burst_incr4  = 3,  /**< Increment 4: transfer 4 data with incrementing address */
    dac_burst_incr8  = 5   /**< Increment 8: transfer 8 data with incrementing address */
} dac_burst_type_t;

/** @brief DAC buffer structure */
typedef struct {
    uint32_t start_addr;  /**< Start address of the buffer in memory */
    uint8_t stop;         /**< Stop position in the buffer */
    uint16_t len;         /**< Buffer length in bytes */
} dac_buffer_t;

/** @brief DAC buffer configuration structure */
typedef struct {
    uint8_t buf_data_mode;      /**< Buffer data structure mode, see @ref dac_data_structure_t */
    uint8_t burst;              /**< Burst type, see @ref dac_burst_type_t */
    dac_buffer_t buf0;          /**< Buffer 0 configuration, see @ref dac_buffer_t */
    dac_buffer_t buf1;          /**< Buffer 1 configuration, see @ref dac_buffer_t */
} dac_buffer_config_t;

/** @brief DAC buffer data structure */
typedef struct {
   uint32_t data0:    12;      /**< DAC data 0 (12 bits) */
   uint32_t reserved: 4;        /**< Reserved bits */
   uint32_t data1:    12;      /**< DAC data 1 (12 bits) */
   uint32_t reserved1: 4;      /**< Reserved bits */
} dac_buffer_data_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get default DAC configuration
 * @param[out] config Pointer to DAC configuration structure
 */
void dac_get_default_config(dac_config_t *config);

/**
 * @brief Initialize DAC
 * @param[in] ptr DAC base address
 * @param[in] config Pointer to DAC configuration structure
 * @return hpm_stat_t Operation result
 *         - @ref status_success Initialization successful
 *         - @ref status_invalid_argument Invalid configuration
 */
hpm_stat_t dac_init(DAC_Type *ptr, dac_config_t *config);

/**
 * @brief Set DAC direct mode configuration
 * @param[in] ptr DAC base address
 * @param[in] data Direct output data value
 * @return hpm_stat_t Operation result
 *         - @ref status_success Configuration successful
 */
hpm_stat_t dac_set_direct_config(DAC_Type *ptr, uint16_t data);

/**
 * @brief Set DAC step mode configuration
 * @param[in] ptr DAC base address
 * @param[in] step_cfg_idx Step configuration index
 * @param[in] config Pointer to step configuration structure
 * @return hpm_stat_t Operation result
 *         - @ref status_success Configuration successful
 *         - @ref status_invalid_argument Invalid configuration
 */
hpm_stat_t dac_set_step_config(DAC_Type *ptr, uint8_t step_cfg_idx, dac_step_config_t *config);

/**
 * @brief Set DAC buffer mode configuration
 * @param[in] ptr DAC base address
 * @param[in] config Pointer to buffer configuration structure
 * @return hpm_stat_t Operation result
 *         - @ref status_success Configuration successful
 *         - @ref status_invalid_argument Invalid configuration
 */
hpm_stat_t dac_set_buffer_config(DAC_Type *ptr, dac_buffer_config_t *config);

/**
 * @brief Set DAC output frequency
 * @param[in] ptr DAC base address
 * @param[in] dac_input_freq DAC input frequency
 * @param[in] dac_output_freq DAC output frequency
 * @return hpm_stat_t Operation result
 *         - @ref status_success Configuration successful
 *         - @ref status_invalid_argument Invalid frequency
 */
hpm_stat_t dac_set_output_frequency(DAC_Type *ptr, uint32_t dac_input_freq, uint32_t dac_output_freq);

/**
 * @brief Set DAC step software trigger
 * @param[in] ptr DAC base address
 * @param[in] step_sw_trig_idx Step software trigger index
 * @return hpm_stat_t Operation result
 *         - @ref status_success Trigger successful
 */
hpm_stat_t dac_set_step_sw_trigger(DAC_Type *ptr, uint8_t step_sw_trig_idx);

/**
 * @brief Set DAC buffer software trigger
 * @param[in] ptr DAC base address
 */
void dac_set_buffer_sw_trigger(DAC_Type *ptr);

/**
 * @brief Enable or disable DAC hardware trigger
 * @param[in] ptr DAC base address
 * @param[in] enable Enable flag (true to enable, false to disable)
 */
void dac_set_hw_trigger_enable(DAC_Type *ptr, bool enable);

/**
 * @brief Enable or disable DAC external DMA request
 * @param[in] ptr DAC base address
 * @param[in] buf_idx Buffer index (0 or 1)
 * @param[in] enable Enable flag (true to enable, false to disable)
 * @return hpm_stat_t Operation result
 *         - @ref status_success Configuration successful
 *         - @ref status_invalid_argument Invalid buffer index
 */
hpm_stat_t dac_external_dma_request_enable(DAC_Type *ptr, uint8_t buf_idx, bool enable);

/**
 * @brief Reset DAC buffer DMA
 * @param[in] ptr DAC base address
 */
void dac_set_buffer_dma_reset(DAC_Type *ptr);

/**
 * @brief Enable or disable DAC conversion
 * @param[in] ptr DAC base address
 * @param[in] enable Enable flag (true to enable, false to disable)
 */
void dac_enable_conversion(DAC_Type *ptr, bool enable);

/**
 * @brief Enable DAC interrupts
 * @param[in] ptr DAC base address
 * @param[in] mask Interrupt mask (combination of @ref DAC_AHB_ERROR_EVENT, @ref DAC_FIFO_EMPTY_EVENT,
 *                 @ref DAC_BUF1_COMPLETE_EVENT, @ref DAC_BUF0_COMPLETE_EVENT)
 */
void dac_enable_interrupts(DAC_Type *ptr, uint32_t mask);

/**
 * @brief Get DAC status flags
 * @param[in] ptr DAC base address
 * @return Status flags
 */
uint32_t dac_get_status_flags(DAC_Type *ptr);

/**
 * @brief Clear DAC status flags
 * @param[in] ptr DAC base address
 * @param[in] mask Status flags mask to clear
 */
void dac_set_status_flags(DAC_Type *ptr, uint32_t mask);

/**
 * @brief Get current DAC buffer index
 * @param[in] ptr DAC base address
 * @return Current buffer index (0 or 1)
 */
uint8_t dac_get_current_buffer_index(DAC_Type *ptr);

/**
 * @brief Get current DAC buffer offset
 * @param[in] ptr DAC base address
 * @return Current buffer offset
 */
uint16_t dac_get_current_buffer_offset(DAC_Type *ptr);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
