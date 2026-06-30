/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcan_drv.h"
#include "hpm_common.h"
#include <stdint.h>
#include <string.h>

/***********************************************************************************************************************
 *
 *  Definitions
 *
 **********************************************************************************************************************/

#define MCAN_CAN_BAUDRATE_DEFAULT   (500UL * 1000UL)        /*!< Default CAN2.0 baudrate:500 kbps */
#define MCAN_CANFD_BAUDRATE_DEFAULT (2UL * 1000UL * 1000UL) /*!< Default CANFD baudrate: 2 Mbps */

/* Hardware restriction of each types of element for MCAN */
#define MCAN_STD_FILTER_ELEM_CNT_MAX (128U)
#define MCAN_EXT_FILTER_ELEM_CNT_MAX (64U)
#define MCAN_RXFIFO_ELEM_CNT_MAX     (64U)
#define MCAN_RXBUF_ELEM_CNT_MAX      (64U)
#define MCAN_TXEVT_FIFO_ELEM_CNT_MAX (32U)
#define MCAN_TXBUF_ELEM_CNT_MAX      (32U)

#define NUM_TQ_SYNC_SEG         (1U)

/*
 * Retry budgets are dimensioned for worst-case polling at the target CPU
 * frequency. They are intentionally different:
 *   MCAN_CLOCK_ENABLE_RETRY_MAX  - short; hardware acknowledges quickly
 *   MCAN_WAIT_RETRY_CNT_MAX      - medium; INIT mode transition
 *   MCAN_TX_RETRY_COUNT_MAX      - long; depends on bus arbitration
 *   MCAN_RX_RETRY_COUNT_MAX      - longest; waits for external frame
 */
#define MCAN_WAIT_RETRY_CNT_MAX (1000000U)

/**
 * @brief CAN2.0 Timing related definitions
 */
#define TSEG1_MIN_FOR_CAN2_0     (2U)
#define TSEG1_MAX_FOR_CAN2_0     (256U)
#define TSEG2_MIN_FOR_CAN2_0     (2U)
#define TSEG2_MAX_FOR_CAN2_0     (128U)
#define TSJW_MIN_FOR_CAN2_0      (1U)
#define TSJW_MAX_FOR_CAN2_0      (128U)
#define PRESCALER_MIN_FOR_CAN2_0 (1U)
#define PRESCALER_MAX_FOR_CAN2_0 (512U)
#define NUM_TQ_MIN_FOR_CAN2_0    (8U)
#define NUM_TQ_MAX_FOR_CAN2_0    (TSEG1_MAX_FOR_CAN2_0 + TSEG2_MAX_FOR_CAN2_0)

/**
 * @brief CANFD Nominal Timing related definitions
 */
#define TSEG1_MIN_FOR_CANFD_NOMINAL     (2U)
#define TSEG1_MAX_FOR_CANFD_NOMINAL     (256U)
#define TSEG2_MIN_FOR_CANFD_NOMINAL     (1U)
#define TSEG2_MAX_FOR_CANFD_NOMINAL     (128U)
#define TSJW_MIN_FOR_CANFD_NOMINAL      (1U)
#define TSJW_MAX_FOR_CANFD_NOMINAL      (128U)
#define NUM_TQ_MIN_FOR_CANFD_NOMINAL    (8U)
#define NUM_TQ_MAX_FOR_CANFD_NOMINAL    (TSEG1_MAX_FOR_CANFD_NOMINAL + TSEG2_MAX_FOR_CANFD_NOMINAL)
#define PRESCALER_MIN_FOR_CANFD_NOMINAL (1U)
#define PRESCALER_MAX_FOR_CANFD_NOMINAL (512U)

/**
 * @brief CANFD Data Timing related definitions
 */
#define TSEG1_MIN_FOR_CANFD_DATA                (1U)
#define TSEG1_MAX_FOR_CANFD_DATA                (32U)
#define TSEG2_MIN_FOR_CANFD_DATA                (2U)
#define TSEG2_MAX_FOR_CANFD_DATA                (16U)
#define TSJW_MIN_FOR_CANFD_DATA                 (1U)
#define TSJW_MAX_FOR_CANFD_DATA                 (16U)
#define PRESCALER_MIN_FOR_CANFD_DATA            (1U)
#define PRESCALER_MAX_FOR_CANFD_DATA            (32U)
#define PRESCALER_MAX_FOR_CANFD_DATA_TDC_ENABLE (2U)

#define NUM_TQ_MIN_FOR_CANFD_DATA (8U)
#define NUM_TQ_MAX_FOR_CANFD_DATA (TSEG1_MAX_FOR_CANFD_DATA + TSEG2_MAX_FOR_CANFD_DATA)

#define MIN_TQ_MUL_PRESCALE (8U)

#define CAN_SAMPLEPOINT_MIN (750U)
#define CAN_SAMPLEPOINT_MAX (875U)

/* Invalid start address of MCAN RAM */
#define MCAN_RAM_ADDR_INVALID ((1UL << 16) - 1U)
#define MCAN_STD_ID_MASK      (0x7FFU)
#define MCAN_EXT_ID_MASK      (0x1FFFFFFFUL)

/* Maximum CAN TDC offset value */
#define MCAN_MAX_TDC_OFFSET ((uint32_t)(MCAN_TDCR_TDCO_MASK >> MCAN_TDCR_TDCO_SHIFT))
/* Maximum CAN TDC Filter Window value */
#define MCAN_MAX_TDC_FILTER_WIN ((uint32_t)(MCAN_TDCR_TDCF_MASK >> MCAN_TDCR_TDCF_SHIFT))

/* Retry budgets for polling loops */
#define MCAN_CLOCK_ENABLE_RETRY_MAX     (10000UL)

/* Minimum difference between SEG1 and SEG2 for valid bit-timing */
#define MCAN_MIN_DIFF_SEG1_SEG2_CAN20       (2U)
#define MCAN_MIN_DIFF_SEG1_SEG2_CANFD_DATA  (1U)

/* Standard filter element bit-field positions */
#define MCAN_STD_FILTER_SYNC_MSG_SHIFT  (15U)
#define MCAN_STD_FILTER_SFEC_SHIFT      (27U)
#define MCAN_STD_FILTER_SFT_SHIFT       (30U)

/* Extended filter element bit-field positions */
#define MCAN_EXT_FILTER_EFEC_SHIFT      (29U)
#define MCAN_EXT_FILTER_EFT_SHIFT       (30U)

/**
 * @brief MCAN bit timing table
 */
typedef struct mcan_bit_timing_table_struct {
    uint16_t tq_min;                  /*!< Minimum Tq */
    uint16_t tq_max;                  /*!< Maximum Tq */
    uint16_t seg1_min;                /*!< Minimum TSEG1 */
    uint16_t seg1_max;                /*!< Maximum TSEG1 */
    uint8_t seg2_min;                 /*!< Minimum TSEG2 */
    uint8_t seg2_max;                 /*!< Maximum TSEG2 */
    uint8_t sjw_min;                  /*!< Minimum SJW */
    uint8_t sjw_max;                  /*!< Maximum SJW */
    uint8_t min_diff_seg1_minus_seg2; /*!< Minimum value of (TSEG1 - TSEG2 ) */
    uint8_t prescaler_min;            /*!< Minimum Prescaler value */
    uint16_t prescaler_max;           /*!< Maximum Prescaler value */
} mcan_bit_timing_table_t;

typedef struct mcan_ram_region_struct {
    uint32_t start; /*!< Start address in MCAN message RAM */
    uint32_t size;  /*!< Region size in bytes */
} mcan_ram_region_t;

/***********************************************************************************************************************
 *
 *  Variables
 *
 **********************************************************************************************************************/

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
mcan_msg_buf_attr_t mcan_soc_msg_buf_attr[MCAN_SOC_MAX_COUNT];
#endif

static const mcan_bit_timing_table_t k_mcan_bit_timing_tbl[3] = {
    /* CAN2.0 bit timing requirement */
    {
        /* TQ range */
        .tq_min = NUM_TQ_MIN_FOR_CAN2_0,
        .tq_max = NUM_TQ_MAX_FOR_CAN2_0,
        /* SEG1 range(Including Sync_Seg +  Prop_Seg + Phase_Seg1 */
        .seg1_min = TSEG1_MIN_FOR_CAN2_0,
        .seg1_max = TSEG1_MAX_FOR_CAN2_0,
        /* SEG2 Range */
        .seg2_min = TSEG2_MIN_FOR_CAN2_0,
        .seg2_max = TSEG2_MAX_FOR_CAN2_0,
        /* SJW range */
        .sjw_min = TSJW_MIN_FOR_CAN2_0,
        .sjw_max = TSJW_MAX_FOR_CAN2_0,
        .min_diff_seg1_minus_seg2 = MCAN_MIN_DIFF_SEG1_SEG2_CAN20,
        /* Prescaler range */
        .prescaler_min = PRESCALER_MIN_FOR_CAN2_0,
        .prescaler_max = PRESCALER_MAX_FOR_CAN2_0,
    },
    /* CANFD Nominal Bit timing requirement */
    { /* TQ range */
      .tq_min = NUM_TQ_MIN_FOR_CANFD_NOMINAL,
      .tq_max = NUM_TQ_MAX_FOR_CANFD_NOMINAL,
      /* SEG1 range(Including Sync_Seg +  Prop_Seg + Phase_Seg1 */
      .seg1_min = TSEG1_MIN_FOR_CANFD_NOMINAL,
      .seg1_max = TSEG1_MAX_FOR_CANFD_NOMINAL,
      /* SEG2 range */
      .seg2_min = TSEG2_MIN_FOR_CANFD_NOMINAL,
      .seg2_max = TSEG2_MAX_FOR_CANFD_NOMINAL,
      /* SJW range */
      .sjw_min = TSJW_MIN_FOR_CANFD_NOMINAL,
      .sjw_max = TSJW_MAX_FOR_CANFD_NOMINAL,
      .min_diff_seg1_minus_seg2 = MCAN_MIN_DIFF_SEG1_SEG2_CAN20,
      /* Prescaler range */
      .prescaler_min = PRESCALER_MIN_FOR_CANFD_NOMINAL,
      .prescaler_max = PRESCALER_MAX_FOR_CANFD_NOMINAL },
    /* CANFD Data Bit timing requirement */
    { /* TQ range */
      .tq_min = NUM_TQ_MIN_FOR_CANFD_DATA,
      .tq_max = NUM_TQ_MAX_FOR_CANFD_DATA,
      /* SEG1 range(Including Sync_Seg +  Prop_Seg + Phase_Seg2 */
      .seg1_min = TSEG1_MIN_FOR_CANFD_DATA,
      .seg1_max = TSEG1_MAX_FOR_CANFD_DATA,
      /* SEG2 range */
      .seg2_min = TSEG2_MIN_FOR_CANFD_DATA,
      .seg2_max = TSEG2_MAX_FOR_CANFD_DATA,
      /* SJW range */
      .sjw_min = TSJW_MIN_FOR_CANFD_DATA,
      .sjw_max = TSJW_MAX_FOR_CANFD_DATA,
      .min_diff_seg1_minus_seg2 = MCAN_MIN_DIFF_SEG1_SEG2_CANFD_DATA,
      /* Prescaler range */
      .prescaler_min = PRESCALER_MIN_FOR_CANFD_DATA,
      .prescaler_max = PRESCALER_MAX_FOR_CANFD_DATA }
};

/**
 * @brief Per-mode default constants for RAM configuration presets.
 */
typedef struct {
    uint8_t filter_count;
    uint8_t txbuf_count;
    uint8_t rxfifo_count;
    uint8_t rxbuf_count;
    uint8_t data_field_size;
    uint32_t txrx_elem_size;
} mcan_default_ram_preset_t;

static const mcan_default_ram_preset_t k_can_preset = {
    .filter_count    = MCAN_FILTER_SIZE_CAN_DEFAULT,
    .txbuf_count     = MCAN_TXBUF_SIZE_CAN_DEFAULT,
    .rxfifo_count    = MCAN_RXFIFO_SIZE_CAN_DEFAULT,
    .rxbuf_count     = MCAN_RXBUF_SIZE_CAN_DEFAULT,
    .data_field_size = MCAN_DATA_FIELD_SIZE_8BYTES,
    .txrx_elem_size  = MCAN_TXRX_ELEM_SIZE_CAN_MAX,
};

static const mcan_default_ram_preset_t k_canfd_preset = {
    .filter_count    = MCAN_FILTER_SIZE_CANFD_DEFAULT,
    .txbuf_count     = MCAN_TXBUF_SIZE_CANFD_DEFAULT,
    .rxfifo_count    = MCAN_RXFIFO_SIZE_CANFD_DEFAULT,
    .rxbuf_count     = MCAN_RXBUF_SIZE_CANFD_DEFAULT,
    .data_field_size = MCAN_DATA_FIELD_SIZE_64BYTES,
    .txrx_elem_size  = MCAN_TXRX_ELEM_SIZE_CANFD_MAX,
};

static const mcan_filter_elem_t k_default_std_id_filter = {
    /* Use classic filter */
    .filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER,
    /* Store message into RXFIFO0 if matching */
    .filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH,
    /* For Standard Identify only */
    .can_id_type = MCAN_CAN_ID_TYPE_STANDARD,
    /* Sync Message, only evaluated when "CCCR.UTSU" is set */
    .sync_message = 0U,
    /* Don't care if mask is set to all 1s */
    .filter_id = 0U,
    /* Accept all messages */
    .filter_mask = 0U,
};

static const mcan_filter_elem_t k_default_ext_id_filter = {
    /* Use classic filter */
    .filter_type = MCAN_FILTER_TYPE_CLASSIC_FILTER,
    /* Store message into RXFIFO0 if matching */
    .filter_config = MCAN_FILTER_ELEM_CFG_STORE_IN_RX_FIFO0_IF_MATCH,
    /* For Standard Identify only */
    .can_id_type = MCAN_CAN_ID_TYPE_EXTENDED,
    /* Sync Message, only evaluated when "CCCR.UTSU" is set */
    .sync_message = 0,
    /* Don't care if mask is set to all 1s */
    .filter_id = 0,
    /* Accept all messages */
    .filter_mask = 0U,
};

/***********************************************************************************************************************
 *
 *  Prototypes
 *
 **********************************************************************************************************************/
/**
 * @brief Set Nominal Bit timing
 *
 * @param [in] ptr MCAN base
 * @param [in] bit_timing Bit timing parameter
 */
