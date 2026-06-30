/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_CAN_DRV_H
#define HPM_CAN_DRV_H

#include "hpm_common.h"
#include "hpm_can_regs.h"
#include "hpm_soc_feature.h"

/**
 * @brief CAN driver APIs
 * @defgroup can_interface CAN driver APIs
 * @ingroup can_interfaces
 * @{
 *
 */

/**
 * @defgroup can_quick_start Quick-start example
 * @ingroup can_interface
 * @{
 *
 * Usage model:
 *  - Configuration: call can_get_default_config(), then override only baudrate,
 *    mode, CAN FD enablement, filters, interrupts, and optional timestamping
 *    before calling can_init().
 *  - Frame construction: choose one of can_init_tx_message_std(),
 *    can_init_tx_message_ext(), can_init_tx_message_std_fd(), or
 *    can_init_tx_message_ext_fd(), then fill payload bytes with
 *    can_set_tx_message_payload().
 *  - Data path: use can_send_message_blocking() / can_receive_message_blocking()
 *    for direct request-response flows, or the non-blocking APIs for
 *    interrupt-driven code. Use can_get_tx_message_info() /
 *    can_get_rx_message_info() only when a decoded metadata view is needed.
 *
 * Frame helper selection:
 *  - Classic CAN, standard ID: can_init_tx_message_std()
 *  - Classic CAN, extended ID: can_init_tx_message_ext()
 *  - CAN FD, standard ID: can_init_tx_message_std_fd()
 *  - CAN FD, extended ID: can_init_tx_message_ext_fd()
 *
 * Rules that matter in practice:
 *  - Classic CAN uses DLC values 0..8 only. DLC values above 8 are valid only
 *    for CAN FD frames.
 *  - CAN FD payload sizes are discrete legal sizes: 0..8, 12, 16, 20, 24, 32,
 *    48, and 64 bytes.
 *  - can_get_payload_size_from_dlc() converts the raw DLC field to payload size
 *    in bytes; use it whenever code needs the actual byte count.
 *  - RTR frames must not carry payload data.
 *  - can_set_tx_message_payload() clears any unused bytes within the payload area
 *    of the frame buffer after copying the caller-provided bytes.
 *  - Timestamps are available only when time-stamping is enabled in can_config_t
 *    before calling can_init().
 *  - Filter helpers only prepare the filter structure; hardware acceptance is not
 *    changed until can_set_filter() or can_init() programs the filter list.
 *
 * Common call sequences:
 *
 * Classic blocking TX/RX:
 * @code{.c}
 * can_get_default_config(&config);
 * config.mode = can_mode_loopback_internal;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 * can_init_tx_message_std(&tx_message, 0x123, 8);
 * can_set_tx_message_payload(&tx_message, payload, sizeof(payload));
 * can_send_message_blocking(BOARD_CAN_BASE, &tx_message);
 * can_receive_message_blocking(BOARD_CAN_BASE, &rx_message);
 * @endcode
 *
 * CAN FD blocking TX/RX:
 * @code{.c}
 * can_get_default_config(&config);
 * config.enable_canfd = true;
 * config.baudrate = 500000;
 * config.baudrate_fd = 2000000;
 * config.enable_tdc = true;
 * config.mode = can_mode_loopback_internal;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 * can_init_tx_message_std_fd(&tx_message, 0x321, can_payload_size_16, true);
 * can_set_tx_message_payload(&tx_message, payload, sizeof(payload));
 * can_send_message_blocking(BOARD_CAN_BASE, &tx_message);
 * can_receive_message_blocking(BOARD_CAN_BASE, &rx_message);
 * @endcode
 *
 * Filtered receive:
 * @code{.c}
 * can_get_default_filter_config(&filter);
 * can_init_filter_standard_frames(&filter, 0, 0x123, 0x7FF, true);
 * config.filter_list = &filter;
 * config.filter_list_num = 1;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 * @endcode
 *
 * PTPC-backed timestamp-enabled transmit and receive:
 * @code{.c}
 * #include "hpm_clock_drv.h"
 * #include "hpm_ptpc_drv.h"
 *
 * ptpc_config_t ptpc_config;
 * can_timestamp_value_t tx_ts;
 * can_timestamp_value_t rx_ts;
 *
 * clock_add_to_group(clock_ptpc, 0);
 * ptpc_get_default_config(HPM_PTPC, &ptpc_config);
 * ptpc_config.src_frequency = clock_get_frequency(clock_ptpc);
 * // Accuracy note: the PTPC nanosecond increment step is computed as
 * // 1,000,000,000 / src_frequency. Choosing an exact integer divisor of 1 GHz
 * // (for example 100 / 125 / 200 / 250 / 500 MHz) avoids truncation.
 * ptpc_config.capture_keep = false;
 * ptpc_config.coarse_increment = false;
 * ptpc_init(HPM_PTPC, PTPC_PTPC_0, &ptpc_config);
 * ptpc_init_timer(HPM_PTPC, PTPC_PTPC_0);
 *
 * can_get_default_config(&config);
 * config.baudrate = 1000000;
 * config.mode = can_mode_loopback_internal;
 * config.enable_time_stamping = true;
 * config.time_stamping_position = CAN_TIME_STAMPING_POSITION_EOF;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 * can_init_tx_message_std(&tx_message, 0x123, 8);
 * can_set_tx_message_payload(&tx_message, payload, sizeof(payload));
 * tx_message.transmit_timestamp_enable = 1;
 * can_send_message_blocking(BOARD_CAN_BASE, &tx_message);
 * can_receive_message_blocking(BOARD_CAN_BASE, &rx_message);
 * can_get_timestamp_for_transmitted_message(BOARD_CAN_BASE, &tx_ts);
 * can_get_timestamp_from_received_message(BOARD_CAN_BASE, &rx_message, &rx_ts);
 * @endcode
 *
 * Common sources of confusion:
 *  - Choosing a classic frame helper, then assigning a CAN FD DLC.
 *  - Treating the @p payload_size parameter of can_set_tx_message_payload() as a
 *    way to enlarge frame capacity; the frame capacity is always determined by the
 *    message DLC and frame type set earlier.
 *  - Expecting an arbitrary CAN FD payload length; use the legal CAN FD payload
 *    sizes only.
 *  - Using a standard-frame helper when a 29-bit identifier is intended.
 *  - Expecting timestamp APIs to work if timestamping was not enabled in config.
 *
 * Minimal loopback example:
 * @code{.c}
 * can_config_t config;
 * can_transmit_buf_t tx_message;
 * can_receive_buf_t rx_message;
 * uint8_t payload[8] = {0, 1, 2, 3, 4, 5, 6, 7};
 *
 * can_get_default_config(&config);
 * config.mode = can_mode_loopback_internal;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 *
 * can_init_tx_message_std(&tx_message, 0x123, 8);
 * can_set_tx_message_payload(&tx_message, payload, sizeof(payload));
 * can_send_message_blocking(BOARD_CAN_BASE, &tx_message);
 * can_receive_message_blocking(BOARD_CAN_BASE, &rx_message);
 * @endcode
 *
 * Minimal CAN FD example:
 * @code{.c}
 * can_config_t config;
 * can_transmit_buf_t tx_message;
 * uint8_t payload[16] = {0};
 *
 * can_get_default_config(&config);
 * config.enable_canfd = true;
 * config.baudrate = 500000;
 * config.baudrate_fd = 2000000;
 * config.enable_tdc = true;
 * config.mode = can_mode_loopback_internal;
 * can_init(BOARD_CAN_BASE, &config, board_init_can_clock(BOARD_CAN_BASE));
 *
 * can_init_tx_message_std_fd(&tx_message, 0x321, can_payload_size_16, true);
 * can_set_tx_message_payload(&tx_message, payload, sizeof(payload));
 * can_send_message_blocking(BOARD_CAN_BASE, &tx_message);
 * @endcode
 *
 * Practical reference helpers:
 *  - Use can_get_payload_size_from_dlc() when code needs payload size in bytes.
 *  - Use can_get_tx_message_info() / can_get_rx_message_info() when code needs a
 *    decoded metadata view rather than raw frame fields.
 *  - Use can_get_default_filter_config() and can_init_filter_*() to keep filter
 *    setup explicit and predictable.
 *
 * @}
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief CAN error codes
 */
