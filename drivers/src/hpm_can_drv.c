/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_can_drv.h"
#include <string.h>

/***********************************************************************************************************************
 *
 *  Definitions
 *
 **********************************************************************************************************************/
#define TSEG1_MIN_FOR_CAN2_0 (2U)
#define TSEG1_MAX_FOR_CAN2_0 (257U)

#define TSEG1_MIN_FOR_CANFD_NOMINAL (2U)
#define TSEG1_MAX_FOR_CANFD_NOMINAL (257U)

#define TSEG1_MIN_FOR_CANFD_DATA (2U)
#define TSEG1_MAX_FOR_CANFD_DATA (33U)

#define TSEG2_MIN_FOR_CAN2_0 (1U)
#define TSEG2_MAX_FOR_CAN2_0 (128U)

#define TSEG2_MIN_FOR_CANFD_NOMINAL (1U)
#define TSEG2_MAX_FOR_CANFD_NOMINAL (128U)

#define TSEG2_MIN_FOR_CANFD_DATA (1U)
#define TSEG2_MAX_FOR_CANFD_DATA (16U)

#define TSJW_MIN_FOR_CAN2_0 (1U)
#define TSJW_MAX_FOR_CAN2_0 (128U)

#define TSJW_MIN_FOR_CANFD_NOMINAL (1U)
#define TSJW_MAX_FOR_CANFD_NOMINAL (128U)

#define TSJW_MIN_FOR_CANFD_DATA (1U)
#define TSJW_MAX_FOR_CANFD_DATA (16U)

#define NUM_TQ_MIN_FOR_CAN2_0 (8U)
#define NUM_TQ_MAX_FOR_CAN2_0 (TSEG1_MAX_FOR_CAN2_0 + TSEG2_MAX_FOR_CAN2_0)

#define NUM_TQ_MIN_FOR_CANFD_NOMINAL (8U)
#define NUM_TQ_MAX_FOR_CANFD_NOMINAL (TSEG1_MAX_FOR_CANFD_NOMINAL + TSEG2_MAX_FOR_CANFD_NOMINAL)

#define NUM_TQ_MIN_FOR_CANFD_DATA (8U)
#define NUM_TQ_MAX_FOR_CANFD_DATA (TSEG1_MAX_FOR_CANFD_DATA + TSEG2_MAX_FOR_CANFD_DATA)

#define MIN_TQ_MUL_PRESCALE (10U)

#define NUM_PRESCALE_MAX (256U)

#define CAN_FILTER_INDEX_MAX (15U)
#define CAN_FILTER_NUM_MAX (16U)

#define CAN_TIMEOUT_CNT (0xFFFFFFUL)
#define CAN_STD_ID_MASK (0x7FFU)
#define CAN_EXT_ID_MASK (0x1FFFFFFFUL)

#define CAN_SAMPLEPOINT_MIN (750U)
#define CAN_SAMPLEPOINT_MAX (875U)
#define CAN_SAMPLEPOINT_HARD_MIN (500U)
#define CAN_SAMPLEPOINT_HARD_MAX (950U)


#define CAN_DEFAULT_FILTER_SETTING {0, can_filter_id_mode_both_frames, true, 0, 0x3FFFFFFFUL }

/*
 * @brief CAN bit-timing table
 */
typedef struct {
    uint16_t tq_min;
    uint16_t tq_max;
    uint16_t seg1_min;
    uint16_t seg1_max;
    uint16_t seg2_min;
    uint16_t seg2_max;
    uint16_t sjw_min;
    uint16_t sjw_max;
    uint8_t min_diff_seg1_minus_seg2;
} can_bit_timing_table_t;

/**
 * @brief CAN bit timing list for all supported bit timing modes
 */