static void mcan_set_can_nominal_bit_timing(MCAN_Type *ptr, const mcan_bit_timing_param_t *bit_timing);

/**
 * @brief Set CANFD Data Bit timing
 *
 * @param [in] ptr MCAN base
 *
 * @param [in] bit_timing Bit timing parameter
 */
static void mcan_set_can_data_bit_timing(MCAN_Type *ptr, const mcan_bit_timing_param_t *bit_timing);

/**
 * @brief Calculate the closest prescaler
 *
 * @param [in] num_tq_mul_prescaler Number of TQ * Prescaler
 * @param [in] start_prescaler Start of Prescaler value
 * @param [in] max_tq Maximum Timing Quantum
 * @param [in] min_tq Minimum Timing Quantum
 *
 * @return Calculated prescaler value
 */
static uint32_t mcan_find_optimal_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler, uint32_t max_tq, uint32_t min_tq);

/**
 * @brief Calculate Bit timing from baudrate
 *
 * @param [in] src_clk_freq CAN IP clock used for generating bit timing
 * @param [in] option CAN bit timing option
 * @param [in] baudrate CAN baudrate in bps
 * @param [in] samplepoint_min Minimum sample point, value = actual sample point * 1000
 * @param [in] samplepoint_max Maximum sample point, value = actual sample point * 1000
 * @param [out] timing_param Calculated Bit timing parameter
 *
 * @retval status_success if no errors happened
 * @retval status_invalid_argument if the timing parameters cannot be calculated from specified baudrate
 */
static hpm_stat_t mcan_calc_bit_timing_from_baudrate(uint32_t src_clk_freq, mcan_bit_timing_option_t option, uint32_t baudrate, uint16_t samplepoint_min,
                                                     uint16_t samplepoint_max, mcan_bit_timing_param_t *timing_param);

/**
 * @brief Set MCAN bit timing from Baudrate
 *
 * @param [in] ptr MCAN base
 * @param [in] option MCAN bit timing option
 * @param [in] src_clk_freq CAN IP clock used for generating bit timing
 * @param [in] baudrate Baudrate in bps
 * @param [in] samplepoint_min Minimum sample point, value = actual sample point * 1000
 * @param [in] samplepoint_max Maximum sample point, value = actual sample point * 1000
 *
 * @retval status_success if no errors happened
 * @retval status_invalid_argument if the timing parameters cannot be calculated from specified baudrate
 */
static hpm_stat_t mcan_set_bit_timing_from_baudrate(MCAN_Type *ptr, mcan_bit_timing_option_t option, uint32_t src_clk_freq, uint32_t baudrate,
                                                    uint16_t samplepoint_min, uint16_t samplepoint_max);

/**
 * @brief Configure MCAN TSU
 *
 * @param [in] ptr MCAN base
 * @param [in] config TSU configuration
 * @retval status_success if no errors happened
 *
 * @retval status_invalid_argument if any parameters are invalid
 */
static hpm_stat_t mcan_set_tsu(MCAN_Type *ptr, mcan_tsu_config_t *config);

/**
 * @brief Configure MCAN internal timestamp
 *
 * @param [in] ptr MCAN base
 * @param [in] config Internal Timestamp Configuration
 * @retval status_success if no errors happened
 *
 * @retval status_invalid_argument if any parameters are invalid
 */
static hpm_stat_t mcan_set_internal_timestamp(MCAN_Type *ptr, mcan_internal_timestamp_config_t *config);

/**
 * @brief Apply transmitter delay compensation settings
 *
 * @param [in] ptr MCAN base
 * @param [in] config TDC configuration
 */
static void mcan_apply_tdc_config(MCAN_Type *ptr, const mcan_tdc_config_t *config);

/**
 * @brief Check whether MCAN protected configuration registers are writable
 *
 * @param [in] ptr MCAN base
 * @retval true if INIT and CCE are set
 * @retval false otherwise
 */
static bool mcan_is_in_config_mode(MCAN_Type *ptr);

/**
 * @brief Require MCAN configuration mode for protected register access
 *
 * @param [in] ptr MCAN base
 * @retval status_success if MCAN is in configuration mode
 * @retval status_invalid_argument if MCAN is not in configuration mode
 */
static hpm_stat_t mcan_require_config_mode(MCAN_Type *ptr);

/**
 * @brief Check whether the MCAN bit timing parameter is valid
 * @param [in] option MCAN bit timing option
 * @param [in] param  MCAN bit timing parameter
 * @param [in] tdc_enable TDC enable flag
 *
 * @retval true MCAN bit timing parameter is valid
 * @retval false MCAN bit timing parameter is invalid
 */
static bool is_mcan_bit_timing_param_valid(mcan_bit_timing_option_t option, const mcan_bit_timing_param_t *param, bool tdc_enable);

/**
 * @brief Validate MCAN filter element parameters
 *
 * @param [in] ptr MCAN base
 * @param [in] filter_elem Filter element
 * @retval status_success if filter element is valid
 * @retval status_invalid_argument if filter element is invalid
 */
static hpm_stat_t mcan_validate_filter_element(MCAN_Type *ptr, const mcan_filter_elem_t *filter_elem);

/**
 * @brief Validate simplified MCAN RAM configuration
 *
 * @param [in] config MCAN RAM configuration
 * @retval status_success if RAM configuration is valid
 * @retval status_invalid_argument if RAM configuration is invalid
 */
static hpm_stat_t mcan_validate_simple_ram_config(const mcan_ram_config_t *config);

/**
 * @brief Validate flexible MCAN RAM configuration
 *
 * @param [in] ptr MCAN base
 * @param [in] config Flexible MCAN RAM configuration
 * @retval status_success if RAM configuration is valid
 * @retval status_invalid_argument if RAM configuration is invalid
 */
static hpm_stat_t mcan_validate_flexible_ram_config(MCAN_Type *ptr, const mcan_ram_flexible_config_t *config);

/**
 * @brief Encode standard filter element into message RAM words
 *
 * @param [in] filter_elem Filter element
 * @param [out] config_word Encoded standard filter word
 */
static void mcan_encode_std_filter_element(const mcan_filter_elem_t *filter_elem, uint32_t *config_word);

/**
 * @brief Encode extended filter element into message RAM words
 *
 * @param [in] filter_elem Filter element
 * @param [out] config_words Encoded extended filter words
 */
static void mcan_encode_ext_filter_element(const mcan_filter_elem_t *filter_elem, uint32_t config_words[2]);

/**
 * @brief Validate MCAN global filter configuration
 *
 * @param [in] config MCAN global filter configuration
 * @retval status_success if global filter configuration is valid
 * @retval status_invalid_argument if global filter configuration is invalid
 */
static hpm_stat_t mcan_validate_global_filter_config(const mcan_global_filter_config_t *config);

/**
 * @brief Validate one MCAN filter list against the configured hardware capacity
 *
 * @param [in] ptr MCAN base
 * @param [in] list Filter list
 * @param [in] can_id_type Expected CAN ID type for all list elements
 * @retval status_success if the filter list is valid
 * @retval status_invalid_argument if the filter list is invalid
 */
static hpm_stat_t mcan_validate_filter_list(MCAN_Type *ptr, const mcan_filter_elem_list_t *list, uint8_t can_id_type);

/**
 * @brief Validate all MCAN filter programming inputs
 *
 * @param [in] ptr MCAN base
 * @param [in] config Filter programming configuration
 * @retval status_success if the filter configuration is valid
 * @retval status_invalid_argument if the filter configuration is invalid
 */
static hpm_stat_t mcan_validate_all_filters_config(MCAN_Type *ptr, const mcan_all_filters_config_t *config);

/**
 * @brief Program a validated MCAN filter list into message RAM
 *
 * @param [in] ptr MCAN base
 * @param [in] list Filter list
 * @retval status_success if the filter list is programmed successfully
 * @retval status_invalid_argument if programming fails
 */
static hpm_stat_t mcan_program_filter_list(MCAN_Type *ptr, const mcan_filter_elem_list_t *list);

/**
 * @brief Check whether an MCAN RAM start address is word aligned
 *
 * @param [in] start_addr Message RAM start address in bytes
 * @retval true if the address is 4-byte aligned
 * @retval false otherwise
 */
static bool mcan_is_word_aligned(uint32_t start_addr);

/**
 * @brief Get the total message RAM element size for one RX/TX data element
 *
 * @param [in] data_field_size_option Encoded data field size option
 * @return Total element size in bytes, including the message header
 */
static uint32_t mcan_get_message_ram_elem_size(uint8_t data_field_size_option);

/**
 * @brief Validate flexible filter region configuration fields
 *
 * @param [in] config Flexible RAM configuration
 * @retval status_success if filter configuration fields are valid
 * @retval status_invalid_argument otherwise
 */
static hpm_stat_t mcan_validate_flexible_filter_config(const mcan_ram_flexible_config_t *config);

/**
 * @brief Validate flexible RX region configuration fields
 *
 * @param [in] config Flexible RAM configuration
 * @retval status_success if RX configuration fields are valid
 * @retval status_invalid_argument otherwise
 */
static hpm_stat_t mcan_validate_flexible_rx_config(const mcan_ram_flexible_config_t *config);

/**
 * @brief Validate flexible TX region configuration fields
 *
 * @param [in] config Flexible RAM configuration
 * @retval status_success if TX configuration fields are valid
 * @retval status_invalid_argument otherwise
 */
static hpm_stat_t mcan_validate_flexible_tx_config(const mcan_ram_flexible_config_t *config);

/**
 * @brief Validate and record all enabled flexible RAM regions
 *
 * @param [in] ptr MCAN base
 * @param [in] config Flexible RAM configuration
 * @param [in,out] known_regions Region bookkeeping array
 * @param [in] region_capacity Capacity of @p known_regions
 * @retval status_success if all enabled regions are valid
 * @retval status_invalid_argument otherwise
 */
static hpm_stat_t mcan_validate_flexible_ram_regions(MCAN_Type *ptr, const mcan_ram_flexible_config_t *config, mcan_ram_region_t *known_regions,
                                                     uint32_t region_capacity);

/**
 * @brief Check whether a RAM region is inside the valid MCAN RAM window
 *
 * @param [in] ptr MCAN base
 * @param [in] start Region start offset in bytes
 * @param [in] size Region size in bytes
 * @retval true if the region is fully inside the valid RAM window
 * @retval false otherwise
 */
static bool mcan_is_ram_region_inside_range(MCAN_Type *ptr, uint32_t start, uint32_t size);

/**
 * @brief Check whether two MCAN RAM regions overlap
 *
 * @param [in] lhs First RAM region
 * @param [in] rhs Second RAM region
 * @retval true if the regions overlap
 * @retval false otherwise
 */
static bool mcan_do_ram_regions_overlap(const mcan_ram_region_t *lhs, const mcan_ram_region_t *rhs);

/**
 * @brief Validate and record one known-size MCAN RAM region
 *
 * @param [in] ptr MCAN base
 * @param [in,out] regions Known regions collected so far
 * @param [in,out] region_count Number of valid entries in @p regions
 * @param [in] region_capacity Capacity of @p regions
 * @param [in] start Region start offset in bytes
 * @param [in] size Region size in bytes
 * @retval status_success if the region is valid and recorded
 * @retval status_invalid_argument if the region is out of range or overlaps another region
 */
static hpm_stat_t mcan_validate_and_record_ram_region(MCAN_Type *ptr, mcan_ram_region_t *regions, uint32_t *region_count, uint32_t region_capacity,
                                                      uint32_t start, uint32_t size);

/**
 * @brief Wait until MCAN reaches or leaves initialization mode
 *
 * @param [in] ptr MCAN base
 * @param [in] expect_init_mode `true` to wait for `CCCR.INIT=1`, `false` to wait for `CCCR.INIT=0`
 * @retval status_success if the expected mode state was observed
 * @retval status_timeout if the transition did not complete within the retry budget
 */
static hpm_stat_t mcan_wait_init_mode_state(MCAN_Type *ptr, bool expect_init_mode);

/**
 * @brief Fill TX frame to MSG RAM slot
 *
 * @param [in] ptr MCAN base
 * @param [in] slot_base_addr Slot base address
 * @param [in] tx_frame TX frame
 */
static void mcan_fill_tx_frame_to_msg_ram_slot(MCAN_Type *ptr, uint32_t slot_base_addr, const mcan_tx_frame_t *tx_frame);

/**
 * @brief Copy RX frame from MSG RAM slot
 *
 * @param [in] slot_base_addr Slot base address
 * @param [in] max_payload_size Maximum payload size
 * @param [out] rx_frame RX frame
 * @retval status_success if no errors happened
 * @retval status_invalid_argument if any parameters are invalid
 */
static hpm_stat_t mcan_copy_rx_frame_from_msg_ram_slot(uint32_t slot_base_addr, uint8_t max_payload_size, mcan_rx_message_t *rx_frame);

/***********************************************************************************************************************
 *
 *  Codes
 */
static void mcan_set_can_nominal_bit_timing(MCAN_Type *ptr, const mcan_bit_timing_param_t *bit_timing)
{
    ptr->NBTP = MCAN_NBTP_NBRP_SET(bit_timing->prescaler - 1U) | MCAN_NBTP_NTSEG1_SET(bit_timing->num_seg1 - 1U) |
                MCAN_NBTP_NTSEG2_SET(bit_timing->num_seg2 - 1U) | MCAN_NBTP_NSJW_SET(bit_timing->num_sjw - 1U);
}

static void mcan_set_can_data_bit_timing(MCAN_Type *ptr, const mcan_bit_timing_param_t *bit_timing)
{
    ptr->DBTP = MCAN_DBTP_DBRP_SET(bit_timing->prescaler - 1U) | MCAN_DBTP_DTSEG1_SET(bit_timing->num_seg1 - 1U) |
                MCAN_DBTP_DTSEG2_SET(bit_timing->num_seg2 - 1U) | MCAN_DBTP_DSJW_SET(bit_timing->num_sjw - 1U) |
                MCAN_DBTP_TDC_SET((uint32_t)bit_timing->enable_tdc);
}

