/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_OWR_DRV_H
#define HPM_OWR_DRV_H

#include "hpm_common.h"
#include "hpm_owr_regs.h"

/*---------------------------------------------------------------------
 *  Macro Constant Declarations
 *-------------------------------------------------------------------*/
#ifndef OWR_RPP_TIMEOUT
#define OWR_RPP_TIMEOUT (50000U)
#endif

#ifndef OWR_WR_TIMEOUT
#define OWR_WR_TIMEOUT (50000U)
#endif

/*---------------------------------------------------------------------
 *  Typedef Enum Declarations
 *-------------------------------------------------------------------*/

/** @brief OWR IRQ Type */
typedef enum {
    owr_irq_receive_shift_register_full = OWR_IRQ_STS_RSRF_MASK,
    owr_irq_receive_buff_full = OWR_IRQ_STS_RBF_MASK,
    owr_irq_transmit_shift_register_empty = OWR_IRQ_STS_TEMT_MASK,
    owr_irq_transmit_buffer_empty = OWR_IRQ_STS_TBE_MASK,
    owr_irq_presence_detected = OWR_IRQ_STS_PST_DET_MASK
} owr_irq_t;

/** @brief OWR config struct */
typedef struct {
    uint8_t clock_source_frequency;
} owr_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Software Reset OWR
 *
 * @param[in] ptr      OWR base address
 * @return hpm_stat_t  result of software reset. @ref hpm_stat_t
 */
hpm_stat_t owr_sw_reset(OWR_Type *ptr);

/**
 * @brief   Init OWR
 *
 * @param[in] ptr       OWR base address
 * @param[in] config    pointer to config for initializing OWR. @ref owr_config_t
 * @return hpm_stat_t   result of init. @ref hpm_stat_t
 */
hpm_stat_t owr_init(OWR_Type *ptr, owr_config_t *config);

/**
 * @brief   Reset and presence pulses
 *
 * @param[in] ptr       OWR base address
 * @return hpm_stat_t   result of reset and presence pulses. @ref hpm_stat_t
 */
hpm_stat_t owr_reset_and_presence_pulses(OWR_Type *ptr);

/**
 * @brief   Get presence status
 *
 * @param[in] ptr       OWR base address
 * @param[out] status   pointer to status for storing presence status
 * @return hpm_stat_t   result of getting presence status. @ref hpm_stat_t
 */
hpm_stat_t owr_get_presence_status(OWR_Type *ptr, uint32_t *status);

/**
 * @brief   Release bus
 *
 * @param[in] ptr       OWR base address
 * @return hpm_stat_t   result of releasing bus. @ref hpm_stat_t
 */
hpm_stat_t owr_release_bus(OWR_Type *ptr);

/**
 * @brief   Write data
 *
 * @param[in] ptr       OWR base address
 * @param[in] data      data to write
 * @return hpm_stat_t   result of writing data. @ref hpm_stat_t
 */
hpm_stat_t owr_write_data(OWR_Type *ptr, uint32_t data);

/**
 * @brief   Read data
 *
 * @param[in] ptr       OWR base address
 * @param[out] data     pointer to data for storing read data
 * @return hpm_stat_t   result of reading data. @ref hpm_stat_t
 */
hpm_stat_t owr_read_data(OWR_Type *ptr, uint32_t *data);

/**
 * @brief   Clear irq status
 *
 * @param[in] ptr       OWR base address
 * @param[in] mask      mask of irq status to clear @ref owr_irq_event_t
 * @return hpm_stat_t   result of clearing irq status. @ref hpm_stat_t
 */
hpm_stat_t owr_clear_irq_status(OWR_Type *ptr, uint32_t mask);

/**
 * @brief   Enable interrupts
 *
 * @param[in] ptr       OWR base address
 * @param[in] mask      mask of interrupts to enable @ref owr_irq_event_t
 * @return hpm_stat_t   result of enabling interrupts. @ref hpm_stat_t
 */
hpm_stat_t owr_enable_interrupts(OWR_Type *ptr, uint32_t mask);

/**
 * @brief   Disable interrupts
 *
 * @param[in] ptr       OWR base address
 * @param[in] mask      mask of interrupts to disable @ref owr_irq_event_t
 * @return hpm_stat_t   result of disabling interrupts. @ref hpm_stat_t
 */
hpm_stat_t owr_disable_interrupts(OWR_Type *ptr, uint32_t mask);

/**
 * @brief   Get irq status
 *
 * @param[in] ptr       OWR base address
 * @param[out] status   pointer to status for storing irq status
 * @return hpm_stat_t   result of getting irq status. @ref hpm_stat_t
 */
hpm_stat_t owr_get_irq_status(OWR_Type *ptr, uint32_t *status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HPM_OWR_DRV_H */

