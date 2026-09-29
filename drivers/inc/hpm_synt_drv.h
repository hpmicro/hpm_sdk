/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SYNT_DRV_H
#define HPM_SYNT_DRV_H
#include "hpm_common.h"
#include "hpm_soc_feature.h"
#include "hpm_synt_regs.h"

/**
 * @brief SYNT driver APIs
 * @defgroup synt_interface SYNT driver APIs
 * @ingroup io_interfaces
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable or disable the counter
 *
 * @param [in] ptr SYNT base address
 * @param [in] enable true to enable the counter, false to disable it
 */
static inline void synt_enable_counter(SYNT_Type *ptr, bool enable)
{
    ptr->GCR = (ptr->GCR & ~(SYNT_GCR_CEN_MASK)) | SYNT_GCR_CEN_SET(enable);
}

/**
 * @brief Reset the counter
 *
 * @param [in] ptr SYNT base address
 */
static inline void synt_reset_counter(SYNT_Type *ptr)
{
    ptr->GCR |= SYNT_GCR_CRST_MASK;
    ptr->GCR &= ~SYNT_GCR_CRST_MASK;
}

/**
 * @brief Set a comparator value
 *
 * The comparator output asserts for one clock cycle when the counter reaches
 * the configured value.
 *
 * @param [in] ptr SYNT base address
 * @param [in] cmp_index comparator index; valid range is 0 to 3, or 0 to 15
 * when the SYNT instance supports 16 comparators
 * @param [in] count counter value at which to assert the comparator output
 * @retval status_success comparator value was set successfully
 * @retval status_invalid_argument comparator index is out of range
 */
static inline hpm_stat_t synt_set_comparator(SYNT_Type *ptr, uint8_t cmp_index, uint32_t count)
{
#if defined(HPM_IP_FEATURE_SYNT_CHAN16) && HPM_IP_FEATURE_SYNT_CHAN16
    if (cmp_index > SYNT_CMP_15) {
#else
    if (cmp_index > SYNT_CMP_3) {
#endif
        return status_invalid_argument;
    }
    ptr->CMP[cmp_index] = SYNT_CMP_CMP_SET(count);
    return status_success;
}

/**
 * @brief Set the counter reload value
 *
 * @param [in] ptr SYNT base address
 * @param [in] reload_count counter reload value
 */
static inline void synt_set_reload(SYNT_Type *ptr, uint32_t reload_count)
{
    ptr->RLD = SYNT_RLD_RLD_SET(reload_count);
}

/**
 * @brief Get the current counter value
 *
 * @param [in] ptr SYNT base address
 * @retval Current counter value
 */
static inline uint32_t synt_get_current_count(SYNT_Type *ptr)
{
    return (ptr->CNT & SYNT_CNT_CNT_MASK) >> SYNT_CNT_CNT_SHIFT;
}

#if defined(HPM_IP_FEATURE_SYNT_ONESHOT_MODE) && HPM_IP_FEATURE_SYNT_ONESHOT_MODE
/**
 * @brief Enable or disable one-shot mode
 *
 * When enabled, the counter stops after reaching the reload value. Reset the
 * counter before starting another one-shot operation.
 *
 * @param [in] ptr SYNT base address
 * @param [in] enable true to enable one-shot mode, false for continuous mode
 */
static inline void synt_enable_oneshot_mode(SYNT_Type *ptr, bool enable)
{
    ptr->GCR = (ptr->GCR & ~(SYNT_GCR_TIMER_ONESHOT_MASK)) | SYNT_GCR_TIMER_ONESHOT_SET(enable);
}
#endif

#if defined(HPM_IP_FEATURE_SYNT_TIMESTAMP) && HPM_IP_FEATURE_SYNT_TIMESTAMP

/**
 * @brief Enable or disable the timestamp counter
 *
 * @param [in] ptr SYNT base address
 * @param [in] enable true to enable the timestamp counter, false to stop it
 */
static inline void synt_enable_timestamp(SYNT_Type *ptr, bool enable)
{
    ptr->GCR = (ptr->GCR & ~(SYNT_GCR_TIMESTAMP_ENABLE_MASK)) | SYNT_GCR_TIMESTAMP_ENABLE_SET(enable);
}

/**
 * @brief Configure whether the timestamp counter stops during CPU debug mode
 *
 * @param [in] ptr SYNT base address
 * @param [in] enable true to stop the timestamp counter during CPU debug mode
 */
static inline void synt_enable_timestamp_debug_stop(SYNT_Type *ptr, bool enable)
{
    ptr->GCR = (ptr->GCR & ~(SYNT_GCR_TIMESTAMP_DEBUG_EN_MASK)) | SYNT_GCR_TIMESTAMP_DEBUG_EN_SET(enable);
}

/**
 * @brief Reset the timestamp counter to zero
 *
 * @param [in] ptr SYNT base address
 */
static inline void synt_reset_timestamp(SYNT_Type *ptr)
{
    ptr->GCR |= SYNT_GCR_TIMESTAMP_RESET_MASK;
}

/**
 * @brief Set the timestamp counter to the new value
 *
 * The update command is cleared automatically by hardware.
 *
 * @param [in] ptr SYNT base address
 */
static inline void synt_update_timestamp_new(SYNT_Type *ptr)
{
    ptr->GCR |= SYNT_GCR_TIMESTAMP_SET_NEW_MASK;
}

/**
 * @brief Subtract the new value from the timestamp counter
 *
 * The update command is cleared automatically by hardware.
 *
 * @param [in] ptr SYNT base address
 */
static inline void synt_update_timestamp_dec(SYNT_Type *ptr)
{
    ptr->GCR |= SYNT_GCR_TIMESTAMP_DEC_NEW_MASK;
}

/**
 * @brief Add the new value to the timestamp counter
 *
 * The update command is cleared automatically by hardware.
 *
 * @param [in] ptr SYNT base address
 */
static inline void synt_update_timestamp_inc(SYNT_Type *ptr)
{
    ptr->GCR |= SYNT_GCR_TIMESTAMP_INC_NEW_MASK;
}

/**
 * @brief Set the timestamp value used by update commands
 *
 * The configured value is used to set, increment, or decrement the timestamp
 * counter.
 *
 * @param [in] ptr SYNT base address
 * @param [in] new_value timestamp update value
 */
static inline void synt_set_timestamp_new_value(SYNT_Type *ptr, uint32_t new_value)
{
    ptr->TIMESTAMP_NEW = SYNT_TIMESTAMP_NEW_VALUE_SET(new_value);
}

/**
 * @brief Get the timestamp value captured by an external trigger
 *
 * @param [in] ptr SYNT base address
 * @retval Timestamp value captured when the external trigger was detected
 */
static inline uint32_t synt_get_timestamp_save_value(SYNT_Type *ptr)
{
    return SYNT_TIMESTAMP_SAV_VALUE_GET(ptr->TIMESTAMP_SAV);
}

/**
 * @brief Get the current timestamp value
 *
 * @param [in] ptr SYNT base address
 * @retval Current timestamp value
 */
static inline uint32_t synt_get_timestamp_current_value(SYNT_Type *ptr)
{
    return SYNT_TIMESTAMP_CUR_VALUE_GET(ptr->TIMESTAMP_CUR);
}

#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* HPM_SYNT_DRV_H */