static uint32_t mcan_find_optimal_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler, uint32_t max_tq, uint32_t min_tq)
{
    bool has_found = false;

    uint32_t prescaler = start_prescaler;

    while (!has_found && (prescaler <= num_tq_mul_prescaler)) {
        if (num_tq_mul_prescaler / prescaler < min_tq) {
            break;
        }
        if ((num_tq_mul_prescaler / prescaler > max_tq) || (num_tq_mul_prescaler % prescaler != 0)) {
            ++prescaler;
            continue;
        } else {
            /* At this point, num_tq_mul_prescaler % prescaler == 0 is guaranteed. */
            has_found = true;
            break;
        }
    }

    return has_found ? prescaler : 0U;
}

static hpm_stat_t mcan_calc_bit_timing_from_baudrate(uint32_t src_clk_freq, mcan_bit_timing_option_t option, uint32_t baudrate, uint16_t samplepoint_min,
                                                     uint16_t samplepoint_max, mcan_bit_timing_param_t *timing_param)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((option > mcan_bit_timing_canfd_data) || (baudrate == 0U) || (timing_param == NULL)) {
            break;
        }

        /* Check for potential overflow and division by zero */
        if ((src_clk_freq == 0U) || (src_clk_freq / baudrate < MIN_TQ_MUL_PRESCALE)) {
            break;
        }

        const mcan_bit_timing_table_t *tbl = &k_mcan_bit_timing_tbl[(uint8_t)option];

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
        while (!has_found && (start_prescaler <= tbl->prescaler_max)) {
            current_prescaler = mcan_find_optimal_prescaler(num_tq_mul_prescaler, start_prescaler, tbl->tq_max, tbl->tq_min);
            if ((current_prescaler < start_prescaler) || (current_prescaler > tbl->prescaler_max)) {
                break;
            }
            uint32_t num_tq = num_tq_mul_prescaler / current_prescaler;

            num_seg2 = (num_tq - tbl->min_diff_seg1_minus_seg2) / 2;
            num_seg1 = num_tq - num_seg2;
            while (num_seg2 > tbl->seg2_max) {
                num_seg2--;
                num_seg1++;
            }

            /* Nudge the split toward the recommended sample point (75 %–87.5 %).
             * If a hardware segment boundary (seg2_min or seg1_max) is reached
             * before the target is met, the current prescaler cannot produce a
             * valid sample point — advance to the next prescaler and retry.
             */
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
            timing_param->num_seg1 = num_seg1 - NUM_TQ_SYNC_SEG; /* Should exclude the Sync_Reg */
            timing_param->num_seg2 = num_seg2;
            timing_param->num_sjw = num_sjw;
            timing_param->prescaler = current_prescaler;
            status = status_success;
        }
    } while (false);

    return status;
}

static hpm_stat_t mcan_set_bit_timing_from_baudrate(MCAN_Type *ptr, mcan_bit_timing_option_t option, uint32_t src_clk_freq, uint32_t baudrate,
                                                    uint16_t samplepoint_min, uint16_t samplepoint_max)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if (ptr == NULL) {
            break;
        }

        mcan_bit_timing_param_t timing_param = { 0 };
        status = mcan_calc_bit_timing_from_baudrate(src_clk_freq, option, baudrate, samplepoint_min, samplepoint_max, &timing_param);

        if (status == status_success) {
            if (option < mcan_bit_timing_canfd_data) {
                mcan_set_can_nominal_bit_timing(ptr, &timing_param);
            } else {
                mcan_set_can_data_bit_timing(ptr, &timing_param);
            }
        }

    } while (false);

    return status;
}

static bool is_mcan_bit_timing_param_valid(mcan_bit_timing_option_t option, const mcan_bit_timing_param_t *param, bool tdc_enable)
{
    bool result = false;
    const mcan_bit_timing_table_t *tbl = &k_mcan_bit_timing_tbl[(uint8_t)option];
    do {
        uint32_t actual_num_seg1 = param->num_seg1 + 1U;
        if ((actual_num_seg1 < tbl->seg1_min) || (actual_num_seg1 > tbl->seg1_max)) {
            break;
        }
        if ((param->num_seg2 < tbl->seg2_min) || (param->num_seg2 > tbl->seg2_max)) {
            break;
        }
        if ((param->num_sjw < tbl->sjw_min) || (param->num_sjw > tbl->sjw_max)) {
            break;
        }
        if ((param->prescaler < tbl->prescaler_min) || (param->prescaler > tbl->prescaler_max)) {
            break;
        }
        if (tdc_enable && (option == mcan_bit_timing_canfd_data)) {
            /* When TDC is enabled, the prescaler value should be in range [1, 2] */
            if (param->prescaler > PRESCALER_MAX_FOR_CANFD_DATA_TDC_ENABLE) {
                break;
            }
        }
        result = true;
    } while (false);

    return result;
}

static hpm_stat_t mcan_validate_filter_element(MCAN_Type *ptr, const mcan_filter_elem_t *filter_elem)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (filter_elem == NULL)) {
            break;
        }

        bool is_ext_id = (filter_elem->can_id_type == MCAN_CAN_ID_TYPE_EXTENDED);
        uint32_t id_mask = is_ext_id ? MCAN_EXT_ID_MASK : MCAN_STD_ID_MASK;
        uint8_t filter_type_max = is_ext_id ? MCAN_FILTER_TYPE_DUAL_ID_EXT_FILTER_IGNORE_MASK : MCAN_FILTER_TYPE_FILTER_DISABLED;

        if ((filter_elem->filter_config > MCAN_FILTER_ELEM_CFG_STORE_INTO_RX_BUFFER_OR_AS_DBG_MSG) || (filter_elem->filter_type > filter_type_max)) {
            break;
        }

        if (filter_elem->filter_config == MCAN_FILTER_ELEM_CFG_STORE_INTO_RX_BUFFER_OR_AS_DBG_MSG) {
            if ((filter_elem->match_id > id_mask) || (filter_elem->filter_event != 0U) || (filter_elem->store_location != 0U) ||
                (ptr->RXBC == MCAN_RAM_ADDR_INVALID)) {
                break;
            }
        } else {
            bool ids_valid = false;
            switch (filter_elem->filter_type) {
            case MCAN_FILTER_TYPE_RANGE_FILTER:
            case MCAN_FILTER_TYPE_SPECIFIED_ID_FILTER:
            case MCAN_FILTER_TYPE_DUAL_ID_EXT_FILTER_IGNORE_MASK:
                ids_valid = ((filter_elem->id1 <= id_mask) && (filter_elem->id2 <= id_mask));
                break;
            case MCAN_FILTER_TYPE_CLASSIC_FILTER:
                ids_valid = ((filter_elem->filter_id <= id_mask) && (filter_elem->filter_mask <= id_mask));
                break;
            case MCAN_FILTER_TYPE_FILTER_DISABLED:
                ids_valid = true;
                break;
            default:
                break;
            }
            if (!ids_valid) {
                break;
            }
        }

        status = status_success;
    } while (false);

    return status;
}

static void mcan_encode_std_filter_element(const mcan_filter_elem_t *filter_elem, uint32_t *config_word)
{
    uint32_t word = (filter_elem->id2 & MCAN_STD_ID_MASK) | ((filter_elem->id1 & MCAN_STD_ID_MASK) << 16);

    if (filter_elem->sync_message != 0U) {
        word |= (1UL << MCAN_STD_FILTER_SYNC_MSG_SHIFT);
    }
    word |= ((uint32_t)filter_elem->filter_config) << MCAN_STD_FILTER_SFEC_SHIFT;
    word |= ((uint32_t)filter_elem->filter_type) << MCAN_STD_FILTER_SFT_SHIFT;

    *config_word = word;
}

static void mcan_encode_ext_filter_element(const mcan_filter_elem_t *filter_elem, uint32_t config_words[2])
{
    config_words[0] = ((uint32_t)filter_elem->filter_config << MCAN_EXT_FILTER_EFEC_SHIFT) | filter_elem->id1;
    config_words[1] = filter_elem->id2;

    if (filter_elem->sync_message != 0U) {
        config_words[1] |= (1UL << MCAN_EXT_FILTER_EFEC_SHIFT);
    }
    config_words[1] |= ((uint32_t)filter_elem->filter_type) << MCAN_EXT_FILTER_EFT_SHIFT;
}

static hpm_stat_t mcan_validate_global_filter_config(const mcan_global_filter_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((config->accept_non_matching_std_frame_option > MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT) ||
            (config->accept_non_matching_ext_frame_option > MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_REJECT)) {
            break;
        }

        status = status_success;
    } while (false);

    return status;
}

static hpm_stat_t mcan_validate_filter_list(MCAN_Type *ptr, const mcan_filter_elem_list_t *list, uint8_t can_id_type)
{
    uint32_t configured_count;
    const mcan_filter_elem_t *elem;

    if ((ptr == NULL) || (list == NULL)) {
        return status_invalid_argument;
    }

    if (list->mcan_filter_elem_count == 0U) {
        return status_success;
    }

    if (list->filter_elem_list == NULL) {
        return status_invalid_argument;
    }

    configured_count = (can_id_type == MCAN_CAN_ID_TYPE_EXTENDED) ? MCAN_XIDFC_LSE_GET(ptr->XIDFC) : MCAN_SIDFC_LSS_GET(ptr->SIDFC);
    if (list->mcan_filter_elem_count > configured_count) {
        return status_invalid_argument;
    }

    elem = list->filter_elem_list;
    for (uint32_t i = 0; i < list->mcan_filter_elem_count; i++) {
        if (elem->can_id_type != can_id_type) {
            return status_invalid_argument;
        }
        if (mcan_validate_filter_element(ptr, elem) != status_success) {
            return status_invalid_argument;
        }
        elem++;
    }

    return status_success;
}

static hpm_stat_t mcan_validate_all_filters_config(MCAN_Type *ptr, const mcan_all_filters_config_t *config)
{
    if ((ptr == NULL) || (config == NULL)) {
        return status_invalid_argument;
    }

    if ((mcan_validate_global_filter_config(&config->global_filter_config) != status_success) || (config->ext_id_mask > MCAN_EXT_ID_MASK)) {
        return status_invalid_argument;
    }

    if (mcan_validate_filter_list(ptr, &config->ext_id_filter_list, MCAN_CAN_ID_TYPE_EXTENDED) != status_success) {
        return status_invalid_argument;
    }

    if (mcan_validate_filter_list(ptr, &config->std_id_filter_list, MCAN_CAN_ID_TYPE_STANDARD) != status_success) {
        return status_invalid_argument;
    }

    return status_success;
}

static hpm_stat_t mcan_program_filter_list(MCAN_Type *ptr, const mcan_filter_elem_list_t *list)
{
    const mcan_filter_elem_t *elem;

    if ((ptr == NULL) || (list == NULL)) {
        return status_invalid_argument;
    }

    elem = list->filter_elem_list;
    for (uint32_t i = 0; i < list->mcan_filter_elem_count; i++) {
        hpm_stat_t status = mcan_set_filter_element(ptr, elem, i);
        if (status != status_success) {
            return status;
        }
        elem++;
    }

    return status_success;
}

static bool mcan_is_word_aligned(uint32_t start_addr)
{
    return ((start_addr & 0x3U) == 0U);
}

static uint32_t mcan_get_message_ram_elem_size(uint8_t data_field_size_option)
{
    return mcan_get_data_field_size(data_field_size_option) + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;
}

static hpm_stat_t mcan_validate_flexible_filter_config(const mcan_ram_flexible_config_t *config)
{
    if (config->enable_std_filter &&
        ((config->std_filter_config.list_size > MCAN_STD_FILTER_ELEM_CNT_MAX) || !mcan_is_word_aligned(config->std_filter_config.list_start_addr))) {
        return status_invalid_argument;
    }

    if (config->enable_ext_filter &&
        ((config->ext_filter_config.list_size > MCAN_EXT_FILTER_ELEM_CNT_MAX) || !mcan_is_word_aligned(config->ext_filter_config.list_start_addr))) {
        return status_invalid_argument;
    }

    return status_success;
}

static hpm_stat_t mcan_validate_flexible_rx_config(const mcan_ram_flexible_config_t *config)
{
    uint32_t fifo0_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.fifo0_data_field_size);
    uint32_t fifo1_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.fifo1_data_field_size);
    uint32_t rxbuf_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.buf_data_field_size);

    if (config->enable_rxfifo0 &&
        ((config->rxfifo0_config.fifo_size > MCAN_RXFIFO_ELEM_CNT_MAX) || (config->rxfifo0_config.watermark > config->rxfifo0_config.fifo_size) ||
         !mcan_is_word_aligned(config->rxfifo0_config.start_addr) || (fifo0_elem_size == MCAN_MESSAGE_HEADER_SIZE_IN_BYTES))) {
        return status_invalid_argument;
    }

    if (config->enable_rxfifo1 &&
        ((config->rxfifo1_config.fifo_size > MCAN_RXFIFO_ELEM_CNT_MAX) || (config->rxfifo1_config.watermark > config->rxfifo1_config.fifo_size) ||
         !mcan_is_word_aligned(config->rxfifo1_config.start_addr) || (fifo1_elem_size == MCAN_MESSAGE_HEADER_SIZE_IN_BYTES))) {
        return status_invalid_argument;
    }

    if (config->enable_rxbuf && (!mcan_is_word_aligned(config->rxbuf_config.start_addr) || (config->rxbuf_elem_count == 0U) ||
                                 (config->rxbuf_elem_count > MCAN_RXBUF_ELEM_CNT_MAX) || (rxbuf_elem_size == MCAN_MESSAGE_HEADER_SIZE_IN_BYTES))) {
        return status_invalid_argument;
    }

    return status_success;
}

static hpm_stat_t mcan_validate_flexible_tx_config(const mcan_ram_flexible_config_t *config)
{
    uint32_t txbuf_elem_count = config->txbuf_config.fifo_queue_size + config->txbuf_config.dedicated_tx_buf_size;
    uint32_t txbuf_elem_size = mcan_get_message_ram_elem_size((uint8_t)config->txbuf_elem_config.data_field_size);

    /* NOTE: The tx_fifo_queue_mode is 1-bit, and the configurable options are FIFO mode or Queue mode, both are valid
     *       for the TX buffer. So there is no need to check whether tx_fifo_queue_mode is within the defined enum range here.
     */
    if (config->enable_txbuf && ((txbuf_elem_count > MCAN_TXBUF_ELEM_CNT_MAX)
                                 || !mcan_is_word_aligned(config->txbuf_config.start_addr)
                                 || (txbuf_elem_size == MCAN_MESSAGE_HEADER_SIZE_IN_BYTES)
                                )) {
        return status_invalid_argument;
    }

    if (config->enable_tx_evt_fifo &&
        ((config->tx_evt_fifo_config.fifo_size > MCAN_TXEVT_FIFO_ELEM_CNT_MAX) ||
         (config->tx_evt_fifo_config.fifo_watermark > config->tx_evt_fifo_config.fifo_size) || !mcan_is_word_aligned(config->tx_evt_fifo_config.start_addr))) {
        return status_invalid_argument;
    }

    return status_success;
}