enum {
    status_can_bit_error = MAKE_STATUS(status_group_can, 0),            /**< CAN bit error */
    status_can_form_error = MAKE_STATUS(status_group_can, 1),           /**< CAN Form error */
    status_can_stuff_error = MAKE_STATUS(status_group_can, 2),          /**< CAN stuff error */
    status_can_ack_error = MAKE_STATUS(status_group_can, 3),            /**< CAN ACK error */
    status_can_crc_error = MAKE_STATUS(status_group_can, 4),            /**< CAN CRC error */
    status_can_other_error = MAKE_STATUS(status_group_can, 5),          /**< CAN other error */
    status_can_tx_fifo_full = MAKE_STATUS(status_group_can, 6),         /**< CAN TX fifo full */
    status_can_filter_index_invalid = MAKE_STATUS(status_group_can, 7), /**< CAN filter index is invalid */
    status_can_filter_num_invalid = MAKE_STATUS(status_group_can, 8),   /**< CAN filter number is invalid */
    status_can_invalid_bit_timing = MAKE_STATUS(status_group_can, 9),   /**< Invalid CAN bit timing parameter */
    status_can_timestamping_disabled = MAKE_STATUS(status_group_can, 10), /**< Time-stamping is disabled */
    status_can_rxbuf_empty = MAKE_STATUS(status_group_can, 11),         /**< CAN RX buffer is empty */
};

/**
 * @brief CAN Event definitions
 */
#define CAN_EVENT_RECEIVE (CAN_RTIF_RIF_MASK)               /**< CAN Receive event */
#define CAN_EVENT_RX_BUF_OVERRUN (CAN_RTIF_ROIF_MASK)       /**< CAN RX buffer overrun event */
#define CAN_EVENT_RX_BUF_FULL (CAN_RTIF_RFIF_MASK)          /**< CAN RX buffer full event */
#define CAN_EVENT_RX_BUF_ALMOST_FULL (CAN_RTIF_RAFIF_MASK)  /**< CAN RX buffer almost full event */
#define CAN_EVENT_TX_PRIMARY_BUF (CAN_RTIF_TPIF_MASK)       /**< CAN TX primary buffer event */
#define CAN_EVENT_TX_SECONDARY_BUF (CAN_RTIF_TSIF_MASK)     /**< CAN TX secondary buffer event */
#define CAN_EVENT_ERROR (CAN_RTIF_EIF_MASK)                 /**< CAN error event */
#define CAN_EVENT_ABORT (CAN_RTIF_AIF_MASK)                 /**< CAN abort event */

/**
 * @brief CAN Secondary Transmit buffer Status
 */
#define CAN_STB_IS_EMPTY (0U)                               /**< CAN Secondary Transmit buffer is empty */
#define CAN_STB_LESS_EQUAL_HALF_FULL (1U)                   /**< CAN Secondary Transmit buffer <= 1/2 * FULL */
#define CAN_STB_MORE_THAN_HALF_FULL (2U)                    /**< CAN Secondary Transmit buffer > 1/2 * FULL */
#define CAN_STB_IS_FULL (3U)                                /**< CAN Secondary Transmit buffer is full */

/**
 * @brief CAN Receive Buffer States
 */
#define CAN_RXBUF_IS_EMPTY (0U)                             /**< CAN Receive buffer is empty */
#define CAN_RXBUF_HAS_DATA (1U)                             /**< CAN Receive buffer has data */
#define CAN_RXBUF_ALMOST_FULL (2U)                          /**< CAN Receive buffer is almost full */
#define CAN_RXBUF_IS_FULL (3U)                              /**< CAN Receive buffer is full */

/**
 * @brief CAN Error interrupts/flags
 *
 */
#define CAN_ERROR_WARNING_LIMIT_FLAG           (CAN_ERRINT_EWARN_MASK)     /**< CAN Error Limit reached */
#define CAN_ERROR_PASSIVE_MODE_ACTIVE_FLAG     (CAN_ERRINT_EPASS_MASK)     /**< CAN Passive mode active */
#define CAN_ERROR_PASSIVE_INT_ENABLE           (CAN_ERRINT_EPIE_MASK)      /**< CAN Passive Interrupt Enable */
#define CAN_ERROR_PASSIVE_INT_FLAG             (CAN_ERRINT_EPIF_MASK)      /**< CAN Passive Interrupt Flag */
#define CAN_ERROR_ARBITRATION_LOST_INT_ENABLE  (CAN_ERRINT_ALIE_MASK)      /**< CAN Arbitration Lost Interrupt Enable */
#define CAN_ERROR_ARBITRATION_LOST_INT_FLAG    (CAN_ERRINT_ALIF_MASK)      /**< CAN arbitration Lost Interrupt Flag */
#define CAN_ERROR_BUS_ERROR_INT_ENABLE         (CAN_ERRINT_BEIE_MASK)      /**< CAN BUS error Interrupt Enable */
#define CAN_ERROR_BUS_ERROR_INT_FLAG           (CAN_ERRINT_BEIF_MASK)      /**< CAN BUS error Interrupt flag */

/**
 * @brief CAN Error Kinds
 *
 */
#define CAN_KIND_OF_ERROR_NO_ERROR      (0U)        /**< No error */
#define CAN_KIND_OF_ERROR_BIT_ERROR     (1U)        /**< Bit error */
#define CAN_KIND_OF_ERROR_FORM_ERROR    (2U)        /**< Form error */
#define CAN_KIND_OF_ERROR_STUFF_ERROR   (3U)        /**< Stuff error */
#define CAN_KIND_OF_ERROR_ACK_ERROR     (4U)        /**< Acknowledgment error */
#define CAN_KIND_OF_ERROR_CRC_ERROR     (5U)        /**< CRC error */
#define CAN_KIND_OF_ERROR_OTHER_ERROR   (6U)        /**< Other errors */
#define CAN_KIND_OF_ERROR_BUS_OFF       (7U)        /**< BUS off error */

/**
 * @brief  CAN Time-stamping position
 */
#define CAN_TIME_STAMPING_POSITION_SOF (0U) /**< Start of Frame */
#define CAN_TIME_STAMPING_POSITION_EOF (1U) /**< End of Frame */

#define CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS 18

typedef struct {
    uint32_t nano_sec;
    uint32_t second;
} can_timestamp_value_t;

/**
 * @brief Decoded CAN frame information
 *
 * This structure provides a normalized metadata view of a raw RX/TX frame buffer.
 * It does not own payload data; use the original frame buffer's @c data field
 * to access the payload bytes after reading this metadata.
 */
typedef struct {
    uint32_t id;                       /**< 11-bit or 29-bit CAN identifier */
    uint8_t dlc;                       /**< Raw DLC field */
    uint8_t payload_size;              /**< Payload size in bytes decoded from DLC */
    bool is_extended_id;               /**< True for 29-bit identifier */
    bool is_canfd_frame;               /**< True for CAN FD frame */
    bool enable_bitrate_switch;        /**< True when BRS is enabled */
    bool is_remote_frame;              /**< True for RTR frame */
    bool loopback_message;             /**< RX only: frame was received through loopback */
    bool error_state_indicator;        /**< RX only: ESI state captured with the frame */
    bool transmit_timestamp_enabled;   /**< TX only: timestamp request flag */
    uint8_t error_type;                /**< RX only: controller-reported error type */
} can_frame_info_t;
/**
 * @brief CAN loopback types
 */