static const can_bit_timing_table_t s_can_bit_timing_tbl[3] = {
        {
                .tq_min = NUM_TQ_MIN_FOR_CAN2_0,
                .tq_max = NUM_TQ_MAX_FOR_CAN2_0,
                .seg1_min = TSEG1_MIN_FOR_CAN2_0,
                .seg1_max = TSEG1_MAX_FOR_CAN2_0,
                .seg2_min = TSEG2_MIN_FOR_CAN2_0,
                .seg2_max = TSEG2_MAX_FOR_CAN2_0,
                .sjw_min = TSJW_MIN_FOR_CAN2_0,
                .sjw_max = TSJW_MAX_FOR_CAN2_0,
                .min_diff_seg1_minus_seg2 = 2,
        },
        {
                .tq_min = NUM_TQ_MIN_FOR_CANFD_NOMINAL,
                .tq_max = NUM_TQ_MAX_FOR_CANFD_NOMINAL,
                .seg1_min = TSEG1_MIN_FOR_CANFD_NOMINAL,
                .seg1_max = TSEG1_MAX_FOR_CANFD_NOMINAL,
                .seg2_min = TSEG2_MIN_FOR_CANFD_NOMINAL,
                .seg2_max = TSEG2_MAX_FOR_CANFD_NOMINAL,
                .sjw_min = TSJW_MIN_FOR_CANFD_NOMINAL,
                .sjw_max = TSJW_MAX_FOR_CANFD_NOMINAL,
                .min_diff_seg1_minus_seg2 = 2,
        },
        {
                .tq_min = NUM_TQ_MIN_FOR_CANFD_DATA,
                .tq_max = NUM_TQ_MAX_FOR_CANFD_DATA,
                .seg1_min = TSEG1_MIN_FOR_CANFD_DATA,
                .seg1_max = TSEG1_MAX_FOR_CANFD_DATA,
                .seg2_min = TSEG2_MIN_FOR_CANFD_DATA,
                .seg2_max = TSEG2_MAX_FOR_CANFD_DATA,
                .sjw_min = TSJW_MIN_FOR_CANFD_DATA,
                .sjw_max = TSJW_MAX_FOR_CANFD_DATA,
                .min_diff_seg1_minus_seg2 = 1,
        }
};

/***********************************************************************************************************************
 *
 *  Prototypes
 */
static uint32_t find_optimal_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler,
                                       uint32_t max_tq, uint32_t min_tq);

static uint8_t can_get_data_words_from_dlc(uint32_t dlc);

static void can_fill_tx_buffer(CAN_Type *base, const can_transmit_buf_t *message);

static bool is_can_bit_timing_param_valid(can_bit_timing_option_t option, const can_bit_timing_param_t *param);
static bool is_can_samplepoint_range_valid(uint16_t samplepoint_min, uint16_t samplepoint_max);
static bool is_can_tx_message_valid(const can_transmit_buf_t *message);
static hpm_stat_t can_get_rx_error_status(uint8_t error_type);
static hpm_stat_t can_read_message_from_hw_rx_buffer(CAN_Type *base, can_receive_buf_t *message);


/***********************************************************************************************************************
 *
 *  Codes
 */
static uint32_t find_optimal_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler,
                                       uint32_t max_tq, uint32_t min_tq)
{
    bool has_found = false;

    uint32_t prescaler = start_prescaler;

    while (!has_found) {
        if (prescaler > NUM_PRESCALE_MAX) {
            break;
        }
        if ((num_tq_mul_prescaler / prescaler > max_tq) || (num_tq_mul_prescaler % prescaler != 0U)) {
            ++prescaler;
            continue;
        } else {
            uint32_t tq = num_tq_mul_prescaler / prescaler;
            if (tq * prescaler == num_tq_mul_prescaler) {
                has_found = true;
                break;
            } else if (tq < min_tq) {
                has_found = false;
                break;
            } else {
                ++prescaler;
            }
        }
    }

    return has_found ? prescaler : 0U;
}