static hpm_stat_t mcan_validate_flexible_ram_regions(MCAN_Type *ptr, const mcan_ram_flexible_config_t *config, mcan_ram_region_t *known_regions,
                                                     uint32_t region_capacity)
{
    hpm_stat_t status;
    uint32_t known_region_count = 0;
    uint32_t fifo0_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.fifo0_data_field_size);
    uint32_t fifo1_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.fifo1_data_field_size);
    uint32_t rxbuf_elem_size = mcan_get_message_ram_elem_size(config->rx_elem_config.buf_data_field_size);
    uint32_t txbuf_elem_count = config->txbuf_config.fifo_queue_size + config->txbuf_config.dedicated_tx_buf_size;
    uint32_t txbuf_elem_size = mcan_get_message_ram_elem_size((uint8_t)config->txbuf_elem_config.data_field_size);

    if (config->enable_std_filter) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->std_filter_config.list_start_addr,
                                                     (uint32_t)config->std_filter_config.list_size * MCAN_FILTER_ELEM_STD_ID_SIZE);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_ext_filter) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->ext_filter_config.list_start_addr,
                                                     (uint32_t)config->ext_filter_config.list_size * MCAN_FILTER_ELEM_EXT_ID_SIZE);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_rxfifo0) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->rxfifo0_config.start_addr,
                                                     (uint32_t)config->rxfifo0_config.fifo_size * fifo0_elem_size);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_rxfifo1) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->rxfifo1_config.start_addr,
                                                     (uint32_t)config->rxfifo1_config.fifo_size * fifo1_elem_size);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_rxbuf) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->rxbuf_config.start_addr,
                                                     (uint32_t)config->rxbuf_elem_count * rxbuf_elem_size);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_txbuf) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->txbuf_config.start_addr,
                                                     txbuf_elem_count * txbuf_elem_size);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    if (config->enable_tx_evt_fifo) {
        status = mcan_validate_and_record_ram_region(ptr, known_regions, &known_region_count, region_capacity, config->tx_evt_fifo_config.start_addr,
                                                     (uint32_t)config->tx_evt_fifo_config.fifo_size * MCAN_TXEVT_ELEM_SIZE);
        if (status != status_success) {
            return status_invalid_argument;
        }
    }

    return status_success;
}

static hpm_stat_t mcan_validate_simple_ram_config(const mcan_ram_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((config->enable_std_filter && (config->std_filter_elem_count > MCAN_STD_FILTER_ELEM_CNT_MAX)) ||
            (config->enable_ext_filter && (config->ext_filter_elem_count > MCAN_EXT_FILTER_ELEM_CNT_MAX))) {
            break;
        }

        bool rxfifo_valid = true;
        for (uint32_t i = 0; i < ARRAY_SIZE(config->rxfifos); i++) {
            if (!config->rxfifos[i].enable) {
                continue;
            }

            if ((config->rxfifos[i].elem_count > MCAN_RXFIFO_ELEM_CNT_MAX) || (config->rxfifos[i].watermark > config->rxfifos[i].elem_count) ||
                (mcan_get_data_field_size(config->rxfifos[i].data_field_size) == 0U)) {
                rxfifo_valid = false;
                break;
            }
        }
        if (!rxfifo_valid) {
            break;
        }

        if (config->enable_rxbuf && ((config->rxbuf_elem_count > MCAN_RXBUF_ELEM_CNT_MAX) || (mcan_get_data_field_size(config->rxbuf_data_field_size) == 0U))) {
            break;
        }

        if (config->enable_txbuf) {
            uint32_t txbuf_elem_count = config->txbuf_fifo_or_queue_elem_count + config->txbuf_dedicated_txbuf_elem_count;
            if ((txbuf_elem_count > MCAN_TXBUF_ELEM_CNT_MAX) || (config->txfifo_or_txqueue_mode > MCAN_TXBUF_OPERATION_MODE_QUEUE) ||
                (mcan_get_data_field_size(config->txbuf_data_field_size) == 0U)) {
                break;
            }
        }

        if (config->enable_tx_evt_fifo &&
            ((config->tx_evt_fifo_elem_count > MCAN_TXEVT_FIFO_ELEM_CNT_MAX) || (config->tx_evt_fifo_watermark > config->tx_evt_fifo_elem_count))) {
            break;
        }

        status = status_success;
    } while (false);

    return status;
}

static bool mcan_is_ram_region_inside_range(MCAN_Type *ptr, uint32_t start, uint32_t size)
{
    uint64_t ram_start = mcan_get_ram_offset(ptr);
    uint64_t ram_end = ram_start + mcan_get_ram_size(ptr);
    uint64_t region_start = start;
    uint64_t region_end = region_start + size;

    return (region_start >= ram_start) && (region_end <= ram_end);
}

static bool mcan_do_ram_regions_overlap(const mcan_ram_region_t *lhs, const mcan_ram_region_t *rhs)
{
    uint64_t lhs_end = (uint64_t)lhs->start + lhs->size;
    uint64_t rhs_end = (uint64_t)rhs->start + rhs->size;

    if ((lhs->size == 0U) || (rhs->size == 0U)) {
        return false;
    }

    return ((uint64_t)lhs->start < rhs_end) && ((uint64_t)rhs->start < lhs_end);
}

static hpm_stat_t mcan_validate_and_record_ram_region(MCAN_Type *ptr, mcan_ram_region_t *regions, uint32_t *region_count, uint32_t region_capacity,
                                                      uint32_t start, uint32_t size)
{
    mcan_ram_region_t region = { .start = start, .size = size };

    if ((*region_count >= region_capacity) || !mcan_is_ram_region_inside_range(ptr, region.start, region.size)) {
        return status_invalid_argument;
    }

    for (uint32_t i = 0; i < *region_count; i++) {
        if (mcan_do_ram_regions_overlap(&region, &regions[i])) {
            return status_invalid_argument;
        }
    }

    regions[*region_count] = region;
    (*region_count)++;

    return status_success;
}

static hpm_stat_t mcan_validate_flexible_ram_config(MCAN_Type *ptr, const mcan_ram_flexible_config_t *config)
{
    mcan_ram_region_t known_regions[7];

    if ((ptr == NULL) || (config == NULL)) {
        return status_invalid_argument;
    }

    if (mcan_validate_flexible_filter_config(config) != status_success) {
        return status_invalid_argument;
    }

    if (mcan_validate_flexible_rx_config(config) != status_success) {
        return status_invalid_argument;
    }

    if (mcan_validate_flexible_tx_config(config) != status_success) {
        return status_invalid_argument;
    }

    if (mcan_validate_flexible_ram_regions(ptr, config, known_regions, ARRAY_SIZE(known_regions)) != status_success) {
        return status_invalid_argument;
    }

    return status_success;
}

void mcan_get_default_ram_config(MCAN_Type *ptr, mcan_ram_config_t *simple_config, bool enable_canfd)
{
    if ((ptr == NULL) || (simple_config == NULL)) {
        return;
    }

    (void)memset(simple_config, 0, sizeof(mcan_ram_config_t));
    const mcan_default_ram_preset_t *p = enable_canfd ? &k_canfd_preset : &k_can_preset;
    uint32_t start_addr = mcan_get_ram_offset(ptr);

    simple_config->enable_std_filter = true;
    simple_config->std_filter_elem_count = p->filter_count;
    start_addr += MCAN_FILTER_ELEM_STD_ID_SIZE * p->filter_count;

    simple_config->enable_ext_filter = true;
    simple_config->ext_filter_elem_count = p->filter_count;
    start_addr += MCAN_FILTER_ELEM_EXT_ID_SIZE * p->filter_count;

    simple_config->enable_txbuf = true;
    simple_config->txbuf_dedicated_txbuf_elem_count = p->txbuf_count / 2;
    simple_config->txbuf_fifo_or_queue_elem_count = p->txbuf_count / 2;
    simple_config->txfifo_or_txqueue_mode = MCAN_TXBUF_OPERATION_MODE_FIFO;
    simple_config->txbuf_data_field_size = p->data_field_size;
    start_addr += p->txbuf_count * p->txrx_elem_size;

    for (uint32_t i = 0; i < ARRAY_SIZE(simple_config->rxfifos); i++) {
        simple_config->rxfifos[i].enable = true;
        simple_config->rxfifos[i].elem_count = p->rxfifo_count;
        simple_config->rxfifos[i].operation_mode = MCAN_FIFO_OPERATION_MODE_BLOCKING;
        simple_config->rxfifos[i].watermark = 1U;
        simple_config->rxfifos[i].data_field_size = p->data_field_size;
        start_addr += p->rxfifo_count * p->txrx_elem_size;
    }
    simple_config->enable_rxbuf = true;
    simple_config->rxbuf_elem_count = p->rxbuf_count;
    simple_config->rxbuf_data_field_size = p->data_field_size;
    start_addr += p->rxbuf_count * p->txrx_elem_size;

    simple_config->enable_tx_evt_fifo = true;
    uint32_t tx_fifo_elem_count = simple_config->txbuf_dedicated_txbuf_elem_count + simple_config->txbuf_fifo_or_queue_elem_count;
    simple_config->tx_evt_fifo_elem_count = tx_fifo_elem_count;
    simple_config->tx_evt_fifo_watermark = 1U;

    start_addr += MCAN_TXEVT_ELEM_SIZE * tx_fifo_elem_count;

    /* Ensure the requested MCAN RAM size is less than or equal to the total MCAN RAM size */
    if ((start_addr - mcan_get_ram_offset(ptr)) > mcan_get_ram_size(ptr)) {
        return; /* Default configuration exceeds RAM — should never happen */
    }
    (void)start_addr; /* Suppress warnings in release build  */
}

void mcan_get_default_ram_flexible_config(MCAN_Type *ptr, mcan_ram_flexible_config_t *ram_config, bool enable_canfd)
{
    if ((ptr == NULL) || (ram_config == NULL)) {
        return;
    }

    (void)memset(ram_config, 0, sizeof(mcan_ram_flexible_config_t));
    const mcan_default_ram_preset_t *p = enable_canfd ? &k_canfd_preset : &k_can_preset;
    uint32_t start_addr = mcan_get_ram_offset(ptr);

    ram_config->enable_std_filter = true;
    ram_config->std_filter_config.list_start_addr = start_addr;
    ram_config->std_filter_config.list_size = p->filter_count;
    start_addr += MCAN_FILTER_ELEM_STD_ID_SIZE * p->filter_count;

    ram_config->enable_ext_filter = true;
    ram_config->ext_filter_config.list_start_addr = start_addr;
    ram_config->ext_filter_config.list_size = p->filter_count;
    start_addr += MCAN_FILTER_ELEM_EXT_ID_SIZE * p->filter_count;

    ram_config->enable_txbuf = true;
    ram_config->txbuf_config.start_addr = start_addr;
    ram_config->txbuf_config.dedicated_tx_buf_size = p->txbuf_count / 2;
    ram_config->txbuf_config.fifo_queue_size = p->txbuf_count / 2;
    ram_config->txbuf_config.tx_fifo_queue_mode = MCAN_TXBUF_OPERATION_MODE_FIFO;
    ram_config->txbuf_elem_config.data_field_size = p->data_field_size;
    start_addr += p->txbuf_count * p->txrx_elem_size;

    ram_config->enable_rxfifo0 = true;
    ram_config->rxfifo0_config.start_addr = start_addr;
    ram_config->rxfifo0_config.fifo_size = p->rxfifo_count;
    ram_config->rxfifo0_config.operation_mode = MCAN_FIFO_OPERATION_MODE_BLOCKING;
    ram_config->rxfifo0_config.watermark = 1U;
    start_addr += p->rxfifo_count * p->txrx_elem_size;

    ram_config->enable_rxfifo1 = true;
    ram_config->rxfifo1_config.start_addr = start_addr;
    ram_config->rxfifo1_config.fifo_size = p->rxfifo_count;
    ram_config->rxfifo1_config.operation_mode = MCAN_FIFO_OPERATION_MODE_BLOCKING;
    ram_config->rxfifo1_config.watermark = 1U;
    start_addr += p->rxfifo_count * p->txrx_elem_size;

    ram_config->enable_rxbuf = true;
    ram_config->rxbuf_config.start_addr = start_addr;
    ram_config->rxbuf_elem_count = p->rxbuf_count;
    start_addr += p->rxbuf_count * p->txrx_elem_size;

    if (enable_canfd) {
        /* Set RX element data field sizes for CANFD mode */
        ram_config->rx_elem_config.fifo0_data_field_size = p->data_field_size;
        ram_config->rx_elem_config.fifo1_data_field_size = p->data_field_size;
        ram_config->rx_elem_config.buf_data_field_size = p->data_field_size;
    }

    ram_config->enable_tx_evt_fifo = true;
    ram_config->tx_evt_fifo_config.start_addr = start_addr;
    uint32_t tx_fifo_size = ram_config->txbuf_config.dedicated_tx_buf_size + ram_config->txbuf_config.fifo_queue_size;
    ram_config->tx_evt_fifo_config.fifo_size = tx_fifo_size;
    ram_config->tx_evt_fifo_config.fifo_watermark = 1U;

    start_addr += MCAN_TXEVT_ELEM_SIZE * tx_fifo_size;

    /* Ensure the requested MCAN RAM size is less than or equal to the total MCAN RAM size */
    if ((start_addr - mcan_get_ram_offset(ptr)) > mcan_get_ram_size(ptr)) {
        return; /* Default configuration exceeds RAM — should never happen */
    }
}

