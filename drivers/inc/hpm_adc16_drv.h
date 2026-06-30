/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ADC16_DRV_H
#define HPM_ADC16_DRV_H

#include "hpm_common.h"
#include "hpm_adc16_regs.h"
#include "hpm_soc_feature.h"

/**
 * @brief ADC16 driver APIs
 * @defgroup adc16_interface ADC16 driver APIs
 * @ingroup adc_interfaces
 * @{
 */

/** @brief Define ADC16 validity check for the channel number */
#if defined (ADC16_SOC_TEMP_CH_EN) && ADC16_SOC_TEMP_CH_EN
#define ADC16_IS_CHANNEL_INVALID(CH) (CH > ADC16_SOC_MAX_CH_NUM && CH != ADC16_SOC_TEMP_CH_NUM)
#else
#define ADC16_IS_CHANNEL_INVALID(CH) (CH > ADC16_SOC_MAX_CH_NUM)
#endif

/** @brief Define ADC16 validity check for the channel sample cycle */
#define ADC16_IS_CHANNEL_SAMPLE_CYCLE_INVALID(CYC) (CYC == 0)

/** @brief Define ADC16 validity check for the trigger number */
#define ADC16_IS_TRIG_CH_INVLAID(CH) (CH > ADC_SOC_MAX_TRIG_CH_NUM)

/** @brief Define ADC16 validity check for the trigger length */
#define ADC16_IS_TRIG_LEN_INVLAID(TRIG_LEN) (TRIG_LEN > ADC_SOC_MAX_TRIG_CH_LEN)

/** @brief Define ADC16 validity check for the sequence length */
#define ADC16_IS_SEQ_LEN_INVLAID(LEN)  ((LEN == 0) || (LEN > ADC_SOC_SEQ_MAX_LEN))

/** @brief Define ADC16 validity check for the DMA buffer length in the sequence mode */
#define ADC16_IS_SEQ_DMA_BUFF_LEN_IN_4BYTES_INVALID(LEN)  ((LEN == 0) || (LEN > ADC_SOC_SEQ_MAX_DMA_BUFF_LEN_IN_4BYTES))

#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
/**
 * @brief Validity for sequence DMA when @c DMA_SEQ16BIT: @a LEN is always a count in 16b (2-byte) halfword units. @c ADC_SOC_SEQ_MAX_DMA_BUFF_LEN_IN_4BYTES
 *        names the 32b-word SoC cap; compare @a LEN to @c (that value / 2).
 */
#define ADC16_IS_SEQ_DMA_BUFF_LEN_IN_2BYTES_INVALID(LEN) ((LEN) == 0U || (LEN) > (uint32_t)ADC_SOC_SEQ_MAX_DMA_BUFF_LEN_IN_4BYTES / 2U)
#endif

/** @brief Define ADC16 validity check for the DMA buffer length in the preemption mode */
#define ADC16_IS_PMT_DMA_BUFF_LEN_INVLAID(LEN)  ((LEN == 0) || (LEN > ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES))

#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
/** @brief Validity check for SAMPLE_CFG.ADC_LOOP exponent n (2^n loops); max n is per-SOC (ADC16_SOC_ADC_LOOP_MAX_EXP). */
#define ADC16_IS_ADC_LOOP_INVALID(EXP) ((EXP) > ADC16_SOC_ADC_LOOP_MAX_EXP)
#endif

/** @brief Define ADC16 resolutions. */
typedef enum {
    adc16_res_8_bits = 9,
    adc16_res_10_bits = 11,
    adc16_res_12_bits = 14,
    adc16_res_16_bits = 21
} adc16_resolution_t;

/** @brief Define ADC16 conversion modes. */
typedef enum {
    adc16_conv_mode_oneshot = 0,
    adc16_conv_mode_period,
    adc16_conv_mode_sequence,
    adc16_conv_mode_preemption
} adc16_conversion_mode_t;

