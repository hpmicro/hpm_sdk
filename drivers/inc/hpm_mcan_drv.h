/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCAN_DRV_H
#define HPM_MCAN_DRV_H

#include "hpm_common.h"
#include "hpm_mcan_regs.h"
#include "hpm_mcan_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief MCAN driver APIs
* @defgroup mcan_interface MCAN driver APIs
* @ingroup mcan_interfaces
* @{
*
*/

/**
 * @defgroup mcan_quick_start Quick-start example
 * @ingroup mcan_interface
 * @{
 *
 * This example brings up one MCAN instance in internal loopback mode, sends
 * a classic CAN frame, and reads it back from RXFIFO0. Internal loopback does
 * not need a transceiver or a bus connection, so it works on any board. To use
 * a real bus, change `mcan_mode_loopback_internal` to `mcan_mode_normal`.
 *
 * AHB-RAM message buffers (SoC-specific)
 *
 * Some SoCs keep the MCAN message RAM in shared AHB SRAM. When
 * `MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1`, place the buffer in `.ahb_sram` and
 * register it with `mcan_set_msg_buf_attr()` before calling
 * `mcan_get_default_config()` or `mcan_init()`.
 *
 * SoCs with message RAM inside the MCAN IP leave `MCAN_SOC_MSG_BUF_IN_AHB_RAM`
 * undefined or set it to 0. The buffer declaration and registration call then
 * compile out.
 *
 * @code{.c}
 * #include "board.h"
 * #include "hpm_mcan_drv.h"
 *
 * // AHB-RAM SoCs: place the message buffer in the ".ahb_sram" linker section.
 * // On SoCs with IP-internal RAM this declaration compiles out.
 * #if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
 * ATTR_PLACE_AT(".ahb_sram") uint32_t mcan_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
 * #endif
 *
 * void board_can_quickstart(void)
 * {
 *     MCAN_Type *ptr = BOARD_APP_CAN_BASE;
 *     mcan_config_t can_config;
 *     hpm_stat_t status;
 *
 *     // Step 1 - Register the AHB-SRAM message buffer (AHB-RAM SoCs only).
 * #if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
 *     mcan_msg_buf_attr_t attr;
 *     attr.ram_base = (uint32_t) mcan_msg_buf;
 *     attr.ram_size = sizeof(mcan_msg_buf);
 *     status = mcan_set_msg_buf_attr(ptr, &attr);
 *     if (status != status_success) {
 *         printf("CAN message buffer setup failed\n");
 *         return;
 *     }
 * #endif
 *
 *     // Step 2 - Board-level pin-mux and clock configuration.
 *     board_init_can(ptr);
 *     uint32_t can_src_clk_freq = board_init_can_clock(ptr);
 *
 *     // Step 3 - Populate defaults, override what is needed, then init.
 *     mcan_get_default_config(ptr, &can_config);
 *     can_config.baudrate = 1000000;  // 1 Mbps
 *     can_config.mode     = mcan_mode_loopback_internal;
 *     status = mcan_init(ptr, &can_config, can_src_clk_freq);
 *     if (status != status_success) {
 *         printf("CAN initialization failed, error code: %d\n", status);
 *         return;
 *     }
 *
 *     // Step 4 - Build and send a classic CAN frame (blocking).
 *     mcan_tx_frame_t tx_buf;
 *     memset(&tx_buf, 0, sizeof(tx_buf));
 *     tx_buf.std_id = 0x123;
 *     tx_buf.dlc    = 8;
 *     for (uint32_t i = 0; i < 8; i++) {
 *         tx_buf.data_8[i] = (uint8_t) i | (i << 4);
 *     }
 *     status = mcan_transmit_blocking(ptr, &tx_buf);
 *     if (status != status_success) {
 *         printf("CAN sent message failed, error_code:%d\n", status);
 *         return;
 *     }
 *
 *     // Step 5 - Read back the looped-back frame from RXFIFO0 (blocking).
 *     mcan_rx_message_t rx_buf;
 *     status = mcan_receive_from_fifo_blocking(ptr, 0, &rx_buf);
 *     if (status != status_success) {
 *         printf("CAN receive failed, error code: %d\n", status);
 *         return;
 *     }
 *     show_received_can_message(&rx_buf);
 * }
 * @endcode
 *
 * @}
 */

/**
 * @brief MCAN Driver Status Codes
 *
 * Recoverable conditions (retry or wait):
 *   - `rxfifo_empty`, `rxbuf_empty`, `tx_evt_fifo_empty` : no data available yet; poll again later.
 *   - `rxfifo_full`, `txbuf_full`, `txfifo_full`         : resource temporarily exhausted; retry after consuming data.
 *   - `rxfifo0_busy`, `rxfifo1_busy`                      : transient FIFO access contention; retry immediately.
 *   - `timeout`                                           : blocking operation exceeded retry budget; check bus state.
 *
 * Configuration / programming errors (fix the caller):
 *   - `filter_index_out_of_range`, `txbuf_index_out_of_range`, `rxbuf_index_out_of_range`
 *   - `ram_out_of_range`, `invalid_bit_timing`, `txqueue_not_enabled`, `timestamp_not_exist`
 */
enum {
    status_mcan_filter_index_out_of_range = MAKE_STATUS(status_group_mcan, 0),   /*!< Filter element index exceeds the configured standard or extended filter list size */
    status_mcan_rxfifo_empty = MAKE_STATUS(status_group_mcan, 1),                 /*!< The selected RX FIFO does not currently contain a readable frame */
    status_mcan_rxfifo_full = MAKE_STATUS(status_group_mcan, 2),                  /*!< The selected RX FIFO is full and cannot accept additional frames */
    status_mcan_txbuf_full = MAKE_STATUS(status_group_mcan, 3),                   /*!< No dedicated TX buffer slot is currently available for the requested operation */
    status_mcan_txfifo_full = MAKE_STATUS(status_group_mcan, 4),                  /*!< The TX FIFO/queue data area has no free element available */
    status_mcan_rxfifo0_busy = MAKE_STATUS(status_group_mcan, 5),                 /*!< RXFIFO0 is busy and could not complete the requested access atomically */
    status_mcan_rxfifo1_busy = MAKE_STATUS(status_group_mcan, 6),                 /*!< RXFIFO1 is busy and could not complete the requested access atomically */
    status_mcan_txbuf_index_out_of_range = MAKE_STATUS(status_group_mcan, 7),     /*!< The requested TX buffer index exceeds the configured TX buffer range */
    status_mcan_rxbuf_index_out_of_range = MAKE_STATUS(status_group_mcan, 8),     /*!< The requested RX buffer index exceeds the configured RX buffer range */
    status_mcan_rxbuf_empty = MAKE_STATUS(status_group_mcan, 9),                  /*!< The selected RX buffer does not contain unread frame data */
    status_mcan_tx_evt_fifo_empty = MAKE_STATUS(status_group_mcan, 10),           /*!< The TX Event FIFO is empty and no event element can be read */
    status_mcan_timestamp_not_exist = MAKE_STATUS(status_group_mcan, 11),         /*!< The requested timestamp information is not present for the supplied frame or event */
    status_mcan_ram_out_of_range = MAKE_STATUS(status_group_mcan, 12),            /*!< The requested message RAM layout exceeds the valid MCAN message RAM window */
    status_mcan_timeout = MAKE_STATUS(status_group_mcan, 13),                     /*!< The requested blocking operation did not complete within the driver's retry budget */
    status_mcan_invalid_bit_timing = MAKE_STATUS(status_group_mcan, 14),          /*!< The requested nominal or data-phase bit timing cannot be represented by the MCAN hardware */
    status_mcan_txqueue_not_enabled = MAKE_STATUS(status_group_mcan, 15),         /*!< A TX Queue specific API was used while the controller is not configured for TX Queue mode */
};

/**
 * @brief CAN Interrupt Mask
 *
 * Each flag corresponds to one bit in the M_CAN Interrupt Register (IR).
 * OR them together and assign to `mcan_config_t.interrupt_mask` to select which events generate an interrupt.
 *
 * Functional groups:
 *   - Receive:    RF0N, RF1N, DRX (new message in FIFO0, FIFO1, or dedicated RX buffer)
 *   - Transmit:   TC, TCF, TFE  (transmission complete, cancellation finished, TX FIFO empty)
 *   - Error:      BO, EW, EP, BEU, PED, PEA  (bus-off, warning, passive, bit/protocol errors)
 *   - FIFO mgmt:  RF0F, RF1F, RF0W, RF1W, RF0L, RF1L, TEFF, TEFW, TEFN, TEFL  (full/watermark/lost)
 *   - Diagnostic: TSW, TOO, WDI, MRAF, ELO, HPM  (timestamp wrap, timeout, watchdog, RAM access, logging overflow)
 */
#define MCAN_INT_ACCESS_TO_RESERVED_ADDR    MCAN_IR_ARA_MASK     /*!< Access to Reserved Address */
#define MCAN_INT_PROTOCOL_ERR_IN_DATA_PHASE MCAN_IR_PED_MASK     /*!< Protocol Error Happened at Data Phase */
#define MCAN_INT_PROTOCOL_ERR_IN_ARB_PHASE  MCAN_IR_PEA_MASK     /*!< Protocol Error Happened at Arbitration Phase */
#define MCAN_INT_WATCHDOG_INT               MCAN_IR_WDI_MASK     /*!< Watchdog interrupt */

#define MCAN_INT_BUS_OFF_STATUS             MCAN_IR_BO_MASK      /*!< Bus-off State Change */
#define MCAN_INT_WARNING_STATUS             MCAN_IR_EW_MASK      /*!< Error Warning State Change */
#define MCAN_INT_ERROR_PASSIVE              MCAN_IR_EP_MASK      /*!< Error Passive State Change */
#define MCAN_INT_ERROR_LOGGING_OVERFLOW     MCAN_IR_ELO_MASK     /*!< Error Logging Overflow */
#define MCAN_INT_BIT_ERROR_UNCORRECTED      MCAN_IR_BEU_MASK     /*!< Bit Error was not corrected */
#define MCAN_INT_BIT_ERROR_CORRECTED        MCAN_IR_BEC_MASK     /*!< Bit Error was corrected */

#define MCAN_INT_MSG_STORE_TO_RXBUF         MCAN_IR_DRX_MASK    /*!< Message was stored to RX Buffer */
#define MCAN_INT_TIMEOUT_OCCURRED           MCAN_IR_TOO_MASK    /*!< Timeout Interrupt */
#define MCAN_INT_MSG_RAM_ACCESS_FAILURE     MCAN_IR_MRAF_MASK   /*!< Message RAM Access Failure */
#define MCAN_INT_TIMESTAMP_WRAPAROUND       MCAN_IR_TSW_MASK    /*!< Timestamp Counter Wrap Around */

#define MCAN_INT_TX_EVT_FIFO_EVT_LOST       MCAN_IR_TEFL_MASK   /*!< TX Event FIFO Element Lost */
#define MCAN_INT_TX_EVT_FIFO_FULL           MCAN_IR_TEFF_MASK   /*!< TX Event FIFO Full */
#define MCAN_INT_TX_EVT_FIFO_WMK_REACHED    MCAN_IR_TEFW_MASK   /*!< TX Event FIFO Watermark Reached */
#define MCAN_INT_TX_EVT_FIFO_NEW_ENTRY      MCAN_IR_TEFN_MASK   /*!< TX Event FIFO New Entry */

#define MCAN_INT_TXFIFO_EMPTY               MCAN_IR_TFE_MASK    /*!< TX FIFO Empty */
#define MCAN_INT_TX_CANCEL_FINISHED         MCAN_IR_TCF_MASK    /*!< Transmission Cancellation Finished */
#define MCAN_INT_TX_COMPLETED               MCAN_IR_TC_MASK     /*!< Transmission Completed */
#define MCAN_INT_HIGH_PRIORITY_MSG          MCAN_IR_HPM_MASK    /*!< High Priority Message */

#define MCAN_INT_RXFIFO1_MSG_LOST           MCAN_IR_RF1L_MASK   /*!< RX FIFO1 Message Lost */
#define MCAN_INT_RXFIFO1_FULL               MCAN_IR_RF1F_MASK   /*!< RX FIFO1 Full */
#define MCAN_INT_RXFIFO1_WMK_REACHED        MCAN_IR_RF1W_MASK   /*!< RX FIFO1 Watermark Reached */
#define MCAN_INT_RXFIFO1_NEW_MSG            MCAN_IR_RF1N_MASK   /*!< RX FIFO1 New Message */

#define MCAN_INT_RXFIFO0_MSG_LOST           MCAN_IR_RF0L_MASK   /*!< RX FIFO0 Message Lost */
#define MCAN_INT_RXFIFO0_FULL               MCAN_IR_RF0F_MASK   /*!< RX FIFO0 Full */
#define MCAN_INT_RXFIFO0_WMK_REACHED        MCAN_IR_RF0W_MASK   /*!< RX FIFO0 Watermark Reached */
#define MCAN_INT_RXFIFO0_NEW_MSG            MCAN_IR_RF0N_MASK   /*!< RX FIFO0 New Message */

/**
 * @brief MCAN Receive Event Flags
 */
#define MCAN_EVENT_RECEIVE (MCAN_INT_RXFIFO0_NEW_MSG | MCAN_INT_RXFIFO1_NEW_MSG | MCAN_INT_MSG_STORE_TO_RXBUF)

/**
 * @brief MCAN Transmit Event Flags
 */
#define MCAN_EVENT_TRANSMIT (MCAN_INT_TX_COMPLETED)
/**
 * @brief MCAN Error Event Flags
 */
#define MCAN_EVENT_ERROR (MCAN_INT_BUS_OFF_STATUS | MCAN_INT_WARNING_STATUS \
                        | MCAN_INT_ERROR_PASSIVE | MCAN_INT_BIT_ERROR_UNCORRECTED \
                        | MCAN_INT_PROTOCOL_ERR_IN_DATA_PHASE | MCAN_INT_PROTOCOL_ERR_IN_ARB_PHASE)

/**
 * @brief Maximum Transmission Retry Count
 */
#define MCAN_TX_RETRY_COUNT_MAX (10000000UL)
/**
 * @brief Maximum Receive Wait Retry Count
 */
#define MCAN_RX_RETRY_COUNT_MAX (80000000UL)

/**
 * @brief MCAN Last Error Code
 *
 * Reports the last CAN bus error recorded by the protocol engine in
 * PSR.LEC or PSR.DLEC. Hardware updates this value on bus errors, but a
 * successful frame does not clear it. Read it after an error interrupt and
 * treat stale values accordingly.
 */
typedef enum mcan_last_error_code {
    mcan_last_error_code_no_error = 0,      /*!< No error has occurred since the last CPU read of PSR */
    mcan_last_error_code_stuff_error,       /*!< Stuff Error: more than 5 consecutive equal bits detected */
    mcan_last_error_code_format_error,      /*!< Format Error: a fixed-form bit field contained an illegal value */
    mcan_last_error_code_ack_error,         /*!< Acknowledge Error: no other node acknowledged the transmitted frame */
    mcan_last_error_code_bit1_error,        /*!< Bit1 Error: node sent recessive (1) but monitored dominant (0) */
    mcan_last_error_code_bit0_error,        /*!< Bit0 Error: node sent dominant (0) but monitored recessive (1)
                                             *   (excludes arbitration field, where this is normal) */
    mcan_last_error_code_crc_error,         /*!< CRC Error: the CRC checksum of a received message was incorrect */
    mcan_last_error_code_no_change,         /*!< No Change: the LEC field has not been written by the protocol
                                             *   engine since the last CPU read of PSR. This does NOT indicate
                                             *   error-free operation; it only means no new error was detected. */
} mcan_last_err_code_t;

/**
 * @brief MCAN Communication State
 *
 * Read-only bus activity state from the Protocol Status Register (PSR.ACT).
 * Indicates the current role of the CAN node on the bus.
 */
typedef enum mcan_activity_enum {
    mcan_activity_sync = 0,                 /*!< Synchronizing: the node is attempting to synchronize with
                                             *   CAN bus communication (initial state after reset or bus-off recovery) */
    mcan_activity_idle,                     /*!< Idle: no bus activity; the node is ready to transmit or receive */
    mcan_activity_receiver,                 /*!< Receiver: the node is currently receiving a CAN frame */
    mcan_activity_transmitter,              /*!< Transmitter: the node is currently transmitting a CAN frame */
} mcan_activity_state_t;

/**
 * @brief MCAN Error State
 *
 * CAN protocol error states are determined by the Transmit Error Counter (TEC) and
 * Receive Error Counter (REC). The controller transitions automatically between states
 * based on the counter values. Read via `mcan_get_error_state()`.
 */