void mcan_get_default_config(MCAN_Type *ptr, mcan_config_t *config)
{
    if ((ptr == NULL) || (config == NULL)) {
        return;
    }
    (void)memset(config, 0, sizeof(mcan_config_t));
    config->baudrate = MCAN_CAN_BAUDRATE_DEFAULT;
    config->baudrate_fd = MCAN_CANFD_BAUDRATE_DEFAULT;
    config->can20_samplepoint_max = CAN_SAMPLEPOINT_MAX;
    config->can20_samplepoint_min = CAN_SAMPLEPOINT_MIN;
    config->canfd_samplepoint_max = CAN_SAMPLEPOINT_MAX;
    config->canfd_samplepoint_min = CAN_SAMPLEPOINT_MIN;

    config->mode = mcan_mode_normal;
    config->enable_canfd = false;

    /* Default Filter settings */
    mcan_all_filters_config_t *filters_config = &config->all_filters_config;
    filters_config->global_filter_config.reject_remote_ext_frame = false;
    filters_config->global_filter_config.reject_remote_std_frame = false;
    filters_config->global_filter_config.accept_non_matching_std_frame_option = MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO0;
    filters_config->global_filter_config.accept_non_matching_ext_frame_option = MCAN_ACCEPT_NON_MATCHING_FRAME_OPTION_IN_RXFIFO0;
    filters_config->ext_id_mask = 0x1FFFFFFFUL;
    filters_config->std_id_filter_list.filter_elem_list = &k_default_std_id_filter;
    filters_config->std_id_filter_list.mcan_filter_elem_count = 1;
    filters_config->ext_id_filter_list.filter_elem_list = &k_default_ext_id_filter;
    filters_config->ext_id_filter_list.mcan_filter_elem_count = 1;

    /* Default MCAN RAM Configuration */
    mcan_ram_config_t *ram_config = &config->ram_config;
    mcan_get_default_ram_config(ptr, ram_config, false);

    /* Default Internal Timestamp Configuration */
    mcan_internal_timestamp_config_t *ts_config = &config->timestamp_cfg;
    ts_config->counter_prescaler = 1U;
    ts_config->timestamp_selection = MCAN_TIMESTAMP_SEL_VALUE_ALWAYS_ZERO;

    /* Default TSU configuration */
    mcan_tsu_config_t *tsu_config = &config->tsu_config;
    tsu_config->prescaler = 1U;
#if defined(MCAN_SOC_TSU_SRC_TWO_STAGES) && (MCAN_SOC_TSU_SRC_TWO_STAGES == 1)
    tsu_config->ext_timebase_src = MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_0;
    tsu_config->tbsel_option = MCAN_TSU_TBSEL_PTPC0;
#else
    tsu_config->ext_timebase_src = MCAN_TSU_EXT_TIMEBASE_SRC_PTPC;
#endif
    tsu_config->use_ext_timebase = false;
    tsu_config->capture_on_sof = false;
    tsu_config->enable_tsu = false;

    config->timeout_cfg.enable_timeout_counter = false;
    config->timeout_cfg.timeout_period = 0xFFFFU;
    config->timeout_cfg.timeout_sel = mcan_timeout_continuous_operation;
}

hpm_stat_t mcan_begin_reconfig(MCAN_Type *ptr)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(ptr == NULL);

        mcan_enter_init_mode(ptr);
        status = mcan_wait_init_mode_state(ptr, true);
        HPM_BREAK_IF(status != status_success);

        mcan_enable_write_to_prot_config_registers(ptr);
        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t mcan_end_reconfig(MCAN_Type *ptr)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(ptr == NULL);

        mcan_disable_write_to_prot_config_registers(ptr);
        mcan_enter_normal_mode(ptr);
        status = mcan_wait_init_mode_state(ptr, false);
    } while (false);

    return status;
}

void mcan_init_tx_frame_std(mcan_tx_frame_t *frame, uint16_t std_id, uint8_t dlc)
{
    if (frame == NULL) {
        return;
    }

    memset(frame, 0, sizeof(*frame));
    frame->use_ext_id = 0U;
    frame->std_id = std_id & MCAN_STD_ID_MASK;
    frame->dlc = MIN(dlc, 8U);
}

void mcan_init_tx_frame_ext(mcan_tx_frame_t *frame, uint32_t ext_id, uint8_t dlc)
{
    if (frame == NULL) {
        return;
    }

    memset(frame, 0, sizeof(*frame));
    frame->use_ext_id = 1U;
    frame->ext_id = ext_id & MCAN_EXT_ID_MASK;
    frame->dlc = MIN(dlc, 8U);
}

static void mcan_config_rxfifo(MCAN_Type *ptr, uint32_t index, uint32_t reg_val)
{
    if (index == 0U) {
        ptr->RXF0C = reg_val;
    } else {
        ptr->RXF1C = reg_val;
    }
}

hpm_stat_t mcan_config_ram(MCAN_Type *ptr, mcan_ram_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (config == NULL)) {
            break;
        }
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        HPM_BREAK_IF(mcan_validate_simple_ram_config(config) != status_success);

        uint32_t elem_bytes;
        uint32_t elem_count;
        uint32_t start_addr = mcan_get_ram_offset(ptr);
        if (config->enable_std_filter) {
            if (config->std_filter_elem_count > MCAN_STD_FILTER_ELEM_CNT_MAX) {
                break;
            }
            mcan_filter_config_t filter_config = { .reg_val = 0 };
            filter_config.list_size = config->std_filter_elem_count;
            filter_config.list_start_addr = start_addr;
            ptr->SIDFC = filter_config.reg_val;
            start_addr += MCAN_FILTER_ELEM_STD_ID_SIZE * (uint32_t)config->std_filter_elem_count;
        } else {
            ptr->SIDFC = MCAN_RAM_ADDR_INVALID;
        }

        if (config->enable_ext_filter) {
            if (config->ext_filter_elem_count > MCAN_EXT_FILTER_ELEM_CNT_MAX) {
                break;
            }
            mcan_filter_config_t filter_config = { .reg_val = 0 };
            filter_config.list_size = config->ext_filter_elem_count;
            filter_config.list_start_addr = start_addr;
            ptr->XIDFC = filter_config.reg_val;
            start_addr += MCAN_FILTER_ELEM_EXT_ID_SIZE * (uint32_t)config->ext_filter_elem_count;
        } else {
            ptr->XIDFC = MCAN_RAM_ADDR_INVALID;
        }

        mcan_rx_fifo_or_buf_elem_config_t rx_fifo_buf_elem_config = { .reg_val = 0U };

        for (uint32_t i = 0; i < ARRAY_SIZE(config->rxfifos); i++) {
            if (config->rxfifos[i].enable) {
                elem_bytes = mcan_get_data_field_size(config->rxfifos[i].data_field_size) + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;
                elem_count = config->rxfifos[i].elem_count;
                if (elem_count > MCAN_RXFIFO_ELEM_CNT_MAX) {
                    return status_invalid_argument;
                }
                mcan_rxfifo_config_t rxfifo_config = { .reg_val = 0 };
                rxfifo_config.start_addr = start_addr;
                rxfifo_config.watermark = config->rxfifos[i].watermark;
                rxfifo_config.operation_mode = config->rxfifos[i].operation_mode;
                rxfifo_config.fifo_size = elem_count;
                mcan_config_rxfifo(ptr, i, rxfifo_config.reg_val);

                if (i == 0) {
                    rx_fifo_buf_elem_config.fifo0_data_field_size = config->rxfifos[i].data_field_size;
                } else {
                    rx_fifo_buf_elem_config.fifo1_data_field_size = config->rxfifos[i].data_field_size;
                }

                start_addr += elem_bytes * elem_count;
            } else {
                mcan_config_rxfifo(ptr, i, MCAN_RAM_ADDR_INVALID);
                if (i == 0) {
                    rx_fifo_buf_elem_config.fifo0_data_field_size = 0;
                } else {
                    rx_fifo_buf_elem_config.fifo1_data_field_size = 0;
                }
            }
        }

        if (config->enable_rxbuf) {
            elem_bytes = mcan_get_data_field_size(config->rxbuf_data_field_size) + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;
            elem_count = config->rxbuf_elem_count;
            if (elem_count > MCAN_RXBUF_ELEM_CNT_MAX) {
                status = status_invalid_argument;
                break;
            }
            ptr->RXBC = start_addr;
            rx_fifo_buf_elem_config.buf_data_field_size = config->rxbuf_data_field_size;

            start_addr += elem_bytes * elem_count;
        } else {
            rx_fifo_buf_elem_config.buf_data_field_size = 0;
            ptr->RXBC = MCAN_RAM_ADDR_INVALID;
        }
        ptr->RXESC = rx_fifo_buf_elem_config.reg_val;

        mcan_txbuf_config_t txbuf_config = { .reg_val = 0 };
        if (config->enable_txbuf) {
            txbuf_config.start_addr = start_addr;
            txbuf_config.fifo_queue_size = config->txbuf_fifo_or_queue_elem_count;
            txbuf_config.dedicated_tx_buf_size = config->txbuf_dedicated_txbuf_elem_count;
            txbuf_config.tx_fifo_queue_mode = config->txfifo_or_txqueue_mode;

            elem_count = config->txbuf_fifo_or_queue_elem_count + config->txbuf_dedicated_txbuf_elem_count;
            if (elem_count > MCAN_TXBUF_ELEM_CNT_MAX) {
                break;
            }

            elem_bytes = mcan_get_data_field_size(config->txbuf_data_field_size) + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;

            start_addr += elem_count * elem_bytes;
            ptr->TXESC = (uint32_t)config->txbuf_data_field_size;
        } else {
            ptr->TXESC = MCAN_RAM_ADDR_INVALID;
        }

        ptr->TXBC = txbuf_config.reg_val;

        mcan_tx_evt_fifo_config_t txevt_fifo_config = { .reg_val = 0 };
        if (config->enable_tx_evt_fifo) {
            elem_bytes = sizeof(mcan_tx_event_fifo_elem_t);
            elem_count = config->tx_evt_fifo_elem_count;
            if ((elem_count > MCAN_TXEVT_FIFO_ELEM_CNT_MAX) || (config->tx_evt_fifo_watermark > elem_count)) {
                break;
            }

            txevt_fifo_config.start_addr = start_addr;
            txevt_fifo_config.fifo_size = elem_count;
            txevt_fifo_config.fifo_watermark = config->tx_evt_fifo_watermark;
            ptr->TXEFC = txevt_fifo_config.reg_val;

            start_addr += elem_bytes * elem_count;
        } else {
            ptr->TXEFC = MCAN_RAM_ADDR_INVALID;
        }

        /* Check whether the requested RAM space exceeds the valid RAM range */
        uint32_t requested_ram_size = start_addr - mcan_get_ram_offset(ptr);
        if (requested_ram_size > mcan_get_ram_size(ptr)) {
            status = status_mcan_ram_out_of_range;
            break;
        }

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t mcan_config_ram_with_flexible_config(MCAN_Type *ptr, mcan_ram_flexible_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (config == NULL)) {
            break;
        }
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        HPM_BREAK_IF(mcan_validate_flexible_ram_config(ptr, config) != status_success);

        ptr->SIDFC = config->enable_std_filter ? config->std_filter_config.reg_val : MCAN_RAM_ADDR_INVALID;
        ptr->XIDFC = config->enable_ext_filter ? config->ext_filter_config.reg_val : MCAN_RAM_ADDR_INVALID;
        ptr->RXF0C = config->enable_rxfifo0 ? config->rxfifo0_config.reg_val : MCAN_RAM_ADDR_INVALID;
        ptr->RXF1C = config->enable_rxfifo1 ? config->rxfifo1_config.reg_val : MCAN_RAM_ADDR_INVALID;
        ptr->RXBC = config->enable_rxbuf ? config->rxbuf_config.start_addr : MCAN_RAM_ADDR_INVALID;
        ptr->RXESC = config->rx_elem_config.reg_val;

        if (config->enable_txbuf) {
            ptr->TXBC = config->txbuf_config.reg_val;
            ptr->TXESC = config->txbuf_elem_config.data_field_size;
        } else {
            ptr->TXBC = 0U;
            ptr->TXESC = MCAN_RAM_ADDR_INVALID;
        }
        ptr->TXEFC = config->enable_tx_evt_fifo ? config->tx_evt_fifo_config.reg_val : MCAN_RAM_ADDR_INVALID;

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t mcan_config_all_filters(MCAN_Type *ptr, mcan_all_filters_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (config == NULL));
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        HPM_BREAK_IF(mcan_validate_all_filters_config(ptr, config) != status_success);

        ptr->XIDAM = config->ext_id_mask;

        ptr->GFC = MCAN_GFC_RRFE_SET(config->global_filter_config.reject_remote_ext_frame) |
                   MCAN_GFC_RRFS_SET(config->global_filter_config.reject_remote_std_frame) |
                   MCAN_GFC_ANFE_SET(config->global_filter_config.accept_non_matching_ext_frame_option) |
                   MCAN_GFC_ANFS_SET(config->global_filter_config.accept_non_matching_std_frame_option);

        HPM_BREAK_IF(mcan_program_filter_list(ptr, &config->ext_id_filter_list) != status_success);
        HPM_BREAK_IF(mcan_program_filter_list(ptr, &config->std_id_filter_list) != status_success);
        status = status_success;

    } while (false);

    return status;
}

static hpm_stat_t mcan_set_tsu(MCAN_Type *ptr, mcan_tsu_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        uint32_t tscfg = 0;

        if ((config->prescaler < 1U) || (config->prescaler > 256U) || (config->ext_timebase_src > MCAN_TSU_EXT_TIMEBASE_SRC_MAX)) {
            break;
        }

        if (config->enable_tsu) {
            tscfg |= MCAN_TSCFG_TSUE_MASK;
        }
        if (config->capture_on_sof) {
            tscfg |= MCAN_TSCFG_SCP_MASK;
        }
        if (config->use_ext_timebase) {
            tscfg |= MCAN_TSCFG_TBCS_MASK;
            mcan_set_tsu_ext_timebase_src(ptr, config->ext_timebase_src);
#if defined(MCAN_SOC_TSU_SRC_TWO_STAGES) && (MCAN_SOC_TSU_SRC_TWO_STAGES == 1)
            mcan_set_tsu_tbsel_option(ptr, config->ext_timebase_src, config->tbsel_option);
#endif
        }
        if (config->enable_64bit_timestamp) {
            tscfg |= MCAN_TSCFG_EN64_MASK;
        }

        tscfg |= MCAN_TSCFG_TBPRE_SET(config->prescaler - 1U);

        ptr->TSCFG = tscfg;

        status = status_success;
    } while (false);

    return status;
}

static hpm_stat_t mcan_set_internal_timestamp(MCAN_Type *ptr, mcan_internal_timestamp_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((config->counter_prescaler < 1U) || (config->counter_prescaler > 16) || (config->timestamp_selection > MCAN_TIMESTAMP_SEL_MAX)) {
            break;
        }

        ptr->TSCC = MCAN_TSCC_TCP_SET(config->counter_prescaler - 1U) | MCAN_TSCC_TSS_SET(config->timestamp_selection);

        status = status_success;
    } while (false);

    return status;
}

