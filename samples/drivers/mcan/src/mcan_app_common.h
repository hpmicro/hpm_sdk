/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MCAN_APP_COMMON_H
#define MCAN_APP_COMMON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "board.h"
#include "hpm_mcan_drv.h"

/***********************************************************************************************************************
 *
 *  Types
 *
 **********************************************************************************************************************/

/**
 * @brief Per-instance descriptor for each MCAN peripheral on the board.
 *
 * Populated once in s_can_info[] inside mcan_app_common.c.  Every demo file
 * that needs to iterate all CAN instances reads the table through the
 * exported s_can_info[] / g_can_info_count pair.
 */
typedef struct {
    MCAN_Type   *can_base;    /**< Peripheral base address (e.g. HPM_MCAN0)            */
    uint32_t     clock_freq;  /**< Source clock frequency in Hz; filled by main()       */
    uint32_t     irq_num;     /**< INTC IRQ number (e.g. IRQn_MCAN0)                   */
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    uint32_t     ram_base;    /**< Start address of the AHB-SRAM message buffer region  */
    uint32_t     ram_size;    /**< Size in bytes of the AHB-SRAM message buffer region  */
#endif
    clock_name_t clock_name;  /**< Clock subsystem name used for clock configuration    */
} can_info_t;

typedef void (*mcan_isr_callback_t)(MCAN_Type *ptr);

/***********************************************************************************************************************
 *
 *  Shared state (defined in mcan_app_common.c, referenced by all test modules)
 *
 **********************************************************************************************************************/

/** Table of all MCAN instances available on the board (defined in mcan_app_common.c). */
extern can_info_t s_can_info[];
/** Number of entries in s_can_info[] (ARRAY_SIZE equivalent, exported for demo modules). */
extern uint32_t g_can_info_count;

/***********************************************************************************************************************
 *
 *  Utility functions (defined in mcan_app_common.c)
 *
 **********************************************************************************************************************/

/**
 * @brief Display the content of a received CAN message
 *
 * @param [in] rx_msg Pointer to the received CAN message
 */
void show_received_can_message(const mcan_rx_message_t *rx_msg);

/**
 * @brief Format a CAN timestamp value into a hex string
 *
 * @param [in] ts_val Pointer to the timestamp value
 * @return Pointer to a static hex string buffer
 */
char *get_timestamp_hex_string(const mcan_timestamp_value_t *ts_val);

/**
 * @brief Handle (decode and print) CAN protocol errors for the given controller
 *
 * @param [in] ptr MCAN peripheral base address
 */
void handle_can_error(MCAN_Type *ptr);

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
/**
 * @brief Find the can_info_t entry that corresponds to BOARD_APP_CAN_BASE
 *
 * @return Pointer to matching can_info_t, or NULL if not found
 */
can_info_t *find_board_can_info(void);
#endif

/**
 * @brief Configure the MCAN message-buffer AHB RAM attribute for a given
 *        peripheral base address.
 *
 * On SoCs where MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1 this function looks up the
 * matching can_info_t entry, calls mcan_set_msg_buf_attr(), and prints an
 * error message on failure.  On all other SoCs it is a no-op that always
 * returns status_success, so call sites need no #if guard.
 *
 * @param [in] ptr  MCAN peripheral base address
 * @return status_success on success, or the driver error code on failure
 */
hpm_stat_t mcan_setup_msg_buf(MCAN_Type *ptr);

/**
 * @brief Configure the MCAN message-buffer AHB RAM attribute using an
 *        already-resolved can_info_t entry.
 *
 * Useful when iterating s_can_info[] directly (e.g. the all-CANs loopback
 * test) to avoid a redundant lookup.  On non-AHB-RAM SoCs this is a no-op
 * returning status_success.
 *
 * @param [in] info  Pointer to the can_info_t entry for the target peripheral
 * @return status_success on success, or the driver error code on failure
 */
hpm_stat_t mcan_setup_msg_buf_by_info(can_info_t *info);

/***********************************************************************************************************************
 *
 *  Demo functions (one per source file)
 *
 **********************************************************************************************************************/

/* mcan_loopback.c */
bool can_loopback_test(MCAN_Type *base, bool enable_canfd);
void can_loopback_test_for_all_cans(void);
void board_can_loopback_test_in_interrupt_mode(void);

/* mcan_echo.c */
void board_can_echo_test_initiator(void);
void board_can_echo_test_responder(void);

/* mcan_txrx.c */
void board_can_send_multiple_classic_can_messages(void);
void board_can_send_multiple_canfd_messages(void);

/* mcan_filter.c */
void board_can_filter_test(void);

/* mcan_error.c */
void board_can_error_test(void);

/* mcan_rxbuf.c */
void board_can_rxbuf_test(void);

/* mcan_tx_event.c */
void board_can_tx_event_test(void);

/* mcan_timestamp.c */
void board_can_timestamp_test(void);

/* mcan_timeout.c */
void board_can_timeout_counter_test(void);

/* mcan_txcancel.c */
void board_can_cancel_can_message_send(void);

/* mcan_txqueue.c */
void board_can_txqueue_test(void);

/**
 * @brief Atomically install a per-test ISR callback.
 *
 * Each demo function calls this before enabling the INTC so that
 * board_can_isr() dispatches to the correct handler.  Global interrupts are
 * disabled for the duration of the pointer write to prevent a half-written
 * value being observed by a pending interrupt.
 *
 * @param [in] callback  Function to invoke from board_can_isr(), or NULL to
 *                       disable dispatching.
 */
void mcan_install_isr_callback(mcan_isr_callback_t callback);

#endif /* MCAN_APP_COMMON_H */