hpm_stat_t can_calculate_bit_timing(uint32_t src_clk_freq, can_bit_timing_option_t option, uint32_t baudrate,
                              uint16_t samplepoint_min, uint16_t samplepoint_max,
                              can_bit_timing_param_t *timing_param)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((option > can_bit_timing_canfd_data) || (baudrate == 0U) || (timing_param == NULL)) {
            break;
        }
        if (!is_can_samplepoint_range_valid(samplepoint_min, samplepoint_max)) {
            break;
        }
        if (src_clk_freq / baudrate < MIN_TQ_MUL_PRESCALE) {
            break;
        }

        const can_bit_timing_table_t *tbl = &s_can_bit_timing_tbl[(uint8_t) option];

        /* According to the CAN specification 2.0,
         * the Tq must be in range specified in the above CAN bit-timing table
         */
        if (src_clk_freq / baudrate < tbl->tq_min) {
            break;
        }

        uint32_t num_tq_mul_prescaler = src_clk_freq / baudrate;
        uint32_t start_prescaler = 1U;
        uint32_t num_seg1, num_seg2;
        bool has_found = false;

        /* Find out the minimum prescaler */
        uint32_t current_prescaler;
        while (!has_found) {
            current_prescaler = find_optimal_prescaler(num_tq_mul_prescaler, start_prescaler,
                                                       tbl->tq_max,
                                                       tbl->tq_min);
            if ((current_prescaler < start_prescaler) || (current_prescaler > NUM_PRESCALE_MAX)) {
                break;
            }
            uint32_t num_tq = num_tq_mul_prescaler / current_prescaler;

            num_seg2 = (num_tq - tbl->min_diff_seg1_minus_seg2) / 2U;
            num_seg1 = num_tq - num_seg2;

            /* Recommended sample point is 75% - 87.5% */
            bool seg_limit_hit = false;
            while ((num_seg1 * 1000U) / num_tq < samplepoint_min) {
                ++num_seg1;
                --num_seg2;
                if (num_seg2 < tbl->seg2_min) {
                    seg_limit_hit = true;
                    break;
                }
                if (num_seg1 > tbl->seg1_max) {
                    seg_limit_hit = true;
                    break;
                }
            }

            if (seg_limit_hit) {
                start_prescaler = current_prescaler + 1U;
                continue;
            }

            if ((num_seg1 * 1000U) / num_tq > samplepoint_max) {
                break;
            }

            if ((num_seg2 >= tbl->seg2_min) && (num_seg1 <= tbl->seg1_max)) {
                has_found = true;
            } else {
                start_prescaler = current_prescaler + 1U;
            }
        }

        if (has_found) {
            uint32_t num_sjw = MIN(tbl->sjw_max, num_seg2);
            timing_param->num_seg1 = num_seg1;
            timing_param->num_seg2 = num_seg2;
            timing_param->num_sjw = num_sjw;
            timing_param->prescaler = current_prescaler;
            status = status_success;
        }
    } while (false);

    return status;
}

static bool is_can_bit_timing_param_valid(can_bit_timing_option_t option, const can_bit_timing_param_t *param)
{
    bool result = false;
    do {
        if ((option > can_bit_timing_canfd_data) || (param == NULL)) {
            break;
        }
        const can_bit_timing_table_t *tbl = &s_can_bit_timing_tbl[(uint8_t) option];

        if ((param->num_seg1 < tbl->seg1_min) || (param->num_seg1 > tbl->seg1_max)) {
            break;
        }
        if ((param->num_seg2 < tbl->seg2_min) || (param->num_seg2 > tbl->seg2_max)) {
            break;
        }
        if ((param->num_sjw < tbl->sjw_min) || (param->num_sjw > tbl->sjw_max)) {
            break;
        }
        if ((param->prescaler == 0U) || (param->prescaler > NUM_PRESCALE_MAX)) {
            break;
        }
        result = true;
    } while (false);

    return result;
}

static bool is_can_samplepoint_range_valid(uint16_t samplepoint_min, uint16_t samplepoint_max)
{
    bool result = false;

    do {
        if ((samplepoint_min < CAN_SAMPLEPOINT_HARD_MIN) || (samplepoint_min > CAN_SAMPLEPOINT_HARD_MAX)) {
            break;
        }
        if ((samplepoint_max < CAN_SAMPLEPOINT_HARD_MIN) || (samplepoint_max > CAN_SAMPLEPOINT_HARD_MAX)) {
            break;
        }
        if (samplepoint_min > samplepoint_max) {
            break;
        }

        result = true;
    } while (false);

    return result;
}

static bool is_can_tx_message_valid(const can_transmit_buf_t *message)
{
    bool result = false;

    do {
        if (message == NULL) {
            break;
        }

        /* NOTE: the width of message->DLC is 4bits, the value is naturally permitted, no need to check here */
        if (message->canfd_frame == 0U) {
            if ((message->bitrate_switch != 0U) || (message->dlc > can_payload_size_8)) {
                break;
            }
        } else {
            if (message->remote_frame != 0U) {
                break;
            }
        }

        result = true;
    } while (false);

    return result;
}

static hpm_stat_t can_get_rx_error_status(uint8_t error_type)
{
    hpm_stat_t status = status_can_other_error;

    switch (error_type) {
    case 0U:
        status = status_success;
        break;
    case 1U:
        status = status_can_bit_error;
        break;
    case 2U:
        status = status_can_form_error;
        break;
    case 3U:
        status = status_can_stuff_error;
        break;
    case 4U:
        status = status_can_ack_error;
        break;
    case 5U:
        status = status_can_crc_error;
        break;
    default:
        break;
    }

    return status;
}