static void mcan_apply_tdc_config(MCAN_Type *ptr, const mcan_tdc_config_t *config)
{
    uint32_t tdc_offset;
    uint32_t tdc_filter;

    if (config->ssp_offset == 0U) {
        /* Use DTSEG1 + 1 as the default SSP offset. */
        uint32_t dseg1 = MCAN_DBTP_DTSEG1_GET(ptr->DBTP) + 1U;
        tdc_offset = dseg1 + 1U;
        tdc_filter = tdc_offset;
    } else {
        tdc_offset = config->ssp_offset;
        tdc_filter = config->filter_window_length;
    }

    tdc_offset = MIN(tdc_offset, MCAN_MAX_TDC_OFFSET);
    tdc_filter = MIN(tdc_filter, MCAN_MAX_TDC_FILTER_WIN);

    ptr->TDCR = MCAN_TDCR_TDCF_SET(tdc_filter) | MCAN_TDCR_TDCO_SET(tdc_offset);
    ptr->DBTP |= MCAN_DBTP_TDC_MASK;
}

static bool mcan_is_in_config_mode(MCAN_Type *ptr)
{
    return ((ptr->CCCR & (MCAN_CCCR_INIT_MASK | MCAN_CCCR_CCE_MASK)) == (MCAN_CCCR_INIT_MASK | MCAN_CCCR_CCE_MASK));
}

static hpm_stat_t mcan_require_config_mode(MCAN_Type *ptr)
{
    return mcan_is_in_config_mode(ptr) ? status_success : status_invalid_argument;
}

static hpm_stat_t mcan_wait_init_mode_state(MCAN_Type *ptr, bool expect_init_mode)
{
    uint32_t retry_cnt = MCAN_WAIT_RETRY_CNT_MAX;
    uint32_t expected_state = expect_init_mode ? MCAN_CCCR_INIT_MASK : 0U;

    while ((ptr->CCCR & MCAN_CCCR_INIT_MASK) != expected_state) {
        if (retry_cnt == 0U) {
            return status_timeout;
        }
        retry_cnt--;
    }

    return status_success;
}

static uint8_t mcan_get_tx_frame_payload_capacity(const mcan_tx_frame_t *frame)
{
    uint8_t payload_capacity = mcan_get_message_size_from_dlc(frame->dlc);

    if ((!frame->canfd_frame) && (payload_capacity > 8U)) {
        payload_capacity = 8U;
    }

    return payload_capacity;
}

/**
 * @brief Set or clear a CCCR flag based on the enable parameter.
 */
static inline void mcan_set_cccr_bit(MCAN_Type *ptr, uint32_t mask, bool enable)
{
    if (enable) {
        ptr->CCCR |= mask;
    } else {
        ptr->CCCR &= ~mask;
    }
}

hpm_stat_t mcan_init(MCAN_Type *ptr, mcan_config_t *config, uint32_t src_clk_freq)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (config == NULL)) {
            break;
        }

        mcan_enable_clock(ptr);
        uint32_t retry_cnt = MCAN_CLOCK_ENABLE_RETRY_MAX;
        do {
            if (retry_cnt == 0UL) {
                break;
            }
            retry_cnt--;
        } while (!mcan_is_clock_enabled(ptr));
        if (retry_cnt == 0UL) {
            status = status_timeout;
            break;
        }

        ptr->CCCR |= MCAN_CCCR_INIT_MASK;
        status = mcan_wait_init_mode_state(ptr, true);
        if (status != status_success) {
            break;
        }

        ptr->CCCR |= MCAN_CCCR_CCE_MASK;

        if (!config->use_lowlevel_timing_setting) {
            if (config->enable_canfd) {
                status = mcan_set_bit_timing_from_baudrate(ptr, mcan_bit_timing_canfd_nominal, src_clk_freq, config->baudrate, config->can20_samplepoint_min,
                                                           config->can20_samplepoint_max);
                HPM_BREAK_IF(status != status_success);
                status = mcan_set_bit_timing_from_baudrate(ptr, mcan_bit_timing_canfd_data, src_clk_freq, config->baudrate_fd, config->canfd_samplepoint_min,
                                                           config->canfd_samplepoint_max);
            } else {
                status = mcan_set_bit_timing_from_baudrate(ptr, mcan_bit_timing_can2_0, src_clk_freq, config->baudrate, config->can20_samplepoint_min,
                                                           config->can20_samplepoint_max);
            }
            HPM_BREAK_IF(status != status_success);
        } else {
            bool param_valid;
            if (config->enable_canfd) {
                param_valid = is_mcan_bit_timing_param_valid(mcan_bit_timing_canfd_nominal, &config->can_timing, false);
                if (!param_valid) {
                    status = status_mcan_invalid_bit_timing;
                    break;
                }
                param_valid = is_mcan_bit_timing_param_valid(mcan_bit_timing_canfd_data, &config->canfd_timing, config->enable_tdc);
                if (!param_valid) {
                    status = status_mcan_invalid_bit_timing;
                    break;
                }
                mcan_set_can_nominal_bit_timing(ptr, &config->can_timing);
                mcan_set_can_data_bit_timing(ptr, &config->canfd_timing);
            } else {
                param_valid = is_mcan_bit_timing_param_valid(mcan_bit_timing_can2_0, &config->can_timing, false);
                if (!param_valid) {
                    status = status_mcan_invalid_bit_timing;
                    break;
                }
                mcan_set_can_nominal_bit_timing(ptr, &config->can_timing);
            }
        }

        switch (config->mode) {
        default:
        case mcan_mode_normal:
            ptr->CCCR &= ~(MCAN_CCCR_MON_MASK | MCAN_CCCR_TEST_MASK);
            break;
        case mcan_mode_loopback_internal:
            ptr->CCCR |= MCAN_CCCR_MON_MASK | MCAN_CCCR_TEST_MASK;
            ptr->TEST |= MCAN_TEST_LBCK_MASK;
            break;
        case mcan_mode_loopback_external:
            ptr->CCCR |= MCAN_CCCR_TEST_MASK;
            ptr->TEST |= MCAN_TEST_LBCK_MASK;
            break;
        case mcan_mode_listen_only:
            ptr->CCCR |= MCAN_CCCR_MON_MASK;
            break;
        }

        mcan_set_cccr_bit(ptr, MCAN_CCCR_FDOE_MASK | MCAN_CCCR_BRSE_MASK, config->enable_canfd);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_NISO_MASK, config->enable_non_iso_mode);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_TXP_MASK, config->enable_transmit_pause);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_PXHD_MASK, config->disable_protocol_exception_handling);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_WMM_MASK, config->enable_wide_message_marker);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_EFBI_MASK, config->enable_edge_filtering);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_DAR_MASK, config->disable_auto_retransmission);
        mcan_set_cccr_bit(ptr, MCAN_CCCR_ASM_MASK, config->enable_restricted_operation_mode);

        /* Configure Transmitter Delay Compensation */
        if (config->enable_tdc) {
            mcan_apply_tdc_config(ptr, &config->tdc_config);
        } else {
            ptr->DBTP &= ~MCAN_DBTP_TDC_MASK;
        }

        /* Configure TSU */
        if (config->use_timestamping_unit) {
            ptr->CCCR |= MCAN_CCCR_UTSU_MASK;
            status = mcan_set_tsu(ptr, &config->tsu_config);
            HPM_BREAK_IF(status != status_success);
        } else {
            ptr->CCCR &= ~MCAN_CCCR_UTSU_MASK;
        }

        /* Configure Internal Timestamp */
        status = mcan_set_internal_timestamp(ptr, &config->timestamp_cfg);
        HPM_BREAK_IF(status != status_success);

        /* Initialize CAN RAM */
        uint32_t can_ram_size = mcan_get_ram_size(ptr);
        if (can_ram_size < 1U) {
            status = status_invalid_argument;
            break;
        }
        volatile uint32_t *ram_base = (volatile uint32_t *)(mcan_get_ram_base(ptr) + mcan_get_ram_offset(ptr));
        for (uint32_t i = 0U; i < can_ram_size / sizeof(uint32_t); i++) {
            ram_base[i] = 0UL;
        }
        status = mcan_config_ram(ptr, &config->ram_config);
        HPM_BREAK_IF(status != status_success);

        /* Configure Filters */
        status = mcan_config_all_filters(ptr, &config->all_filters_config);
        HPM_BREAK_IF(status != status_success);

        /* Disable all interrupts by default */
        mcan_disable_interrupts(ptr, ~0UL);
        mcan_disable_txbuf_transmission_interrupt(ptr, ~0UL);
        mcan_disable_txbuf_cancel_finish_interrupt(ptr, ~0UL);
        /* Enable interrupts on demand */
        mcan_enable_interrupts(ptr, config->interrupt_mask);
        mcan_enable_txbuf_transmission_interrupt(ptr, config->txbuf_trans_interrupt_mask);
        mcan_enable_txbuf_cancel_finish_interrupt(ptr, config->txbuf_cancel_finish_interrupt_mask);

        /* Clear all Interrupt Flags */
        mcan_clear_interrupt_flags(ptr, ~0UL);

        /* Configure timeout */
        const mcan_timeout_config_t *timeout_cfg = &config->timeout_cfg;
        ptr->TOCC = MCAN_TOCC_RP_SET(timeout_cfg->enable_timeout_counter) | MCAN_TOCC_TOP_SET(timeout_cfg->timeout_period) |
                    MCAN_TOCC_TOS_SET(timeout_cfg->timeout_sel);

        ptr->CCCR &= ~MCAN_CCCR_INIT_MASK;
        status = status_success;
        retry_cnt = MCAN_WAIT_RETRY_CNT_MAX;
        while ((ptr->CCCR & MCAN_CCCR_INIT_MASK) != 0U) {
            if (retry_cnt == 0U) {
                status = status_timeout;
                break;
            }
            retry_cnt--;
        }
    } while (false);

    return status;
}

void mcan_deinit(MCAN_Type *ptr)
{
    if (ptr != NULL) {
        mcan_enter_init_mode(ptr); /* Stop MCAN function */

        /* Enable write access to protected configuration registers */
        mcan_enable_write_to_prot_config_registers(ptr);

        /* Restore critical registers to default values */
        ptr->RWD = 0UL;
        ptr->TSCC = 0UL;
        ptr->GFC = 0UL;
        ptr->SIDFC = 0UL;
        ptr->XIDFC = 0UL;
        ptr->XIDAM = MCAN_EXT_ID_MASK;
        ptr->RXBC = 0UL;
        ptr->RXF0C = 0UL;
        ptr->RXF1C = 0UL;
        ptr->RXESC = 0UL;
        ptr->TXBC = 0UL;
        ptr->TXESC = 0UL;
        ptr->TXEFC = 0UL;
        ptr->TSCFG = 0UL;

        /* Disable all interrupts and clear all flags */
        mcan_disable_interrupts(ptr, ~0UL);
        mcan_clear_interrupt_flags(ptr, ~0UL);
        mcan_disable_txbuf_transmission_interrupt(ptr, ~0UL);
        mcan_disable_txbuf_cancel_finish_interrupt(ptr, ~0UL);

        /* Restore CCCR to default value */
        ptr->CCCR = MCAN_CCCR_INIT_MASK;
    }
}

hpm_stat_t mcan_set_filter_element(MCAN_Type *ptr, const mcan_filter_elem_t *filter_elem, uint32_t index)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (filter_elem == NULL));
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        status = mcan_validate_filter_element(ptr, filter_elem);
        HPM_BREAK_IF(status != status_success);
        bool is_ext_id = (filter_elem->can_id_type == MCAN_CAN_ID_TYPE_EXTENDED);

        uint32_t ram_offset;
        uint32_t filter_elem_size;
        uint32_t size_max;
        if (is_ext_id) {
            ram_offset = MCAN_XIDFC_FLESA_GET(ptr->XIDFC) << MCAN_XIDFC_FLESA_SHIFT;
            filter_elem_size = MCAN_FILTER_ELEM_EXT_ID_SIZE;
            size_max = MCAN_XIDFC_LSE_GET(ptr->XIDFC);
        } else {
            ram_offset = MCAN_SIDFC_FLSSA_GET(ptr->SIDFC) << MCAN_SIDFC_FLSSA_SHIFT;
            filter_elem_size = MCAN_FILTER_ELEM_STD_ID_SIZE;
            size_max = MCAN_SIDFC_LSS_GET(ptr->SIDFC);
        }

        if (index >= size_max) {
            status = status_mcan_filter_index_out_of_range;
            break;
        }

        volatile uint32_t *config_start;
        config_start = (volatile uint32_t *)(mcan_get_ram_base(ptr) + ram_offset + filter_elem_size * index);

        uint32_t config_words[2] = { 0, 0 };
        if (is_ext_id) {
            mcan_encode_ext_filter_element(filter_elem, config_words);
            config_start[0] = config_words[0];
            config_start[1] = config_words[1];
        } else {
            mcan_encode_std_filter_element(filter_elem, &config_words[0]);
            config_start[0] = config_words[0];
        }

        status = status_success;

    } while (false);

    return status;
}


uint8_t mcan_get_message_size_from_dlc(uint8_t dlc)
{
    uint8_t msg_size = 0U;
    if (dlc <= 8U) {
        msg_size = dlc;
    } else if (dlc <= 12U) {
        msg_size = 8U + (dlc - 8U) * 4U;
    } else if (dlc < 16U) {
        msg_size = 32U + (dlc - 13U) * 16U;
    } else {
        /* Wrong DLC encoding, should never happen */
        msg_size = 0U;
    }
    return msg_size;
}

uint8_t mcan_get_data_field_size(uint8_t data_field_size_option)
{
    uint8_t size_in_bytes = 0;

    switch (data_field_size_option) {
    case MCAN_DATA_FIELD_SIZE_8BYTES:
        size_in_bytes = 8U;
        break;
    case MCAN_DATA_FIELD_SIZE_12BYTES:
        size_in_bytes = 12U;
        break;
    case MCAN_DATA_FIELD_SIZE_16BYTES:
        size_in_bytes = 16U;
        break;
    case MCAN_DATA_FIELD_SIZE_20BYTES:
        size_in_bytes = 20U;
        break;
    case MCAN_DATA_FIELD_SIZE_24BYTES:
        size_in_bytes = 24U;
        break;
    case MCAN_DATA_FIELD_SIZE_32BYTES:
        size_in_bytes = 32U;
        break;
    case MCAN_DATA_FIELD_SIZE_48BYTES:
        size_in_bytes = 48U;
        break;
    case MCAN_DATA_FIELD_SIZE_64BYTES:
        size_in_bytes = 64U;
        break;
    default:
        /* Invalid option */
        break;
    }
    return size_in_bytes;
}