typedef enum _can_mode {
    can_mode_normal,              /**< Non-loopback mode */
    can_mode_loopback_internal,   /**< Internal loopback mode */
    can_mode_loopback_external,   /**< External loopback mode */
    can_mode_listen_only,         /**< CAN listen mode */
} can_node_mode_t;

/**
 * @brief CAN bit timing options
 */
typedef enum _can_bit_timing_option {
    can_bit_timing_can2_0,          /**< CAN 2.0 bit timing option */
    can_bit_timing_canfd_nominal,  /**< CANFD nominal timing option */
    can_bit_timing_canfd_data,      /**< CANFD data timing option */
} can_bit_timing_option_t;

/**
 * @brief CAN DLC definitions
 */
enum {
    can_payload_size_0 = 0,                 /**< Payload size is 0 */
    can_payload_size_1,                     /**< Payload size is 1 */
    can_payload_size_2,                     /**< Payload size is 2 */
    can_payload_size_3,                     /**< Payload size is 3 */
    can_payload_size_4,                     /**< Payload size is 4 */
    can_payload_size_5,                     /**< Payload size is 5 */
    can_payload_size_6,                     /**< Payload size is 6 */
    can_payload_size_7,                     /**< Payload size is 7 */
    can_payload_size_8,                     /**< Payload size is 8 */
    can_payload_size_12,                    /**< Payload size is 12 */
    can_payload_size_16,                    /**< Payload size is 16 */
    can_payload_size_20,                    /**< Payload size is 20 */
    can_payload_size_24,                    /**< Payload size is 24 */
    can_payload_size_32,                    /**< Payload size is 32 */
    can_payload_size_48,                    /**< Payload size is 48 */
    can_payload_size_64,                    /**< Payload size is 64 */
    can_payload_size_max = can_payload_size_64,
};

/**
 * @brief CAN bit timing parameters used by the low-level timing path
 *
 * These fields store the actual values expected by the driver. They are not
 * pre-decremented register values.
 *
 * In this driver, the bit timing formulas are:
 *  - total_time_quanta = num_seg1 + num_seg2
 *  - baudrate = src_clk_freq / (prescaler * total_time_quanta)
 *  - sample_point = num_seg1 / (num_seg1 + num_seg2)
 *
 * Meaning of the fields:
 *  - prescaler: divides the CAN source clock to create the time quantum
 *  - num_seg1: time quanta from the start of the bit to the sampling point
 *  - num_seg2: time quanta from the sampling point to the end of the bit
 *  - num_sjw: synchronization jump width in time quanta
 *
 * Example with src_clk_freq = 80 MHz:
 *  - prescaler = 2, num_seg1 = 60, num_seg2 = 20
 *  - total_time_quanta = 80
 *  - baudrate = 80 MHz / (2 * 80) = 500 kbit/s
 *  - sample_point = 60 / 80 = 75%
 */
typedef struct {
    uint16_t prescaler;     /**< Clock divider used in baudrate = src_clk_freq / (prescaler * (num_seg1 + num_seg2)) */
    uint16_t num_seg1;      /**< Time quanta before the sample point; sample point numerator */
    uint16_t num_seg2;      /**< Time quanta after the sample point; sample point denominator uses num_seg1 + num_seg2 */
    uint16_t num_sjw;       /**< Synchronization jump width in time quanta */
} can_bit_timing_param_t;

/**
 * @brief CAN receive buffer data structure
 */
typedef union _can_rx_buf {
    uint32_t buffer[20];                        /**< CAN buffer size */
    struct {
        struct {
            uint32_t id: 29;                    /**< CAN ID */
            uint32_t : 1;
            uint32_t error_state_indicator: 1;  /**< Error state indicator */
        };
        struct {
            uint32_t dlc: 4;                   /**< Data length option */
            uint32_t bitrate_switch: 1;        /**< bitrate switch flag */
            uint32_t canfd_frame: 1;           /**< CANFD frame flag */
            uint32_t remote_frame: 1;          /**< Remote frame flag */
            uint32_t extend_id: 1;             /**< Extended ID flag */
            uint32_t : 4;
            uint32_t loopback_message: 1;      /**< Loopback message flag */
            uint32_t error_type: 3;            /**< Error type */
            uint32_t cycle_time: 16;           /**< Cycle time */
        };
        uint8_t data[64];                      /**< Payload bytes for classic CAN or CAN FD data frames */
        can_timestamp_value_t timestamp;       /**< Timestamp value */
    };
} can_receive_buf_t;

/**
 * @brief CAN transmit buffer data structure
 */
typedef union _can_tx_buf {
    uint32_t buffer[18];                            /**< CAN transmit buffer */
    struct {
        struct {
            uint32_t id: 29;                       /**< CAN ID */
            uint32_t : 2;
            uint32_t transmit_timestamp_enable: 1;  /**< Timestamp enable flag */
        };
        struct {
            uint32_t dlc: 4;                        /**< Data length option */
            uint32_t bitrate_switch: 1;             /**< CAN FD bit-rate-switch flag. Must be 0 for classic CAN frames */
            uint32_t canfd_frame: 1;                /**< CANFD frame flag */
            uint32_t remote_frame: 1;               /**< Remote-frame flag. For CAN FD this must be 0 (ISO CAN FD) */
            uint32_t extend_id: 1;                  /**< Extended ID flag */
            uint32_t : 24;
        };
        uint8_t data[64];                           /**< Payload bytes for classic CAN or CAN FD data frames */
    };
} can_transmit_buf_t;

/**
 * @brief CAN acceptance filter modes
 */
typedef enum _can_filter_id_mode {
    can_filter_id_mode_both_frames,                /**< Accept both standard frame and extended frame */
    can_filter_id_mode_standard_frames,            /**< Accept standard frame only */
    can_filter_id_mode_extended_frames,            /**< Accept extended frame only */
} can_filter_id_mode_t;

/**
 * @brief CAN acceptance filter configuration
 */
typedef struct {
    uint16_t index;                            /**< CAN filter index */
    can_filter_id_mode_t id_mode;              /**< CAN filter id mode */
    bool enable;                               /**< CAN filter enable flag */
    uint32_t code;                             /**< CAN filter code */
    uint32_t mask;                             /**< CAN filter mask */
} can_filter_config_t;

/**
 * @brief CAN configuration
 *
 * Bit timing can be configured in one of two ways:
 *
 * 1. Automatic timing calculation
 *    - Set use_lowlevel_timing_setting = false
 *    - Fill baudrate for classic CAN or CAN FD nominal phase
 *    - Fill baudrate_fd for the CAN FD data phase when enable_canfd = true
 *    - Optionally constrain the sample point through can20_samplepoint_min/max
 *      and canfd_samplepoint_min/max
 *
 * 2. Low-level timing configuration
 *    - Set use_lowlevel_timing_setting = true
 *    - Fill can_timing for classic CAN or for the CAN FD nominal phase
 *    - Fill canfd_timing for the CAN FD data phase when enable_canfd = true
 *
 * Driver timing formulas used by the low-level path:
 *  - total_time_quanta = num_seg1 + num_seg2
 *  - baudrate = src_clk_freq / (prescaler * (num_seg1 + num_seg2))
 *  - sample_point = num_seg1 / (num_seg1 + num_seg2)
 *
 * Phase mapping:
 *  - Classic CAN: use baudrate or can_timing only
 *  - CAN FD nominal phase: use baudrate or can_timing
 *  - CAN FD data phase: use baudrate_fd or canfd_timing
 *
 * Example for CAN FD with src_clk_freq = 80 MHz:
 *  - Nominal phase: prescaler = 2, num_seg1 = 60, num_seg2 = 20
 *      baudrate = 80 MHz / (2 * (60 + 20)) = 500 kbit/s
 *      sample_point = 60 / (60 + 20) = 75%
 *  - Data phase: prescaler = 1, num_seg1 = 12, num_seg2 = 4
 *      baudrate_fd = 80 MHz / (1 * (12 + 4)) = 5 Mbit/s
 *      sample_point = 12 / (12 + 4) = 75%
 *
 * The sample point fields use actual_sample_point * 1000. For example:
 *  - 750 means 75.0%
 *  - 875 means 87.5%
 */
