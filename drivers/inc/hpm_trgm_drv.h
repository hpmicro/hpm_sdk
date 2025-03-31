/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TRGM_DRV_H
#define HPM_TRGM_DRV_H

#include "hpm_common.h"
#include "hpm_soc_ip_feature.h"
#include "hpm_trgm_regs.h"
#include "hpm_trgmmux_src.h"

/**
 *
 * @brief TRGM driver APIs
 * @defgroup trgm_interface TRGM driver APIs
 * @{
 */

/**
 * @brief Filter mode
 */
typedef enum trgm_filter_mode {
    trgm_filter_mode_bypass = 0,
    trgm_filter_mode_rapid_change = 4,
    trgm_filter_mode_delay = 5,
    trgm_filter_mode_stable_low = 6,
    trgm_filter_mode_stable_high = 7,
} trgm_filter_mode_t;

/**
 * @brief Output type
 */
typedef enum trgm_output_type {
    trgm_output_same_as_input = 0,
    trgm_output_pulse_at_input_falling_edge = TRGM_TRGOCFG_FEDG2PEN_MASK,
    trgm_output_pulse_at_input_rising_edge = TRGM_TRGOCFG_REDG2PEN_MASK,
    trgm_output_pulse_at_input_both_edge = trgm_output_pulse_at_input_falling_edge
                                    | trgm_output_pulse_at_input_rising_edge,
} trgm_output_type_t;

typedef enum {
    trgm_pwmv2_calibration_mode_begin = 0,
    trgm_pwmv2_calibration_mode_wait = 1,
    trgm_pwmv2_calibration_mode_end = 2,
} trgm_pwmv2_calibration_mode_t;

/**
 * @brief Filter configuration
 */
typedef struct trgm_input_filter {
    bool invert;                /**< Invert output */
    bool sync;                  /**< Sync with TRGM clock */
    uint32_t filter_length;     /**< Filter length in TRGM clock cycle */
    trgm_filter_mode_t mode;    /**< Filter working mode */
} trgm_filter_t;

typedef trgm_filter_t trgm_input_filter_t;

/**
 * @brief Output configuration
 */
typedef struct trgm_output {
    bool invert;                /**< Invert output */
    trgm_output_type_t type;    /**< Output type */
    uint8_t input;              /**< Input selection */
} trgm_output_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable IO output
 *
 * @param[in] ptr TRGM base address
 * @param[in] mask Mask of IOs to be enabled
 */
static inline void trgm_enable_io_output(TRGM_Type *ptr, uint32_t mask)
{
    ptr->GCR |= mask;
}

/**
 * @brief   Disable IO output
 *
 * @param[in] ptr TRGM base address
 * @param[in] mask Mask of IOs to be disabled
 */
static inline void trgm_disable_io_output(TRGM_Type *ptr, uint32_t mask)
{
    ptr->GCR &= ~mask;
}

/**
 * @brief   Set filter length
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] length Filter length in TRGM clock cycles
 */
static inline void trgm_filter_set_filter_length(TRGM_Type *ptr, uint8_t input, uint32_t length)
{
#if defined(TRGM_SOC_HAS_FILTER_SHIFT) && TRGM_SOC_HAS_FILTER_SHIFT
    uint32_t len = length;
    uint8_t shift;
    for (shift = 0; shift <= (TRGM_FILTCFG_FILTLEN_SHIFT_MASK >> TRGM_FILTCFG_FILTLEN_SHIFT_SHIFT); shift++) {
        if (shift > 0) {
            len >>= 1u;
        }
        if (len <= (TRGM_FILTCFG_FILTLEN_BASE_MASK >> TRGM_FILTCFG_FILTLEN_BASE_SHIFT)) {
            break;
        }
    }
    if (len > (TRGM_FILTCFG_FILTLEN_BASE_MASK >> TRGM_FILTCFG_FILTLEN_BASE_SHIFT)) {
        len = (TRGM_FILTCFG_FILTLEN_BASE_MASK >> TRGM_FILTCFG_FILTLEN_BASE_SHIFT);
        shift = (TRGM_FILTCFG_FILTLEN_SHIFT_MASK >> TRGM_FILTCFG_FILTLEN_SHIFT_SHIFT);
    }
    ptr->FILTCFG[input] = (ptr->FILTCFG[input] & ~(TRGM_FILTCFG_FILTLEN_BASE_MASK | TRGM_FILTCFG_FILTLEN_SHIFT_MASK))
                        | TRGM_FILTCFG_FILTLEN_BASE_SET(len) | TRGM_FILTCFG_FILTLEN_SHIFT_SET(shift);
#else
    ptr->FILTCFG[input] = (ptr->FILTCFG[input] & ~TRGM_FILTCFG_FILTLEN_MASK)
                        | TRGM_FILTCFG_FILTLEN_SET(length);
#endif
}