hpm_stat_t mcan_set_tx_frame_payload(mcan_tx_frame_t *frame, const uint8_t *payload, uint8_t payload_size)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if (frame == NULL) {
            break;
        }

        if ((payload == NULL) && (payload_size != 0U)) {
            break;
        }

        if ((frame->rtr != 0U) && (payload_size != 0U)) {
            break;
        }

        if ((!frame->canfd_frame) && (frame->dlc > 8U)) {
            frame->dlc = 8U;
        }

        uint8_t payload_capacity = mcan_get_tx_frame_payload_capacity(frame);
        if (payload_size > payload_capacity) {
            break;
        }

        memset(frame->data_8, 0, sizeof(frame->data_8));
        if (payload_size != 0U) {
            memcpy(frame->data_8, payload, payload_size);
        }

        status = status_success;
    } while (false);

    return status;
}

static void mcan_fill_tx_frame_to_msg_ram_slot(MCAN_Type *ptr, uint32_t slot_base_addr, const mcan_tx_frame_t *tx_frame)
{
    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;
    uint8_t msg_size_words = (mcan_get_message_size_from_dlc(tx_frame->dlc) + 3U) / sizeof(uint32_t);
    uint8_t max_words = mcan_get_data_field_size(MCAN_TXESC_TBDS_GET(ptr->TXESC)) / sizeof(uint32_t);
    if (msg_size_words > max_words) {
        msg_size_words = max_words;
    }

    uint32_t hdr_words[MCAN_MESSAGE_HEADER_SIZE_IN_WORDS];
    (void)memcpy(hdr_words, tx_frame, sizeof(hdr_words));
    msg_hdr[0] = hdr_words[0];
    msg_hdr[1] = hdr_words[1];
    for (uint32_t i = 0; i < msg_size_words; i++) {
        msg_data[i] = tx_frame->data_32[i];
    }
}

hpm_stat_t mcan_write_txbuf(MCAN_Type *ptr, uint32_t index, mcan_tx_frame_t *tx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }

        uint32_t txbuf_index_max = MCAN_TXBC_NDTB_GET(ptr->TXBC);
        if (index >= txbuf_index_max) {
            status = status_mcan_txbuf_index_out_of_range;
            break;
        }

        if (!mcan_is_transmit_request_pending(ptr, index)) {
            uint32_t slot_base_addr = mcan_get_txbuf_elem_addr(ptr, index);
            mcan_fill_tx_frame_to_msg_ram_slot(ptr, slot_base_addr, tx_frame);
            status = status_success;
        } else {
            status = status_fail;
        }
    } while (false);

    return status;
}

hpm_stat_t mcan_write_txfifo(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }
        if (!mcan_is_txfifo_full(ptr)) {
            uint32_t put_index = mcan_get_txfifo_put_index(ptr);
            uint32_t slot_base_addr = mcan_get_txbuf_elem_addr(ptr, put_index);
            mcan_fill_tx_frame_to_msg_ram_slot(ptr, slot_base_addr, tx_frame);
            status = status_success;
        } else {
            status = status_mcan_txfifo_full;
        }
    } while (false);

    return status;
}

static hpm_stat_t mcan_copy_rx_frame_from_msg_ram_slot(uint32_t slot_base_addr, uint8_t max_payload_size, mcan_rx_message_t *rx_frame)
{
    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;
    uint32_t hdr_words[MCAN_MESSAGE_HEADER_SIZE_IN_WORDS];
    hdr_words[0] = msg_hdr[0];
    hdr_words[1] = msg_hdr[1];
    (void)memcpy(rx_frame, hdr_words, sizeof(hdr_words));

    uint8_t dlc_size_words = (mcan_get_message_size_from_dlc(rx_frame->dlc) + 3U) / sizeof(uint32_t);
    uint8_t max_payload_words = max_payload_size / sizeof(uint32_t);
    uint8_t copy_words = (dlc_size_words < max_payload_words) ? dlc_size_words : max_payload_words;

    for (uint32_t i = 0; i < copy_words; i++) {
        rx_frame->data_32[i] = msg_data[i];
    }

    return status_success;
}

hpm_stat_t mcan_read_rxbuf(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (rx_frame == NULL)) {
            break;
        }

        if (!mcan_is_rxbuf_data_available(ptr, index)) {
            status = status_mcan_rxbuf_empty;
            break;
        }

        uint32_t slot_base_addr = mcan_get_rxbuf_elem_addr(ptr, index);
        uint8_t max_payload_size = mcan_get_data_field_size(MCAN_RXESC_RBDS_GET(ptr->RXESC));
        status = mcan_copy_rx_frame_from_msg_ram_slot(slot_base_addr, max_payload_size, rx_frame);

    } while (false);

    return status;
}

hpm_stat_t mcan_read_rxfifo(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (rx_frame == NULL)) {
            break;
        }

        /* Validate FIFO index - only 0 and 1 are valid */
        if (fifo_index > 1U) {
            status = status_invalid_argument;
            break;
        }

        uint32_t base_addr;
        uint32_t elem_index;
        uint32_t elem_size;
        uint32_t elem_size_option;
        if (fifo_index == 0) {
            uint32_t rxf0s = ptr->RXF0S;
            if (MCAN_RXF0S_F0FL_GET(rxf0s) == 0) {
                status = status_mcan_rxfifo_empty;
                break;
            }
            base_addr = mcan_get_rxfifo0_base(ptr);
            elem_size_option = MCAN_RXESC_F0DS_GET(ptr->RXESC);
            elem_index = MCAN_RXF0S_F0GI_GET(rxf0s);
        } else {
            uint32_t rxf1s = ptr->RXF1S;
            if (MCAN_RXF1S_F1FL_GET(rxf1s) == 0) {
                status = status_mcan_rxfifo_empty;
                break;
            }
            base_addr = mcan_get_rxfifo1_base(ptr);
            elem_size_option = MCAN_RXESC_F1DS_GET(ptr->RXESC);
            elem_index = MCAN_RXF1S_F1GI_GET(rxf1s);
        }

        uint32_t elem_data_size_max = mcan_get_data_field_size(elem_size_option);
        elem_size = elem_data_size_max + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;

        uint32_t slot_base_addr = base_addr + elem_size * elem_index;
        status = mcan_copy_rx_frame_from_msg_ram_slot(slot_base_addr, elem_data_size_max, rx_frame);

        if (fifo_index == 0) {
            ptr->RXF0A = elem_index;
        } else {
            ptr->RXF1A = elem_index;
        }

    } while (false);

    return status;
}

hpm_stat_t mcan_read_tx_evt_fifo(MCAN_Type *ptr, mcan_tx_event_fifo_elem_t *tx_evt)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (tx_evt == NULL)) {
            break;
        }

        uint32_t txefs = ptr->TXEFS;

        if (MCAN_TXEFS_EFFL_GET(txefs) == 0U) {
            status = status_mcan_tx_evt_fifo_empty;
            break;
        }

        uint32_t base_addr = mcan_get_ram_base(ptr) + (MCAN_TXEFC_EFSA_GET(ptr->TXEFC) << MCAN_TXEFC_EFSA_SHIFT);
        uint32_t elem_size = sizeof(mcan_tx_event_fifo_elem_t);
        uint32_t elem_index = MCAN_TXEFS_EFGI_GET(txefs);

        uint32_t elem_addr = base_addr + elem_size * elem_index;
        uint32_t *tx_evt_buf = (uint32_t *)elem_addr;

        tx_evt->words[0] = tx_evt_buf[0];
        tx_evt->words[1] = tx_evt_buf[1];

        ptr->TXEFA = MCAN_TXEFA_EFAI_SET(elem_index);

        status = status_success;

    } while (false);

    return status;
}

#if defined(CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND) && (CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND == 1)
#define MCAN_SOC_WORKAROUND_RETRY_MAX (1000000UL)

/**
 * @brief Wait until the MCAN peripheral is not in receiver-active state.
 *
 * @param ptr MCAN base address
 * @return status_success or status_timeout
 */
static hpm_stat_t mcan_wait_for_non_receiver_activity(MCAN_Type *ptr)
{
    uint32_t wait_cnt = MCAN_SOC_WORKAROUND_RETRY_MAX;
    while (mcan_get_activity(ptr) == mcan_activity_receiver) {
        if (--wait_cnt == 0U) {
            return status_timeout;
        }
    }
    return status_success;
}
#endif

hpm_stat_t mcan_transmit_via_txbuf_nonblocking(MCAN_Type *ptr, uint32_t index, mcan_tx_frame_t *tx_frame)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }

        status = mcan_write_txbuf(ptr, index, tx_frame);
        HPM_BREAK_IF(status != status_success);

#if defined(CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND) && (CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND == 1)
        status = mcan_wait_for_non_receiver_activity(ptr);
        HPM_BREAK_IF(status != status_success);
#endif

        mcan_send_add_request(ptr, index);
        status = status_success;

    } while (false);
    return status;
}

hpm_stat_t mcan_transmit_blocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        uint32_t put_index = 0;
        status = mcan_transmit_via_txfifo_nonblocking(ptr, tx_frame, &put_index);
        if (status != status_success) {
            break;
        }

        uint32_t retry_cnt = 0;
        while (!mcan_is_transmit_occurred(ptr, put_index)) {
            retry_cnt++;
            if (retry_cnt >= MCAN_TX_RETRY_COUNT_MAX) {
                break;
            }
        }
        if (retry_cnt >= MCAN_TX_RETRY_COUNT_MAX) {
            status = status_timeout;
        } else {
            status = status_success;
        }
    } while (false);
    return status;
}

hpm_stat_t mcan_request_and_fill_txfifo(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *fifo_index)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }
        if (mcan_is_txfifo_full(ptr)) {
            status = status_mcan_txfifo_full;
            break;
        }

        uint32_t put_index = mcan_get_txfifo_put_index(ptr);

        status = mcan_write_txfifo(ptr, tx_frame);
        HPM_BREAK_IF(status != status_success);

        if (fifo_index != NULL) {
            *fifo_index = put_index;
        }
    } while (false);
    return status;
}

hpm_stat_t mcan_transmit_via_txfifo_nonblocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *fifo_index)
{
    hpm_stat_t status;
    do {
        uint32_t put_index;
        status = mcan_request_and_fill_txfifo(ptr, tx_frame, &put_index);
        HPM_BREAK_IF(status != status_success);

#if defined(CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND) && (CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND == 1)
        status = mcan_wait_for_non_receiver_activity(ptr);
        HPM_BREAK_IF(status != status_success);
#endif
        mcan_send_add_request(ptr, put_index);

        if (fifo_index != NULL) {
            *fifo_index = put_index;
        }

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t mcan_write_tx_fifo_or_queue(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *slot_index)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }
        if (!mcan_is_txfifo_full(ptr)) {
            uint32_t put_index = mcan_get_txfifo_put_index(ptr);
            uint32_t slot_base_addr = mcan_get_txbuf_elem_addr(ptr, put_index);
            mcan_fill_tx_frame_to_msg_ram_slot(ptr, slot_base_addr, tx_frame);
            if (slot_index != NULL) {
                *slot_index = put_index;
            }
            status = status_success;
        } else {
            status = status_mcan_txfifo_full;
        }
    } while (false);

    return status;
}

hpm_stat_t mcan_transmit_via_txqueue_nonblocking(MCAN_Type *ptr, mcan_tx_frame_t *tx_frame, uint32_t *queue_index)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((ptr == NULL) || (tx_frame == NULL)) {
            break;
        }
        if (!mcan_is_txqueue_enabled(ptr)) {
            status = status_mcan_txqueue_not_enabled;
            break;
        }
        uint32_t put_index;
        status = mcan_write_tx_fifo_or_queue(ptr, tx_frame, &put_index);
        HPM_BREAK_IF(status != status_success);

#if defined(CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND) && (CAN_SOC_TX_RX_BUFFER_ACCESS_WORKAROUND == 1)
        status = mcan_wait_for_non_receiver_activity(ptr);
        HPM_BREAK_IF(status != status_success);
#endif
        mcan_send_add_request(ptr, put_index);

        if (queue_index != NULL) {
            *queue_index = put_index;
        }

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t mcan_receive_from_buf_blocking(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (rx_frame == NULL)) {
            break;
        }

        uint32_t retry_cnt = 0;

        while (!mcan_is_rxbuf_data_available(ptr, index)) {
            retry_cnt++;
            if (retry_cnt >= MCAN_RX_RETRY_COUNT_MAX) {
                status = status_timeout;
                break;
            }
        }
        if (status == status_timeout) {
            break;
        }

        status = mcan_read_rxbuf(ptr, index, rx_frame);
        mcan_clear_rxbuf_data_available_flag(ptr, index);
    } while (false);

    return status;
}

hpm_stat_t mcan_receive_from_fifo_blocking(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (rx_frame == NULL) || (fifo_index >= 2U));
        uint32_t retry_cnt = 0;
        while (mcan_get_rxfifo_fill_level(ptr, fifo_index) == 0) {
            retry_cnt++;
            if (retry_cnt >= MCAN_RX_RETRY_COUNT_MAX) {
                status = status_timeout;
                break;
            }
        }
        if (status == status_timeout) {
            break;
        }
        status = mcan_read_rxfifo(ptr, fifo_index, rx_frame);
    } while (false);

    return status;
}

hpm_stat_t mcan_get_high_priority_message_status(MCAN_Type *ptr, mcan_high_priority_message_status_t *status)
{
    if ((ptr == NULL) || (status == NULL)) {
        return status_invalid_argument;
    }
    uint32_t hpms = ptr->HPMS;
    status->buffer_index = MCAN_HPMS_BIDX_GET(hpms);
    status->filter_index = MCAN_HPMS_FIDX_GET(hpms);
    status->filter_list_type = MCAN_HPMS_FLST_GET(hpms);
    status->message_storage_indicator = MCAN_HPMS_MSI_GET(hpms);
    return status_success;
}