/** @brief  Define ADC16 Clock Divider */
typedef enum {
    adc16_clock_divider_1 = 1,
    adc16_clock_divider_2,
    adc16_clock_divider_3,
    adc16_clock_divider_4,
    adc16_clock_divider_5,
    adc16_clock_divider_6,
    adc16_clock_divider_7,
    adc16_clock_divider_8,
    adc16_clock_divider_9,
    adc16_clock_divider_10,
    adc16_clock_divider_11,
    adc16_clock_divider_12,
    adc16_clock_divider_13,
    adc16_clock_divider_14,
    adc16_clock_divider_15,
    adc16_clock_divider_16,
} adc16_clock_divider_t;

/** @brief  Define ADC16 irq events. */
typedef enum {
    /** This mask indicates that a trigger conversion is complete. */
    adc16_event_trig_complete       = ADC16_INT_STS_TRIG_CMPT_MASK,

    /** This mask indicates that a conflict caused by software-triggered conversions. */
    adc16_event_trig_sw_conflict    = ADC16_INT_STS_TRIG_SW_CFLCT_MASK,

    /** This mask indicates that a conflict caused by hardware-triggered conversions. */
    adc16_event_trig_hw_conflict    = ADC16_INT_STS_TRIG_HW_CFLCT_MASK,

    /** This mask indicates that a conflict caused when bus reading from different channels. */
    adc16_event_read_conflict       = ADC16_INT_STS_READ_CFLCT_MASK,

    /** This mask indicates that a conflict caused by sequence-triggered conversions. */
    adc16_event_seq_sw_conflict     = ADC16_INT_STS_SEQ_SW_CFLCT_MASK,

    /** This mask indicates that a conflict caused by hardware-triggered conversions. */
    adc16_event_seq_hw_conflict     = ADC16_INT_STS_SEQ_HW_CFLCT_MASK,

    /** This mask indicates that DMA is stopped currently. */
    adc16_event_seq_dma_abort       = ADC16_INT_STS_SEQ_DMAABT_MASK,

    /** This mask indicates that all of the configured conversion(s) in a queue is(are) complete. */
    adc16_event_seq_full_complete   = ADC16_INT_STS_SEQ_CMPT_MASK,

    /** This mask indicates that one of the configured conversion(s) in a queue is complete. */
    adc16_event_seq_single_complete = ADC16_INT_STS_SEQ_CVC_MASK,

    /** This mask indicates that DMA FIFO is full currently. */
    adc16_event_dma_fifo_full       = ADC16_INT_STS_DMA_FIFO_FULL_MASK
#if defined(HPM_IP_FEATURE_ADC16_SEQ_STOP_POS_INT) && HPM_IP_FEATURE_ADC16_SEQ_STOP_POS_INT
    ,
    /** INT_STS STOP_POS only; @c HPM_IP_FEATURE_ADC16_SEQ_STOP_POS_INT gates this IRQ bit not SEQ_DMA_CFG STOP_EN. */
    adc16_event_seq_stop_pos        = ADC16_INT_STS_STOP_POS_MASK
#endif
} adc16_irq_event_t;

#if defined(HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE) && HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE
/**
 * @brief ADC16 differential result code format (CONV_CFG0 POS_MODE).
 * @note Applies only when differential mode is enabled. This is not ADC input
 *       single-ended vs differential wiring; it selects how the master-minus-slave
 *       result is encoded on the 16-bit bus. @c full_resolution=1 ignores POS_MODE on HW;
 *       only @c full_resolution=0 applies @c position_mode. Combine both fields accordingly.
 * @note Enum names @c differential / @c single_ended are misleading; planned rename
 *       to @c signed / @c offset — see @c .cursor/plans/adc16_diff_pos_mode_rename.plan.md.
 */
typedef enum {
    /** POS_MODE=0: signed -32768..+32767; output 0 when master and slave ADC results are equal (zero differential). */
    adc16_diff_pos_mode_differential = 0,
    /** POS_MODE=1: offset binary 0..65535; output 32768 when master and slave ADC results are equal (zero differential). */
    adc16_diff_pos_mode_single_ended
} adc16_diff_pos_mode_t;
#endif