static hpm_stat_t can_read_message_from_hw_rx_buffer(CAN_Type *base, can_receive_buf_t *message)
{
    message->buffer[0] = base->RBUF[0];
    message->buffer[1] = base->RBUF[1];

    /* Timestamp words are always copied; caller can validate TIMEEN if needed. */
    message->buffer[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS] = base->RBUF[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS];
    message->buffer[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS + 1] = base->RBUF[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS + 1];

    if (message->remote_frame == 0U) {
        uint32_t copy_words = can_get_data_words_from_dlc(message->dlc);
        for (uint32_t i = 0; i < copy_words; i++) {
            message->buffer[2U + i] = base->RBUF[2U + i];
        }
    }

    /* Always release RX buffer slot after reading metadata/data from hardware. */
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RREL_MASK;

    return can_get_rx_error_status(message->error_type);
}

hpm_stat_t can_set_bit_timing(CAN_Type *base, can_bit_timing_option_t option,
                              uint32_t src_clk_freq, uint32_t baudrate,
                              uint16_t samplepoint_min, uint16_t samplepoint_max)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if (base == NULL) {
            break;
        }

        can_bit_timing_param_t timing_param;
        status = can_calculate_bit_timing(src_clk_freq, option, baudrate, samplepoint_min, samplepoint_max, &timing_param);

        if (status == status_success) {
            if (option < can_bit_timing_canfd_data) {
                base->S_PRESC = CAN_S_PRESC_S_PRESC_SET(timing_param.prescaler - 1U) | CAN_S_PRESC_S_SEG_1_SET(timing_param.num_seg1 - 2U) |
                                CAN_S_PRESC_S_SEG_2_SET(timing_param.num_seg2 - 1U) | CAN_S_PRESC_S_SJW_SET(timing_param.num_sjw - 1U);
            } else {
                base->F_PRESC = CAN_F_PRESC_F_PRESC_SET(timing_param.prescaler - 1U) | CAN_F_PRESC_F_SEG_1_SET(timing_param.num_seg1 - 2U) |
                                CAN_F_PRESC_F_SEG_2_SET(timing_param.num_seg2 - 1U) | CAN_F_PRESC_F_SJW_SET(timing_param.num_sjw - 1U);

            }
            status = status_success;
        }

    } while (false);

    return status;
}

hpm_stat_t can_set_filter(CAN_Type *base, const can_filter_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((base == NULL) || (config == NULL)) {
            break;
        }
        if (config->index > CAN_FILTER_INDEX_MAX) {
            status = status_can_filter_index_invalid;
            break;
        }
        if (config->id_mode > can_filter_id_mode_extended_frames) {
            break;
        }

        /* Configure acceptance code */
        base->ACFCTRL = CAN_ACFCTRL_ACFADR_SET(config->index);
        base->ACF = CAN_ACF_CODE_MASK_SET(config->code);

        /* Configure acceptance mask */
        uint32_t acf_value = CAN_ACF_CODE_MASK_SET(config->mask);
        if (config->id_mode == can_filter_id_mode_standard_frames) {
            acf_value |= CAN_ACF_AIDEE_MASK;
        } else if (config->id_mode == can_filter_id_mode_extended_frames) {
            acf_value |= CAN_ACF_AIDEE_MASK | CAN_ACF_AIDE_MASK;
        } else {
            /* Treat it as the default mode */
            acf_value |= 0;
        }

        base->ACFCTRL = CAN_ACFCTRL_SELMASK_MASK | CAN_ACFCTRL_ACFADR_SET(config->index);
        base->ACF = acf_value;

        if (config->enable) {
            base->ACF_EN |= (1U << config->index);
        } else {
            base->ACF_EN &= (uint16_t) ~(1U << config->index);
        }
        status = status_success;
    } while (false);

    return status;
}

static uint8_t can_get_data_words_from_dlc(uint32_t dlc)
{
    uint8_t size_bytes = can_get_payload_size_from_dlc((uint8_t) dlc);
    return (uint8_t)((size_bytes + 3U) / sizeof(uint32_t));
}


static void can_fill_tx_buffer(CAN_Type *base, const can_transmit_buf_t *message)
{
    base->TBUF[0] = message->buffer[0];
    base->TBUF[1] = message->buffer[1];

    uint32_t copy_words = can_get_data_words_from_dlc(message->dlc);
    for (uint32_t i = 0U; i < copy_words; i++) {
        base->TBUF[2U + i] = message->buffer[2U + i];
    }
}