typedef struct {
    union {
        struct {
            uint32_t baudrate;                  /**< Classic CAN baudrate, or CAN FD nominal-phase baudrate, in bit/s */
            uint32_t baudrate_fd;               /**< CAN FD data-phase baudrate in bit/s; used only when enable_canfd = true */
            /**< Minimum classic CAN or CAN FD nominal-phase sample point, range [500, 950], value = actual sample point * 1000 */
            uint16_t can20_samplepoint_min;
            /**< Maximum classic CAN or CAN FD nominal-phase sample point, range [500, 950], value = actual sample point * 1000 */
            uint16_t can20_samplepoint_max;
            /**< Minimum CAN FD data-phase sample point, range [500, 950], value = actual sample point * 1000 */
            uint16_t canfd_samplepoint_min;
            /**< Maximum CAN FD data-phase sample point, range [500, 950], value = actual sample point * 1000 */
            uint16_t canfd_samplepoint_max;
        };
        struct {
            can_bit_timing_param_t can_timing;  /**< Low-level timing for classic CAN, or for the CAN FD nominal phase */
            can_bit_timing_param_t canfd_timing;/**< Low-level timing for the CAN FD data phase */
        };
    };

    can_node_mode_t mode;                       /**< CAN work mode */
    bool use_lowlevel_timing_setting;           /**< false: derive timing from baudrate/baudrate_fd; true: use can_timing/canfd_timing directly */
    bool enable_canfd;                          /**< Enable CAN FD */
    bool enable_self_ack;                       /**< CAN self-ack flag */
    bool disable_ptb_retransmission;            /**< disable re-transmission for primary transmit buffer */
    bool disable_stb_retransmission;            /**< disable re-transmission for secondary transmit buffer */
    bool enable_tdc;                            /**< Enable transmitter delay compensation */

    uint8_t filter_list_num;                    /**< element number of CAN filters in filter list */
    can_filter_config_t *filter_list;           /**< CAN filter list pointer */

    uint8_t irq_txrx_enable_mask;               /**< CAN TX and RX IRQ Enable Mask */
    uint8_t irq_error_enable_mask;              /**< CAN Error IRQ Enable Mask */
    bool enable_tx_buffer_priority_mode;        /**< Enable Priority-based priority */
    bool enable_can_fd_iso_mode;                /**< Enable CAN-FD ISO mode */
    uint8_t time_stamping_position;             /**< Time-stamping position. Use CAN_TIME_STAMPING_POSITION_SOF or CAN_TIME_STAMPING_POSITION_EOF */
    bool enable_time_stamping;                  /**< Enable time-stamping during can_init(). CAN timestamps are sourced directly from the PTPC timebase for this CAN IP. This is mandatory when the application domain requires frame-level time traceability, latency measurement, or event correlation. */
} can_config_t;


#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************************************************************
 *
 * Low-level controller lifecycle and mode control helpers
 *
 **********************************************************************************************************************/


/**
 * @brief Reset CAN controller
 *
 * @param [in] base  CAN base address
 * @param [in] enable reset flag for CAN controller
 *  @arg true reset the CAN controller
 *  @arg false clear the CAN controller reset flag
 */
static inline void can_reset(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RESET_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_RESET_MASK;
    }
}

/**
 * @brief Force CAN controller to Bus-off mode
 * @param [in] base CAN base address
 */
static inline void can_force_bus_off(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL = CAN_CMD_STA_CMD_CTRL_BUSOFF_MASK;
}

/**
 * @brief Set CAN mode
 *
 * @param [in] base CAN base address
 * @param [in] mode CAN loopback mode
 *  @arg can_mode_normal Normal work mode
 *  @arg can_mode_loopback_internal internal loopback mode
 *  @arg can_mode_loopback_external external loopback mode
 *  @arg can_mode_listen_only CAN listen-only mode
 */
static inline void can_set_node_mode(CAN_Type *base, can_node_mode_t mode)
{
    uint32_t cfg_stat = base->CMD_STA_CMD_CTRL & ~(CAN_CMD_STA_CMD_CTRL_LBME_MASK | CAN_CMD_STA_CMD_CTRL_LBMI_MASK | CAN_CMD_STA_CMD_CTRL_LOM_MASK);
    if (mode == can_mode_loopback_internal) {
        cfg_stat |= CAN_CMD_STA_CMD_CTRL_LBMI_MASK;
    } else if (mode == can_mode_loopback_external) {
        cfg_stat |= CAN_CMD_STA_CMD_CTRL_LBME_MASK;
    } else if (mode == can_mode_listen_only) {
        cfg_stat |= CAN_CMD_STA_CMD_CTRL_LOM_MASK;
    } else {
        /* CAN normal work mode, no change needed here */
    }
    base->CMD_STA_CMD_CTRL = cfg_stat;
}

/**
 * @brief Enable CAN listen-only mode
 * @param [in] base CAN base address
 * @param [in] enable CAN listen-only mode enable flag
 *  @arg true enable listen-only mode
 *  @arg false disable listen-only mode
 */
static inline void can_enable_listen_only_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_LOM_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_LOM_MASK;
    }
}

/**
 * @brief Enter CAN standby mode
 * @param [in] base CAN base address
 * @param [in] enable  Can standby mode enable flag
 *  @arg true enable standby mode
 *  @arg false disable standby mode
 */
static inline void can_enter_standby_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_STBY_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_STBY_MASK;
    }
}

/**
 * @brief Disable the re-transmission for the primary transmission buffer
 *
 * @param [in] base CAN base address
 * @param [in] enable Flag for disabling re-transmission for PTB
 */
static inline void can_disable_ptb_retransmission(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPSS_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TPSS_MASK;
    }
}

/**
 * @brief Check whether re-transmission is disabled for PTB or not
 *
 * @param [in] base CAN base address
 * @return true Re-transmission is disabled for PTB
 * @return false Re-transmission is enabled for PTB
 */
static inline bool can_is_ptb_retransmission_disabled(CAN_Type *base)
{
    return ((base->CMD_STA_CMD_CTRL & CAN_CMD_STA_CMD_CTRL_TPSS_MASK) != 0);
}

/**
 * @brief Disable the re-transmission for the secondary transmission buffer
 *
 * @param [in] base CAN base address
 * @param [in] enable Flag for disabling re-transmission for STB
 */
static inline void can_disable_stb_retransmission(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSSS_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TSSS_MASK;
    }
}

/**
 * @brief Check whether re-transmission is disabled for STB or not
 *
 * @param [in] base CAN base address
 * @return true Re-transmission is disabled for STB
 * @return false Re-transmission is enabled for STB
 */
static inline bool can_is_stb_retransmission_disabled(CAN_Type *base)
{
    return ((base->CMD_STA_CMD_CTRL & CAN_CMD_STA_CMD_CTRL_TSSS_MASK) != 0);
}


/***********************************************************************************************************************
 *
 * Low-level transmit-path helpers
 *
 **********************************************************************************************************************/


/**
 * @brief Select CAN TX buffer
 * @param [in] base CAN base address
 * @param [in] select_secondary_buffer CAN secondary transmit buffer selection flag
 *  @arg true Select the secondary transmit buffer
 *  @arg false Select the primary transmit buffer
 */
static inline void can_select_tx_buffer(CAN_Type *base, bool select_secondary_buffer)
{
    if (select_secondary_buffer) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
    }
}

/**
 * @brief Start CAN high priority message transmit
 * @param [in] base CAN base address
 *
 */
static inline void can_start_high_priority_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPE_MASK;
}