/**
 * @brief   Set filter length shift.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] shift Filter length shift
 */
static inline void trgm_filter_set_filter_shift(TRGM_Type *ptr, uint8_t input, uint8_t shift)
{
#if defined(TRGM_SOC_HAS_FILTER_SHIFT) && TRGM_SOC_HAS_FILTER_SHIFT
    ptr->FILTCFG[input] = (ptr->FILTCFG[input] & ~TRGM_FILTCFG_FILTLEN_SHIFT_MASK)
                        | TRGM_FILTCFG_FILTLEN_SHIFT_SET(shift);
#else
    (void) ptr;
    (void) input;
    (void) shift;
#endif
}

/**
 * @brief   Enable sync input with TRGM clock
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 */
static inline void trgm_filter_enable_sync(TRGM_Type *ptr, uint8_t input)
{
    ptr->FILTCFG[input] |= TRGM_FILTCFG_SYNCEN_MASK;
}

/**
 * @brief   Disable sync input with TRGM clock
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 */
static inline void trgm_filter_disable_sync(TRGM_Type *ptr, uint8_t input)
{
    ptr->FILTCFG[input] &= ~TRGM_FILTCFG_SYNCEN_MASK;
}

/**
 * @brief   Set filter working mode
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] mode Working mode
 */
static inline void trgm_filter_set_mode(TRGM_Type *ptr, uint8_t input, trgm_filter_mode_t mode)
{
    ptr->FILTCFG[input] = (ptr->FILTCFG[input] & ~TRGM_FILTCFG_MODE_MASK)
                        | TRGM_FILTCFG_MODE_SET(mode);
}

/**
 * @brief   Invert filter output
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] invert Set true to invert output
 */
static inline void trgm_filter_invert(TRGM_Type *ptr, uint8_t input, bool invert)
{
    ptr->FILTCFG[input] = (ptr->FILTCFG[input] & ~TRGM_FILTCFG_OUTINV_MASK)
                        | TRGM_FILTCFG_OUTINV_SET(invert);
}

/**
 * @brief   Configure filter
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] filter Pointer to filter configuration
 */
static inline void trgm_filter_config(TRGM_Type *ptr, uint8_t input, trgm_filter_t *filter)
{
    ptr->FILTCFG[input] = TRGM_FILTCFG_OUTINV_SET(filter->invert)
                        | TRGM_FILTCFG_MODE_SET(filter->mode)
                        | TRGM_FILTCFG_SYNCEN_SET(filter->sync);
    trgm_filter_set_filter_length(ptr, input, filter->filter_length);
}

/**
 * @brief   Set filter length, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] length Filter length in TRGM clock cycles
 */
static inline void trgm_input_filter_set_filter_length(TRGM_Type *ptr, uint8_t input, uint32_t length)
{
    trgm_filter_set_filter_length(ptr, input, length);
}

/**
 * @brief   Set filter length shift, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] shift Filter length shift
 */
static inline void trgm_input_filter_set_filter_shift(TRGM_Type *ptr, uint8_t input, uint8_t shift)
{
    trgm_filter_set_filter_shift(ptr, input, shift);
}

/**
 * @brief   Enable sync input with TRGM clock, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 */