static hpm_stat_t can_wait_reset_state(CAN_Type *base, bool expect_reset_asserted)
{
    hpm_stat_t status = status_success;
    int32_t timeout_cnt = CAN_TIMEOUT_CNT;
    uint32_t expected_bit = expect_reset_asserted ? CAN_CMD_STA_CMD_CTRL_RESET_MASK : 0U;

    while ((base->CMD_STA_CMD_CTRL & CAN_CMD_STA_CMD_CTRL_RESET_MASK) != expected_bit) {
        timeout_cnt--;
        if (timeout_cnt <= 0) {
            status = status_timeout;
            break;
        }
    }

    return status;
}

hpm_stat_t can_send_message_blocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {

        if ((base == NULL) || (message == NULL)) {
            break;
        }
        if (!is_can_tx_message_valid(message)) {
            break;
        }

        /* Wait until STB is not full */
        status = status_success;
        int32_t timeout_cnt = CAN_TIMEOUT_CNT;
        while (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_STB_IS_FULL) {
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
            timeout_cnt--;
        }
        if (status != status_success) {
            break;
        }

        status = status_success;
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
        can_fill_tx_buffer(base, message);

        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSNEXT_MASK | CAN_CMD_STA_CMD_CTRL_TSONE_MASK;
        timeout_cnt = CAN_TIMEOUT_CNT;
        while (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) != CAN_STB_IS_EMPTY) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }

    } while (false);

    return status;
}

hpm_stat_t can_send_high_priority_message_blocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL));
        HPM_BREAK_IF(!is_can_tx_message_valid(message));
        status = status_success;

        int32_t timeout_cnt = CAN_TIMEOUT_CNT;
        while (IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_TPE_MASK)) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }
        HPM_BREAK_IF(status != status_success);

        /* Select the high-priority buffer */
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;

        can_fill_tx_buffer(base, message);

        /* Send the data out */
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPE_MASK;
        /* Wait until the data is sent out */
        timeout_cnt = CAN_TIMEOUT_CNT;
        while (IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_TPE_MASK)) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }
    } while (false);

    return status;
}

hpm_stat_t can_send_message_nonblocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {

        if ((base == NULL) || (message == NULL)) {
            break;
        }
        if (!is_can_tx_message_valid(message)) {
            break;
        }

        if (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_STB_IS_FULL) {
            status = status_can_tx_fifo_full;
            break;
        }

        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
        can_fill_tx_buffer(base, message);
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSNEXT_MASK | CAN_CMD_STA_CMD_CTRL_TSONE_MASK;

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t can_send_high_priority_message_nonblocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL));
        HPM_BREAK_IF(!is_can_tx_message_valid(message));
        status = status_success;

        if (IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_TPE_MASK)) {
            status = status_can_tx_fifo_full;
            break;
        }
        /* Select the high-priority buffer */
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;

        can_fill_tx_buffer(base, message);

        /* Send the data out */
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPE_MASK;
    } while (false);

    return status;
}

hpm_stat_t can_receive_message_blocking(CAN_Type *base, can_receive_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL));

        int32_t timeout_cnt = CAN_TIMEOUT_CNT;
        while (CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_RXBUF_IS_EMPTY) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }

        HPM_BREAK_IF(status != status_invalid_argument);

        status = can_read_message_from_hw_rx_buffer(base, message);

    } while (false);

    return status;
}

hpm_stat_t can_receive_message_nonblocking(CAN_Type *base, can_receive_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL));

        if (CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_RXBUF_IS_EMPTY) {
            status = status_can_rxbuf_empty;
            break;
        }

        status = can_read_message_from_hw_rx_buffer(base, message);
    } while (false);

    return status;
}

hpm_stat_t can_read_received_message(CAN_Type *base, can_receive_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL));

        if (CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_RXBUF_IS_EMPTY) {
            status = status_can_rxbuf_empty;
            break;
        }

        status = can_read_message_from_hw_rx_buffer(base, message);

    } while (false);

    return status;
}

void can_init_tx_message_std(can_transmit_buf_t *message, uint16_t std_id, uint8_t dlc)
{
    do {
        if (message == NULL) {
            break;
        }

        memset(message, 0, sizeof(*message));
        message->extend_id = 0U;
        message->id = std_id & CAN_STD_ID_MASK;
        message->dlc = MIN(dlc, can_payload_size_8);
    } while (false);
}