typedef enum mcan_error_state_enum {
    mcan_error_state_active = 0,    /*!< Error Active: TEC < 96 and REC < 96. Normal operating state;
                                     *   the node can send active error flags. */
    mcan_error_state_warning,       /*!< Error Warning: TEC >= 96 or REC >= 96. The node is still active
                                     *   but error counters are approaching the passive threshold. */
    mcan_error_state_passive,       /*!< Error Passive: TEC >= 128 or REC >= 128. The node sends passive
                                     *   error flags and must wait before retransmitting after an error. */
    mcan_error_state_bus_off,       /*!< Bus-Off: TEC >= 256. The node is disconnected from the bus and
                                     *   cannot transmit. Recovery requires detecting 128 occurrences of
                                     *   11 consecutive recessive bits. See `mcan_recover_from_busoff()`. */
} mcan_error_state_t;

/***********************************************************************************************************************
 * @brief Default CAN RAM definitions
 **********************************************************************************************************************/
#define MCAN_RAM_WORD_SIZE  (640U)                                  /*!< Default MCAN message RAM size in 32-bit words
                                                                     *   (640 words = 2560 bytes). This value applies to
                                                                     *   SoCs with built-in message RAM inside the MCAN IP
                                                                     *   (e.g., HPM6200). On SoCs where all MCAN instances
                                                                     *   share external AHB SRAM (`MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1`),
                                                                     *   the effective message RAM size is determined by the
                                                                     *   application's AHB buffer allocation, not by this constant. */
/* CAN Filter Element Size Definitions */
#define MCAN_FILTER_ELEM_STD_ID_SIZE (4U)                           /*!< Standard Filter Element Size in Bytes (1 word) */
#define MCAN_FILTER_ELEM_EXT_ID_SIZE (8U)                           /*!< Extended Filter Element Size in Bytes (2 words, because the 29-bit ID needs more encoding space) */
#define MCAN_STD_FILTER_ELEM_SIZE_MAX (128U)                        /*!< Maximum Standard Filter Element Count */
#define MCAN_EXT_FILTER_ELEM_SIZE_MAX  (64U)                        /*!< Maximum Extended Filter Element Count */
/* MCAN Default Field Size Definitions for CAN2.0 */
#define MCAN_TXRX_ELEM_SIZE_CAN_MAX (4U * sizeof(uint32_t))        /*!< Classic CAN: 8-byte header + 8-byte payload = 16 bytes */
#define MCAN_FILTER_SIZE_CAN_DEFAULT (32U)
#define MCAN_TXBUF_SIZE_CAN_DEFAULT (32U)
#define MCAN_RXFIFO_SIZE_CAN_DEFAULT (32U)
#define MCAN_RXBUF_SIZE_CAN_DEFAULT (16U)
/* MCAN Default Field Size Definitions for CANFD */
#define MCAN_TXRX_ELEM_SIZE_CANFD_MAX (18U * sizeof(uint32_t))     /*!< CAN FD: 8-byte header + 64-byte payload = 72 bytes */
#define MCAN_FILTER_SIZE_CANFD_DEFAULT (16U)
#define MCAN_TXBUF_SIZE_CANFD_DEFAULT (8U)
#define MCAN_RXFIFO_SIZE_CANFD_DEFAULT (8U)
#define MCAN_RXBUF_SIZE_CANFD_DEFAULT (4U)

#define MCAN_TXEVT_ELEM_SIZE (8U)                                   /*!< TX Event FIFO element size: 8 bytes (2 words) */

/**
 * @brief CAN Bit Timing Parameters
 *
 * Fields use the values a developer would enter in a timing calculator. The
 * driver subtracts 1 before writing fields whose hardware encoding is N - 1.
 */
typedef struct mcan_bit_timing_param_struct {
    uint16_t prescaler; /*!< Bit-rate prescaler.
                         *   CAN 2.0 / CANFD nominal: 1-512  (NBTP.NBRP = prescaler - 1).
                         *   CANFD data:               1-32   (DBTP.DBRP = prescaler - 1).
                         *   When TDC is enabled on CANFD data, must be ≤ 2. */
    uint16_t num_seg1;  /*!< Time segment 1 before the sample point (prop_seg + phase_seg1,
                         *   excluding the implicit Sync_Seg).
                         *   CAN 2.0 / CANFD nominal: 1-256  (NBTP.NTSEG1 = num_seg1 - 1).
                         *   CANFD data:               1-32   (DBTP.DTSEG1 = num_seg1 - 1). */
    uint16_t num_seg2;  /*!< Time segment 2 after the sample point (phase_seg2).
                         *   CAN 2.0 / CANFD nominal: 2-128  (NBTP.NTSEG2 = num_seg2 - 1).
                         *   CANFD data:               2-16   (DBTP.DTSEG2 = num_seg2 - 1). */
    uint8_t num_sjw;    /*!< (Re)Synchronisation Jump Width.
                         *   CAN 2.0 / CANFD nominal: 1-128  (NBTP.NSJW  = num_sjw - 1).
                         *   CANFD data:               1-16   (DBTP.DSJW  = num_sjw - 1). */
    bool enable_tdc;    /*!< Enable Transmitter Delay Compensation (TDC), applicable to
                         *   CANFD data bit timing only. When set, prescaler must be ≤ 2. */
} mcan_bit_timing_param_t;

/**
 * @brief CAN Bit Timing Options
 *
 * CAN FD uses two bit-timing settings:
 *   - Nominal timing applies to arbitration, and to the whole frame in classic CAN 2.0.
 *   - Data timing applies only to the CAN FD data phase after BRS.
 */
typedef enum mcan_bit_timing_option {
    mcan_bit_timing_can2_0,          /**< Classic CAN 2.0 bit timing (same register as CANFD nominal) */
    mcan_bit_timing_canfd_nominal,   /**< CAN FD arbitration-phase (nominal) bit timing */
    mcan_bit_timing_canfd_data,      /**< CAN FD data-phase bit timing (higher rate, after BRS) */
} mcan_bit_timing_option_t;

/**
 * @brief CAN Message Header Size
 */
#define MCAN_MESSAGE_HEADER_SIZE_IN_BYTES (8U)
#define MCAN_MESSAGE_HEADER_SIZE_IN_WORDS (2U)

/**
 * @brief CAN Transmit Message Frame
 *
 * This structure is a memory-mapped image of the M_CAN TX Buffer Element as defined
 * by the Bosch M_CAN User Manual (Section 2.4.5).
 *   - Word 0: CAN identifier + control flags (RTR, XTD, ESI).
 *   - Word 1: DLC, BRS, FDF, message marker, and event-FIFO / timestamp controls.
 *   - Words 2..17: Payload data (up to 64 bytes for CAN FD).
 *
 * @note CAN Identifier usage:
 *       - Standard ID (11-bit): set `use_ext_id = 0`, write the ID into `std_id`.
 *       - Extended ID (29-bit): set `use_ext_id = 1`, write the ID into `ext_id`.
 */
typedef struct mcan_tx_message_struct {
    union {
        struct {
            uint32_t ext_id: 29;                        /*!< Extended CAN Identifier (29-bit). Only valid when `use_ext_id = 1`. */
            uint32_t rtr: 1;                            /*!< Remote Transmission Request. Set to 1 to send a remote frame
                                                         *   (classic CAN only; ignored when `canfd_frame = 1`). */
            uint32_t use_ext_id: 1;                     /*!< Extended Identifier selector. 0 = standard 11-bit ID, 1 = extended 29-bit ID. */
            uint32_t error_state_indicator: 1;          /*!< Error State Indicator. In CAN FD, set to 1 to signal that the
                                                         *   transmitting node is in error-passive state. Ignored in classic CAN. */
        };
        struct {
            uint32_t : 18;
            uint32_t std_id: 11;                        /*!< Standard CAN Identifier (11-bit). Only valid when `use_ext_id = 0`. */
            uint32_t : 3;
        };
    };
    struct {
        uint32_t : 8;
        uint32_t message_marker_h: 8;                   /*!< Message Marker high byte [15:8]. Only valid when
                                                         *   `mcan_config_t.enable_wide_message_marker = true` (16-bit marker mode).
                                                         *   Otherwise only `message_marker_l` (8-bit) is used. */
        uint32_t dlc: 4;                                /*!< Data Length Code. Values 0-8 map directly to 0-8 bytes.
                                                         *   In CAN FD mode, values 9-15 map to 12,16,20,24,32,48,64 bytes.
                                                         *   See @ref MCAN_MSG_DLC_0_BYTES and related macros. */
        uint32_t bitrate_switch: 1;                     /*!< Bit Rate Switch. Set to 1 to transmit the data phase at the
                                                         *   higher CAN FD data baudrate. Only meaningful when `canfd_frame = 1`. */
        uint32_t canfd_frame: 1;                        /*!< CAN FD Format indicator. 0 = classic CAN frame, 1 = CAN FD frame. */
        uint32_t timestamp_capture_enable: 1;           /*!< Timestamp Capture Enable for TSU. Set to 1 to request the
                                                         *   Timestamp Unit (TSU) to capture a hardware timestamp on transmission.
                                                         *   Requires `mcan_config_t.use_timestamping_unit = true`. */
        uint32_t event_fifo_control: 1;                 /*!< Event FIFO Control. Set to 1 to store a TX Event FIFO entry upon
                                                         *   successful transmission. The entry echoes the `message_marker`
                                                         *   value for application-layer TX-complete correlation. */
        uint32_t message_marker_l: 8;                   /*!< Message Marker low byte [7:0]. Application-defined tag echoed
                                                         *   back in the TX Event FIFO entry when `event_fifo_control = 1`. */
    };
    union {
        uint8_t data_8[64];                             /*!< Data buffer as byte array (up to 64 bytes for CAN FD) */
        uint32_t data_32[16];                           /*!< Data buffer as word array (up to 16 words for CAN FD) */
    };
} mcan_tx_frame_t;

/**
 * @brief CAN Receive Message Frame
 *
 * This structure is a memory-mapped image of the M_CAN RX Buffer / FIFO Element.
 *   - Word 0: CAN identifier + control flags (RTR, XTD, ESI), populated by hardware on reception.
 *   - Word 1 low half: timestamp or TSU pointer. Word 1 high half: DLC, filter match info.
 *   - Words 2..17: Received payload data (length determined by DLC).
 *
 * @note CAN Identifier usage:
 *       - Standard ID (11-bit): `use_ext_id == 0`, read `std_id`.
 *       - Extended ID (29-bit): `use_ext_id == 1`, read `ext_id`.
 */
typedef struct mcan_rx_message_struct {
    union {
        struct {
            uint32_t ext_id: 29;                   /*!< Extended CAN Identifier (29-bit). Valid when `use_ext_id == 1`. */
            uint32_t rtr: 1;                       /*!< Remote Frame Flag. 1 = remote frame (classic CAN only). */
            uint32_t use_ext_id: 1;                /*!< Extended Identifier flag. 0 = standard 11-bit, 1 = extended 29-bit. */
            uint32_t error_state_indicator: 1;     /*!< Error State Indicator. In CAN FD, 1 indicates the transmitting
                                                    *   node was in error-passive state. Always 0 in classic CAN. */
        };
        struct {
            uint32_t : 18;
            uint32_t std_id: 11;                   /*!< Standard CAN Identifier (11-bit). Valid when `use_ext_id == 0`. */
            uint32_t : 3;
        };
    };
    struct {
        union {
            uint16_t rx_timestamp;                  /*!< Received timestamp (16-bit internal counter value).
                                                     *   Valid when using the internal timestamp counter
                                                     *   (`MCAN_TIMESTAMP_SEL_VALUE_INCREMENT`). */
            struct {
                uint16_t rx_timestamp_pointer: 4;   /*!< TSU Timestamp Pointer. Index into the TSU timestamp register
                                                     *   array. Only valid when `rx_timestamp_captured == 1`. */
                uint16_t rx_timestamp_captured: 1;  /*!< Timestamp Captured flag. 1 = the TSU successfully captured a
                                                     *   timestamp; read `rx_timestamp_pointer` to locate it. */
                uint16_t : 11;
            };
        };
    };
    struct {
        uint16_t dlc: 4;                            /*!< Data Length Code. See @ref MCAN_MSG_DLC_0_BYTES. */
        uint16_t bitrate_switch: 1;                 /*!< Bit Rate Switch. 1 = data phase was sent at the higher FD rate. */
        uint16_t canfd_frame: 1;                    /*!< CAN FD Frame flag. 0 = classic CAN, 1 = CAN FD. */
        uint16_t : 2;
        uint16_t filter_index: 7;                   /*!< Index of the filter element (within the standard or extended
                                                     *   filter list) that accepted this frame. Combine with
                                                     *   `use_ext_id` to identify the exact filter list. */
        uint16_t accepted_non_matching_frame: 1;    /*!< Non-matching acceptance flag. Set by hardware when no filter
                                                     *   element matched, but the global filter configuration directed
                                                     *   non-matching frames to an RX FIFO instead of rejecting them.
                                                     *   See @ref mcan_global_filter_config_t. */
    };
    union {
        uint8_t data_8[64];                         /*!< Data buffer as byte array (up to 64 bytes for CAN FD) */
        uint32_t data_32[16];                       /*!< Data buffer as word array (up to 16 words for CAN FD) */
    };
} mcan_rx_message_t;

/**
 * @brief TX Event FIFO Element Structure
 *
 * After a successful transmission of a frame that had `event_fifo_control = 1`,
 * the M_CAN hardware writes a 2-word event element into the TX Event FIFO.
 * This element echoes the CAN ID, DLC, and the application-defined `message_marker`
 * so that software can correlate completed transmissions. Read via `mcan_read_tx_evt_fifo()`.
 */
typedef union mcan_tx_event_fifo_elem_struct {
    struct {
        /* First word */
        union {
            struct {
                uint32_t ext_id: 29;                    /*!< CAN Identifier */
                uint32_t rtr: 1;                        /*!< Remote Transmission Request */
                uint32_t extend_id: 1;                  /*!< Extended Identifier */
                uint32_t error_state_indicator: 1;      /*!< Error State Indicator */
            };
            struct {
                uint32_t : 18;
                uint32_t std_id: 11;
                uint32_t : 3;
            };
        };

        /* first 16-bit of the 2nd word */
        union {
            uint16_t tx_timestamp;                      /*!< Tx Timestamp */
            struct {
                uint16_t tx_timestamp_pointer: 4;       /*!< TX timestamp pointer */
                uint16_t tx_timestamp_captured: 1;      /*!< TX timestamp captured flag */
                uint16_t : 11;
            };
        };
        /* high-half 16-bit of the 2nd word */
        struct {
            uint16_t dlc: 4;                            /*!< Data length code */
            uint16_t bitrate_switch: 1;                 /*!< Bitrate Switch flag */
            uint16_t canfd_frame: 1;                    /*!< CANFD frame */
            uint16_t event_type: 2;                     /*!< Event type: 0 = reserved, 1 = TX event,
                                                         *   2 = TX in spite of cancellation. */
            uint16_t message_marker: 8;                 /*!< Message Marker */
        };
    };
    uint32_t words[2];
} mcan_tx_event_fifo_elem_t;



/**
 * @brief CAN Identifier Types
 */
#define MCAN_CAN_ID_TYPE_STANDARD (0U)      /*!< Standard Identifier */
#define MCAN_CAN_ID_TYPE_EXTENDED (1U)      /*!< Extended Identifier */

/**
 * @brief MCAN Filter Configuration
 *
 * Describes the location and size of a filter element list within the MCAN message RAM.
 * Typically populated automatically by `mcan_init()` when using `mcan_ram_config_t`.
 */
typedef union mcan_filter_config_struct {
    struct {
        uint16_t list_start_addr;       /*!< Byte offset of the filter list from the start of MCAN
                                         *   message RAM. Must be 32-bit word-aligned. */
        uint16_t list_size;             /*!< Number of filter elements in this list */
    };
    uint32_t reg_val;                   /*!< Raw register value */
} mcan_filter_config_t;

/**
 * @brief MCAN RXFIFO Configuration
 *
 * Describes the location, size, and behavior of an RX FIFO within the MCAN message RAM.
 * Typically populated automatically by `mcan_init()` when using `mcan_ram_config_t`.
 */
typedef union mcan_rxfifo_config_struct {
    struct {
        uint32_t start_addr: 16;        /*!< Byte offset from the start of MCAN message RAM.
                                         *   Must be 32-bit word-aligned. */
        uint32_t fifo_size: 8;          /*!< Number of FIFO elements (0-64). Determines how many
                                         *   frames the FIFO can hold before overflow. */
        uint32_t watermark: 7;          /*!< FIFO watermark level (0-127). When the FIFO fill level
                                         *   reaches this value, the watermark interrupt fires
                                         *   (MCAN_INT_RXFIFO0_WMK_REACHED / MCAN_INT_RXFIFO1_WMK_REACHED).
                                         *   Set to 0 to disable the watermark feature. */
        uint32_t operation_mode: 1;     /*!< FIFO operation mode. 0 = blocking (FIFO full stops writes),
                                         *   1 = overwrite (newest frame replaces oldest on overflow).
                                         *   See @ref MCAN_FIFO_OPERATION_MODE_BLOCKING. */
    };
    uint32_t reg_val;                   /*!< Raw register value */
} mcan_rxfifo_config_t;

/**
 * @brief MCAN RXBUF Configuration
 */