/**
 * @brief Abort CAN high priority message transmit
 * @param [in] base CAN base address
 */
static inline void can_abort_high_priority_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPA_MASK;
}

/**
 * @brief Start CAN message transmit
 * @param [in] base CAN base address
 */
static inline void can_start_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSONE_MASK;
}

/**
 * @brief Start CAN all messages transmit
 * @param [in] base CAN base address
 */
static inline void can_start_all_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSALL_MASK;
}

/**
 * @brief Abort CAN message transmit
 * @param [in] base CAN base address
 */
static inline void can_abort_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSA_MASK;
}

/**
 * @brief Switch to next CAN tx buffer
 * @param [in] base CAN base address
 */
static inline void can_switch_to_next_tx_buffer(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSNEXT_MASK;
}

/**
 * @brief Select CAN tx buffer priority mode
 * @param [in] base CAN base address
 * @param [in] enable_priority_decision CAN tx buffer priority mode selection flag
 *  @arg true priority decision mode
 *  @arg false FIFO mode
 */
static inline void can_select_tx_buffer_priority_mode(CAN_Type *base, bool enable_priority_decision)
{
    if (enable_priority_decision) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSMODE_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TSMODE_MASK;
    }
}

/**
 * @brief Get CAN secondary transmit buffer status
 * @param [in] base CAN base address
 * @retval secondary transmit buffer status, 0: empty, 1: <= 1/2 full, 2: > 1/2 full, 3: full
 *
 */
static inline uint8_t can_get_secondary_transmit_buffer_status(CAN_Type *base)
{
    return CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL);
}

/**
 * @brief Check whether the primary transmit buffer is full
 * @param [in] base CAN base address
 * @return true for full
 */
static inline bool can_is_primary_transmit_buffer_full(CAN_Type *base)
{
    return ((base->CMD_STA_CMD_CTRL & CAN_CMD_STA_CMD_CTRL_TPE_MASK) != 0U);
}

/**
 * @brief Check whether the secondary transmit buffer is full
 * @param [in] base CAN base address
 * @return true for full
 */
static inline bool can_is_secondary_transmit_buffer_full(CAN_Type *base)
{
    return (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_STB_IS_FULL);
}

/**
 * @brief Check whether the CAN is in BUS off mode
 *
 * @param [in] base CAN base address
 * @return true for bus off mode
 * @return false for non-bus off mode
 */
static inline bool can_is_in_bus_off_mode(CAN_Type *base)
{
    return ((base->CMD_STA_CMD_CTRL & CAN_CMD_STA_CMD_CTRL_BUSOFF_MASK) != 0U);
}

/**
 * @brief Enable can self ack
 * @param [in] base CAN base address
 * @param [in] enable Self-ack enable flag, true or false
 *
 */
static inline void can_enable_self_ack(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_SACK_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_SACK_MASK;
    }
}


/***********************************************************************************************************************
 *
 * Low-level receive-path helpers
 *
 **********************************************************************************************************************/

/**
 * @brief Set CAN receive buffer overflow mode
 * @param [in] base CAN base address
 * @param [in] override_enable receive buffer overflow mode option
 *  @arg true the oldest message will be overwritten
 *  @arg false the new message will not be stored
 */
static inline void can_set_receive_buffer_overflow_mode(CAN_Type *base, bool override_enable)
{
    if (override_enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_ROM_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_ROM_MASK;
    }
}

/**
 * @brief Check whether CAN receive buffer is overflow
 * @param [in] base CAN base address
 * @retval true or false
 */
static inline bool can_is_receive_buffer_overflow(CAN_Type *base)
{
    return IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_ROV_MASK);
}

/**
 * @brief Release CAN receive buffer
 *
 * @param [in] base CAN base address
 */
static inline void can_release_receive_buffer(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RREL_MASK;
}

/**
 * @brief Allow can store all date in receive buffer
 * @param [in] base CAN base address
 * @param [in] allow flag to determine whether to store error data frame
 */
static inline void can_allow_store_all_data_in_receive_buffer(CAN_Type *base, bool allow)
{
    if (allow) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RBALL_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_RBALL_MASK;
    }
}

/**
 * @brief Get CAN receive buffer status
 * @param [in] base CAN base address
 * @retval Receive buffer status, 0: empty, 1: > empty, < almost full, 2: >= almost full, 3: full
 */
static inline uint8_t can_get_receive_buffer_status(CAN_Type *base)
{
    return CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL);
}

/**
 * @brief Check whether there are data available in receive buffer
 * @param [in] base CAN base address
 * @return true if the receive buffer is not empty
 */
static inline bool can_is_data_available_in_receive_buffer(CAN_Type *base)
{
    return (CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL) != CAN_RXBUF_IS_EMPTY);
}


/***********************************************************************************************************************
 *
 * Low-level CAN FD, interrupt, diagnostic, and timestamp helpers
 *
 **********************************************************************************************************************/


/**
 * @brief Enable CAN FD ISO mode
 * @param [in] base CAN base address
 * @param enable CAN-FD ISO mode enable flag
 */
static inline void can_enable_can_fd_iso_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_FD_ISO_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_FD_ISO_MASK;
    }
}

/**
 * @brief Enable CAN TX/RX interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN interrupt mask
 */
static inline void can_enable_tx_rx_irq(CAN_Type *base, uint8_t mask)
{
    base->RTIE |= mask;
}

/**
 * @brief Disable CAN TX/RX interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN interrupt mask
 */
static inline void can_disable_tx_rx_irq(CAN_Type *base, uint8_t mask)
{
    base->RTIE &= ~mask;
}

/**
 * @brief Clear CAN TX/RX flags
 * @param [in] base CAN base address
 * @param [in] flags CAN TX/RX interrupt flags
 */
static inline void can_clear_tx_rx_flags(CAN_Type *base, uint8_t flags)
{
    base->RTIF = flags;
}

/**
 * @brief Get CAN TX/RX flags
 *
 * @param [in] base CAN base address
 * @retval CAN Tx/RX interrupt flags
 */
static inline uint8_t can_get_tx_rx_flags(CAN_Type *base)
{
    return base->RTIF;
}

/**
 * @brief Enable CAN error interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN error interrupt mask
 */
static inline void can_enable_error_irq(CAN_Type *base, uint8_t mask)
{
    base->ERRINT |= mask;
}

/**
 * @brief Disable CAN error interrupt
 *
 * @param [in] base CAN base address
 * @param [in] mask CAN error interrupt mask
 */
static inline void can_disable_error_irq(CAN_Type *base, uint8_t mask)
{
    base->ERRINT &= ~mask;
}

/**
 * @brief Get CAN error interrupt flags
 * @param [in] base CAN base address
 * @retval CAN error interrupt flags
 */
static inline uint8_t can_get_error_interrupt_flags(CAN_Type *base)
{
    return (base->ERRINT & (uint8_t) ~(CAN_ERRINT_EPIE_MASK | CAN_ERRINT_ALIE_MASK | CAN_ERRINT_BEIE_MASK));
}

/**
 * @brief Get CAN clear interrupt flags
 * @param [in] base CAN base address
 * @param [in] flags CAN error interrupt flags
 */
static inline void can_clear_error_interrupt_flags(CAN_Type *base, uint8_t flags)
{
    flags &= (uint8_t)~(CAN_ERRINT_EPIE_MASK | CAN_ERRINT_ALIE_MASK | CAN_ERRINT_BEIE_MASK);
    base->ERRINT |= flags;
}

/**
 * @brief Get last CAN error kind
 * @param [in] base CAN base address
 * @retval CAN last error kind
 */
static inline uint8_t can_get_last_error_kind(CAN_Type *base)
{
    return CAN_EALCAP_KOER_GET(base->EALCAP);
}

/**
 * @brief Get CAN last arbitrary lost position
 * @param [in] base CAN base address
 * @retval CAN last arbitrary lost position
 */