void can_init_tx_message_ext(can_transmit_buf_t *message, uint32_t ext_id, uint8_t dlc)
{
    do {
        if (message == NULL) {
            break;
        }

        memset(message, 0, sizeof(*message));
        message->extend_id = 1U;
        message->id = ext_id & CAN_EXT_ID_MASK;
        message->dlc = MIN(dlc, can_payload_size_8);
    } while (false);
}

hpm_stat_t can_set_tx_message_payload(can_transmit_buf_t *message, const uint8_t *payload, uint8_t payload_size)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(message == NULL);
        HPM_BREAK_IF((payload == NULL) && (payload_size != 0U));
        HPM_BREAK_IF((message->remote_frame != 0U) && (payload_size != 0U));

        HPM_BREAK_IF((message->canfd_frame == 0U) && (message->dlc > can_payload_size_8));

        uint8_t payload_capacity = can_get_payload_size_from_dlc((uint8_t) message->dlc);
        HPM_BREAK_IF(payload_size > payload_capacity);

        /* Clear the full payload area first so bytes beyond payload_size remain deterministic. */
        memset(message->data, 0, sizeof(message->buffer) - sizeof(uint32_t) * 2U);
        if (payload_size != 0U) {
            memcpy(message->data, payload, payload_size);
        }

        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t can_get_default_config(can_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;
    if (config != NULL) {

        /*
         * Canonical base configuration:
         * classic CAN is the default mode, while CAN FD-related fields stay
         * available for callers that opt into CAN FD explicitly before can_init().
         */
        config->baudrate = 1000000UL; /* 1Mbit/s */
        config->baudrate_fd = 0U;
        config->use_lowlevel_timing_setting = false;
        config->can20_samplepoint_min = CAN_SAMPLEPOINT_MIN;
        config->can20_samplepoint_max = CAN_SAMPLEPOINT_MAX;
        config->canfd_samplepoint_min = CAN_SAMPLEPOINT_MIN;
        config->canfd_samplepoint_max = CAN_SAMPLEPOINT_MAX;
        config->enable_canfd = false;
        config->enable_can_fd_iso_mode = true;

        config->mode = can_mode_normal;
        config->enable_self_ack = false;
        config->disable_stb_retransmission = false;
        config->disable_ptb_retransmission = false;
        config->enable_tx_buffer_priority_mode = false;
        config->enable_tdc = false;

        /* No user filter list by default; the init path applies the permissive default filter. */
        config->filter_list_num = 0;
        config->filter_list = NULL;

        /* Keep interrupts disabled by default; enable them explicitly when needed. */
        config->irq_txrx_enable_mask = 0;
        config->irq_error_enable_mask = 0;

        config->time_stamping_position = CAN_TIME_STAMPING_POSITION_EOF;
        config->enable_time_stamping = false;

        status = status_success;
    }

    return status;
}