typedef struct {
    uint32_t start_addr;                /*!< Byte offset of the dedicated RX buffer region from the start
                                         *   of MCAN message RAM. Must be 32-bit word-aligned. */
} mcan_rxbuf_config_t;


/**
 * @brief MCAN Data Length Code definitions
 *
 * DLC is the per-frame payload length indicator carried inside each CAN frame on the bus.
 *   - Classic CAN: DLC codes 0-8 map linearly to 0-8 payload bytes. Codes above 8 are invalid.
 *   - CAN FD:      DLC codes 0-8 are identical to classic CAN. Codes 9-15 map to
 *                   12, 16, 20, 24, 32, 48, and 64 bytes respectively (non-linear).
 *
 * @note DLC is distinct from @ref MCAN_DATA_FIELD_SIZE_8BYTES "Data Field Size" (below).
 *       Data Field Size configures the maximum payload capacity allocated per element
 *       in message RAM and determines memory consumption. DLC must not exceed the
 *       configured data field size for the buffer or FIFO in use.
 */
#define MCAN_MSG_DLC_0_BYTES    (0U)
#define MCAN_MSG_DLC_1_BYTES    (1U)
#define MCAN_MSG_DLC_2_BYTES    (2U)
#define MCAN_MSG_DLC_3_BYTES    (3U)
#define MCAN_MSG_DLC_4_BYTES    (4U)
#define MCAN_MSG_DLC_5_BYTES    (5U)
#define MCAN_MSG_DLC_6_BYTES    (6U)
#define MCAN_MSG_DLC_7_BYTES    (7U)
#define MCAN_MSG_DLC_8_BYTES    (8U)
#define MCAN_MSG_DLC_12_BYTES   (9U)
#define MCAN_MSG_DLC_16_BYTES   (10U)
#define MCAN_MSG_DLC_20_BYTES   (11U)
#define MCAN_MSG_DLC_24_BYTES   (12U)
#define MCAN_MSG_DLC_32_BYTES   (13U)
#define MCAN_MSG_DLC_48_BYTES   (14U)
#define MCAN_MSG_DLC_64_BYTES   (15U)

/**
 * @brief MCAN Data Field Size Definitions
 *
 * Data Field Size is a per-buffer/FIFO configuration that sets the maximum payload capacity
 * (in bytes) allocated for each element in message RAM. Larger values consume more RAM per
 * element. Classic CAN applications typically use @ref MCAN_DATA_FIELD_SIZE_8BYTES; CAN FD
 * applications that need full 64-byte payloads use @ref MCAN_DATA_FIELD_SIZE_64BYTES.
 */
#define MCAN_DATA_FIELD_SIZE_8BYTES     (0U)
#define MCAN_DATA_FIELD_SIZE_12BYTES    (1U)
#define MCAN_DATA_FIELD_SIZE_16BYTES    (2U)
#define MCAN_DATA_FIELD_SIZE_20BYTES    (3U)
#define MCAN_DATA_FIELD_SIZE_24BYTES    (4U)
#define MCAN_DATA_FIELD_SIZE_32BYTES    (5U)
#define MCAN_DATA_FIELD_SIZE_48BYTES    (6U)
#define MCAN_DATA_FIELD_SIZE_64BYTES    (7U)

/**
 * @brief MCAN FIFO Operation Mode types
 */
#define MCAN_FIFO_OPERATION_MODE_BLOCKING (0U)
#define MCAN_FIFO_OPERATION_MODE_OVERWRITE (1U)

/**
 * @brief  MCAN RXBUF or RXFIFO Element Configuration
 */
typedef union mcan_rx_fifo_or_buf_elem_config_struct {
    struct {
        uint32_t fifo0_data_field_size: 4;      /*!< FIFO0 data field size option */
        uint32_t fifo1_data_field_size: 4;      /*!< FIFO1 data field size option */
        uint32_t buf_data_field_size: 4;        /*!< Buffer Data field size option */
        uint32_t : 20;
    };
    uint32_t reg_val;                           /*!< Register value */
} mcan_rx_fifo_or_buf_elem_config_t;

/**
 * @brief MCAN TXBUF operation mode types
 */
#define MCAN_TXBUF_OPERATION_MODE_FIFO  (0U) /*!< TX FIFO mode */
#define MCAN_TXBUF_OPERATION_MODE_QUEUE (1U) /*!< TX Queue mode */


typedef union mcan_txbuf_config_struct {
    struct {
        uint32_t start_addr: 16;                /*!< Start address (CAN Message Buffer Offset) */
        uint32_t dedicated_tx_buf_size: 6;      /*!< Dedicated TX buffer size */
        uint32_t : 2;
        uint32_t fifo_queue_size: 6;            /*!< FIFO or Queue Size */
        uint32_t tx_fifo_queue_mode: 1;         /*!< FIFO or Queue mode selection */
        uint32_t : 1;
    };
    uint32_t reg_val;                           /*!< register value */
} mcan_txbuf_config_t;

typedef struct mcan_txbuf_elem_config_struct {
    uint32_t data_field_size;                   /*!< Data Field size option */
} mcan_txbuf_elem_config_t;

/**
 * @brief MCAN TX Event FIFO Structure
 */
typedef union {
    struct {
        uint16_t start_addr;                    /*!< Start Address(CAN Message Buffer Offset */
        uint8_t fifo_size;                      /*!< FIFO element count */
        uint8_t fifo_watermark;                 /*!< FIFO watermark */
    };
    uint32_t reg_val;                           /*!< register value */
} mcan_tx_evt_fifo_config_t;

/**
 * @brief MCAN RAM Flexible Configuration
 *
 * @note This structure exposes the full MCAN RAM layout. Use it only when the
 *       simplified RAM configuration cannot describe the layout you need.
 */
typedef struct mcan_ram_flexible_config_struct {
    bool enable_std_filter;                             /*!< Enable Standard Identifier Filter */
    bool enable_ext_filter;                             /*!< Enable Extended Identifier Filter */
    bool enable_rxfifo0;                                /*!< Enable RXFIFO */
    bool enable_rxfifo1;                                /*!< Enable RXFIF1 */
    bool enable_rxbuf;                                  /*!< Enable RXBUF */
    bool enable_txbuf;                                  /*!< Enable TXBUF */
    bool enable_tx_evt_fifo;                            /*!< Enable TX Event FIFO */

    mcan_filter_config_t std_filter_config;             /*!< Standard Identifier Filter Configuration */
    mcan_filter_config_t ext_filter_config;             /*!< Extended Identifier Filter Configuration */

    mcan_txbuf_config_t txbuf_config;                   /*!< TXBUF Configuration */
    mcan_txbuf_elem_config_t txbuf_elem_config;         /*!< TXBUF Element Configuration */
    mcan_tx_evt_fifo_config_t tx_evt_fifo_config;       /*!< TX Event FIFO Configuration */

    mcan_rxfifo_config_t rxfifo0_config;                /*!< RXFIFO0 Configuration */
    mcan_rxfifo_config_t rxfifo1_config;                /*!< RXFIFO1 Configuration */
    mcan_rxbuf_config_t rxbuf_config;                   /*!< RXBUF Configuration */
    uint8_t rxbuf_elem_count;                           /*!< RXBUF element count for software RAM layout validation */
    mcan_rx_fifo_or_buf_elem_config_t rx_elem_config;   /*!< RX Element Configuration */
} mcan_ram_flexible_config_t;

/**
 * @brief MCAN RAM configuration
 *
 * @note This structure contains the fields most applications need. The driver
 *       calculates each buffer and FIFO start address during initialization.
 *
 * @note The total message RAM consumed by all enabled sections must not exceed the available
 *       message RAM. On SoCs with built-in MCAN RAM (e.g., HPM6200), this is fixed at
 *       @ref MCAN_RAM_WORD_SIZE words (2560 bytes). On SoCs where MCAN instances share a
 *       32 KB AHB SRAM (`MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1`), the limit depends on the
 *       application's buffer allocation. The driver validates this constraint inside
 *       `mcan_init()`. Use `mcan_get_default_ram_config()` to generate a valid layout
 *       automatically.
 */
typedef struct mcan_ram_config_struct {
    bool enable_std_filter;                             /*!< Enable Standard Identifier Filter */
    uint8_t std_filter_elem_count;                      /*!< Standard Identifier Filter Element Count */

    bool enable_ext_filter;                             /*!< Enable Extended Identifier Filter */
    uint8_t ext_filter_elem_count;                      /*!< Extended Identifier Filter Element Count */

    struct {
        uint32_t enable: 4;                            /*!< Enable flag for this RXFIFO. Treated as boolean:
                                                         *   0 = RXFIFO disabled, non-zero (typically 1) = enabled.
                                                         *   Declared as 4-bit field for alignment; only bit 0 is significant. */
        uint32_t elem_count: 8;                        /*!< Element Count for RXFIFO */
        uint32_t watermark: 8;                         /*!< Watermark for RXFIFO */
        uint32_t operation_mode: 4;                    /*!< Operation Mode for RXFIFO */
        uint32_t data_field_size: 8;                   /*!< Data field size option for RXFIFO */
    } rxfifos[2];

    bool enable_rxbuf;                                  /*!< Enable RXBUF */
    uint8_t rxbuf_elem_count;                           /*!< RXBUF Element count */
    uint16_t rxbuf_data_field_size;                     /*!< RXBUF Data Field Size option */

    bool enable_txbuf;                                  /*!< Enable TXBUF */
    uint8_t txbuf_data_field_size;                      /*!< TXBUF Data Field Size option */
    uint8_t txbuf_dedicated_txbuf_elem_count;           /*!< Dedicated TXBUF element count */
    uint8_t txbuf_fifo_or_queue_elem_count;             /*!< FIFO/Queue element count */
    uint8_t txfifo_or_txqueue_mode;                     /*!< TXFIFO/QUEUE mode */

    bool enable_tx_evt_fifo;                            /*!< Enable TX Event FIFO */
    uint8_t tx_evt_fifo_elem_count;                     /*!< TX Event FIFO Element count */
    uint8_t tx_evt_fifo_watermark;                      /*!< TX Event FIFO Watermark */
} mcan_ram_config_t;

/**
 * @brief MCAN Accept Non-Matching Frame options
 */
#define MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO0 (0U)   /*!< Save non-matching frame to RXFIFO0 */
#define MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO1 (1U)   /*!< Save non-matching frame to RXFIFO1 */
#define MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT (2U)       /*!< Reject non-matching frame */

/**
 * @brief MCAN Global CAN Filter configuration
 *
 * Controls the default disposition of frames that do not match any element in the
 * standard or extended filter lists. Also controls whether remote frames are accepted.
 */
typedef struct mcan_global_filter_config_struct {
    uint8_t accept_non_matching_std_frame_option;   /*!< Action for standard frames that match no filter.
                                                     *   See @ref MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO0. */
    uint8_t accept_non_matching_ext_frame_option;   /*!< Action for extended frames that match no filter.
                                                     *   See @ref MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO0. */
    bool reject_remote_std_frame;                   /*!< Reject all remote frames with standard IDs (CAN 2.0 feature;
                                                     *   remote frames do not exist in CAN FD). */
    bool reject_remote_ext_frame;                   /*!< Reject all remote frames with extended IDs. */
} mcan_global_filter_config_t;

/**
 * @brief MCAN Filter type definitions
 */
#define MCAN_FILTER_TYPE_RANGE_FILTER (0U)                      /*!< CAN Identifier Range filter */
#define MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER (1U)               /*!< CAN Identifier ID filter */
#define MCAN_FILTER_TYPE_CLASSIC_FILTER (2U)                    /*!< CAN classic filter */
#define MCAN_FILTER_TYPE_FILTER_DISABLED (3U)                   /*!< CAN filter disabled */
#define MCAN_FILTER_TYPE_DUAL_ID_EXT_FILTER_IGNORE_MASK (4U)    /*!< CAN Identifier Range filter, ignoring extended ID mask */

/**
 * @brief MCAN Filter Configuration Options
 */
#define MCAN_FILTER_ELEM_CFG_DISABLED (0)                                   /*!<  Disable Filter Element */
#define MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH (1U)                /*!< Store data into RXFIFO0 if matching */
#define MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO1_IF_MATCH (2U)                /*!< Store data into RXFIFO1 if matching */
#define MCAN_FILTER_ELEM_CFG_REJECT_ID_IF_MATCH (3U)                        /*!< Reject ID if matching */
#define MCAN_FILTER_ELEM_CFG_SET_PRIORITY_IF_MATCH (4U)                     /*!< Set Priority if matching */
/*!< Set Priority and store into RXFIFO0 if matching */
#define MCAN_FILTER_ELEM_CFG_SET_PRIORITY_AND_STORE_IN_FIFO0_IF_MATCH (5U)
/*!< Set Priority and store into RXFIFO1 if matching */
#define MCAN_FILTER_ELEM_CFG_SET_PRIORITY_AND_STORE_IN_FIFO1_IF_MATCH (6U)
#define MCAN_FILTER_ELEM_CFG_STORE_INTO_RX_BUFFER_OR_AS_DBG_MSG (7U)        /*!< Store into RXBUF if matching */

/**
 * @brief MCAN Filter Element definitions
 *
 * Each filter element tells the M_CAN hardware how to match incoming CAN identifiers and
 * where to route accepted frames. Populate the union member that corresponds to `filter_type`:
 *   - @ref MCAN_FILTER_TYPE_RANGE_FILTER     : accept IDs in `[start_id, end_id]`.
 *   - @ref MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER : accept frames matching `id1` OR `id2` (dual exact-match).
 *   - @ref MCAN_FILTER_TYPE_CLASSIC_FILTER    : accept IDs where `(rx_id & filter_mask) == (filter_id & filter_mask)`.
 *   - @ref MCAN_FILTER_ELEM_CFG_STORE_INTO_RX_BUFFER_OR_AS_DBG_MSG : exact-match `match_id`, store into
 *     a dedicated RX buffer at index `offset`.
 *
 * Only the union member matching the selected `filter_type` is read by hardware; the others are ignored.
 */
typedef struct mcan_std_id_filter_elem_struct {
    uint8_t filter_type;            /*!< Filter type selector. Use one of the @ref MCAN_FILTER_TYPE_RANGE_FILTER constants. */
    uint8_t filter_config;          /*!< Filter action when a match occurs. Use one of the
                                     *   @ref MCAN_FILTER_ELEM_CFG_DISABLED constants (e.g., store to FIFO0/1,
                                     *   reject, set priority, or store to RX buffer). */
    uint8_t can_id_type;            /*!< CAN ID type this element applies to. 0 = standard (11-bit),
                                     *   1 = extended (29-bit). See @ref MCAN_CAN_ID_TYPE_STANDARD. */
    uint8_t sync_message;           /*!< Sync Message flag. Set to 1 to mark matching frames as CAN
                                     *   time-synchronization messages (TTCAN / TSU integration). Set to 0
                                     *   for normal operation. */
    union {
        /* This definition takes effect if the filter type is range filter */
        struct {
            uint32_t start_id;      /*!< Start of ID range */
            uint32_t end_id;        /*!< End of ID range */
        };
        /* This definition takes effect if the filter type is dual id filter */
        struct {
            uint32_t id1;           /*!< ID1 */
            uint32_t id2;           /*!< ID2 */
        };
        /* This definition takes effect if the filter type is classic filter */
        struct {
            uint32_t filter_id;     /*!< Filter ID */
            uint32_t filter_mask;   /*!< Filter Mask */
        };
        /* This definition takes effect if the filter configuration is "store into RX Buffer or as debug message"
         *
         *  In this definition, only the exact ID matching mode is activated
         */
        struct {
            uint32_t match_id;          /*!< Matching ID */
            uint32_t offset: 6;         /*!< RX Buffer Index */
            uint32_t filter_event: 3;   /*!< Filter event, set to 0 */
            uint32_t store_location: 2; /*!< Store location, 0 - RX buffer */
            uint32_t : 21;
        };
    };
} mcan_filter_elem_t;

/**
 * @brief MCAN Filter Element List structure
 */
typedef struct mcan_filter_elem_list_struct {
    uint32_t mcan_filter_elem_count;            /*!< Number of Filter element */
    const mcan_filter_elem_t *filter_elem_list; /*!< Filter element list */
} mcan_filter_elem_list_t;

/**
 * @brief MCAN Configuration for all filters
 *
 * @note The MCAN RAM related settings are excluded
 */
typedef struct mcan_all_filters_config_struct {
    mcan_global_filter_config_t global_filter_config;   /*!< Global Filter configuration */
    uint32_t ext_id_mask;                               /*!< Extended ID mask */
    mcan_filter_elem_list_t std_id_filter_list;         /*!< Standard Identifier Configuration List */
    mcan_filter_elem_list_t ext_id_filter_list;         /*!< Extended Identifier Configuration List */
} mcan_all_filters_config_t;

/**
 * @brief CAN Node Mode types
 *
 * Selects the operating mode of the CAN controller. Normal mode is the default for
 * production use. The other modes are primarily for testing and diagnostics.
 */