/** @brief ADC16 common configuration struct. */
typedef struct {
    uint8_t res;
    uint8_t conv_mode;
    uint32_t adc_clk_div;
    bool port3_realtime;
    bool wait_dis;
#if !defined(HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB) || !HPM_IP_FEATURE_ADC16_FORCE_SYNC_AHB
    bool sel_sync_ahb;
#endif
    /**
     * @deprecated Since HPM SDK v1.12.0. Ignored: @ref adc16_init sets ADC_CFG0 ADC_AHB_EN from @c conv_mode (enabled for sequence and preemption, disabled for oneshot and period).
     */
    bool adc_ahb_en;
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
    /**
     * If true, sequence-mode DMA writes one 16-bit conversion result per sample (no packed cycle bit/sequence num /channel flags).
     * See User Manual DMA_SEQ16BIT.
     */
    bool dma_seq16bit;
#endif
} adc16_config_t;

/** @brief ADC16 channel configuration struct. */
typedef struct {
   uint8_t ch;
   uint16_t thshdh;
   uint16_t thshdl;
  /** If true, threshold/sample are applied for watchdog. When ADC16_SOC_WDOG_INT_EN_DEFERRED is set for the SoC, INT_EN is not set in adc16_init_channel. Call adc16_enable_wdog_interrupt() after in-window conversion(s). */
   bool wdog_int_en;
   uint8_t sample_cycle_shift;
   uint32_t sample_cycle;
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
   /**
    * Oversampling loop exponent n: repeat count is 2^n (0 = once). Max n is ADC16_SOC_ADC_LOOP_MAX_EXP; see SAMPLE_CFG.ADC_LOOP;
    * conversion time scales as (convert_clock_number + sample_clock_number) * 2^n.
    */
   uint8_t adc_loop_exp;
#endif
} adc16_channel_config_t;

/** @brief ADC16 channel configuration struct. */
typedef struct {
   uint8_t ch;
   uint16_t thshdh;
   uint16_t thshdl;
} adc16_channel_threshold_t;

/**
 * @brief ADC16 sequence-mode DMA: base address in @c start_addr and length in the union.
 *        Whichever form applies is determined by @c DMA_SEQ16BIT in @c ADC_CFG0 when the feature exists:
 *        use @c buff_len_in_4bytes for 32b packed output, or @c buff_len_in_2bytes for 16b-only output. In
 *        both cases the value is the same meaning as the IP’s @c (BUF_LEN+1) in that data width: count of
 *        32b buffer slots, or count of 16b halfwords, respectively. See @ref adc16_init_seq_dma.
 */
typedef struct {
    uint32_t *start_addr;
    union {
        /** 32b packed path (@c DMA_SEQ16BIT=0): @c (BUF_LEN+1) in whole 32b words. Each word is one @ref adc16_seq_dma_data_t (one sample). */
        uint32_t buff_len_in_4bytes;
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
        /**
         * 16b path (@c DMA_SEQ16BIT=1): @c (BUF_LEN+1) in 16b halfwords.
         * Each sample is a raw 16b value only, not an @ref adc16_seq_dma_data_t (no channel number, sequence number, or cycle bit packed in one 32b word).
         */
        uint32_t buff_len_in_2bytes;
#endif
    };
    /** DMA write index threshold in @c SEQ_DMA_CFG / @c SEQ_HIGH_CFG (applied regardless of @c stop_en). */
    uint32_t stop_pos;
    /** @c SEQ_DMA_CFG STOP_EN only: stop DMA when write pointer reaches @c stop_pos. */
    bool stop_en;
} adc16_dma_config_t;

/** @brief ADC16 DMA configuration struct for the sequence mode. */
#if defined(ADC_SOC_IP_VERSION) && (ADC_SOC_IP_VERSION < 2)
typedef struct {
    uint32_t result    :16;
    uint32_t seq_num   :4;
    uint32_t           :4;
    uint32_t adc_ch    :5;
    uint32_t           :2;
    uint32_t cycle_bit :1;
} adc16_seq_dma_data_t;
#else
typedef struct {
    uint32_t result    :16;
    uint32_t seq_num   :4;
    uint32_t adc_ch    :5;
    uint32_t           :6;
    uint32_t cycle_bit :1;
} adc16_seq_dma_data_t;
#endif