static inline uint8_t can_get_last_arbitration_lost_position(CAN_Type *base)
{
    return CAN_EALCAP_ALC_GET(base->EALCAP);
}

/**
 * @brief Set CAN transmitter delay compensation
 *
 * @param [in] base CAN base address
 * @param [in] sample_point delay compensation sample point
 * @param [in] enable delay compensation enable flag
 */
static inline void can_set_transmitter_delay_compensation(CAN_Type *base, uint8_t sample_point, bool enable)
{
#if defined(CAN_SOC_CANFD_TDC_REQUIRE_STUFF_EXCEPTION_WORKAROUND) && (CAN_SOC_CANFD_TDC_REQUIRE_STUFF_EXCEPTION_WORKAROUND == 1)
    (void) sample_point;
    (void) enable;
    base->TDC = CAN_TDC_TDCEN_SET((uint8_t) enable);
#else
    base->TDC = CAN_TDC_SSPOFF_SET(sample_point) | CAN_TDC_TDCEN_SET((uint8_t) enable);
#endif
}

/**
 * @brief Set CAN warning limits
 * @param [in] base CAN base address
 * @param [in] almost_full_limit
 * @param [in] error_warning_limit
 */
static inline void can_set_warning_limits(CAN_Type *base, uint8_t almost_full_limit, uint8_t error_warning_limit)
{
    base->LIMIT = CAN_LIMIT_AFWL_SET(almost_full_limit) | CAN_LIMIT_EWL_SET(error_warning_limit);
}

/**
 * @brief Get CAN receive error count
 * @param [in] base CAN base address
 * @retval CAN receive error count
 */
static inline uint8_t can_get_receive_error_count(CAN_Type *base)
{
    return base->RECNT;
}

/**
 * @brief Get CAN transmit error count
 * @param [in] base CAN base address
 * @retval CAN transmit error count
 */
static inline uint8_t can_get_transmit_error_count(CAN_Type *base)
{
    return base->TECNT;
}

/**
 * @brief Disable time-stamping feature
 * @param [in] base CAN base address
 */
static inline void can_disable_time_stamping(CAN_Type *base)
{
    base->TIMECFG &= ~CAN_TIMECFG_TIMEEN_MASK;
}

/**
 * @brief Determine whether time-stamping is enabled or not
 * @param [in] base CAN base address
 * @return Time-stamping enablement state
 */
static inline bool can_is_time_stamping_enabled(CAN_Type *base)
{
    return ((base->TIMECFG & CAN_TIMECFG_TIMEEN_MASK) != 0);
}

/**
 * @brief Set the time-stamping mode for CAN controller
 * @param [in] base CAN base address
 * @param [in] position T   ime-stamping position. 0 - SOF, 1 - EOF
 * @param [in] enable Time-stamping enable flag
 */
static inline void can_set_time_stamping_mode(CAN_Type *base, uint8_t position, bool enable)
{
    position = (position != 0U) ? 1U : 0U;
    /*  Note: TIMEPOS can only be changed if TIMEEN is 0,  the TIMEEN bit must be cleared prior to he TIMEPOS change */
    if (enable && can_is_time_stamping_enabled(base)) {
        can_disable_time_stamping(base);
    }
    base->TIMECFG = CAN_TIMECFG_TIMEPOS_SET(position) | CAN_TIMECFG_TIMEEN_SET(enable);
}

/**
 * @brief Enable a specified CAN filter
 *
 * @param [in] base CAN base address
 * @param index  CAN filter index
 */
static inline void can_enable_filter(CAN_Type *base, uint32_t index)
{
    base->ACF_EN |= (uint16_t) (1U << index);
}

/**
 * @brief Disable a specified CAN filter
 *
 * @param [in] base CAN base address
 * @param index  CAN filter index
 */
static inline void can_disable_filter(CAN_Type *base, uint32_t index)
{
    base->ACF_EN &= (uint16_t) ~(1U << index);
}


/***********************************************************************************************************************
 *
 * Frame and filter helper APIs
 *
 **********************************************************************************************************************/

/**
 * @brief Convert a CAN DLC field into payload size in bytes
 *
 * @param [in] dlc DLC field value
 * @return Payload size in bytes, or 0 if @p dlc is invalid
 */
static inline uint8_t can_get_payload_size_from_dlc(uint8_t dlc)
{
    dlc &= 0xFU;
    if (dlc <= can_payload_size_8) {
        return dlc;
    }

    switch (dlc) {
    case can_payload_size_12:
        return 12U;
    case can_payload_size_16:
        return 16U;
    case can_payload_size_20:
        return 20U;
    case can_payload_size_24:
        return 24U;
    case can_payload_size_32:
        return 32U;
    case can_payload_size_48:
        return 48U;
    case can_payload_size_64:
        return 64U;
    default:
        return 0U;
    }
}

/**
 * @brief Get decoded payload size from a received frame
 *
 * @param [in] message Received frame buffer
 * @return Payload size in bytes, or 0 if @p message is NULL or DLC is invalid
 */
static inline uint8_t can_get_rx_message_payload_size(const can_receive_buf_t *message)
{
    return (message == NULL) ? 0U : can_get_payload_size_from_dlc((uint8_t) message->dlc);
}

/**
 * @brief Get decoded payload size from a transmit frame
 *
 * @param [in] message Transmit frame buffer
 * @return Payload size in bytes, or 0 if @p message is NULL or DLC is invalid
 */
static inline uint8_t can_get_tx_message_payload_size(const can_transmit_buf_t *message)
{
    return (message == NULL) ? 0U : can_get_payload_size_from_dlc((uint8_t) message->dlc);
}

/**
 * @brief Get identifier from a received frame
 *
 * @param [in] message Received frame buffer
 * @return 11-bit or 29-bit identifier, depending on @c extend_id
 */
static inline uint32_t can_get_rx_message_id(const can_receive_buf_t *message)
{
    return (message == NULL) ? 0U : message->id;
}

/**
 * @brief Get identifier from a transmit frame
 *
 * @param [in] message Transmit frame buffer
 * @return 11-bit or 29-bit identifier, depending on @c extend_id
 */
static inline uint32_t can_get_tx_message_id(const can_transmit_buf_t *message)
{
    return (message == NULL) ? 0U : message->id;
}

/**
 * @brief Populate a filter with a permissive default configuration
 *
 * The default accepts both standard and extended frames and enables the filter.
 * Callers typically override @c index, @c id_mode, @c code, and @c mask next.
 *
 * @param [out] config Filter configuration buffer
 */
static inline void can_get_default_filter_config(can_filter_config_t *config)
{
    if (config != NULL) {
        config->index = 0U;
        config->id_mode = can_filter_id_mode_both_frames;
        config->enable = true;
        config->code = 0U;
        config->mask = 0x3FFFFFFFUL;
    }
}

/**
 * @brief Initialize a permissive filter for both standard and extended frames
 *
 * @param [out] config Filter configuration buffer
 * @param [in] index Filter slot index
 * @param [in] code Filter code value
 * @param [in] mask Filter mask value
 * @param [in] enable True to enable the filter after programming
 */
static inline void can_init_filter_both_frames(can_filter_config_t *config, uint16_t index, uint32_t code, uint32_t mask, bool enable)
{
    if (config != NULL) {
        config->index = index;
        config->id_mode = can_filter_id_mode_both_frames;
        config->enable = enable;
        config->code = code;
        config->mask = mask;
    }
}

/**
 * @brief Initialize a filter that matches standard frames only
 *
 * @param [out] config Filter configuration buffer
 * @param [in] index Filter slot index
 * @param [in] code Filter code value
 * @param [in] mask Filter mask value
 * @param [in] enable True to enable the filter after programming
 */