typedef enum mcan_node_mode_enum {
    mcan_mode_normal = 0,               /*!< Normal: the node participates fully on the CAN bus (TX and RX) */
    mcan_mode_loopback_internal = 1,    /*!< Internal Loopback: TX output is internally routed to RX input.
                                         *   No frames appear on the physical bus. Useful for self-test
                                         *   without external hardware. */
    mcan_mode_loopback_external = 2,    /*!< External Loopback: frames are transmitted on the physical bus
                                         *   and simultaneously received back by the same node. Requires a
                                         *   connected transceiver. */
    mcan_mode_listen_only = 3,          /*!< Listen Only (Bus Monitoring): the node can receive frames but
                                         *   does not transmit anything (no ACK, no error frames, no data
                                         *   frames). Useful for passive bus analysis. */
} mcan_node_mode_t;

/**
 * @defgroup mcan_timestamping MCAN Timestamping
 * @ingroup mcan_interface
 * @{
 * @section timestamping_guide Timestamping configuration guide
 *
 * MCAN has three timestamp sources. Pick one source, call `mcan_init()`, then
 * decode received or transmitted timestamps with
 * `mcan_get_timestamp_from_received_message()` or
 * `mcan_get_timestamp_from_tx_event()`.
 *
 * ### Mode 1: internal 16-bit counter
 *
 * This mode uses a free-running 16-bit counter derived from the CAN bit clock.
 * It is enough for message ordering, but the value wraps at 0xFFFF.
 *
 * @code
 *     config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_VALUE_INCREMENT;
 *     config.timestamp_cfg.counter_prescaler   = 1;   // Increment every bit time
 *     mcan_init(ptr, &config, can_src_clk_freq);
 *
 *     // After reception:
 *     //   rx_frame.rx_timestamp contains the 16-bit counter snapshot.
 * @endcode
 *
 * ### Mode 2: TSU with internal AHB timebase
 *
 * The Timestamp Unit (TSU) can capture 32-bit or 64-bit timestamps from the AHB
 * bus clock through a prescaler. This gives better resolution than Mode 1, but
 * the timestamps are still relative to SoC power-on.
 *
 * @code
 *     config.use_timestamping_unit = true;
 *     config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;
 *
 *     config.tsu_config.enable_tsu             = true;
 *     config.tsu_config.prescaler              = 1;       // Full AHB clock resolution
 *     config.tsu_config.capture_on_sof         = true;    // Capture at Start-of-Frame
 *     config.tsu_config.enable_64bit_timestamp = false;   // 32-bit mode, or true for 64-bit
 *     config.tsu_config.use_ext_timebase       = false;   // AHB-derived clock
 *     mcan_init(ptr, &config, can_src_clk_freq);
 * @endcode
 *
 * ### Mode 3: TSU with PTPC external timebase
 *
 * Use PTPC when timestamps must line up with other IEEE 1588 time sources or
 * with other CAN buses. PTPC provides a seconds and nanoseconds monotonic clock
 * that can drive the MCAN TSU.
 *
 * Step 1: initialize the PTPC timer.
 * @code
 *     #include "hpm_ptpc_drv.h"
 *     #include "hpm_clock_drv.h"
 *
 *     clock_add_to_group(clock_ptpc, 0);
 *     ptpc_config_t ptpc_config;
 *     ptpc_get_default_config(HPM_PTPC, &ptpc_config);
 *     ptpc_config.src_frequency    = clock_get_frequency(clock_ptpc);
 *     ptpc_config.ns_rollover_mode = ptpc_ns_counter_rollover_digital;
 *     ptpc_config.coarse_increment = false;
 *     ptpc_config.capture_keep     = false;
 *     ptpc_init(HPM_PTPC, 0, &ptpc_config);
 *     ptpc_init_timer(HPM_PTPC, 0);
 * @endcode
 *
 * @note The PTPC nanosecond increment step is computed as
 *       `1,000,000,000 / src_frequency`. The source clock frequency must be an
 *       exact integer divisor of 1 GHz, such as 100, 125, 200, 250, or 500 MHz.
 *       Other frequencies are truncated and introduce timestamp drift.
 *
 * Step 2: route PTPC0 output to the target MCAN instance.
 * @code
 *     ptpc_set_timer_output(HPM_PTPC, mcan_instance_index, false);  // false = PTPC0
 * @endcode
 *
 * Step 3: configure the MCAN TSU.
 *
 * On SoCs with two-stage timebase selection (`MCAN_SOC_TSU_SRC_TWO_STAGES == 1`):
 *
 * @code
 *     config.use_timestamping_unit = true;
 *     config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED;
 *
 *     config.tsu_config.enable_tsu             = true;
 *     config.tsu_config.prescaler              = 1;
 *     config.tsu_config.capture_on_sof         = true;
 *     config.tsu_config.enable_64bit_timestamp = true;    // 64-bit for full PTPC range
 *     config.tsu_config.use_ext_timebase       = true;
 *     config.tsu_config.ext_timebase_src       = MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_0;
 *     config.tsu_config.tbsel_option           = MCAN_TSU_TBSEL_PTPC0;
 *     mcan_init(ptr, &config, can_src_clk_freq);
 * @endcode
 *
 * On SoCs without two-stage routing, set
 * `ext_timebase_src = MCAN_TSU_EXT_TIMEBASE_SRC_PTPC` directly. `tbsel_option`
 * is ignored.
 *
 * ### Reading timestamps
 *
 * @code
 *     mcan_timestamp_value_t ts;
 *
 *     // From a received message:
 *     mcan_get_timestamp_from_received_message(ptr, &rx_frame, &ts);
 *
 *     // From a TX event:
 *     mcan_get_timestamp_from_tx_event(ptr, &tx_evt, &ts);
 *
 *     if (ts.is_64bit)       { printf("64-bit: %llu\n", ts.ts_64bit); }
 *     else if (ts.is_32bit)  { printf("32-bit: %u\n",   ts.ts_32bit); }
 *     else if (ts.is_16bit)  { printf("16-bit: %u\n",   ts.ts_16bit); }
 *     else                   { }  // ts.is_empty: no timestamp captured
 * @endcode
 *
 * ### Mode comparison
 *
 * | Feature              | Internal 16-bit          | TSU internal           | TSU + PTPC                         |
 * |----------------------|--------------------------|------------------------|------------------------------------|
 * | Resolution           | Bit-time dependent       | AHB clock / prescaler  | 1 ns (digital) or ~0.466 ns (bin)  |
 * | Range                | 16-bit (wraps quickly)   | 32/64-bit              | 64-bit (seconds + nanoseconds)     |
 * | Absolute time        | No                       | No                     | Yes, via PTPC sync                 |
 * | Cross-bus correlation| No                       | Same SoC only          | Yes, with IEEE 1588 synchronization|
 * | Complexity           | Low                      | Medium                 | Higher, due to PTPC and routing    |
 * | Typical use          | Simple ordering          | Single-bus profiling   | Logging or multi-bus sync          |
 *
 * @}
 */

/**
 * @brief MCAN Timestamp Value
 *
 * Output-only structure populated by `mcan_get_timestamp_from_*` APIs. Exactly one of the
 * boolean flags is set to indicate which union member contains the valid timestamp.
 * Check in order: `is_empty` (no timestamp available), then `is_64bit`, `is_32bit`, `is_16bit`.
 */
typedef struct mcan_timestamp_value_struct {
    bool is_16bit;                  /*!< Timestamp value is in `ts_16bit` (internal counter mode) */
    bool is_32bit;                  /*!< Timestamp value is in `ts_32bit` (TSU 32-bit mode) */
    bool is_64bit;                  /*!< Timestamp value is in `ts_64bit` (TSU 64-bit mode) */
    bool is_empty;                  /*!< No timestamp was captured for this frame or event */
    union {
        uint16_t ts_16bit;          /*!< 16-bit timestamp value */
        uint32_t ts_32bit;          /*!< 32-bit timestamp value */
        uint64_t ts_64bit;          /*!< 64-bit timestamp value */
        uint32_t words[2];          /*!< timestamp words */
    };
} mcan_timestamp_value_t;

/**
 * @brief MCAN TSU Configuration
 *
 * The Timestamp Unit (TSU) provides high-resolution hardware timestamps that are more precise
 * than the internal 16-bit timestamp counter. When enabled, timestamps are captured into
 * dedicated TSU registers and referenced via pointer fields in RX/TX event elements.
 *
 * To enable the TSU, set @ref mcan_config_t.use_timestamping_unit to `true` and
 * @ref mcan_internal_timestamp_config_t.timestamp_selection to @ref MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED.
 *
 * For internal-AHB timebase, set `use_ext_timebase = false`; the TSU clock is derived from
 * `AHB_clock / (prescaler + 1)`.
 *
 * For a PTPC external timebase, set `use_ext_timebase = true` and configure
 * `ext_timebase_src` and `tbsel_option` according to your SoC's routing. See
 * @ref timestamping_guide for examples.
 */
typedef struct mcan_tsu_config_struct {
    uint16_t prescaler;             /*!< TSU clock prescaler. Clock source: AHB bus clock.
                                     *   TSU tick period = (prescaler + 1) / AHB_clock_Hz. */
    bool capture_on_sof;            /*!< Capture trigger. true = capture timestamp on Start-of-Frame (SOF),
                                     *   false = capture on End-of-Frame (EOF). */
    bool use_ext_timebase;          /*!< Use an external timebase instead of the AHB-derived TSU clock.
                                     *   When true, configure `ext_timebase_src`. */
    uint8_t ext_timebase_src;       /*!< External timebase source selector. SoC-specific; see
                                     *   hpm_mcan_soc.h for valid values on your device. */
    uint8_t tbsel_option;           /*!< Timebase selection option. SoC-specific; see hpm_mcan_soc.h. */
    bool enable_tsu;                /*!< Master enable for the Timestamp Unit */
    bool enable_64bit_timestamp;    /*!< Enable 64-bit timestamp mode. When false, 32-bit timestamps are used. */
} mcan_tsu_config_t;

/**
 * @brief MCAN Timestamp Selection
 *
 * Controls which timestamping source the MCAN uses for frames and events.
 * - @ref MCAN_TIMESTAMP_SEL_VALUE_ALWAYS_ZERO : Timestamps are always zero (disabled).
 * - @ref MCAN_TIMESTAMP_SEL_VALUE_INCREMENT   : Use the internal 16-bit free-running counter.
 * - @ref MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED   : Use the external TSU (requires
 *   @ref mcan_config_t.use_timestamping_unit = true and a valid @ref mcan_tsu_config_t).
 */
#define MCAN_TIMESTAMP_SEL_MIN               (0U)
#define MCAN_TIMESTAMP_SEL_VALUE_ALWAYS_ZERO (0U)     /*!< Timestamp value always equal to 0 */
#define MCAN_TIMESTAMP_SEL_VALUE_INCREMENT   (1U)     /*!< Timestamp value increments according to prescaler */
#define MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED   (2U)     /*!< External Timestamp value used */
#define MCAN_TIMESTAMP_SEL_MAX               (MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED)

/**
 * @brief MCAN Internal Timestamp Configuration
 *
 * Configures the built-in 16-bit timestamp counter (Mode 1).  When using the external TSU
 * instead (Mode 2 or 3), set `timestamp_selection` to @ref MCAN_TIMESTAMP_SEL_EXT_TS_VAL_USED
 * and configure @ref mcan_tsu_config_t.
 *
 * @code
 *     // Mode 1 example -- internal 16-bit counter:
 *     config.timestamp_cfg.timestamp_selection = MCAN_TIMESTAMP_SEL_VALUE_INCREMENT;
 *     config.timestamp_cfg.counter_prescaler   = 1;
 * @endcode
 */
typedef struct mcan_internal_timestamp_config_struct {
    uint8_t counter_prescaler;          /*!< Timestamp Counter Prescaler, clock source: AHB clock */
    uint8_t timestamp_selection;        /*!< Timestamp Select */
} mcan_internal_timestamp_config_t;

/**
 * @brief MCAN Timeout Selection Options
 *
 * The MCAN timeout counter is a configurable watchdog that counts down from `timeout_period`.
 * When it reaches zero, the `MCAN_INT_TIMEOUT_OCCURRED` interrupt fires. The trigger source
 * determines when the countdown is active.
 */
typedef enum mcan_timeout_sel_enum {
    mcan_timeout_continuous_operation     = 0,  /*!< Continuously count down timeout after writing to TOCV register */
    mcan_timeout_triggered_by_tx_evt_fifo = 1,  /*!< Count down if the TX EVT FIFO is not empty */
    mcan_timeout_triggered_by_rx_fifo0    = 2,  /*!< Count down if the RX FIFO0 is not empty */
    mcan_timeout_triggered_by_rx_fifo1    = 3,  /*!< Count down if the RX FIFO1 is not empty */
} mcan_timeout_sel_t;

/**
 * @brief MCAN Timeout configuration structure
 */
typedef struct mcan_timeout_config_struct {
    bool enable_timeout_counter;        /*!< Enable Timeout Counter */
    mcan_timeout_sel_t timeout_sel;     /*!< Timeout source selection */
    uint16_t timeout_period;            /*!< Timeout period */
} mcan_timeout_config_t;

/**
 * @brief MCAN Transmitter Delay Compensation Configuration
 *
 * TDC compensates for the propagation delay through the CAN transceiver, which becomes
 * significant at CAN FD data-phase bit rates above approximately 1 Mbps. Enable TDC
 * via `mcan_config_t.enable_tdc` and configure these parameters to match the transceiver
 * characteristics.
 */
typedef struct mcan_tdc_config_t {
    uint8_t ssp_offset;                 /*!< Secondary Sample Point (SSP) offset in minimum time quanta
                                         *   (mtq). Determines where the controller samples the received
                                         *   bit during the CAN FD data phase. Valid range: 0-127. */
    uint8_t filter_window_length;       /*!< TDC filter window length in mtq. Defines the width of the
                                         *   transceiver delay measurement filter. Valid range: 0-127. */
} mcan_tdc_config_t;


/**
 * @brief MCAN Configuration Structure
 */
typedef struct mcan_config_struct {
    union {
        /* This struct takes effect if "use_lowlevel_timing_setting = false" */
        struct {
            uint32_t baudrate;                      /*!< CAN 2.0 baudrate/CAN-FD Nominal Baudrate, in terms of bps */
            uint32_t baudrate_fd;                   /*!< CANFD data baudrate, in terms of bps */
            uint16_t can20_samplepoint_min;         /*!< Minimum acceptable CAN 2.0 sample point in tenths of a percent.
                                                     *   Example: 75.0% = 750, 87.5% = 875. Valid range: 500-950. */
            uint16_t can20_samplepoint_max;         /*!< Maximum acceptable CAN 2.0 sample point in tenths of a percent. */
            uint16_t canfd_samplepoint_min;         /*!< Minimum acceptable CAN FD sample point in tenths of a percent. */
            uint16_t canfd_samplepoint_max;         /*!< Maximum acceptable CAN FD sample point in tenths of a percent. */
        };
        /* This struct takes effect if "use_lowlevel_timing_setting = true" */
        struct {
            mcan_bit_timing_param_t can_timing;     /*!< CAN2.0/CANFD nominal timing setting */
            mcan_bit_timing_param_t canfd_timing;   /*!< CANFD data timing setting */
        };
    };
    bool use_lowlevel_timing_setting;               /*!< Use Low-level timing setting */
    mcan_node_mode_t mode;                          /*!< CAN node mode */
    bool enable_non_iso_mode;                       /*!< Enable Non-ISO FD mode. When true, the controller uses the
                                                     *   original Bosch CAN FD specification (pre-ISO 11898-1:2015).
                                                     *   Set to true only for interoperability with legacy non-ISO nodes. */
    bool enable_transmit_pause;                     /*!< Enable Transmit Pause. When true, the controller inserts a
                                                     *   two-bit pause between consecutive transmissions, reducing the
                                                     *   risk of monopolizing the bus. */
    bool enable_edge_filtering;                     /*!< Enable Edge Filtering on RX input. Improves noise immunity
                                                     *   by rejecting short glitches at the expense of a small delay. */
    bool disable_protocol_exception_handling;       /*!< Disable Protocol Exception Handling. When true, the controller
                                                     *   treats a protocol exception as a form error instead of entering
                                                     *   bus integration mode. */
    bool enable_wide_message_marker;                /*!< Enable 16-bit Wide Message Marker. When true, both
                                                     *   `message_marker_h` and `message_marker_l` in the TX frame are
                                                     *   used (16-bit total). When false, only `message_marker_l` (8-bit). */
    bool use_timestamping_unit;                     /*!< Use the external Timestamp Unit (TSU) instead of the internal
                                                     *   16-bit timestamp counter. The TSU provides higher-resolution
                                                     *   hardware timestamps. See @ref mcan_tsu_config_t. */
    bool enable_canfd;                              /*!< Enable CAN FD mode. When false, only classic CAN 2.0 frames
                                                     *   (up to 8 bytes) are supported. */
    bool enable_tdc;                                /*!< Enable Transmitter Delay Compensation (TDC). Required for
                                                     *   CAN FD data-phase bit rates above ~1 Mbps to compensate for
                                                     *   transceiver loop delay. See @ref mcan_tdc_config_t. */
    bool enable_restricted_operation_mode;          /*!< Enable Restricted Operation Mode. The node can receive frames
                                                     *   and acknowledge them, but cannot initiate transmissions. Useful
                                                     *   for passive bus monitoring or diagnostics. */
    bool disable_auto_retransmission;               /*!< Disable Automatic Retransmission. When true, a frame that loses
                                                     *   arbitration or encounters an error is not retransmitted
                                                     *   automatically; the application must re-submit it. */
    mcan_tdc_config_t tdc_config;                   /*!< Transmitter Delay Compensation Configuration */
    mcan_internal_timestamp_config_t timestamp_cfg; /*!< Internal Timestamp Configuration */
    mcan_tsu_config_t tsu_config;                   /*!< TSU configuration */
    mcan_ram_config_t ram_config;                   /*!< MCAN RAM configuration */
    mcan_all_filters_config_t all_filters_config;   /*!< All Filter configuration */

    mcan_timeout_config_t timeout_cfg;              /*!< Timeout configuration */

    uint32_t interrupt_mask;                        /*!< Interrupt Enable mask */
    uint32_t txbuf_trans_interrupt_mask;            /*!< Tx Buffer Transmission Interrupt Enable mask */
    uint32_t txbuf_cancel_finish_interrupt_mask;    /*!< TX Buffer Cancellation Finished Interrupt Enable Mask */
} mcan_config_t;