/** @brief ADC16 DMA configuration struct for the preemption mode. */
#if defined(ADC_SOC_IP_VERSION) && (ADC_SOC_IP_VERSION < 2)
typedef struct {
    uint32_t result    :16;
    uint32_t seq_num   :2;
    uint32_t           :2;
    uint32_t trig_ch   :4;
    uint32_t adc_ch    :5;
    uint32_t           :2;
    uint32_t cycle_bit :1;
} adc16_pmt_dma_data_t;
#else
typedef struct {
    uint32_t result    :16;
    uint32_t           :4;
    uint32_t adc_ch    :5;
    uint32_t trig_ch   :4;
    uint32_t seq_num   :2;
    uint32_t cycle_bit :1;
} adc16_pmt_dma_data_t;
#endif

/** @brief ADC16 configuration struct for the period mode. */
typedef struct {
    uint8_t ch;
    uint8_t prescale;      /** 0: 1*PRD; 1: 2*PRD; 2: 4*PRD */
    uint8_t period_count;  /** period_count = actual period reload - 1 */
} adc16_prd_config_t;
/** @brief ADC16 queue configuration struct for the sequence mode. */
typedef struct {
    bool seq_int_en;
    uint8_t ch;
} adc16_seq_queue_config_t;

/** @brief ADC16 configuration struct for the sequence mode. */
typedef struct {
    adc16_seq_queue_config_t queue[ADC_SOC_SEQ_MAX_LEN];
    bool restart_en;
    bool cont_en;
    bool sw_trig_en;
    bool hw_trig_en;
    uint8_t seq_len;
} adc16_seq_config_t;

/** @brief ADC16 trigger configuration struct for the preemption mode. */
typedef struct {
    bool    inten[ADC_SOC_MAX_TRIG_CH_LEN];
    uint8_t adc_ch[ADC_SOC_MAX_TRIG_CH_LEN];
    uint8_t trig_ch;
    uint8_t trig_len;
} adc16_pmt_config_t;