hpm_stat_t can_init(CAN_Type *base, can_config_t *config, uint32_t src_clk_freq)
{
    hpm_stat_t status = status_invalid_argument;

    do {

        HPM_BREAK_IF((base == NULL) || (config == NULL) || (src_clk_freq == 0U));
        if (config->filter_list_num > CAN_FILTER_NUM_MAX) {
            status = status_can_filter_num_invalid;
            break;
        }
        HPM_BREAK_IF((config->filter_list_num > 0U) && (config->filter_list == NULL));
        if (!config->use_lowlevel_timing_setting) {
            HPM_BREAK_IF(config->baudrate == 0U);
            HPM_BREAK_IF(config->enable_canfd && (config->baudrate_fd == 0U));
            HPM_BREAK_IF(!is_can_samplepoint_range_valid(config->can20_samplepoint_min, config->can20_samplepoint_max));
            if (config->enable_canfd) {
                HPM_BREAK_IF(!is_can_samplepoint_range_valid(config->canfd_samplepoint_min, config->canfd_samplepoint_max));
            }
        }

        can_reset(base, true);

        base->TTCFG &= ~CAN_TTCFG_TTEN_MASK;
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TTTBM_MASK;

        if (!config->use_lowlevel_timing_setting) {
            if (config->enable_canfd) {
                status = can_set_bit_timing(base,
                                            can_bit_timing_canfd_nominal,
                                            src_clk_freq,
                                            config->baudrate,
                                            config->can20_samplepoint_min,
                                            config->can20_samplepoint_max);
                HPM_BREAK_IF(status != status_success);
                status = can_set_bit_timing(base,
                                            can_bit_timing_canfd_data,
                                            src_clk_freq,
                                            config->baudrate_fd,
                                            config->canfd_samplepoint_min,
                                            config->canfd_samplepoint_max);
            } else {
                status = can_set_bit_timing(base,
                                            can_bit_timing_can2_0,
                                            src_clk_freq,
                                            config->baudrate,
                                            config->can20_samplepoint_min,
                                            config->can20_samplepoint_max);
            }
        } else {
            if (config->enable_canfd) {
                bool param_valid = is_can_bit_timing_param_valid(can_bit_timing_canfd_nominal, &config->can_timing);
                if (!param_valid) {
                    status = status_can_invalid_bit_timing;
                    break;
                }
                param_valid = is_can_bit_timing_param_valid(can_bit_timing_canfd_data, &config->canfd_timing);
                if (!param_valid) {
                    status = status_can_invalid_bit_timing;
                    break;
                }
                can_set_slow_speed_timing(base, &config->can_timing);
                can_set_fast_speed_timing(base, &config->canfd_timing);
            } else {
                bool param_valid = is_can_bit_timing_param_valid(can_bit_timing_can2_0, &config->can_timing);
                if (!param_valid) {
                    status = status_can_invalid_bit_timing;
                    break;
                }
                can_set_slow_speed_timing(base, &config->can_timing);
            }
            status = status_success;
        }

        /* TDC applies to the CAN FD data phase only. */
        if (config->enable_canfd) {
            uint32_t ssp_offset = CAN_F_PRESC_F_SEG_1_GET(base->F_PRESC) + 2U;
            can_set_transmitter_delay_compensation(base, ssp_offset, config->enable_tdc);
        } else {
            can_set_transmitter_delay_compensation(base, 0U, false);
        }

        HPM_BREAK_IF(status != status_success);


        /* Program hardware acceptance filters after the timing configuration is accepted. */
        if (config->filter_list_num > CAN_FILTER_NUM_MAX) {
            status = status_can_filter_num_invalid;
            break;
        } else if (config->filter_list_num == 0) {
            can_filter_config_t default_filter = CAN_DEFAULT_FILTER_SETTING;
            for (uint32_t i = 0; i < CAN_FILTER_NUM_MAX; i++) {
                can_disable_filter(base, i);
            }
            (void) can_set_filter(base, &default_filter);
        } else {
            for (uint32_t i = 0; i < CAN_FILTER_NUM_MAX; i++) {
                can_disable_filter(base, i);
            }
            for (uint32_t i = 0; i < config->filter_list_num; i++) {
                status = can_set_filter(base, &config->filter_list[i]);
                if (status != status_success) {
                    break;
                }
            }
            HPM_BREAK_IF(status != status_success);
        }

        /* Apply ISO CAN FD behavior selection before leaving initialization. */
        can_enable_can_fd_iso_mode(base, config->enable_can_fd_iso_mode);

         can_reset(base, false);

        /* The following mode must be set when the CAN controller is not in reset mode */

        /* Disable re-transmission on PTB on demand */
        can_disable_ptb_retransmission(base, config->disable_ptb_retransmission);
        /* Disable re-transmission on STB on demand */
        can_disable_stb_retransmission(base, config->disable_stb_retransmission);

        /* Set Self-ack mode*/
        can_enable_self_ack(base, config->enable_self_ack);

        /* Set CAN work mode */
        can_set_node_mode(base, config->mode);

        /* Configure TX Buffer priority mode */
        can_select_tx_buffer_priority_mode(base, config->enable_tx_buffer_priority_mode);

        /* Configure interrupt */
        can_disable_tx_rx_irq(base, 0xFFU);
        can_disable_error_irq(base, 0xFFU);
        can_enable_tx_rx_irq(base, config->irq_txrx_enable_mask);
        can_enable_error_irq(base, config->irq_error_enable_mask);

        can_set_time_stamping_mode(base, config->time_stamping_position, config->enable_time_stamping);

        status = status_success;
    } while (false);

    return status;
}

void can_init_tx_message_std_fd(can_transmit_buf_t *message, uint16_t std_id, uint8_t dlc, bool enable_bitrate_switch)
{
    do {
        if (message == NULL) {
            break;
        }

        memset(message, 0, sizeof(*message));
        message->extend_id = 0U;
        message->id = std_id & CAN_STD_ID_MASK;
        message->canfd_frame = 1U;
        message->bitrate_switch = (uint32_t) enable_bitrate_switch;
        message->remote_frame = 0U;
        message->dlc = MIN(dlc, can_payload_size_max);
    } while (false);
}