/**
 * @brief MCAN Timeout Selection Options
 */
#define MCAN_TIMEOUT_SEL_TYPE_CONTINUOUS_OPERATION (0U)
#define MCAN_TIMEOUT_SEL_TYPE_TIMEOUT_CTRL_BY_TX_EVT_FIFO (1U)
#define MCAN_TIMEOUT_SEL_TYPE_TIMEOUT_CTRL_BY_RX_FIFO0 (2U)
#define MCAN_TIMEOUT_SEL_TYPE_TIMEOUT_CTRL_BY_RX_FIFO1 (3U)

/**
 * @brief MCAN Timeout Counter Configuration
 * @deprecated This type is a structural duplicate of `mcan_timeout_config_t` and is retained
 *             only for backward source compatibility. New code should use `mcan_timeout_config_t`
 *             instead. This type may be removed in a future release.
 * @note Field `timeout_selection` (uint8_t) maps to the same semantic as
 *       `mcan_timeout_config_t::timeout_sel` (mcan_timeout_sel_t).
 */
typedef struct mcan_timeout_counter_config_struct {
    bool enable_timeout_counter;            /*!< Enable Timeout counter */
    uint8_t timeout_selection;              /*!< Timeout source selection (see mcan_timeout_sel_t) */
    uint16_t timeout_period;                /*!< Timeout period */
} mcan_timeout_counter_config_t;

/**
 * @brief MCAN Error Count Information
 */
typedef struct mcan_error_count_struct {
    uint8_t transmit_error_count;       /*!< Transmit Error Count */
    uint8_t receive_error_count;        /*!< Receive Error Count */
    bool receive_error_passive;         /*!< The Receive Error Counter has reached the error passive level */
    uint8_t can_error_logging_count;    /*!< CAN Error Logging count */
} mcan_error_count_t;

/**
 * @brief MCAN Protocol Status
 */
typedef struct mcan_protocol_status {
    uint8_t tdc_val;                            /*!< Transmitter Delay Compensation Value */
    mcan_activity_state_t activity;             /*!< Current communication state */
    mcan_last_err_code_t last_error_code;       /*!< Last Error code */
    bool protocol_exception_evt_occurred;       /*!< Protocol Exception Event occurred */
    bool canfd_msg_received;                    /*!< CANFD message was received */
    bool brs_flag_set_in_last_rcv_canfd_msg;    /*!< Bitrate Switch bit is set in last received CANFD message */
    bool esi_flag_set_in_last_rcv_canfd_msg;    /*!< Error State Indicator bit is set in last received CANFD message */
    bool in_bus_off_state;                      /*!< Node is in bus-off state */
    bool in_warning_state;                      /*!< Node is in warning state */
    bool in_error_passive_state;                /*!< Node is in error passive state */
} mcan_protocol_status_t;

/**
 * @brief MCAN Message Storage Indicator Types
 */
#define MCAN_MESSAGE_STORAGE_INDICATOR_NO_FIFO_SELECTED     (0U)
#define MCAN_MESSAGE_STORAGE_INDICATOR_FIFO_MESSAGE_LOST    (1U)
#define MCAN_MESSAGE_STORAGE_INDICATOR_RXFIFO0              (2U)
#define MCAN_MESSAGE_STORAGE_INDICATOR_RXFIFO1              (3U)

/**
 * @brief MCAN High Priority Message Status Information
 */
typedef struct mcan_high_priority_message_status_struct {
    uint8_t filter_list_type;                            /*!< Filter List Type, 0 - STD filter, 1 - EXT filter */
    uint8_t filter_index;                                /*!< Index within the matching filter list */
    uint8_t message_storage_indicator;                   /*!< Message Storage Indicator. See
                                                          *   @ref MCAN_MESSAGE_STORAGE_INDICATOR_NO_FIFO_SELECTED. */
    uint8_t buffer_index;                                /*!< Index of the RX buffer or FIFO element where the
                                                          *   high-priority message was stored */
} mcan_high_priority_message_status_t;

/**
 * @brief Consolidated MCAN diagnostic snapshot
 *
 * @note This structure is intended for application logging and fault handling paths that want one coherent view of
 *       runtime MCAN state without stitching together many separate low-level register reads.
 */
typedef struct mcan_diagnostic_snapshot_struct {
    uint32_t interrupt_flags;                        /*!< Raw interrupt flag register snapshot */
    mcan_protocol_status_t protocol_status;          /*!< Decoded protocol status snapshot */
    mcan_error_count_t error_count;                  /*!< Error counter snapshot */
    uint8_t last_data_error_code;                    /*!< Raw data-phase last error code (`PSR.DLEC`) */
    uint32_t rxfifo0_fill_level;                     /*!< RXFIFO0 fill level snapshot */
    uint32_t rxfifo1_fill_level;                     /*!< RXFIFO1 fill level snapshot */
    bool rxfifo0_full;                               /*!< RXFIFO0 full flag snapshot */
    bool rxfifo1_full;                               /*!< RXFIFO1 full flag snapshot */
    bool txfifo_full;                                /*!< TX FIFO/queue full flag snapshot */
    bool txqueue_enabled;                            /*!< TX Queue mode flag snapshot */
    bool in_init_mode;                               /*!< `CCCR.INIT` snapshot */
    bool config_write_enabled;                       /*!< `CCCR.CCE` snapshot */
    bool canfd_enabled;                              /*!< `CCCR.FDOE` snapshot */
} mcan_diagnostic_snapshot_t;

/***********************************************************************************************************************
 * @brief Low-level inline register helper APIs
 *
 * @note Unless documented otherwise, helpers that modify `CCCR` or other protected configuration state must be used
 *       only while MCAN is in initialization/configuration mode and protected configuration writes are enabled.
 *       These inline helpers perform direct register accesses and generally do not validate index or bitmask
 *       arguments; callers are responsible for providing values that are valid for the selected MCAN instance.
 *
 * @note Typical low-level configuration sequence (handled automatically by `mcan_init()`):
 *       1. `mcan_enter_init_mode(ptr)`                      -- request INIT mode
 *       2. `mcan_enable_write_to_prot_config_registers(ptr)` -- unlock protected registers
 *       3. Call configuration helpers (bit timing, RAM, filters, etc.)
 *       4. `mcan_enter_normal_mode(ptr)`                    -- exit INIT mode and start bus communication
 *
 * @note Most applications can bring up MCAN with `mcan_get_default_config()` and
 *       `mcan_init()`, then use the runtime transmit, receive, and diagnostic
 *       APIs below. Use this low-level layer for board support code or direct
 *       register programming flows.
 **********************************************************************************************************************/
/**
 * @brief Enable transmit pause mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.TXP` directly.
 */
static inline void mcan_enable_transmit_pause(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_TXP_MASK;
}

/**
 * @brief Disable transmit pause mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.TXP` directly.
 */
static inline void mcan_disable_transmit_pause(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_TXP_MASK;
}

/**
 * @brief Enable edge filtering on the receive input
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.EFBI` directly.
 */
static inline void mcan_enable_edge_filtering(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_EFBI_MASK;
}

/**
 * @brief Disable edge filtering on the receive input
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.EFBI` directly.
 */
static inline void mcan_disable_edge_filtering(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_EFBI_MASK;
}

/**
 * @brief Enable protocol exception handling
 * @param [in] ptr MCAN base
 * @note This helper clears `CCCR.PXHD`, which enables protocol exception handling.
 */
static inline void mcan_enable_protocol_exception_handling(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_PXHD_MASK;
}

/**
 * @brief Disable protocol exception handling
 * @param [in] ptr MCAN base
 * @note This helper sets `CCCR.PXHD`, which disables protocol exception handling.
 */
static inline void mcan_disable_protocol_exception_handling(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_PXHD_MASK;
}

/**
 * @brief Enable wide message marker mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.WMM` directly.
 */
static inline void mcan_enable_wide_message_marker(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_WMM_MASK;
}

/**
 * @brief Disable wide message marker mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.WMM` directly.
 */
static inline void mcan_disable_wide_message_marker(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_WMM_MASK;
}

/**
 * @brief Select the external timestamp unit as the timestamp source
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.UTSU` directly.
 */
static inline void mcan_enable_tsu(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_UTSU_MASK;
}

/**
 * @brief Disable external timestamp unit selection
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.UTSU` directly.
 */
static inline void mcan_disable_tsu(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_UTSU_MASK;
}

/**
 * @brief Check whether the external timestamp unit is selected
 * @param [in] ptr MCAN base
 * @retval true if the TSU path is selected
 * @retval false if the TSU path is not selected
 */
static inline bool mcan_is_tsu_used(MCAN_Type *ptr)
{
    return ((ptr->CCCR & MCAN_CCCR_UTSU_MASK) != 0U);
}

/**
 * @brief Check whether TSU timestamps are configured for 64-bit operation
 * @param [in] ptr MCAN base
 * @retval true if 64-bit TSU timestamps are enabled
 * @retval false if TSU timestamps are operating in 32-bit mode
 */
static inline bool mcan_is_64bit_tsu_timestamp_used(MCAN_Type *ptr)
{
    return ((ptr->TSCFG & MCAN_TSCFG_EN64_MASK) != 0U);
}

/**
 * @brief Enable CAN FD bit-rate switching
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.BRSE` directly.
 */
static inline void mcan_enable_bitrate_switch(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_BRSE_MASK;
}

/**
 * @brief Disable CAN FD bit-rate switching
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.BRSE` directly.
 */
static inline void mcan_disable_bitrate_switch(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_BRSE_MASK;
}

/**
 * @brief Enable automatic retransmission
 * @param [in] ptr MCAN base
 * @note This helper clears `CCCR.DAR`, which enables automatic retransmission.
 */
static inline void mcan_enable_auto_retransmission(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_DAR_MASK;
}

/**
 * @brief Disable automatic retransmission
 * @param [in] ptr MCAN base
 * @note This helper sets `CCCR.DAR`, which disables automatic retransmission.
 */
static inline void mcan_disable_auto_retransmission(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_DAR_MASK;
}

/**
 * @brief Enable bus monitoring mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.MON` directly.
 */
static inline void mcan_enable_bus_monitoring_mode(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_MON_MASK;
}

/**
 * @brief Request the MCAN host clock to stop
 * @param [in] ptr MCAN base
 * @note This helper sets `CCCR.CSR`; software should check `mcan_is_clock_stopped()` before assuming the clock is
 *       fully stopped.
 */
static inline void mcan_stop_clock(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_CSR_MASK;
}

/**
 * @brief Request the MCAN host clock to run
 * @param [in] ptr MCAN base
 * @note This helper clears `CCCR.CSR`.
 */
static inline void mcan_enable_clock(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_CSR_MASK;
}

/**
 * @brief Check whether the MCAN host clock request is active
 * @param [in] ptr MCAN base
 * @retval true if the MCAN clock is requested to run
 * @retval false if a clock stop request is active
 */
static inline bool mcan_is_clock_enabled(MCAN_Type *ptr)
{
    return ((ptr->CCCR & MCAN_CCCR_CSR_MASK) == 0UL);
}

/**
 * @brief Disable bus monitoring mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.MON` directly.
 */
static inline void mcan_disable_bus_monitoring_mode(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_MON_MASK;
}

/**
 * @brief Check whether the MCAN host clock has acknowledged a stopped state
 * @param [in] ptr MCAN base
 * @retval true if the clock-stop acknowledge bit is set
 * @retval false if the MCAN clock is still running or the stop request is not yet acknowledged
 */
static inline bool mcan_is_clock_stopped(MCAN_Type *ptr)
{
    return ((ptr->CCCR & MCAN_CCCR_CSA_MASK) != 0U);
}

/**
 * @brief Enable restricted operation mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.ASM` directly.
 */
static inline void mcan_enable_restricted_operation_mode(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_ASM_MASK;
}

/**
 * @brief Disable restricted operation mode
 * @param [in] ptr MCAN base
 * @note This helper updates `CCCR.ASM` directly.
 */
static inline void mcan_disable_restricted_operation_mode(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_ASM_MASK;
}

/**
 * @brief Enable write access to protected configuration registers
 * @param [in] ptr MCAN base
 * @note MCAN hardware requires `INIT=1` together with `CCE=1` before most protected configuration registers can be
 *       programmed safely.
 */
static inline void mcan_enable_write_to_prot_config_registers(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_CCE_MASK;
}

/**
 * @brief Disable write access to protected configuration registers
 * @param [in] ptr MCAN base
 * @note This helper clears `CCCR.CCE`.
 */
static inline void mcan_disable_write_to_prot_config_registers(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_CCE_MASK;
}

/**
 * @brief Read the current timestamp counter value
 * @param [in] ptr MCAN base
 * @return Current `TSCV` register value
 */
static inline uint16_t mcan_get_timestamp_counter_value(MCAN_Type *ptr)
{
    return ptr->TSCV;
}

/**
 * @brief Request MCAN initialization mode
 * @param [in] ptr MCAN base
 * @note Callers that need to wait for the mode transition to complete should poll `CCCR.INIT` after using this
 *       helper.
 */
static inline void mcan_enter_init_mode(MCAN_Type *ptr)
{
    ptr->CCCR |= MCAN_CCCR_INIT_MASK;
}

/**
 * @brief Request normal operating mode
 * @param [in] ptr MCAN base
 * @note Callers that need to wait for the mode transition to complete should poll `CCCR.INIT` after using this
 *       helper.
 */
static inline void mcan_enter_normal_mode(MCAN_Type *ptr)
{
    ptr->CCCR &= ~MCAN_CCCR_INIT_MASK;
}

/**
 * @brief Read the current timeout counter value
 * @param [in] ptr MCAN base
 * @return Current `TOCV` register value
 */
static inline uint16_t mcan_get_timeout_counter_value(MCAN_Type *ptr)
{
    return ptr->TOCV;
}

/**
 * @brief Clear the timeout counter value
 * @param [in] ptr MCAN base
 * @note This helper writes the timeout counter view directly and is intended for low-level timeout control paths.
 */
static inline void mcan_reset_timeout_counter_value(MCAN_Type *ptr)
{
    *((volatile uint32_t *) &ptr->TOCV) = 0;
}

/**
 * @brief Read and decode the MCAN error counters
 * @param [in] ptr MCAN base
 * @param [out] err_cnt Pointer to the destination error counter structure
 */
static inline void mcan_get_error_counter(MCAN_Type *ptr, mcan_error_count_t *err_cnt)
{
    uint32_t ecr = ptr->ECR;
    err_cnt->transmit_error_count = MCAN_ECR_TEC_GET(ecr);
    err_cnt->receive_error_count = MCAN_ECR_REC_GET(ecr);
    err_cnt->receive_error_passive = (MCAN_ECR_RP_GET(ecr) == 1U);
    err_cnt->can_error_logging_count = MCAN_ECR_CEL_GET(ecr);
}

/**
 * @brief Read the last protocol error code
 * @param [in] ptr MCAN base
 * @return Decoded `PSR.LEC` field value
 */
static inline uint8_t mcan_get_last_error_code(MCAN_Type *ptr)
{
    return MCAN_PSR_LEC_GET(ptr->PSR);
}

/**
 * @brief Read the last CAN FD data-phase error code
 * @param [in] ptr MCAN base
 * @return Decoded `PSR.DLEC` field value
 */
static inline uint8_t mcan_get_last_data_error_code(MCAN_Type *ptr)
{
    return MCAN_PSR_DLEC_GET(ptr->PSR);
}

/**
 * @brief Read the current MCAN activity state
 * @param [in] ptr MCAN base
 * @return Decoded `PSR.ACT` field value
 */
static inline uint8_t mcan_get_activity(MCAN_Type *ptr)
{
    return MCAN_PSR_ACT_GET(ptr->PSR);
}