#if defined(HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE) && HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE
/** @brief ADC16 differential configuration struct. */
typedef struct {
    /** CONV_CFG0 FULL_RESOLUTION: clamp algebraic diff < 0 to 0 when set; HW ignores POS_MODE when set. */
    uint8_t full_resolution;
    /** CONV_CFG0 POS_MODE: @ref adc16_diff_pos_mode_t result code format; effective only when @c full_resolution is 0. */
    uint8_t position_mode;
    bool    master;
} adc16_diff_config_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @name Initialization and Deinitialization
 * @{
 */

/**
 * @brief Get a default configuration for an ADC16 instance.
 *
 * @param[out] config A pointer to the configuration struct of @ref adc16_config_t.
 *
 */
void adc16_get_default_config(adc16_config_t *config);

/**
 * @brief Get a default configuration for an ADC16 Channel.
 *
 * @param[out] config A pointer to the configuration struct of @ref adc16_channel_config_t.
 */
void adc16_get_channel_default_config(adc16_channel_config_t *config);

#if defined(HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE) && HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE
/**
 * @brief Get a default configuration for the differential mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[out] config A pointer to a specified ADC16 differential configuration struct of @ref adc16_diff_config_t.
 */
void adc16_get_default_diff_mode_config(ADC16_Type *ptr, adc16_diff_config_t *config);
#endif

/**
 * @brief De-initialize an ADC16 instance.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @return A result of de-initializing an ADC16 instance.
 * @retval status_success De-initialize an ADC16 instance successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument De-initialize an ADC16 instance unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_deinit(ADC16_Type *ptr);

/**
 * @brief Initialize an ADC16 instance.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to the configuration struct of @ref adc16_config_t. @ref adc16_init programs ADC_AHB_EN from @c conv_mode; the struct field @c adc_ahb_en is deprecated and ignored.
 * @return A result of initializing an ADC16 instance.
 * @retval status_success Initialize an ADC16 instance successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Initialize an ADC16 instance unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_init(ADC16_Type *ptr, adc16_config_t *config);

/**
 * @brief Initialize an ADC16 channel
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to the configuration struct of @ref adc16_channel_config_t.
 * @return A result of initializing an ADC16 channel.
 * @retval status_success Initialize an ADC16 channel successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Initialize an ADC16 channel unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_init_channel(ADC16_Type *ptr, adc16_channel_config_t *config);

#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
/**
 * @brief Set per-channel ADC loop exponent for oversampling (SAMPLE_CFG.ADC_LOOP).
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 channel number.
 * @param[in] adc_loop_exponent exponent n for 2^n repeat loops; 0 = single pass; n must be <= ADC16_SOC_ADC_LOOP_MAX_EXP.
 * @return status_success or status_invalid_argument if channel or exponent is out of range.
 */
hpm_stat_t adc16_set_channel_adc_loop(ADC16_Type *ptr, uint8_t ch, uint8_t adc_loop_exponent);
#endif

/**
 * @brief Read per-channel ADC loop exponent (SAMPLE_CFG.ADC_LOOP) when IP supports ADC_LOOP.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 channel number.
 * @param[out] adc_loop_exponent n where repeat count is 2^n; set to 0 when IP has no ADC_LOOP.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t adc16_get_channel_adc_loop(ADC16_Type *ptr, uint8_t ch, uint8_t *adc_loop_exponent);

/**
 * @brief Get thresholds of an ADC16 channel
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 channel number
 * @param[out] config A pointer to the structure of channel threshold
 * @return A result of getting thresholds of an ADC16 channel .
 * @retval status_success Initialize an ADC16 channel successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Initialize an ADC16 channel unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_get_channel_threshold(ADC16_Type *ptr, uint8_t ch, adc16_channel_threshold_t *config);

#if defined (ADC_SOC_BUSMODE_ENABLE_CTRL_SUPPORT) && ADC_SOC_BUSMODE_ENABLE_CTRL_SUPPORT
/**
 * @brief Enable oneshot mode (bus mode)
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
void adc16_enable_oneshot_mode(ADC16_Type *ptr);

/**
 * @brief Disable oneshot mode (bus mode)
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
void adc16_disable_oneshot_mode(ADC16_Type *ptr);
#endif

/**
 * @brief Configure the the period mode for an ADC16 instance.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to the configuration struct of @ref adc16_prd_config_t.
 * @return A result of configuring the the period mode for an ADC16 instance.
 * @retval status_success Configure the the period mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Configure the the period mode unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_set_prd_config(ADC16_Type *ptr, adc16_prd_config_t *config);

/**
 * @brief Configure the sequence mode for an ADC16 instance.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to configuration struct of @ref adc16_seq_config_t.
 * @return A result of configuring the sequence mode for an ADC16 instance.
 * @retval status_success Configure the sequence mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Configure the sequence mode unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_set_seq_config(ADC16_Type *ptr, adc16_seq_config_t *config);

/**
 * @brief Configure the preemption mode for an ADC16 instance.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to configuration struct of @ref adc16_pmt_config_t.
 * @return A result of configuring the preemption mode for an ADC16 instance.
 * @retval status_success Configure the preemption mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Configure the preemption mode unsuccessfully due to passing one or more invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_set_pmt_config(ADC16_Type *ptr, adc16_pmt_config_t *config);

/**
 * @brief Set the queue enable control.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] trig_ch An ADC16 peripheral trigger channel.
 * @param[in] enable Set true to enable and false to disable.
 * @return A result of setting queue enable in preemption.
 * @retval status_success Get the result of an ADC16 conversion in oneshot mode successfully.
 * @retval status_invalid_argument Get the result of an ADC16 conversion in oneshot mode unsuccessfully due to passing invalid arguments.
 * @deprecated This API will be deleted from V2.0.x
 */
hpm_stat_t adc16_set_pmt_queue_enable(ADC16_Type *ptr, uint8_t trig_ch, bool enable);

/**
 * @brief Enable the specified preemption queue.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] trig_ch An ADC16 peripheral trigger channel.
 * @return A result of setting queue enable in preemption.
 * @retval status_success Get the result of an ADC16 conversion in oneshot mode successfully.
 * @retval status_invalid_argument Get the result of an ADC16 conversion in oneshot mode unsuccessfully due to passing invalid arguments.
 */
hpm_stat_t adc16_enable_pmt_queue(ADC16_Type *ptr, uint8_t trig_ch);

/**
 * @brief Disable the specified preemption queue.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] trig_ch An ADC16 peripheral trigger channel.
 * @return A result of setting queue enable in preemption.
 * @retval status_success Get the result of an ADC16 conversion in oneshot mode successfully.
 * @retval status_invalid_argument Get the result of an ADC16 conversion in oneshot mode unsuccessfully due to passing invalid arguments.
 */
hpm_stat_t adc16_disable_pmt_queue(ADC16_Type *ptr, uint8_t trig_ch);

#if defined(HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE) && HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE
/**
 * @brief Enable an ADC in differential mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to a specified ADC16 differential configuration struct of @ref adc16_diff_config_t.
 */
void adc16_enable_diff_mode(ADC16_Type *ptr, adc16_diff_config_t *config);

/**
 * @brief Disable an ADC in differential mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
void adc16_disable_diff_mode(ADC16_Type *ptr);
#endif

/** @} */

/**
 * @name Enablement Control
 * @{
 */
/**
 * @brief Enable the hw trigger control for the sequence mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 *
 */
static inline void adc16_seq_enable_hw_trigger(ADC16_Type *ptr)
{
    ptr->SEQ_CFG0 |= ADC16_SEQ_CFG0_HW_TRIG_EN_MASK;
}
/**
 * @brief Disable the hw trigger control for the sequence mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 *
 */
static inline void adc16_seq_disable_hw_trigger(ADC16_Type *ptr)
{
    ptr->SEQ_CFG0 &= ~ADC16_SEQ_CFG0_HW_TRIG_EN_MASK;
}
/** @} */

/**
 * @name DMA Control
 * @{
 */

/**
 * @brief Configure the stop position offset in the specified memory of DMA write operation for the sequence mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] stop_pos A stop position offset.
 */
static inline void adc16_set_seq_stop_pos(ADC16_Type *ptr, uint16_t stop_pos)
{
    ptr->SEQ_DMA_CFG = (ptr->SEQ_DMA_CFG & ~ADC16_SEQ_DMA_CFG_STOP_POS_MASK)
                     | ADC16_SEQ_DMA_CFG_STOP_POS_SET(stop_pos);
}

/**
 * @brief Configure the start address of DMA write operation for the preemption mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] addr A start address of DMA write operation.
 */
static inline void adc16_init_pmt_dma(ADC16_Type *ptr, uint32_t addr)
{
    ptr->TRG_DMA_ADDR = addr & ADC16_TRG_DMA_ADDR_TRG_DMA_ADDR_MASK;
}

/**
 * @brief Configure sequence-mode DMA (start address and @c BUF_LEN / high length if present).
 *        Use @c buff_len_in_2bytes when @c DMA_SEQ16BIT is set, else @c buff_len_in_4bytes (same union storage, @ref adc16_dma_config_t).
 *        Clears the buffer first: @c buff_len_in_2bytes*2 bytes in the 16b path, @c buff_len_in_4bytes*4 bytes in the 32b packed path.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] config A pointer to configuration struct of @ref adc16_dma_config_t.
 * @return An implementation result of DMA initializing for the sequence mode
 * @retval status_success ADC16 initialize in sequence mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument ADC16 initialize in sequence mode unsuccessfully due to passing invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_init_seq_dma(ADC16_Type *ptr, adc16_dma_config_t *config);

#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
/**
 * @brief Enable 16-bit DMA data width in sequence mode (DMA_SEQ16BIT in ADC_CFG0).
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
static inline void adc16_enable_seq_dma_16bit(ADC16_Type *ptr)
{
    ptr->ADC_CFG0 |= ADC16_ADC_CFG0_DMA_SEQ16BIT_MASK;
}

/**
 * @brief Disable 16-bit DMA data width in sequence mode (use default width).
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
static inline void adc16_disable_seq_dma_16bit(ADC16_Type *ptr)
{
    ptr->ADC_CFG0 &= ~ADC16_ADC_CFG0_DMA_SEQ16BIT_MASK;
}
#endif

/**
 * @brief Return whether 16-bit sequence-mode DMA is enabled (DMA_SEQ16BIT in ADC_CFG0).
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @retval true if DMA_SEQ16BIT is set on IPs that support it; always false otherwise.
 */
static inline bool adc16_is_seq_dma_16bit_enabled(ADC16_Type *ptr)
{
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
    return ADC16_ADC_CFG0_DMA_SEQ16BIT_GET(ptr->ADC_CFG0) != 0U;
#else
    (void) ptr;
    return false;
#endif
}

/** @} */

/**
 * @name Status
 * @{
 */

/**
 * @brief Get all ADC16 status flags.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @return A mask indicating all corresponding interrupt statuses.
 * @retval A mask. Please refer to @ref adc16_irq_event_t.
 */
static inline uint32_t adc16_get_status_flags(ADC16_Type *ptr)
{
    return ptr->INT_STS;
}

/**
 * @brief Set value of the WAIT_DIS bit. The ADC does not block access to the associated peripheral bus
 * until the ADC has completed its conversion.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @deprecated This API will be deleted from V2.0.x
 */
static inline void adc16_disable_busywait(ADC16_Type *ptr)
{
    ptr->BUF_CFG0 |= ADC16_BUF_CFG0_WAIT_DIS_SET(1);
}

/**
 * @brief Set value of the WAIT_DIS bit. ADC blocks access to the associated peripheral bus
 * until the ADC completes the conversion.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @deprecated This API will be deleted from V2.0.x
 */
static inline void adc16_enable_busywait(ADC16_Type *ptr)
{
    ptr->BUF_CFG0 &= ~ADC16_BUF_CFG0_WAIT_DIS_MASK;
}

/**
 * @brief Set nonblocking read in oneshot mode.
 * @note An ADC does not block access to the associated peripheral whether it completes a conversion or not.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
static inline void adc16_set_nonblocking_read(ADC16_Type *ptr)
{
    ptr->BUF_CFG0 |= ADC16_BUF_CFG0_WAIT_DIS_MASK;
}

/**
 * @brief Set blocking read in oneshot mode.
 * @note An ADC blocks access to the associated peripheral bus until it completes a conversion.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
static inline void adc16_set_blocking_read(ADC16_Type *ptr)
{
    ptr->BUF_CFG0 &= ~ADC16_BUF_CFG0_WAIT_DIS_MASK;
}

/**
 * @brief Judge whether the current setting is none-blocking mode or not.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @return A result indicating the status of bus waiting.
 * @retval True means that nonblocking reading.
 * @retval False means that blocking reading.
 *
 */
static inline bool adc16_is_nonblocking_mode(ADC16_Type *ptr)
{
    return (ADC16_BUF_CFG0_WAIT_DIS_GET(ptr->BUF_CFG0)  ? true : false);
}

/**
 * @brief Get the status of a conversion validity.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 peripheral channel.
 * @return Status indicating the validity of the current conversion result.
 *
 * @note This function is only used when the WAIT_DIS bit in the BUF_RESULT register is 1.
 */
static inline bool adc16_get_conv_valid_status(ADC16_Type *ptr, uint8_t ch)
{
    return ADC16_BUS_RESULT_VALID_GET(ptr->BUS_RESULT[ch]);
}

/**
 * @brief Clear the status flags.
 *
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] mask A mask that means the specified flags to be cleared. Please refer to @ref adc16_irq_event_t.
 *
 * @note Only the specified flags can be cleared by writing the INT_STS register.
 */
static inline void adc16_clear_status_flags(ADC16_Type *ptr, uint32_t mask)
{
    ptr->INT_STS = mask;
}

/** @} */

/**
 * @name Interrupts
 * @{
 */

/**
 * @brief Enable interrupts.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] mask A mask indicating the specified ADC interrupt events. Please refer to @ref adc16_irq_event_t.
 */
static inline void adc16_enable_interrupts(ADC16_Type *ptr, uint32_t mask)
{
    ptr->INT_EN |= mask;
}

/**
 * @brief Disable interrupts.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] mask A mask indicating the specified interrupt events. Please refer to @ref adc16_irq_event_t.
 */
static inline void adc16_disable_interrupts(ADC16_Type *ptr, uint32_t mask)
{
    ptr->INT_EN &= ~mask;
}

/**
 * @brief Enable watchdog interrupt after a valid in-window conversion has completed (per User Manual/silicon).
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] wdog_ch_mask Bit mask of physical channel indices (bits 0..15); each bit is INT_STS/INT_EN WDOG for that channel.
 *
 * @note Call only after the corresponding channel(s) have completed a conversion with the result
 *       inside the programmed threshold window; otherwise RW1C clear of WDOG in INT_STS may not
 *       clear the latched state. Typical use: one call per channel bit after priming conversions.
 */
static inline void adc16_enable_wdog_interrupt(ADC16_Type *ptr, uint32_t wdog_ch_mask)
{
    uint32_t m = wdog_ch_mask & ADC16_INT_STS_WDOG_MASK;

    ptr->INT_STS = m;
    ptr->INT_EN |= m;
}

/** @} */

/**
 * @name Trigger and Conversion
 * @{
 */

/**
 * @brief Trigger ADC conversions by software in sequence mode
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @return An implementation result of getting an ADC16 software trigger.
 * @retval status_success ADC16 software triggers successfully. Please refer to @ref hpm_stat_t.
 * @retval status_fail ADC16 software triggers unsuccessfully. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_trigger_seq_by_sw(ADC16_Type *ptr);

/**
 * @brief Trigger ADC conversions by software in preemption mode
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] trig_ch A trigger channel number(e.g. TRIG0A,TRIG0B,TRIG0C...).
 * @return An implementation result of getting an ADC16 software trigger.
 * @retval status_success ADC16 software triggers successfully. Please refer to @ref hpm_stat_t.
 * @retval status_fail ADC16 software triggers unsuccessfully. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_trigger_pmt_by_sw(ADC16_Type *ptr, uint8_t trig_ch);

/**
 * @brief Get the result in oneshot mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 peripheral channel.
 * @param[out] result A pointer to an ADC16 conversion result.
 * @return An implementation result of getting an ADC16 conversion result in oneshot mode.
 * @retval status_success Get the result of an ADC16 conversion in oneshot mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Get the result of an ADC16 conversion in oneshot mode unsuccessfully due to passing invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_get_oneshot_result(ADC16_Type *ptr, uint8_t ch, uint16_t *result);

/**
 * @brief Get the result in the period mode.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 * @param[in] ch An ADC16 peripheral channel.
 * @param[out] result A pointer to a specified ADC16 conversion result
 * @return An implementation of getting an ADC16 conversion result in the period mode.
 * @retval status_success Get the result of an ADC16 conversion in the period mode successfully. Please refer to @ref hpm_stat_t.
 * @retval status_invalid_argument Get the result of an ADC16 conversion in the period mode unsuccessfully due to passing invalid arguments. Please refer to @ref hpm_stat_t.
 */
hpm_stat_t adc16_get_prd_result(ADC16_Type *ptr, uint8_t ch, uint16_t *result);

#if defined(ADC16_SOC_TEMP_CH_EN) && ADC16_SOC_TEMP_CH_EN
/**
 * @brief Enable the temperature sensor
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
void adc16_enable_temp_sensor(ADC16_Type *ptr);

/**
 * @brief Disable the temperature sensor
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
void adc16_disable_temp_sensor(ADC16_Type *ptr);
#endif

/**
 * @brief enable the transmission of adc data to the motor sensor unit.
 *
 * @param[in] ptr An ADC16 peripheral base address.
 */
#if defined(HPM_IP_FEATURE_ADC16_HAS_MOT_EN) && HPM_IP_FEATURE_ADC16_HAS_MOT_EN
static inline void adc16_enable_motor(ADC16_Type *ptr)
{
    ptr->ANA_CTRL0 |= ADC16_ANA_CTRL0_MOTO_EN_MASK;
}
#endif

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HPM_ADC16_DRV_H */