static inline void can_init_filter_standard_frames(can_filter_config_t *config, uint16_t index, uint32_t code, uint32_t mask, bool enable)
{
    if (config != NULL) {
        config->index = index;
        config->id_mode = can_filter_id_mode_standard_frames;
        config->enable = enable;
        config->code = code;
        config->mask = mask;
    }
}

/**
 * @brief Initialize a filter that matches extended frames only
 *
 * @param [out] config Filter configuration buffer
 * @param [in] index Filter slot index
 * @param [in] code Filter code value
 * @param [in] mask Filter mask value
 * @param [in] enable True to enable the filter after programming
 */
static inline void can_init_filter_extended_frames(can_filter_config_t *config, uint16_t index, uint32_t code, uint32_t mask, bool enable)
{
    if (config != NULL) {
        config->index = index;
        config->id_mode = can_filter_id_mode_extended_frames;
        config->enable = enable;
        config->code = code;
        config->mask = mask;
    }
}

/***********************************************************************************************************************
 *
 * High-level driver APIs
 *
 **********************************************************************************************************************/

/**
 * @name Initialization and lifecycle
 * @{ */

/**
 * @brief Get default CAN configuration parameters
 * @param [out] config CAN configuration structure
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_get_default_config(can_config_t *config);

/**
 * @brief Initialize the CAN controller
 * @param [in] base CAN base address
 * @param [in] config CAN configuration
 * @param [in] src_clk_freq CAN IP source clock frequency
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_init(CAN_Type *base, can_config_t *config, uint32_t src_clk_freq);

/**
 * @brief De-initialize the CAN controller
 * @param [in] base CAN base address
 */
void can_deinit(CAN_Type *base);

/**
 * @brief Start the driver's recommended bus-off recovery sequence
 * @param [in] base CAN base address
 * @retval status_success if the runtime restart sequence was issued successfully
 * @retval status_invalid_argument if @p base is invalid
 * @retval status_timeout if the controller did not complete the required reset transitions in time
 * @note This helper performs a runtime restart only. It requests local reset, clears sticky runtime
 *       TX/RX and error interrupt flags, then releases reset. The controller rejoins the bus only
 *       after the required bus idle time and only when external bus conditions are valid again.
 */
hpm_stat_t can_recover_from_busoff(CAN_Type *base);

/** @} */


/**
 * @name Bit timing configuration
 * @{ */

/**
 * @brief Configure the Slow Speed Bit timing using low-level interface
 * @param [in] base CAN base address
 * @param [in] param CAN bit timing parameter
 */
static inline void can_set_slow_speed_timing(CAN_Type *base, const can_bit_timing_param_t *param)
{
    base->S_PRESC = CAN_S_PRESC_S_PRESC_SET(param->prescaler - 1U) | CAN_S_PRESC_S_SEG_1_SET(param->num_seg1 - 2U) |
                                CAN_S_PRESC_S_SEG_2_SET(param->num_seg2 - 1U) | CAN_S_PRESC_S_SJW_SET(param->num_sjw - 1U);
}

/**
 * @brief Configure the Fast Speed Bit timing using low-level interface
 * @param [in] base CAN base address
 * @param [in] param CAN bit timing parameter
 */
static inline void can_set_fast_speed_timing(CAN_Type *base, const can_bit_timing_param_t *param)
{
    base->F_PRESC = CAN_F_PRESC_F_PRESC_SET(param->prescaler - 1U) | CAN_F_PRESC_F_SEG_1_SET(param->num_seg1 - 2U) |
                                CAN_F_PRESC_F_SEG_2_SET(param->num_seg2 - 1U) | CAN_F_PRESC_F_SJW_SET(param->num_sjw - 1U);
}

/**
 * @brief Configure the CAN bit timing for CAN BUS
 * @param [in] base CAN base address
 * @param [in] option CAN bit timing option
 * @param [in] src_clk_freq CAN IP source clock frequency
 * @param [in] baudrate CAN baudrate in bps
 * @param [in] samplepoint_min Minimum Sample point, range(0-1000), samplepoint_min / 1000 will be used in calculation
 * @param [in] samplepoint_max Maximum Sample point, range(0-1000), samplepoint_max / 1000 will be used in calculation
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_set_bit_timing(CAN_Type *base, can_bit_timing_option_t option,
                              uint32_t src_clk_freq, uint32_t baudrate,
                              uint16_t samplepoint_min, uint16_t samplepoint_max);

/**
 * @brief Calculate the CAN bit timing for CAN BUS
 * @param [in] src_clk_freq CAN IP source clock frequency
 * @param [in] option CAN bit timing option
 * @param [in] baudrate CAN baudrate in bps
 * @param [in] samplepoint_min Minimum Sample point, range(0-1000), samplepoint_min / 1000 will be used in calculation
 * @param [in] samplepoint_max Maximum Sample point, range(0-1000), samplepoint_max / 1000 will be used in calculation
 * @param [out] timing_param Calculated CAN bit timing parameter
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_calculate_bit_timing(uint32_t src_clk_freq, can_bit_timing_option_t option, uint32_t baudrate,
                              uint16_t samplepoint_min, uint16_t samplepoint_max,
                              can_bit_timing_param_t *timing_param);

/** @} */


/**
 * @name Frame construction and inspection
 * @{ */

/**
 * @brief Initialize a transmit message as a classic CAN standard frame
 *
 * Use this helper for 11-bit identifiers and classic CAN payload sizes only.
 * The helper masks the ID to 11 bits and clamps classic DLC to the valid range.
 *
 * @param [out] message CAN message buffer
 * @param [in] std_id 11-bit standard ID
 * @param [in] dlc Payload length code (0..8 for classic CAN)
 */
void can_init_tx_message_std(can_transmit_buf_t *message, uint16_t std_id, uint8_t dlc);

/**
 * @brief Initialize a transmit message as a classic CAN extended frame
 *
 * Use this helper for 29-bit identifiers and classic CAN payload sizes only.
 * The helper masks the ID to 29 bits and clamps classic DLC to the valid range.
 *
 * @param [out] message CAN message buffer
 * @param [in] ext_id 29-bit extended ID
 * @param [in] dlc Payload length code (0..8 for classic CAN)
 */
void can_init_tx_message_ext(can_transmit_buf_t *message, uint32_t ext_id, uint8_t dlc);

/**
 * @brief Initialize a transmit message as a CAN FD standard frame
 *
 * Use this helper when the frame is CAN FD and the identifier is 11-bit.
 * The caller is still responsible for choosing a legal CAN FD DLC.
 *
 * @param [out] message CAN message buffer
 * @param [in] std_id 11-bit standard ID
 * @param [in] dlc Payload length code (0..15 for CAN FD)
 * @param [in] enable_bitrate_switch True to enable BRS
 */
void can_init_tx_message_std_fd(can_transmit_buf_t *message, uint16_t std_id, uint8_t dlc, bool enable_bitrate_switch);

/**
 * @brief Initialize a transmit message as a CAN FD extended frame
 *
 * Use this helper when the frame is CAN FD and the identifier is 29-bit.
 * The caller is still responsible for choosing a legal CAN FD DLC.
 *
 * @param [out] message CAN message buffer
 * @param [in] ext_id 29-bit extended ID
 * @param [in] dlc Payload length code (0..15 for CAN FD)
 * @param [in] enable_bitrate_switch True to enable BRS
 */
void can_init_tx_message_ext_fd(can_transmit_buf_t *message, uint32_t ext_id, uint8_t dlc, bool enable_bitrate_switch);

/**
 * @brief Set CAN TX payload bytes with range and frame-type checks
 *
 * The function validates payload size against the frame type and DLC already set
 * in @p message. It does not change the configured DLC. If fewer bytes are
 * provided than the frame capacity, the remaining payload bytes in the frame
 * buffer are cleared to zero.
 *
 * @param [in,out] message CAN message buffer
 * @param [in] payload Payload data pointer
 * @param [in] payload_size Number of bytes to copy
 *
 * @retval status_success Payload copied successfully
 * @retval status_invalid_argument Invalid parameters or payload exceeds DLC/frame constraints
 */