/**
 * @brief Check whether the node is currently in error-passive state
 * @param [in] ptr MCAN base
 * @retval true if the node is in error-passive state
 * @retval false otherwise
 */
static inline bool mcan_is_in_err_passive_state(MCAN_Type *ptr)
{
    return (MCAN_PSR_EP_GET(ptr->PSR) == 1U);
}

/**
 * @brief Check whether the node is currently in error-warning state
 * @param [in] ptr MCAN base
 * @retval true if the node is in error-warning state
 * @retval false otherwise
 */
static inline bool mcan_is_in_error_warning_state(MCAN_Type *ptr)
{
    return (MCAN_PSR_EW_GET(ptr->PSR) == 1U);
}

/**
 * @brief Check whether the node is currently bus-off
 * @param [in] ptr MCAN base
 * @retval true if the node is bus-off
 * @retval false otherwise
 */
static inline bool mcan_is_in_busoff_state(MCAN_Type *ptr)
{
    return (MCAN_PSR_BO_GET(ptr->PSR) == 1U);
}

/**
 * @brief Read the last CAN FD data-phase error code
 * @param [in] ptr MCAN base
 * @deprecated This API will be removed in a later SDK release. Use `mcan_get_last_data_error_code()` instead.
 * @return Decoded `PSR.DLEC` field value
 */
static inline uint8_t mcan_get_data_phase_last_error_code(MCAN_Type *ptr)
{
    return MCAN_PSR_DLEC_GET(ptr->PSR);
}

/**
 * @brief Check whether the last received CAN FD frame reported ESI set
 * @param [in] ptr MCAN base
 * @retval true if `PSR.RESI` indicates ESI was set in the last received CAN FD frame
 * @retval false otherwise
 */
static inline bool mcan_is_error_state_indicator_flag_set_in_last_received_canfd_msg(MCAN_Type *ptr)
{
    return (MCAN_PSR_RESI_GET(ptr->PSR) == 1U);
}

/**
 * @brief Check whether the last received CAN FD frame used bit-rate switching
 * @param [in] ptr MCAN base
 * @retval true if `PSR.RBRS` indicates BRS was set in the last received CAN FD frame
 * @retval false otherwise
 */
static inline bool mcan_is_bitrate_switch_flag_set_in_last_received_canfd_msg(MCAN_Type *ptr)
{
    return (MCAN_PSR_RBRS_GET(ptr->PSR) == 1U);
}

/**
 * @brief Check whether the last received frame was a CAN FD frame
 * @param [in] ptr MCAN base
 * @retval true if `PSR.RFDF` indicates a CAN FD frame was received
 * @retval false otherwise
 */
static inline bool mcan_is_canfd_message_received(MCAN_Type *ptr)
{
    return (MCAN_PSR_RFDF_GET(ptr->PSR) == 1U);
}

/**
 * @brief Check whether a protocol exception event has been reported
 * @param [in] ptr MCAN base
 * @retval true if `PSR.PXE` indicates a protocol exception event occurred
 * @retval false otherwise
 */
static inline bool mcan_is_protocol_exception_event_occurred(MCAN_Type *ptr)
{
    return (MCAN_PSR_PXE_GET(ptr->PSR) == 1U);
}

/**
 * @brief Read the measured transmitter delay compensation value
 * @param [in] ptr MCAN base
 * @return Decoded `PSR.TDCV` field value
 */
static inline uint8_t mcan_get_transmitter_delay_compensation_value(MCAN_Type *ptr)
{
    return MCAN_PSR_TDCV_GET(ptr->PSR);
}

/**
 * @brief Read the current interrupt flag register
 * @param [in] ptr MCAN base
 * @return Current `IR` register value
 */
static inline uint32_t mcan_get_interrupt_flags(MCAN_Type *ptr)
{
    return ptr->IR;
}

/**
 * @brief Clear one or more interrupt flags
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of interrupt flags to clear
 */
static inline void mcan_clear_interrupt_flags(MCAN_Type *ptr, uint32_t mask)
{
    ptr->IR = mask;
}

/**
 * @brief Enable one or more MCAN interrupts on interrupt line 0
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of interrupts to enable
 * @note On current HPMicro SoCs, the driver uses interrupt line 0 by default.
 */
static inline void mcan_enable_interrupts(MCAN_Type *ptr, uint32_t mask)
{
    ptr->ILS &= ~mask;
    ptr->IE |= mask;
    ptr->ILE |= 1U; /* As on HPMicro SoCs, only one interrupt line is used */
}

/**
 * @brief Enable TX buffer transmission interrupts
 * @deprecated This API is deprecated and will be removed in a later SDK release. Use
 *             `mcan_enable_txbuf_transmission_interrupt()` instead.
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer transmission interrupts to enable
 */
static inline void mcan_enable_txbuf_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBTIE |= mask;
}

/**
 * @brief Disable TX buffer transmission interrupts
 * @deprecated This API is deprecated and will be removed in a later SDK release. Use
 *             `mcan_disable_txbuf_transmission_interrupt()` instead.
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer transmission interrupts to disable
 */
static inline void mcan_disable_txbuf_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBTIE &= ~mask;
}

/**
 * @brief Disable one or more MCAN interrupts
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of interrupts to disable
 */
static inline void mcan_disable_interrupts(MCAN_Type *ptr, uint32_t mask)
{
    ptr->IE &= ~mask;
}

/**
 * @brief Enable TX buffer transmission-complete interrupts
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer interrupt sources to enable
 */
static inline void mcan_enable_txbuf_transmission_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBTIE |= mask;
}

/**
 * @brief Disable TX buffer transmission-complete interrupts
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer interrupt sources to disable
 */
static inline void mcan_disable_txbuf_transmission_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBTIE &= ~mask;
}

/**
 * @brief Enable TX buffer cancellation-finished interrupts
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer cancellation interrupt sources to enable
 */
static inline void mcan_enable_txbuf_cancel_finish_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBCIE |= mask;
}

/**
 * @brief Disable TX buffer cancellation-finished interrupts
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of TX buffer cancellation interrupt sources to disable
 */
static inline void mcan_disable_txbuf_cancel_finish_interrupt(MCAN_Type *ptr, uint32_t mask)
{
    ptr->TXBCIE &= ~mask;
}

/**
 * @brief Route selected MCAN interrupt sources to the chosen interrupt line
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of interrupt sources to route
 * @param [in] line_index Interrupt line index
 */
static inline void mcan_interrupt_line_select(MCAN_Type *ptr, uint32_t mask, uint8_t line_index)
{
    if (line_index == 0) {
        ptr->ILS &= ~mask;
    } else {
        ptr->ILS |= mask;
    }
    ptr->ILE |= (1UL << line_index);
}

/**
 * @brief Check whether a transmission request is still pending for the selected TX element
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer or TX FIFO element index
 * @retval true if a transmission request is pending for the selected element
 * @retval false otherwise
 */
static inline bool mcan_is_transmit_request_pending(MCAN_Type *ptr, uint32_t index)
{
    return ((ptr->TXBRP & ((1UL << index))) != 0U);
}

/**
 * @brief Check whether the TX FIFO/queue data area is full
 * @param [in] ptr MCAN base
 * @retval true if no additional TX FIFO/queue element can currently be allocated
 * @retval false otherwise
 */
static inline bool mcan_is_txfifo_full(MCAN_Type *ptr)
{
    return ((ptr->TXFQS & MCAN_TXFQS_TFQF_MASK) != 0U);
}

/**
 * @brief Read the current TX FIFO put index
 * @param [in] ptr MCAN base
 * @return Decoded `TXFQS.TFQPI` field value
 */
static inline uint32_t mcan_get_txfifo_put_index(MCAN_Type *ptr)
{
    return MCAN_TXFQS_TFQPI_GET(ptr->TXFQS);
}

/**
 * @brief Request transmission for one TX element
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer or prepared TX FIFO element index
 */
static inline void mcan_send_add_request(MCAN_Type *ptr, uint32_t index)
{
    ptr->TXBAR = (1UL << index);
}

/**
 * @brief Request transmission for multiple TX elements at once
 * @param [in] ptr MCAN base
 * @param [in] index_bitmask Bitmask selecting TX elements to transmit
 * @note Each asserted bit in `index_bitmask` maps directly to one TX element.
 */
static inline void mcan_send_add_multiple_requests(MCAN_Type *ptr, uint32_t index_bitmask)
{
    ptr->TXBAR = index_bitmask;
}

/**
 * @brief Request cancellation of one TX buffer transmission
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer index
 */
static inline void mcan_cancel_tx_buf_send_request(MCAN_Type *ptr, uint32_t index)
{
    ptr->TXBCR = (1UL << index);
}

/**
 * @brief Check whether cancellation has completed for the selected TX buffer
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer index
 * @retval true if cancellation has completed
 * @retval false otherwise
 */
static inline bool mcan_is_tx_buf_cancellation_finished(MCAN_Type *ptr, uint32_t index)
{
    return ((ptr->TXBCF & ((1UL << index))) != 0U);
}

/**
 * @brief Check whether transmission has completed for the selected TX element
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer or TX FIFO element index
 * @retval true if transmission completed for the selected element
 * @retval false otherwise
 */
static inline bool mcan_is_transmit_occurred(MCAN_Type *ptr, uint32_t index)
{
    return ((ptr->TXBTO & (1UL << index)) != 0U);
}

/**
 * @brief Check whether unread data is available in the selected RX buffer
 * @param [in] ptr MCAN base
 * @param [in] index RX buffer index
 * @retval true if unread data is available in the selected RX buffer
 * @retval false otherwise
 */
static inline bool mcan_is_rxbuf_data_available(MCAN_Type *ptr, uint32_t index)
{
    bool result;
    if (index < 32U) {
        result = (ptr->NDAT1 & (1UL << index)) != 0U;
    } else if (index < 64U) {
        result = (ptr->NDAT2 & (1UL << (index - 32U))) != 0U;
    } else {
        result = false;
    }
    return result;
}

/**
 * @brief Clear the data-available flag for the selected RX buffer
 * @param [in] ptr MCAN base
 * @param [in] index RX buffer index
 * @note Indices outside the supported RX buffer range are ignored by this helper.
 */
static inline void mcan_clear_rxbuf_data_available_flag(MCAN_Type *ptr, uint32_t index)
{
    if (index < 32U) {
        ptr->NDAT1 = (1UL << index);
    } else if (index < 64U) {
        ptr->NDAT2 = (1UL << (index - 32U));
    } else {
        /* Do nothing */
    }
}

/**
 * @brief Check whether one or more interrupt flags are currently set
 * @param [in] ptr MCAN base
 * @param [in] mask Bitmask of interrupt flags to test
 * @retval true if any bit from `mask` is set in `IR`
 * @retval false otherwise
 */
static inline bool mcan_is_interrupt_flag_set(MCAN_Type *ptr, uint32_t mask)
{
    return ((ptr->IR & mask) != 0U);
}

/**
 * @brief Read the fill level of the selected RX FIFO
 * @param [in] ptr MCAN base
 * @param [in] fifo_index RX FIFO index
 * @return Current fill level of the selected RX FIFO
 */
static inline uint32_t mcan_get_rxfifo_fill_level(MCAN_Type *ptr, uint32_t fifo_index)
{
    return (fifo_index == 0U) ? MCAN_RXF0S_F0FL_GET(ptr->RXF0S) : MCAN_RXF1S_F1FL_GET(ptr->RXF1S);
}

/**
 * @brief Check whether the selected RX FIFO is full
 * @param [in] ptr MCAN base
 * @param [in] fifo_index RX FIFO index, `0` for RXFIFO0 or `1` for RXFIFO1
 * @retval true if the selected FIFO is full
 * @retval false if the selected FIFO still has room
 * @note `fifo_index` is interpreted directly and is not range-checked by this inline helper.
 */
static inline bool mcan_is_rxfifo_full(MCAN_Type *ptr, uint32_t fifo_index)
{
    return (fifo_index == 0U) ? (MCAN_RXF0S_F0F_GET(ptr->RXF0S) != 0U) : (MCAN_RXF1S_F1F_GET(ptr->RXF1S) != 0U);
}

/**
 * @brief Check whether a TSU timestamp sample is available in the selected slot
 * @param [in] ptr MCAN base
 * @param [in] index TSU timestamp slot index
 * @retval true if a timestamp sample is available
 * @retval false otherwise
 * @note The TSU must be enabled during `mcan_init()` by setting
 *       `mcan_config_t::use_timestamping_unit = true` and
 *       `mcan_config_t::tsu_config.enable_tsu = true` before timestamp
 *       samples can be captured into the TSU slot array.
 */
static inline bool mcan_is_tsu_timestamp_available(MCAN_Type *ptr, uint32_t index)
{
    bool is_available = false;
    if (index < ARRAY_SIZE(ptr->TS_SEL)) {
        is_available = ((ptr->TSS1 & (1UL << index)) != 0U);
    }
    return is_available;
}

/**
 * @brief Check whether TX Queue mode is enabled
 * @param [in] ptr MCAN base
 * @return true if TX Queue mode is enabled
 */
static inline bool mcan_is_txqueue_enabled(MCAN_Type *ptr)
{
    return (MCAN_TXBC_TFQM_GET(ptr->TXBC) != 0U);
}

/**
 * @brief Read a 32-bit timestamp sample from the TSU capture array
 * @param [in] ptr MCAN base
 * @param [in] index TSU timestamp slot index
 * @return Captured 32-bit timestamp value stored in the selected slot
 * @note Call `mcan_is_tsu_timestamp_available()` before reading to avoid consuming an empty slot.
 * @note The TSU must be enabled via `mcan_init()` with `mcan_config_t::use_timestamping_unit = true`
 *       and `mcan_config_t::tsu_config.enable_tsu = true` for captures to occur.
 */
uint32_t mcan_read_32bit_tsu_timestamp(MCAN_Type *ptr, uint32_t index);

/**
 * @brief Read a 64-bit timestamp sample from the TSU capture array
 * @param [in] ptr MCAN base
 * @param [in] index TSU timestamp slot index
 * @return Captured 64-bit timestamp value stored in the selected slot
 * @note Call `mcan_is_tsu_timestamp_available()` before reading to avoid consuming an empty slot.
 * @note The TSU must be enabled via `mcan_init()` with `mcan_config_t::use_timestamping_unit = true`
 *       and `mcan_config_t::tsu_config.enable_64bit_timestamp = true` for 64-bit captures.
 */
uint64_t mcan_read_64bit_tsu_timestamp(MCAN_Type *ptr, uint32_t index);

/**
 * @brief Read the high-priority message status register and decode it into a software structure
 * @param [in] ptr MCAN base
 * @param [out] status Pointer to the decoded high-priority message status structure
 * @retval status_success if the status was read successfully
 * @retval status_invalid_argument if @p ptr or @p status is NULL
 */
hpm_stat_t mcan_get_high_priority_message_status(MCAN_Type *ptr, mcan_high_priority_message_status_t *status);

/***********************************************************************************************************************
 * @brief High-level lifecycle and helper APIs
 *
 * @note Common setup flow:
 *       `mcan_get_default_config()` -> adjust fields in `mcan_config_t` -> `mcan_init()`
 *
 * @note Use this section for the common application path:
 *       - bring the controller up with `mcan_get_default_config()` + `mcan_init()`
 *       - use `mcan_transmit_*()` / `mcan_receive_*()` during normal runtime
 *       - use `mcan_get_error_state()`, `mcan_get_diagnostic_snapshot()`, and `mcan_recover_from_busoff()` for
 *         diagnostics and recovery
 *       - use `mcan_begin_reconfig()` / `mcan_end_reconfig()` only when protected configuration must change at runtime
 **********************************************************************************************************************/
/**
 * @brief Populate the default controller configuration
 * @note Call this first, adjust only the fields your application needs, then
 *       pass the structure to `mcan_init()`.
 * @param [in] ptr MCAN base
 * @param [out] config Pointer to the configuration structure to populate
 */
void mcan_get_default_config(MCAN_Type *ptr, mcan_config_t *config);

/**
 * @brief Enter a safe runtime reconfiguration session
 * @param [in] ptr MCAN base
 * @retval status_success if the controller is in initialization/configuration mode and protected writes are enabled
 * @retval status_invalid_argument if `ptr` is invalid
 * @retval status_timeout if the controller did not enter initialization mode within the driver's retry budget
 * @note This helper requests `CCCR.INIT`, waits for the transition to complete, then enables `CCCR.CCE`.
 */
hpm_stat_t mcan_begin_reconfig(MCAN_Type *ptr);

/**
 * @brief Leave a runtime reconfiguration session and return to normal operation
 * @param [in] ptr MCAN base
 * @retval status_success if the controller returned to normal operation successfully
 * @retval status_invalid_argument if `ptr` is invalid
 * @retval status_timeout if the controller did not leave initialization mode within the driver's retry budget
 * @note This helper clears `CCCR.CCE`, requests normal mode, and waits until `CCCR.INIT` is cleared.
 */
hpm_stat_t mcan_end_reconfig(MCAN_Type *ptr);