void can_init_tx_message_ext_fd(can_transmit_buf_t *message, uint32_t ext_id, uint8_t dlc, bool enable_bitrate_switch)
{
    do {
        if (message == NULL) {
            break;
        }

        memset(message, 0, sizeof(*message));
        message->extend_id = 1U;
        message->id = ext_id & CAN_EXT_ID_MASK;
        message->canfd_frame = 1U;
        message->bitrate_switch = (uint32_t) enable_bitrate_switch;
        message->remote_frame = 0U;
        message->dlc = MIN(dlc, can_payload_size_max);
    } while (false);
}

hpm_stat_t can_get_timestamp_for_transmitted_message(CAN_Type *base, can_timestamp_value_t *timestamp)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((base == NULL) || (timestamp == NULL));

        if (!can_is_time_stamping_enabled(base)) {
            status = status_can_timestamping_disabled;
            break;
        }
        timestamp->nano_sec = base->TTS[0];
        timestamp->second = base->TTS[1];
        status = status_success;

    } while (false);
    return status;
}

hpm_stat_t can_get_rx_message_info(const can_receive_buf_t *message, can_frame_info_t *info)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((message == NULL) || (info == NULL));

        uint8_t payload_size = can_get_payload_size_from_dlc((uint8_t) message->dlc);
        HPM_BREAK_IF((payload_size == 0U) && (message->dlc != can_payload_size_0));

        memset(info, 0, sizeof(*info));
        info->id = message->id;
        info->dlc = (uint8_t) message->dlc;
        info->payload_size = payload_size;
        info->is_extended_id = (message->extend_id != 0U);
        info->is_canfd_frame = (message->canfd_frame != 0U);
        info->enable_bitrate_switch = (message->bitrate_switch != 0U);
        info->is_remote_frame = (message->remote_frame != 0U);
        info->loopback_message = (message->loopback_message != 0U);
        info->error_state_indicator = (message->error_state_indicator != 0U);
        info->transmit_timestamp_enabled = false;
        info->error_type = (uint8_t) message->error_type;

        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t can_get_tx_message_info(const can_transmit_buf_t *message, can_frame_info_t *info)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((message == NULL) || (info == NULL));

        uint8_t payload_size = can_get_payload_size_from_dlc((uint8_t) message->dlc);
        HPM_BREAK_IF((payload_size == 0U) && (message->dlc != can_payload_size_0));

        memset(info, 0, sizeof(*info));
        info->id = message->id;
        info->dlc = (uint8_t) message->dlc;
        info->payload_size = payload_size;
        info->is_extended_id = (message->extend_id != 0U);
        info->is_canfd_frame = (message->canfd_frame != 0U);
        info->enable_bitrate_switch = (message->bitrate_switch != 0U);
        info->is_remote_frame = (message->remote_frame != 0U);
        info->loopback_message = false;
        info->error_state_indicator = false;
        info->transmit_timestamp_enabled = (message->transmit_timestamp_enable != 0U);
        info->error_type = 0U;

        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t can_get_timestamp_from_received_message(CAN_Type *base, const can_receive_buf_t *message, can_timestamp_value_t *timestamp)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((base == NULL) || (message == NULL) || (timestamp == NULL));

        if (!can_is_time_stamping_enabled(base)) {
            status = status_can_timestamping_disabled;
            break;
        }
        timestamp->nano_sec = message->buffer[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS];
        timestamp->second = message->buffer[CAN_TIMESTAMP_OFFSET_IN_RX_MSG_IN_WORDS + 1];
        status = status_success;

    } while (false);
    return status;
}

void can_deinit(CAN_Type *base)
{
    do {
        HPM_BREAK_IF(base == NULL);
        can_force_bus_off(base);
        can_reset(base, true);
    } while (false);
}

hpm_stat_t can_recover_from_busoff(CAN_Type *base)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(base == NULL);

        if (!can_is_in_bus_off_mode(base)) {
            status = status_success;
            break;
        }

        can_reset(base, true);
        status = can_wait_reset_state(base, true);
        HPM_BREAK_IF(status != status_success);

        can_clear_tx_rx_flags(base, 0xFFU);
        can_clear_error_interrupt_flags(base, 0xFFU);

        can_reset(base, false);
        status = can_wait_reset_state(base, false);
    } while (false);

    return status;
}