static inline void trgm_input_filter_enable_sync(TRGM_Type *ptr, uint8_t input)
{
    trgm_filter_enable_sync(ptr, input);
}

/**
 * @brief   Disable sync input with TRGM clock, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 */
static inline void trgm_input_filter_disable_sync(TRGM_Type *ptr, uint8_t input)
{
    trgm_filter_disable_sync(ptr, input);
}

/**
 * @brief   Set filter working mode, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] mode Working mode
 */
static inline void trgm_input_filter_set_mode(TRGM_Type *ptr, uint8_t input, trgm_filter_mode_t mode)
{
    trgm_filter_set_mode(ptr, input, mode);
}

/**
 * @brief   Invert filter output, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] invert Set true to invert output
 */
static inline void trgm_input_filter_invert(TRGM_Type *ptr, uint8_t input, bool invert)
{
    trgm_filter_invert(ptr, input, invert);
}

/**
 * @brief   Configure filter, legacy API for compatibility.
 *
 * @param[in] ptr TRGM base address
 * @param[in] input Input selection
 * @param[in] filter Pointer to filter configuration
 */
static inline void trgm_input_filter_config(TRGM_Type *ptr, uint8_t input, trgm_input_filter_t *filter)
{
    trgm_filter_config(ptr, input, filter);
}

/**
 * @brief   Update source for TRGM output
 *
 * @param[in] ptr TRGM base address
 * @param[in] output Target output
 * @param[in] source Source for output
 */
static inline void trgm_output_update_source(TRGM_Type *ptr, uint8_t output, uint8_t source)
{
    ptr->TRGOCFG[output] = (ptr->TRGOCFG[output] & ~TRGM_TRGOCFG_TRIGOSEL_MASK)
                        | TRGM_TRGOCFG_TRIGOSEL_SET(source);
}

/**
 * @brief   Configure output
 *
 * @param[in] ptr TRGM base address
 * @param[in] output Target output
 * @param[in] config Pointer to output configuration
 */
static inline void trgm_output_config(TRGM_Type *ptr, uint8_t output, trgm_output_t *config)
{
    ptr->TRGOCFG[output] = TRGM_TRGOCFG_TRIGOSEL_SET(config->input)
                        | (config->type & TRGM_TRGOCFG_FEDG2PEN_MASK)
                        | (config->type & TRGM_TRGOCFG_REDG2PEN_MASK)
                        | TRGM_TRGOCFG_OUTINV_SET(config->invert);
}

/**
 * @brief   Configure DMA request
 *
 * @param[in] ptr TRGM base address
 * @param[in] dma_out Target DMA out
 * @param[in] dma_src DMA source selection
 */
static inline void trgm_dma_request_config(TRGM_Type *ptr, uint8_t dma_out, uint8_t dma_src)
{
#if defined(TRGM_SOC_HAS_DMAMUX_EN) && TRGM_SOC_HAS_DMAMUX_EN
    ptr->DMACFG[dma_out] = TRGM_DMACFG_DMASRCSEL_SET(dma_src) | TRGM_DMACFG_DMAMUX_EN_MASK;
#else
    ptr->DMACFG[dma_out] = TRGM_DMACFG_DMASRCSEL_SET(dma_src);
#endif
}

#if defined(HPM_IP_FEATURE_TRGM_HAS_TRGM_IN_OUT_STATUS) && HPM_IP_FEATURE_TRGM_HAS_TRGM_IN_OUT_STATUS
/**
 * @brief Get input signal group status
 *
 * @param ptr TRGM base address
 * @param trgm_in_group input signal group, TRGM_TRGM_IN_X (X = 0, 1 ...)
 * @return uint32_t group bits, trgm_in_group not exist return 0
 */
static inline uint32_t trgm_input_get_group_status(TRGM_Type *ptr, uint8_t trgm_in_group)
{
    if (trgm_in_group > TRGM_SOC_TRIM_IN_GROUP_MAX)
        return 0;

    return ptr->TRGM_IN[trgm_in_group];
}