/**
 * @brief Initialize a standard-ID transmit frame template
 * @param [out] frame Pointer to the frame object to initialize
 * @param [in] std_id Standard 11-bit CAN identifier
 * @param [in] dlc Desired DLC value for the frame template
 * @note This helper clears the frame, selects standard-ID classic CAN format, and clamps `dlc` to the classic CAN
 *       data range `0..8`.
 */
void mcan_init_tx_frame_std(mcan_tx_frame_t *frame, uint16_t std_id, uint8_t dlc);

/**
 * @brief Initialize an extended-ID transmit frame template
 * @param [out] frame Pointer to the frame object to initialize
 * @param [in] ext_id Extended 29-bit CAN identifier
 * @param [in] dlc Desired DLC value for the frame template
 * @note This helper clears the frame, selects extended-ID classic CAN format, and clamps `dlc` to the classic CAN
 *       data range `0..8`.
 */
void mcan_init_tx_frame_ext(mcan_tx_frame_t *frame, uint32_t ext_id, uint8_t dlc);

/**
 * @brief Convert a CAN/CAN FD DLC encoding to a payload size in bytes
 * @param [in] dlc Data Length Code value from a TX or RX frame header
 * @return Payload size in bytes represented by the supplied DLC
 */
uint8_t mcan_get_message_size_from_dlc(uint8_t dlc);

/**
 * @brief Convert an MCAN message RAM data-field-size encoding to a payload size in bytes
 * @param [in] data_field_size_option Encoded MCAN data field size option
 * @return Configured payload capacity in bytes, or `0` if the option is invalid
 */
uint8_t mcan_get_data_field_size(uint8_t data_field_size_option);

/**
 * @brief Copy payload data into a prepared transmit frame
 * @param [in,out] frame Pointer to the frame to update
 * @param [in] payload Pointer to the source payload bytes
 * @param [in] payload_size Number of bytes to copy from `payload`
 * @retval status_success if the payload was copied successfully
 * @retval status_invalid_argument if arguments are invalid or the payload exceeds the capacity implied by `frame->dlc`
 * @note For classic CAN frames, this helper normalizes the effective DLC to the 8-byte classic CAN limit. Any unused
 *       payload bytes inside the frame buffer are cleared to zero.
 */
hpm_stat_t mcan_set_tx_frame_payload(mcan_tx_frame_t *frame, const uint8_t *payload, uint8_t payload_size);

/***********************************************************************************************************************
 * @brief Default configuration helpers
 *
 * @note These APIs are intended to populate safe starting configurations for users who do not need
 *       low-level MCAN RAM layout control.
 *
 * @note `mcan_get_default_config()` is the normal entry point for applications. The RAM helper APIs in this section
 *       are mainly for users who want to inspect or customize the default message RAM layout before calling `mcan_init()`.
 **********************************************************************************************************************/
/**
 * @brief Populate a default full flexible MCAN RAM layout descriptor
 * If the device is configured as classic CAN node, the default CAN RAM settings are as below:
 *  - Standard Identifier Filter Elements: 32
 *  - Extended Identifier Filter Elements: 32
 *  - TXBUF Elements Info:
 *      - Element Count:32
 *      - Dedicated TXBUF element count: 16
 *      - TXFIFO/QQueue element count: 16
 *      - Data Field Size: 8
 *      .
 *   - RXFIFO0 Elements Info:
 *      - Element Count :32
 *      - Data Field Size: 8
 *      .
 *   - RXFIFO1 Elements Info:
 *      - Element Count : 32
 *      - Data Field Size: 8
 *      .
 *    - RXBUF Element Info:
 *      - Element Count: 16
 *      - Data Field Size : 8
 *      .
 *    - TX Event FIFO Element Count: 32
 *  .
 * If the device is configured as CANFD node, the default CAN RAM settings are as below:
 *  - Standard Identifier Filter Elements: 16
 *  - Extended Identifier Filter Elements: 16
 *  - TXBUF Elements Info:
 *      - Element Count:8
 *      - Dedicated TXBUF element count: 4
 *      - TXFIFO/QQueue element count: 4
 *      - Data Field Size: 64
 *      .
 *   - RXFIFO0 Elements Info:
 *      - Element Count : 8
 *      - Data Field Size: 64
 *      .
 *   - RXFIFO1 Elements Info:
 *      - Element Count : 8
 *      - Data Field Size: 64
 *      .
 *    - RXBUF Element Info:
 *      - Element Count: 4
 *      - Data Field Size : 64
 *      .
 *    - TX Event FIFO Element Count: 8
 *    .
 * @param [in] ptr MCAN base
 * @param [out] ram_config Pointer to the flexible RAM layout structure to populate
 * @param [in] enable_canfd `true` to build CAN FD defaults, `false` to build classic CAN defaults
 * @note The returned structure is intended for expert users who need direct ownership of message RAM placement.
 */
void mcan_get_default_ram_flexible_config(MCAN_Type *ptr, mcan_ram_flexible_config_t *ram_config, bool enable_canfd);

/**
 * @brief Populate a simplified default MCAN RAM configuration
 *
 * @note Default element counts and data field sizes are identical to those
 *       documented in the @ref mcan_get_default_ram_flexible_config description.
 *
 * @param [in] ptr MCAN base
 * @param [out] simple_config Pointer to the simplified RAM configuration structure to populate
 * @param [in] enable_canfd `true` to build CAN FD defaults, `false` to build classic CAN defaults
 * @note This helper is preferred over the flexible layout variant unless the application must place message RAM
 *       regions at explicit offsets.
 */
void mcan_get_default_ram_config(MCAN_Type *ptr, mcan_ram_config_t *simple_config, bool enable_canfd);

/***********************************************************************************************************************
 * @brief High-level controller lifecycle APIs
 **********************************************************************************************************************/
/**
 * @brief Fully initialize an MCAN instance from a high-level configuration structure
 * @note This is the preferred API for normal driver bring-up. It applies timing, RAM, filter, timestamp, interrupt,
 *       and timeout configuration from `mcan_config_t`, then leaves the controller ready for runtime use.
 * @param [in] ptr MCAN base
 * @param [in] config Pointer to the desired controller configuration
 * @param [in] src_clk_freq Functional clock frequency supplied to the MCAN instance
 * @retval status_success if initialization completed successfully
 * @retval status_invalid_argument if any configuration parameters are invalid
 * @retval status_timeout if the controller did not complete clock enable or mode transitions in time
 * @retval status_mcan_invalid_bit_timing if the requested bit timing cannot be represented by the MCAN hardware
 * @retval status_mcan_ram_out_of_range if the requested message RAM allocation exceeds the available RAM window
 */
hpm_stat_t mcan_init(MCAN_Type *ptr, mcan_config_t *config, uint32_t src_clk_freq);

/**
 * @brief De-initialize an MCAN instance and return it to a stopped state
 * @param [in] ptr MCAN base
 * @note This helper stops the controller and clears its configuration-oriented state so that a later `mcan_init()`
 *       starts from a known baseline.
 */
void mcan_deinit(MCAN_Type *ptr);

/***********************************************************************************************************************
 * @brief Expert low-level configuration APIs
 *
 * @note These APIs program protected MCAN configuration registers directly. Use
 *       them for custom RAM layouts, filter tables, or runtime reconfiguration
 *       while MCAN is in initialization/configuration mode.
 *
 * @note If the application does not need direct control of RAM placement,
 *       filter table layout, timing, or TDC setup, configure `mcan_config_t`
 *       and call `mcan_init()` instead.
 **********************************************************************************************************************/
/**
 * @brief Program a fully explicit MCAN message RAM layout
 * @note Prefer `mcan_get_default_config()` + `mcan_init()` unless a custom RAM layout is required.
 * @param [in] ptr MCAN base
 * @param [in] config Pointer to the full flexible RAM configuration
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if the layout is valid and has been programmed
 * @retval status_invalid_argument if the layout is invalid or MCAN is not in configuration mode
 */
hpm_stat_t mcan_config_ram_with_flexible_config(MCAN_Type *ptr, mcan_ram_flexible_config_t *config);

/**
 * @brief Program MCAN message RAM using the simplified layout descriptor
 * @note Prefer `mcan_get_default_config()` + `mcan_init()` unless partial low-level control is required.
 * @param [in] ptr MCAN base
 * @param [in] config Pointer to the simplified RAM configuration
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if the layout is valid and has been programmed
 * @retval status_invalid_argument if the configuration is invalid or MCAN is not in configuration mode
 * @retval status_mcan_ram_out_of_range if the requested RAM footprint exceeds the available MCAN RAM window
 */
hpm_stat_t mcan_config_ram(MCAN_Type *ptr, mcan_ram_config_t *config);

/**
 * @brief Program global filter settings, extended ID mask, and all configured filter lists
 * @note Prefer configuring filters through `mcan_config_t` and `mcan_init()` for normal use cases.
 * @param [in] ptr MCAN base
 * @param [in] config Pointer to the aggregate filter configuration
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if all filter settings were accepted and programmed
 * @retval status_invalid_argument if the configuration is invalid or MCAN is not in configuration mode
 */
hpm_stat_t mcan_config_all_filters(MCAN_Type *ptr, mcan_all_filters_config_t *config);

/**
 * @brief Configure transmitter delay compensation (TDC) settings directly
 * @note This low-level API is intended for expert users who need direct TDC reconfiguration.
 * @param [in] ptr MCAN base
 * @param [in] config Pointer to the desired TDC configuration
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if the TDC settings were accepted and programmed
 * @retval status_invalid_argument if the configuration is invalid or MCAN is not in configuration mode
 */
hpm_stat_t mcan_config_transmitter_delay_compensation(MCAN_Type *ptr, mcan_tdc_config_t *config);

/**
 * @brief Program the MCAN global filter configuration only
 * @note Prefer configuring filters through `mcan_config_t` and `mcan_init()` for normal use cases.
 * @param [in] ptr MCAN base
 * @param [in] filter_config Pointer to the global filter configuration
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if the global filter settings were accepted and programmed
 * @retval status_invalid_argument if the configuration is invalid or MCAN is not in configuration mode
 */
hpm_stat_t mcan_set_global_filter_config(MCAN_Type *ptr, mcan_global_filter_config_t *filter_config);

/**
 * @brief Program a single standard or extended filter element into message RAM
 * @note Use this for low-level filter programming.
 * @param [in] ptr MCAN base
 * @param [in] filter_elem Pointer to the filter element to encode and write
 * @param [in] index Filter element index within the selected standard or extended list
 * @note Call this only while MCAN is in initialization/configuration mode and
 *       protected configuration registers are writable.
 * @retval status_success if the filter element was accepted and programmed
 * @retval status_invalid_argument if the element is invalid or MCAN is not in configuration mode
 * @retval status_mcan_filter_index_out_of_range if `index` exceeds the configured filter list size
 */
hpm_stat_t mcan_set_filter_element(MCAN_Type *ptr, const mcan_filter_elem_t *filter_elem, uint32_t index);

/***********************************************************************************************************************
 * @brief Runtime transmit and receive APIs
 *
 * @note These APIs are intended for normal operation after `mcan_init()` has completed.
 *
 * @note Start with `mcan_transmit_blocking()` for the simplest transmit path.
 *       Use the nonblocking transmit APIs when the application manages completion
 *       itself. Use `mcan_write_*()` and `mcan_request_and_fill_txfifo()` only
 *       when you need to stage frames separately from transmit requests.
 **********************************************************************************************************************/
/**
 * @brief Write a prepared frame into a dedicated TX buffer slot
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer index
 * @param [in] tx_frame Pointer to the frame to encode into the selected TX buffer
 * @note This only stages the frame. Use `mcan_transmit_via_txbuf_nonblocking()`
 *       unless you need to issue the transmit request separately.
 * @retval status_success if the frame was written successfully
 * @retval status_invalid_argument if `ptr` or `tx_frame` is NULL
 * @retval status_mcan_txbuf_index_out_of_range if `index` exceeds the configured dedicated TX buffer count
 * @retval status_fail if the selected slot has a transmission request pending
 */
hpm_stat_t mcan_write_txbuf(MCAN_Type *ptr, uint32_t index, mcan_tx_frame_t *tx_frame);

/**
 * @brief Write a frame into the next free TX FIFO element
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to enqueue
 * @note This only stages the frame in TX FIFO mode. Use
 *       `mcan_transmit_via_txfifo_nonblocking()` unless the send request must be
 *       issued later. In TX Queue mode, use `mcan_transmit_via_txqueue_nonblocking()`.
 * @retval status_success if the frame was written successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txfifo_full if the TX FIFO has no available slot
 */
hpm_stat_t mcan_write_txfifo(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame);

/**
 * @brief Write a frame into the active TX FIFO or TX Queue data slot
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to enqueue
 * @param [out] slot_index Pointer to the assigned TX element index
 * @note This is the shared staging helper behind the TX FIFO and TX Queue
 *       wrappers. Use the wrapper APIs unless the application needs this lower
 *       level control.
 * @retval status_success if the frame was written successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txfifo_full if no TX FIFO/Queue slot is currently available
 */
hpm_stat_t mcan_write_tx_fifo_or_queue(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *slot_index);

/**
 * @brief Read a received frame from an RX buffer slot
 * @param [in] ptr MCAN base
 * @param [in] index RX buffer index
 * @param [out] rx_frame Pointer to the destination frame buffer
 * @note This is a direct receive primitive. Use
 *       `mcan_receive_from_buf_blocking()` when the caller should wait for data.
 * @note This primitive does not clear the NDAT (New Data) flag for the slot.
 *       After calling it, call `mcan_clear_rxbuf_data_available_flag(ptr, index)`
 *       to acknowledge the slot and let hardware overwrite it with a new frame.
 * @retval status_success if the frame was read successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_rxbuf_empty if the NDAT flag for the specified index is not set
 */
hpm_stat_t mcan_read_rxbuf(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame);

/**
 * @brief Read one received frame from RXFIFO0 or RXFIFO1
 * @param [in] ptr MCAN base
 * @param [in] fifo_index RX FIFO index, `0` for RXFIFO0 or `1` for RXFIFO1
 * @param [out] rx_frame Pointer to the destination frame buffer
 * @note This is a direct receive primitive. Use
 *       `mcan_receive_from_fifo_blocking()` when the caller should wait for data.
 * @note Software should ensure that the read/acknowledge sequence is effectively atomic with respect to competing
 *       readers.
 * @retval status_success if the frame was read successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_rxfifo_empty if the selected FIFO is empty
 */
hpm_stat_t mcan_read_rxfifo(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame);

/**
 * @brief Read one element from the TX Event FIFO
 * @param [in] ptr MCAN base
 * @param [out] tx_evt Pointer to the destination TX event structure
 * @retval status_success if a TX event was read successfully
 * @retval status_mcan_tx_evt_fifo_empty if no TX event is available
 * @retval status_invalid_argument if any parameters are invalid
 */
hpm_stat_t mcan_read_tx_evt_fifo(MCAN_Type *ptr, mcan_tx_event_fifo_elem_t *tx_evt);

/**
 * @brief Transmit one frame using the driver's default blocking transmit flow
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to transmit
 * @retval status_success if the frame was transmitted successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txfifo_full if the TX FIFO has no available slot at the time of submission
 * @retval status_timeout if transmission could not complete within the driver's retry budget
 */
hpm_stat_t mcan_transmit_blocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame);

/**
 * @brief Reserve a TX FIFO element and fill it without starting transmission
 * @note Use this in TX FIFO mode to prepare one or more frames before starting
 *       transmission. After staging, call `mcan_send_add_request(ptr, *fifo_index)`
 *       for one frame or `mcan_send_add_multiple_requests(ptr, fifo_index_masks)`
 *       for several frames. Behavior is not defined in TX Queue mode.
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to stage
 * @param [out] fifo_index Pointer to the reserved FIFO element index
 * @retval status_success if a FIFO element was reserved and filled
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txfifo_full if the TX FIFO has no available slot
 */
hpm_stat_t mcan_request_and_fill_txfifo(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *fifo_index);

/**
 * @brief Queue one frame for transmission through TX FIFO without blocking
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to transmit
 * @param [out] fifo_index Pointer to the FIFO slot index used for this frame
 * @note Use this in TX FIFO mode. Behavior is not defined in TX Queue mode.
 * @retval status_success if the frame was accepted for transmission
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txfifo_full if the TX FIFO has no available slot
 * @retval status_timeout if the SoC-specific buffer access workaround timed out (platform-dependent)
 */
hpm_stat_t mcan_transmit_via_txfifo_nonblocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *fifo_index);


/**
 * @brief Queue one frame for transmission through TX Queue without blocking
 * @param [in] ptr MCAN base
 * @param [in] tx_frame Pointer to the frame to transmit
 * @param [out] queue_index Pointer to the queue slot index used for this frame
 * @note Use this only when TX Queue mode is enabled.
 * @retval status_success if the frame was accepted for transmission
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txqueue_not_enabled if the controller is not configured for TX Queue mode
 * @retval status_mcan_txfifo_full if the TX Queue has no available slot
 * @retval status_timeout if the SoC-specific buffer access workaround timed out (platform-dependent)
 */