hpm_stat_t can_set_tx_message_payload(can_transmit_buf_t *message, const uint8_t *payload, uint8_t payload_size);

/**
 * @brief Decode common metadata from a received CAN frame
 *
 * Use this helper when software prefers a normalized view of frame properties
 * instead of reading raw bitfields from the receive buffer directly.
 *
 * @param [in] message Received frame buffer
 * @param [out] info Decoded metadata
 * @retval status_success Metadata was decoded successfully
 * @retval status_invalid_argument Invalid parameters or unsupported DLC
 */
hpm_stat_t can_get_rx_message_info(const can_receive_buf_t *message, can_frame_info_t *info);

/**
 * @brief Decode common metadata from a transmit CAN frame
 *
 * Use this helper to verify the effective frame type, payload size, and ID form
 * represented by a prepared transmit buffer.
 *
 * @param [in] message Transmit frame buffer
 * @param [out] info Decoded metadata
 * @retval status_success Metadata was decoded successfully
 * @retval status_invalid_argument Invalid parameters or unsupported DLC
 */
hpm_stat_t can_get_tx_message_info(const can_transmit_buf_t *message, can_frame_info_t *info);

/** @} */


/**
 * @name Filter configuration
 * @{ */

/**
 * @brief Configure the acceptable filter
 * @param [in] base CAN base address
 * @param [in] config CAN filter configuration
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_set_filter(CAN_Type *base, const can_filter_config_t *config);

/** @} */


/**
 * @name Transmit operations
 * @{ */

/**
 * @brief Send CAN message using blocking transfer
 *
 * This API expects a fully prepared frame buffer. The normal preparation order is:
 * initialize frame type and DLC with one of can_init_tx_message_*(), optionally
 * modify message flags, then copy payload bytes with can_set_tx_message_payload().
 *
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_send_message_blocking(CAN_Type *base, const can_transmit_buf_t *message);

/**
 * @brief Send high-priority message using blocking transfer
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_send_high_priority_message_blocking(CAN_Type *base, const can_transmit_buf_t *message);

/**
 * @brief Send CAN message using non-blocking transfer
 *
 * Use this API when the caller owns completion handling through interrupts or
 * polling of transmit state. It does not wait for the frame to be sent.
 *
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval status_success API execution is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_tx_fifo_full Secondary transmit buffer is full
 */
hpm_stat_t can_send_message_nonblocking(CAN_Type *base, const can_transmit_buf_t *message);

/**
 * @brief Send high-priority message using non-blocking transfer
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval status_success API execution is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_tx_fifo_full Primary high-priority buffer is busy
 */
hpm_stat_t can_send_high_priority_message_nonblocking(CAN_Type *base, const can_transmit_buf_t *message);

/** @} */


/**
 * @name Receive operations
 * @{ */

/**
 * @brief Receive CAN message using non-blocking transfer
 *
 * This API reads one raw frame if one is currently available. Use
 * can_get_rx_message_info() afterward when software needs normalized metadata
 * such as decoded payload size, ID type, or frame type.
 *
 * @param [in] base CAN base address
 * @param [out] message CAN message buffer
 *
 * @retval status_success API execution is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_rxbuf_empty No message is available in RX buffer
 * @retval status_can_bit_error CAN bit error happened during receiving message
 * @retval status_can_form_error  CAN form error happened during receiving message
 * @retval status_can_stuff_error CAN stuff error happened during receiving message
 * @retval status_can_ack_error CAN ack error happened during receiving message
 * @retval status_can_crc_error CAN crc error happened during receiving message
 * @retval status_can_other_error Other error happened during receiving message
 */
hpm_stat_t can_receive_message_nonblocking(CAN_Type *base, can_receive_buf_t *message);

/**
 * @brief Receive CAN message using blocking transfer
 *
 * This API waits until one raw frame has been read or an error condition is
 * reported by the controller.
 *
 * @param [in] base CAN base address
 * @param [out] message CAN message buffer
 *
 * @retval status_success API execution is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_bit_error CAN bit error happened during receiving message
 * @retval status_can_form_error  CAN form error happened during receiving message
 * @retval status_can_stuff_error CAN stuff error happened during receiving message
 * @retval status_can_ack_error CAN ack error happened during receiving message
 * @retval status_can_crc_error CAN crc error happened during receiving message
 * @retval status_can_other_error Other error happened during receiving message
 */
hpm_stat_t can_receive_message_blocking(CAN_Type *base, can_receive_buf_t *message);

/**
 * @brief Read Received CAN message
 *
 * @note  This API can be used in an interrupt handler.
 *        It returns status_can_rxbuf_empty when no message is available.
 * @param [in] base CAN base address
 * @param [out] message CAN message buffer
 *
 * @retval status_success API execution is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_bit_error CAN bit error happened during receiving message
 * @retval status_can_form_error  CAN form error happened during receiving message
 * @retval status_can_stuff_error CAN stuff error happened during receiving message
 * @retval status_can_ack_error CAN ack error happened during receiving message
 * @retval status_can_crc_error CAN crc error happened during receiving message
 * @retval status_can_other_error Other error happened during receiving message
 * @retval status_can_rxbuf_empty No message is available in RX buffer
 */
hpm_stat_t can_read_received_message(CAN_Type *base, can_receive_buf_t *message);

/** @} */


/**
 * @name Timestamp operations
 * @{ */

/**
 * @brief Timestamp usage notes
 *
 * To use any timestamp API in this driver:
 *  - Set can_config_t.enable_time_stamping = true before calling can_init().
 *  - Select can_config_t.time_stamping_position with CAN_TIME_STAMPING_POSITION_SOF or
 *    CAN_TIME_STAMPING_POSITION_EOF according to the system timing reference.
 *  - Ensure the PTPC timebase is initialized and, when required by the system, synchronized to
 *    the application's master time reference before relying on CAN timestamps.
 *  - For best timestamp accuracy, note that the PTPC nanosecond increment step is computed as
 *    `1,000,000,000 / src_frequency`. Choosing an exact integer divisor of 1 GHz
 *    (for example 100 / 125 / 200 / 250 / 500 MHz) avoids truncation. The driver does
 *    not require this, but non-divisor frequencies introduce accumulated timing error
 *    over time.
 *  - For transmit timestamps, set can_transmit_buf_t.transmit_timestamp_enable = 1 for each
 *    frame that requires a captured TX timestamp.
 *  - For receive timestamps, first read the frame into can_receive_buf_t, then call
 *    can_get_timestamp_from_received_message().
 *
 * For this CAN IP, the timestamp value is taken directly from the PTPC timebase rather than from
 * a CAN-local free-running counter.
 *
 * In application domains that require auditable timing behavior, deterministic latency
 * analysis, or cross-node event correlation, timestamping must be configured as part of
 * controller initialization rather than enabled later in the traffic path.
 */

/**
 * @brief Get the timestamp for the transmitted message
 * @param [in] base CAN base address
 * @param [out] timestamp Buffer to hold the timestamp value
 * @retval API execution status, status_success, status_invalid_argument or status_can_timestamping_disabled
 */
hpm_stat_t can_get_timestamp_for_transmitted_message(CAN_Type *base, can_timestamp_value_t *timestamp);

/**
 * @brief Get the timestamp from the received message
 * @param [in] base CAN base address
 * @param [in] message The pointer to a received message frame
 * @param [out] timestamp Buffer to hold the timestamp value
 * @retval API execution status, status_success, status_invalid_argument or status_can_timestamping_disabled
 */
hpm_stat_t can_get_timestamp_from_received_message(CAN_Type *base, const can_receive_buf_t *message, can_timestamp_value_t *timestamp);

/** @} */

/**
 * @}
 *
 */


#ifdef __cplusplus
}
#endif


#endif /* HPM_CAN_DRV_H */