hpm_stat_t mcan_set_global_filter_config(MCAN_Type *ptr, mcan_global_filter_config_t *filter_config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((ptr == NULL) || (filter_config == NULL)) {
            break;
        }
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        if (mcan_validate_global_filter_config(filter_config) != status_success) {
            break;
        }

        ptr->GFC = MCAN_GFC_ANFE_SET(filter_config->accept_non_matching_ext_frame_option) |
                   MCAN_GFC_ANFS_SET(filter_config->accept_non_matching_std_frame_option) | MCAN_GFC_RRFS_SET(filter_config->reject_remote_std_frame) |
                   MCAN_GFC_RRFE_SET(filter_config->reject_remote_ext_frame);

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t mcan_config_transmitter_delay_compensation(MCAN_Type *ptr, mcan_tdc_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (config == NULL));
        HPM_BREAK_IF(mcan_require_config_mode(ptr) != status_success);
        mcan_apply_tdc_config(ptr, config);
        status = status_success;

    } while (false);
    return status;
}

uint32_t mcan_read_32bit_tsu_timestamp(MCAN_Type *ptr, uint32_t index)
{
    uint32_t ts_val = 0U;
    if (index < ARRAY_SIZE(ptr->TS_SEL)) {
        ts_val = ptr->TS_SEL[index];
    }
    return ts_val;
}

uint64_t mcan_read_64bit_tsu_timestamp(MCAN_Type *ptr, uint32_t index)
{
    uint64_t ts_val = 0U;
    uint32_t real_index = index & 0x7U; /* Clear bit3 according to IP design */
    uint32_t elem_count = ARRAY_SIZE(ptr->TS_SEL);
    if ((index < elem_count) && ((2U * real_index + 1) < elem_count)) {
        ts_val = ((uint64_t)(ptr->TS_SEL[2U * real_index + 1]) << 32U);
        ts_val |= ptr->TS_SEL[2U * real_index];
        /* Workaround: dummy read to clear the corresponding bits in TSS1 if the index is equal to/greater than 8 */
        if (index >= 8U) {
            (void)ptr->TS_SEL[index];
        }
    }

    return ts_val;
}

hpm_stat_t mcan_get_timestamp_from_tx_event(MCAN_Type *ptr, const mcan_tx_event_fifo_elem_t *tx_evt, mcan_timestamp_value_t *timestamp)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (tx_evt == NULL) || (timestamp == NULL));

        (void)memset(timestamp, 0, sizeof(mcan_timestamp_value_t));

        bool is_tsu_used = mcan_is_tsu_used(ptr);
        if (!is_tsu_used) {
            timestamp->is_16bit = true;
            timestamp->ts_16bit = tx_evt->tx_timestamp;
            status = status_success;
        } else if (tx_evt->tx_timestamp_captured != 0U) {
            bool is_64bit_ts = mcan_is_64bit_tsu_timestamp_used(ptr);
            uint32_t ts_index = tx_evt->tx_timestamp_pointer;
            if (!is_64bit_ts) {
                timestamp->is_32bit = true;
                timestamp->ts_32bit = mcan_read_32bit_tsu_timestamp(ptr, ts_index);
            } else {
                timestamp->is_64bit = true;
                timestamp->ts_64bit = mcan_read_64bit_tsu_timestamp(ptr, ts_index);
            }
            status = status_success;
        } else {
            status = status_mcan_timestamp_not_exist;
        }

    } while (false);

    return status;
}

hpm_stat_t mcan_get_timestamp_from_received_message(MCAN_Type *ptr, const mcan_rx_message_t *rx_msg, mcan_timestamp_value_t *timestamp)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (rx_msg == NULL) || (timestamp == NULL));

        (void)memset(timestamp, 0, sizeof(mcan_timestamp_value_t));

        bool is_tsu_used = mcan_is_tsu_used(ptr);
        if (!is_tsu_used) {
            timestamp->is_16bit = true;
            timestamp->ts_16bit = rx_msg->rx_timestamp;
            status = status_success;
        } else if (rx_msg->rx_timestamp_captured != 0U) {
            bool is_64bit_ts = mcan_is_64bit_tsu_timestamp_used(ptr);
            uint32_t ts_index = rx_msg->rx_timestamp_pointer;
            if (!is_64bit_ts) {
                timestamp->is_32bit = true;
                timestamp->ts_32bit = mcan_read_32bit_tsu_timestamp(ptr, ts_index);
            } else {
                timestamp->is_64bit = true;
                timestamp->ts_64bit = mcan_read_64bit_tsu_timestamp(ptr, ts_index);
            }
            status = status_success;
        } else {
            status = status_mcan_timestamp_not_exist;
        }
    } while (false);

    return status;
}

hpm_stat_t mcan_parse_protocol_status(uint32_t psr, mcan_protocol_status_t *protocol_status)
{
    if (protocol_status == NULL) {
        return status_invalid_argument;
    }
    memset(protocol_status, 0, sizeof(mcan_protocol_status_t));
    uint32_t psr_val = psr;
    if (MCAN_PSR_PXE_GET(psr_val) != 0U) {
        protocol_status->protocol_exception_evt_occurred = true;
    }
    if (MCAN_PSR_RFDF_GET(psr_val) != 0U) {
        protocol_status->canfd_msg_received = true;
    }
    if (MCAN_PSR_RBRS_GET(psr_val) != 0U) {
        protocol_status->brs_flag_set_in_last_rcv_canfd_msg = true;
    }
    if (MCAN_PSR_RESI_GET(psr_val) != 0U) {
        protocol_status->esi_flag_set_in_last_rcv_canfd_msg = true;
    }
    if (MCAN_PSR_BO_GET(psr_val) != 0U) {
        protocol_status->in_bus_off_state = true;
    }
    if (MCAN_PSR_EW_GET(psr_val) != 0U) {
        protocol_status->in_warning_state = true;
    }
    if (MCAN_PSR_EP_GET(psr_val) != 0U) {
        protocol_status->in_error_passive_state = true;
    }
    protocol_status->activity = (mcan_activity_state_t)MCAN_PSR_ACT_GET(psr_val);
    protocol_status->tdc_val = MCAN_PSR_TDCV_GET(psr_val);

    if (protocol_status->brs_flag_set_in_last_rcv_canfd_msg) {
        protocol_status->last_error_code = (mcan_last_err_code_t)MCAN_PSR_DLEC_GET(psr_val);
    } else {
        protocol_status->last_error_code = (mcan_last_err_code_t)MCAN_PSR_LEC_GET(psr_val);
    }
    return status_success;
}

hpm_stat_t mcan_get_protocol_status(MCAN_Type *ptr, mcan_protocol_status_t *protocol_status)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (protocol_status == NULL));
        status = mcan_parse_protocol_status(ptr->PSR, protocol_status);

    } while (false);

    return status;
}

hpm_stat_t mcan_get_error_state(MCAN_Type *ptr, mcan_error_state_t *error_state)
{
    hpm_stat_t status = status_invalid_argument;
    mcan_protocol_status_t protocol_status;

    do {
        HPM_BREAK_IF((ptr == NULL) || (error_state == NULL));

        status = mcan_get_protocol_status(ptr, &protocol_status);
        HPM_BREAK_IF(status != status_success);

        if (protocol_status.in_bus_off_state) {
            *error_state = mcan_error_state_bus_off;
        } else if (protocol_status.in_error_passive_state) {
            *error_state = mcan_error_state_passive;
        } else if (protocol_status.in_warning_state) {
            *error_state = mcan_error_state_warning;
        } else {
            *error_state = mcan_error_state_active;
        }

        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t mcan_get_diagnostic_snapshot(MCAN_Type *ptr, mcan_diagnostic_snapshot_t *snapshot)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((ptr == NULL) || (snapshot == NULL));

        memset(snapshot, 0, sizeof(*snapshot));
        snapshot->interrupt_flags = mcan_get_interrupt_flags(ptr);
        status = mcan_parse_protocol_status(ptr->PSR, &snapshot->protocol_status);
        HPM_BREAK_IF(status != status_success);

        mcan_get_error_counter(ptr, &snapshot->error_count);
        snapshot->last_data_error_code = mcan_get_last_data_error_code(ptr);
        snapshot->rxfifo0_fill_level = mcan_get_rxfifo_fill_level(ptr, 0U);
        snapshot->rxfifo1_fill_level = mcan_get_rxfifo_fill_level(ptr, 1U);
        snapshot->rxfifo0_full = mcan_is_rxfifo_full(ptr, 0U);
        snapshot->rxfifo1_full = mcan_is_rxfifo_full(ptr, 1U);
        snapshot->txfifo_full = mcan_is_txfifo_full(ptr);
        snapshot->txqueue_enabled = mcan_is_txqueue_enabled(ptr);
        snapshot->in_init_mode = ((ptr->CCCR & MCAN_CCCR_INIT_MASK) != 0U);
        snapshot->config_write_enabled = ((ptr->CCCR & MCAN_CCCR_CCE_MASK) != 0U);
        snapshot->canfd_enabled = mcan_is_canfd_enabled(ptr);
        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t mcan_clear_runtime_faults(MCAN_Type *ptr, uint32_t interrupt_mask)
{
    if (ptr == NULL) {
        return status_invalid_argument;
    }

    if (interrupt_mask != 0U) {
        mcan_clear_interrupt_flags(ptr, interrupt_mask);
    }

    return status_success;
}

hpm_stat_t mcan_recover_from_busoff(MCAN_Type *ptr)
{
    hpm_stat_t status = status_invalid_argument;
    mcan_protocol_status_t protocol_status;

    do {
        HPM_BREAK_IF(ptr == NULL);

        status = mcan_get_protocol_status(ptr, &protocol_status);
        HPM_BREAK_IF(status != status_success);

        if (!protocol_status.in_bus_off_state) {
            status = status_success;
            break;
        }

        mcan_enter_init_mode(ptr);
        status = mcan_wait_init_mode_state(ptr, true);
        HPM_BREAK_IF(status != status_success);

        status = mcan_clear_runtime_faults(ptr, ~0UL);
        HPM_BREAK_IF(status != status_success);

        mcan_enter_normal_mode(ptr);
        status = mcan_wait_init_mode_state(ptr, false);
    } while (false);

    return status;
}

bool mcan_is_canfd_enabled(MCAN_Type *ptr)
{
    return (ptr != NULL) && ((ptr->CCCR & MCAN_CCCR_FDOE_MASK) != 0U);
}

/*---------------------------------------------------------------------
 * Unchecked TX/RX API -- validation-free, guard-free primitives
 *---------------------------------------------------------------------*/

void mcan_write_txfifo_unchecked(MCAN_Type *ptr, const mcan_tx_frame_t *tx_frame, uint32_t *put_index)
{
    uint32_t idx = mcan_get_txfifo_put_index(ptr);
    *put_index = idx;

    uint32_t slot_base_addr = mcan_get_txbuf_elem_addr(ptr, idx);
    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;

    uint8_t data_words = (mcan_get_message_size_from_dlc(tx_frame->dlc) + 3U) / sizeof(uint32_t);

    memcpy((void *)msg_hdr, tx_frame, MCAN_MESSAGE_HEADER_SIZE_IN_BYTES);
    for (uint32_t i = 0; i < data_words; i++) {
        msg_data[i] = tx_frame->data_32[i];
    }

    mcan_send_add_request(ptr, idx);
}

void mcan_write_txbuf_unchecked(MCAN_Type *ptr, uint32_t index, const mcan_tx_frame_t *tx_frame)
{
    uint32_t slot_base_addr = mcan_get_txbuf_elem_addr(ptr, index);
    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;

    uint8_t data_words = (mcan_get_message_size_from_dlc(tx_frame->dlc) + 3U) / sizeof(uint32_t);

    memcpy((void *)msg_hdr, tx_frame, MCAN_MESSAGE_HEADER_SIZE_IN_BYTES);
    for (uint32_t i = 0; i < data_words; i++) {
        msg_data[i] = tx_frame->data_32[i];
    }

    mcan_send_add_request(ptr, index);
}

void mcan_read_rxfifo_unchecked(MCAN_Type *ptr, uint32_t fifo_index, mcan_rx_message_t *rx_frame)
{
    uint32_t base_addr;
    uint32_t elem_index;
    uint32_t elem_size_option;

    if (fifo_index == 0U) {
        uint32_t rxf0s = ptr->RXF0S;
        base_addr = mcan_get_rxfifo0_base(ptr);
        elem_size_option = MCAN_RXESC_F0DS_GET(ptr->RXESC);
        elem_index = MCAN_RXF0S_F0GI_GET(rxf0s);
    } else {
        uint32_t rxf1s = ptr->RXF1S;
        base_addr = mcan_get_rxfifo1_base(ptr);
        elem_size_option = MCAN_RXESC_F1DS_GET(ptr->RXESC);
        elem_index = MCAN_RXF1S_F1GI_GET(rxf1s);
    }

    uint32_t elem_size = mcan_get_data_field_size(elem_size_option) + MCAN_MESSAGE_HEADER_SIZE_IN_BYTES;
    uint32_t slot_base_addr = base_addr + elem_size * elem_index;

    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;
    memcpy(rx_frame, (const void *)msg_hdr, MCAN_MESSAGE_HEADER_SIZE_IN_BYTES);

    uint8_t data_words = (mcan_get_message_size_from_dlc(rx_frame->dlc) + 3U) / sizeof(uint32_t);
    for (uint32_t i = 0; i < data_words; i++) {
        rx_frame->data_32[i] = msg_data[i];
    }

    if (fifo_index == 0U) {
        ptr->RXF0A = elem_index;
    } else {
        ptr->RXF1A = elem_index;
    }
}

void mcan_read_rxbuf_unchecked(MCAN_Type *ptr, uint32_t index, mcan_rx_message_t *rx_frame)
{
    uint32_t slot_base_addr = mcan_get_rxbuf_elem_addr(ptr, index);

    volatile uint32_t *msg_hdr = (volatile uint32_t *)slot_base_addr;
    volatile uint32_t *msg_data = msg_hdr + MCAN_MESSAGE_HEADER_SIZE_IN_WORDS;
    memcpy(rx_frame, (const void *)msg_hdr, MCAN_MESSAGE_HEADER_SIZE_IN_BYTES);

    uint8_t data_words = (mcan_get_message_size_from_dlc(rx_frame->dlc) + 3U) / sizeof(uint32_t);
    for (uint32_t i = 0; i < data_words; i++) {
        rx_frame->data_32[i] = msg_data[i];
    }
}