hpm_stat_t mcan_transmit_via_txqueue_nonblocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *queue_index);

/**
 * @brief Start transmission through a dedicated TX buffer without blocking
 * @param [in] ptr MCAN base
 * @param [in] index TX buffer index
 * @param [in] tx_frame Pointer to the frame to transmit
 * @retval status_success if the frame was accepted for transmission
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_txbuf_index_out_of_range if `index` exceeds the configured dedicated TX buffer count
 * @retval status_fail if the selected slot has a transmission request pending
 * @retval status_timeout if the SoC-specific buffer access workaround timed out (platform-dependent)
 */
hpm_stat_t mcan_transmit_via_txbuf_nonblocking(MCAN_Type *ptr, uint32_t index, mcan_tx_frame_t *tx_frame);

/**
 * @brief Wait for and receive one frame from the selected RX buffer
 * @param [in] ptr MCAN base
 * @param [in] index RXBUF index
 * @param [out] rx_frame Pointer to the destination frame buffer
 * @retval status_success if a frame was received successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_timeout if no frame became available within the driver's retry budget
 * @retval status_mcan_rxbuf_empty if the buffer became empty before the read completed; caller should retry
 */
hpm_stat_t mcan_receive_from_buf_blocking(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame);

/**
 * @brief Wait for and receive one frame from the selected RX FIFO
 * @param [in] ptr MCAN base
 * @param [in] fifo_index RX FIFO index, `0` for RXFIFO0 or `1` for RXFIFO1
 * @param [out] rx_frame Pointer to the destination frame buffer
 * @retval status_success if a frame was received successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_timeout if no frame became available within the driver's retry budget
 * @retval status_mcan_rxfifo_empty if the FIFO was drained before the read completed; caller should retry
 */
hpm_stat_t mcan_receive_from_fifo_blocking(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame);

/***********************************************************************************************************************
 * @brief Diagnostics, timestamp, and status APIs
 *
 * @note Use these helpers to query controller state during normal runtime. They
 *       provide decoded snapshots, so callers do not have to combine several
 *       low-level register reads unless they need raw register values.
 **********************************************************************************************************************/
/**
 * @brief Decode timestamp information carried by a TX Event FIFO element
 *
 * Extracts the timestamp from a TX event element and populates a @ref mcan_timestamp_value_t
 * structure.  The returned structure indicates the timestamp width (16 / 32 / 64-bit) via its
 * boolean flags, matching the active timestamping mode.
 *
 * @code
 *     mcan_tx_event_fifo_elem_t tx_evt;
 *     mcan_read_tx_evt_fifo(ptr, &tx_evt);
 *
 *     mcan_timestamp_value_t ts;
 *     if (mcan_get_timestamp_from_tx_event(ptr, &tx_evt, &ts) == status_success) {
 *         if (ts.is_64bit) { printf("TX ts: %llu\n", ts.ts_64bit); }
 *     }
 * @endcode
 *
 * @param [in] ptr MCAN base
 * @param [in] tx_evt Pointer to the TX event element to decode
 * @param [out] timestamp Pointer to the decoded timestamp value
 * @retval status_success if timestamp information was decoded successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_timestamp_not_exist if no timestamp information is available for the supplied event
 */
hpm_stat_t mcan_get_timestamp_from_tx_event(MCAN_Type *ptr,
                                            const mcan_tx_event_fifo_elem_t *tx_evt,
                                            mcan_timestamp_value_t *timestamp);

/**
 * @brief Decode timestamp information carried by a received RX frame
 *
 * Extracts the timestamp from an RX message and populates a @ref mcan_timestamp_value_t
 * structure.  In internal-counter mode the 16-bit value comes from @c rx_timestamp; in TSU mode
 * the driver reads the TSU register indicated by @c rx_timestamp_pointer.
 *
 * @code
 *     mcan_rx_message_t rx_frame;
 *     mcan_read_rxfifo(ptr, 0, &rx_frame);
 *
 *     mcan_timestamp_value_t ts;
 *     if (mcan_get_timestamp_from_received_message(ptr, &rx_frame, &ts) == status_success) {
 *         if (ts.is_64bit)      { printf("RX ts: %llu\n", ts.ts_64bit); }
 *         else if (ts.is_32bit) { printf("RX ts: %u\n",   ts.ts_32bit); }
 *         else if (ts.is_16bit) { printf("RX ts: %u\n",   ts.ts_16bit); }
 *     }
 * @endcode
 *
 * @param [in] ptr MCAN base
 * @param [in] rx_msg Pointer to the received message to inspect
 * @param [out] timestamp Pointer to the decoded timestamp value
 * @retval status_success if timestamp information was decoded successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @retval status_mcan_timestamp_not_exist if no timestamp information is available for the supplied frame
 */
hpm_stat_t mcan_get_timestamp_from_received_message(MCAN_Type *ptr,
                                                    const mcan_rx_message_t *rx_msg,
                                                    mcan_timestamp_value_t *timestamp);

/**
 * @brief Decode a raw PSR register value into a software-friendly status structure
 * @param [in] psr Raw Protocol Status Register value
 * @param [out] protocol_status Pointer to the decoded status structure
 * @retval status_success if the PSR value was decoded successfully
 * @retval status_invalid_argument if any parameters are invalid
 */
hpm_stat_t mcan_parse_protocol_status(uint32_t psr, mcan_protocol_status_t *protocol_status);

/**
 * @brief Read and decode the live MCAN protocol status
 * @param [in] ptr MCAN base
 * @param [out] protocol_status Pointer to the decoded protocol status structure
 * @retval status_success if the protocol status was read and decoded successfully
 * @retval status_invalid_argument if any parameters are invalid
 */
hpm_stat_t mcan_get_protocol_status(MCAN_Type *ptr, mcan_protocol_status_t *protocol_status);

/**
 * @brief Read the current MCAN error state
 * @param [in] ptr MCAN base
 * @param [out] error_state Pointer to the destination error state
 * @retval status_success if the error state was read successfully
 * @retval status_invalid_argument if any parameters are invalid
 * @note This helper derives one consolidated state from protocol status with the priority order:
 *       bus-off -> error passive -> error warning -> error active.
 */
hpm_stat_t mcan_get_error_state(MCAN_Type *ptr, mcan_error_state_t *error_state);

/**
 * @brief Read a consolidated runtime diagnostic snapshot
 * @param [in] ptr MCAN base
 * @param [out] snapshot Pointer to the destination diagnostic snapshot
 * @retval status_success if the snapshot was collected successfully
 * @retval status_invalid_argument if any parameters are invalid
 */
hpm_stat_t mcan_get_diagnostic_snapshot(MCAN_Type *ptr, mcan_diagnostic_snapshot_t *snapshot);

/**
 * @brief Clear software-clearable runtime fault indications
 * @param [in] ptr MCAN base
 * @param [in] interrupt_mask Bitmask of interrupt flags to clear from `IR`
 * @retval status_success if the requested interrupt flags were cleared successfully
 * @retval status_invalid_argument if `ptr` is invalid
 * @note This helper clears sticky interrupt indications only. It does not rebuild message RAM or reconfigure the
 *       controller.
 */
hpm_stat_t mcan_clear_runtime_faults(MCAN_Type *ptr, uint32_t interrupt_mask);

/**
 * @brief Start the driver's bus-off recovery sequence
 * @param [in] ptr MCAN base
 * @retval status_success if the restart sequence was issued successfully
 * @retval status_invalid_argument if `ptr` is invalid
 * @retval status_timeout if the controller did not complete the required mode transitions in time
 * @note This helper performs a runtime restart only: enter initialization mode, clear sticky runtime interrupt flags,
 *       then return to normal mode. The node will rejoin the bus only when external bus conditions are valid again.
 */
hpm_stat_t mcan_recover_from_busoff(MCAN_Type *ptr);

/**
 * @brief Query whether CAN FD operation is currently enabled
 * @param [in] ptr MCAN base
 * @retval true if `CCCR.FDOE` is set
 * @retval false otherwise
 */
bool mcan_is_canfd_enabled(MCAN_Type *ptr);

/*---------------------------------------------------------------------
 * Address-computation helpers (promoted from .c for use by fast API)
 *---------------------------------------------------------------------*/

/**
 * @brief Calculate the total element size (header + data) in bytes
 *        for a given data field size option.
 *
 * @param [in] data_field_size_option  One of `MCAN_DATA_FIELD_SIZE_*` constants
 * @return Element size in bytes (header + payload)
 */
static inline uint32_t mcan_calc_elem_total_size(uint8_t data_field_size_option)
{
    uint32_t data_size;
    if (data_field_size_option < MCAN_DATA_FIELD_SIZE_32BYTES) {
        data_size = 8U + 4U * data_field_size_option;
    } else {
        data_size = 32U + (data_field_size_option - MCAN_DATA_FIELD_SIZE_32BYTES) * 16U;
    }
    return data_size + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;
}

/**
 * @brief Get the base address of RXFIFO0 in the message RAM
 * @param [in] ptr MCAN base
 * @return Absolute byte address of the first RXFIFO0 element
 */
static inline uint32_t mcan_get_rxfifo0_base(MCAN_Type *ptr)
{
    uint32_t rxfifo0_buf_offset = MCAN_RXF0C_F0SA_GET(ptr->RXF0C) << 2;
    return mcan_get_ram_base(ptr) + rxfifo0_buf_offset;
}

/**
 * @brief Get the base address of RXFIFO1 in the message RAM
 * @param [in] ptr MCAN base
 * @return Absolute byte address of the first RXFIFO1 element
 */
static inline uint32_t mcan_get_rxfifo1_base(MCAN_Type *ptr)
{
    uint32_t rxfifo1_buf_offset = MCAN_RXF1C_F1SA_GET(ptr->RXF1C) << 2;
    return mcan_get_ram_base(ptr) + rxfifo1_buf_offset;
}

/**
 * @brief Get the absolute address of a dedicated RX buffer element
 * @param [in] ptr   MCAN base
 * @param [in] index RX buffer element index
 * @return Absolute byte address of the element in message RAM
 */
static inline uint32_t mcan_get_rxbuf_elem_addr(MCAN_Type *ptr, uint32_t index)
{
    uint32_t elem_size = mcan_calc_elem_total_size((uint8_t)MCAN_RXESC_RBDS_GET(ptr->RXESC));
    uint32_t rxbuf_offset = MCAN_RXBC_RBSA_GET(ptr->RXBC) << MCAN_RXBC_RBSA_SHIFT;
    return mcan_get_ram_base(ptr) + rxbuf_offset + elem_size * index;
}

/**
 * @brief Get the absolute address of a TX buffer element (dedicated or FIFO/Queue)
 * @param [in] ptr   MCAN base
 * @param [in] index TX buffer element index
 * @return Absolute byte address of the element in message RAM
 */
static inline uint32_t mcan_get_txbuf_elem_addr(MCAN_Type *ptr, uint32_t index)
{
    uint32_t elem_size = mcan_calc_elem_total_size((uint8_t)MCAN_TXESC_TBDS_GET(ptr->TXESC));
    uint32_t txbuf_offset = MCAN_TXBC_TBSA_GET(ptr->TXBC) << MCAN_TXBC_TBSA_SHIFT;
    return mcan_get_ram_base(ptr) + txbuf_offset + elem_size * index;
}

/*---------------------------------------------------------------------
 * Unchecked TX/RX API: validation-free primitives
 *---------------------------------------------------------------------
 *
 * These functions skip parameter validation and payload-size clamping. They are
 * meant for ISR handlers and other tight paths where the caller has already
 * checked every precondition listed in the function comment.
 *
 * Use the standard APIs, such as `mcan_transmit_via_txfifo_nonblocking()` and
 * `mcan_read_rxfifo()`, unless this shorter path is needed. The standard APIs
 * validate arguments, clamp copies to the configured data-field capacity, and
 * return an `hpm_stat_t` error code on misuse.
 *
 * If any precondition is wrong, these unchecked functions can corrupt message
 * RAM, cause bus errors, or trigger hard faults.
 */

/**
 * @brief [Unchecked] Write a TX frame to the next TX FIFO slot and issue a transmit request
 *
 * Unchecked equivalent of @ref mcan_transmit_via_txfifo_nonblocking().
 * Collapses the 3-level call chain into a single function with no validation
 * and no payload-size clamping.
 *
 * @warning Call @ref mcan_transmit_via_txfifo_nonblocking() unless the caller
 *          has already checked the preconditions below.
 *
 * Caller preconditions (NOT checked):
 *   - @p ptr is a valid, initialized MCAN base pointer
 *   - @p tx_frame points to a fully populated @ref mcan_tx_frame_t with correct DLC
 *   - @p put_index is not NULL
 *   - TX FIFO is not full (caller verified via interrupt flag or @ref mcan_is_txfifo_full())
 *   - The payload size implied by `tx_frame->dlc` does not exceed the configured
 *     TX element data-field capacity (`TXESC.TBDS`); no clamping is performed
 *
 * @param [in]  ptr        MCAN base address
 * @param [in]  tx_frame   Pointer to the TX frame to transmit
 * @param [out] put_index  Receives the FIFO slot index used (for later completion polling
 *                         via @ref mcan_is_transmit_occurred())
 */
void mcan_write_txfifo_unchecked(MCAN_Type *ptr, const mcan_tx_frame_t *tx_frame, uint32_t *put_index);

/**
 * @brief [Unchecked] Write a TX frame to a dedicated TX buffer slot and issue a transmit request
 *
 * Unchecked equivalent of @ref mcan_transmit_via_txbuf_nonblocking().
 *
 * @warning Call @ref mcan_transmit_via_txbuf_nonblocking() unless the caller
 *          has already checked the preconditions below.
 *
 * Caller preconditions (NOT checked):
 *   - @p ptr is a valid, initialized MCAN base pointer
 *   - @p tx_frame points to a fully populated @ref mcan_tx_frame_t with correct DLC
 *   - @p index < configured dedicated TX buffer count (`TXBC.NDTB`)
 *   - No transmit request is pending on this buffer index
 *   - The payload size implied by `tx_frame->dlc` does not exceed the configured
 *     TX element data-field capacity (`TXESC.TBDS`); no clamping is performed
 *
 * @param [in] ptr       MCAN base address
 * @param [in] index     Dedicated TX buffer index
 * @param [in] tx_frame  Pointer to the TX frame to transmit
 */
void mcan_write_txbuf_unchecked(MCAN_Type *ptr, uint32_t index, const mcan_tx_frame_t *tx_frame);

/**
 * @brief [Unchecked] Read one frame from an RX FIFO and acknowledge it
 *
 * Unchecked equivalent of @ref mcan_read_rxfifo().
 * Reads the next available element from the selected RX FIFO, copies the header
 * and payload into @p rx_frame, then writes the FIFO acknowledge register so the
 * hardware can reuse the slot.
 *
 * @warning Call @ref mcan_read_rxfifo() unless the caller has already checked
 *          the preconditions below.
 *
 * Caller preconditions (NOT checked):
 *   - @p ptr is a valid, initialized MCAN base pointer
 *   - @p rx_frame points to a valid @ref mcan_rx_message_t buffer
 *   - @p fifo_index is 0 or 1
 *   - The selected FIFO is not empty (caller confirmed via interrupt flag or
 *     @ref mcan_get_rxfifo_fill_level())
 *   - The received frame's DLC does not exceed the configured RX FIFO element
 *     data-field capacity (`RXESC.F0DS` / `RXESC.F1DS`); no clamping is performed
 *
 * @param [in]  ptr         MCAN base address
 * @param [in]  fifo_index  0 for RXFIFO0, 1 for RXFIFO1
 * @param [out] rx_frame    Destination buffer for the received frame
 */
void mcan_read_rxfifo_unchecked(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame);

/**
 * @brief [Unchecked] Read one frame from a dedicated RX buffer
 *
 * Unchecked equivalent of @ref mcan_read_rxbuf().
 * Copies the header and payload from the selected RX buffer element into
 * @p rx_frame.
 *
 * @note This function leaves the NDAT flag set. Call
 *       @ref mcan_clear_rxbuf_data_available_flag() after processing the frame.
 *
 * @warning Call @ref mcan_read_rxbuf() unless the caller has already checked
 *          the preconditions below.
 *
 * Caller preconditions (NOT checked):
 *   - @p ptr is a valid, initialized MCAN base pointer
 *   - @p rx_frame points to a valid @ref mcan_rx_message_t buffer
 *   - @p index < configured RX buffer count
 *   - NDAT flag is set for this index (caller confirmed via interrupt or
 *     @ref mcan_is_rxbuf_data_available())
 *   - The received frame's DLC does not exceed the configured RX buffer element
 *     data-field capacity (`RXESC.RBDS`); no clamping is performed
 *
 * @param [in]  ptr       MCAN base address
 * @param [in]  index     RX buffer element index
 * @param [out] rx_frame  Destination buffer for the received frame
 */
void mcan_read_rxbuf_unchecked(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame);

/**
 * @}
 *
 */

#ifdef __cplusplus
}
#endif


#endif /* HPM_MCAN_DRV_H */
