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

/**
 * @brief OWR driver APIs
 * @defgroup owr_interface OWR driver APIs
 * @{
 */

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

/** @brief OWR IRQ Event Type */
typedef enum {
    owr_irq_receive_shift_register_full = OWR_IRQ_STS_RSRF_MASK,
    owr_irq_receive_buff_full = OWR_IRQ_STS_RBF_MASK,
    owr_irq_transmit_shift_register_empty = OWR_IRQ_STS_TEMT_MASK,
    owr_irq_transmit_buffer_empty = OWR_IRQ_STS_TBE_MASK,
    owr_irq_presence_detected = OWR_IRQ_STS_PST_DET_MASK
} owr_irq_event_t;

/** @brief OWR config struct */
typedef struct {
    uint32_t clock_source_frequency;
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
 * @brief   Write one byte data to OWR bus
 *
 * This function writes one byte of data to the OWR (One-Wire) bus. It clears the transmit shift
 * register empty interrupt status, writes the data to the DATA register, and then polls for
 * the transmit shift register empty interrupt to ensure the data has been transmitted.
 * A dummy read is performed after clearing the interrupt to ensure proper status clearing.
 *
 * @param[in] ptr       OWR base address
 * @param[in] data      One byte data to write to OWR bus (0x00-0xFF)
 * @return hpm_stat_t   Operation result
 *                      - @ref status_success Data written successfully
 *                      - @ref status_timeout Timeout occurred while waiting for transmission
 *                                             completion (exceeded @ref OWR_WR_TIMEOUT)
 */
hpm_stat_t owr_write_data(OWR_Type *ptr, uint8_t data);

/**
 * @brief   Read one byte data from OWR bus
 *
 * This function reads one byte of data from the OWR (One-Wire) bus. It first releases the bus
 * by calling @ref owr_release_bus to send 0xFF, then polls for the receive buffer full interrupt
 * to ensure data has been received. The received data is read from the DATA register and the
 * interrupt status is cleared.
 *
 * @param[in] ptr       OWR base address
 * @param[out] data     Pointer to buffer for storing the read data (must not be NULL)
 * @return hpm_stat_t   Operation result
 *                      - @ref status_success Data read successfully
 *                      - @ref status_invalid_argument data pointer is NULL
 *                      - @ref status_timeout Timeout occurred while waiting for reception
 *                                             completion (exceeded @ref OWR_WR_TIMEOUT)
 */
hpm_stat_t owr_read_data(OWR_Type *ptr, uint8_t *data);

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

/**
 * @}
 */

#endif /* HPM_OWR_DRV_H */