/**
 * @brief Get input signal status
 *
 * @param ptr TRGM base address
 * @param trgm_in_index input signal index, HPM_TRGM0_INPUT_SRC_X
 * @return int input signal status, trgm_in_index not in group return -1
 */
static inline int trgm_input_get_signal_status(TRGM_Type *ptr, uint8_t trgm_in_index)
{
    uint8_t group = trgm_in_index / 32;
    uint8_t index = trgm_in_index % 32;

    if (group > TRGM_SOC_TRIM_IN_GROUP_MAX)
        return -1;

    return ((ptr->TRGM_IN[group] >> index) & 0x01);
}
#endif

#if defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_2) || defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_1)
/**
 * @brief Software calibration of high precision pwm
 *
 * @param trgm @ref TRGM_Type
 * @param status @ref trgm_pwmv2_calibration_mode_t
 *
 */
static inline void trgm_pwmv2_calibrate_delay_chain(TRGM_Type *trgm, trgm_pwmv2_calibration_mode_t *status)
{
#if defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_2)
    uint16_t val_n, val_p;
#endif
    uint32_t delay_val;
#if defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_1)
    static uint16_t last_val = 0;
    int16_t diff_val;
#endif

    switch (*status) {
    case trgm_pwmv2_calibration_mode_begin:
        trgm->PWM_CALIB_CFG &= ~TRGM_PWM_CALIB_CFG_CALIB_HW_ENABLE_MASK;
        trgm->PWM_CALIB_CFG &= ~TRGM_PWM_CALIB_CFG_CALIB_SW_START_MASK;
        trgm->PWM_CALIB_CFG |= TRGM_PWM_CALIB_CFG_CALIB_SW_START_MASK;
        *status = trgm_pwmv2_calibration_mode_wait;
        break;
    case trgm_pwmv2_calibration_mode_wait:
        if (TRGM_PWM_CALIB_STATUS0_CALIB_ON_GET(trgm->PWM_CALIB_STATUS0) == 0U) {
#if defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_2)
            val_n = TRGM_PWM_CALIB_STATUS1_CALIB_RESULT_N_GET(trgm->PWM_CALIB_STATUS1);
            val_p = TRGM_PWM_CALIB_STATUS1_CALIB_RESULT_P_GET(trgm->PWM_CALIB_STATUS1);
            if ((val_n > 3) && (val_p > 3)) {
                delay_val = TRGM_PWM_DELAY_CFG_DELAY_CHAN_CALIB_N_SW_SET(val_n) |
                            TRGM_PWM_DELAY_CFG_DELAY_CHAN_CALIB_P_SW_SET(val_p);
                trgm->PWM_DELAY_CFG = delay_val;
                *status = trgm_pwmv2_calibration_mode_end;
            } else {
                *status = trgm_pwmv2_calibration_mode_begin;
            }
#endif
#if defined(HPM_IP_FEATURE_TRGM_HRPWM_CALIBRATION_1)
            delay_val = TRGM_PWM_CALIB_STATUS0_CALIB_RESULT_GET(trgm->PWM_CALIB_STATUS0);
            if (last_val == 0U) {
                if (delay_val > 10) {
                    last_val = delay_val;
                    trgm->PWM_DELAY_CFG = delay_val;
                    *status = trgm_pwmv2_calibration_mode_end;
                } else {
                    *status = trgm_pwmv2_calibration_mode_begin;
                }
            } else {
                diff_val = ((delay_val - last_val) > 0) ? (delay_val - last_val) : (last_val - delay_val);

                if (((float)diff_val / last_val) > 0.25f) {
                    trgm->PWM_DELAY_CFG = delay_val;
                    last_val = delay_val;
                    *status = trgm_pwmv2_calibration_mode_end;
                } else {
                    *status = trgm_pwmv2_calibration_mode_begin;
                }
            }
#endif
        }
        break;
    case trgm_pwmv2_calibration_mode_end:
    default:
        break;
    }
}
#endif

#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* HPM_TRGM_DRV_H */


