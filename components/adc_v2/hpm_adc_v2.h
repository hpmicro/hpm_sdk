/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_ADC_V2_H
#define HPM_ADC_V2_H

#include "hpm_common.h"
#include "hpm_soc_feature.h"

/**
 * @name ADC v2 compile-time capability
 * @brief Build-time flags for ADC12/ADC16 presence and optional ADC16 IP features.
 * Use these macros to gate IP-specific application code. Struct fields and APIs that
 * depend on a capability are ignored or return @c status_invalid_argument when it is 0.
 * Runtime IP type for a given instance comes from @ref hpm_adc_v2_get_ip_type.
 * @{
 */

/** Set when the SoC builds against the HPMSDK ADC12 driver. */
#if defined(HPMSOC_HAS_HPMSDK_ADC12)
#define HPM_ADC_V2_HAS_ADC12 1
#else
#define HPM_ADC_V2_HAS_ADC12 0
#endif

/** Set when the SoC builds against the HPMSDK ADC16 driver. */
#if defined(HPMSOC_HAS_HPMSDK_ADC16)
#define HPM_ADC_V2_HAS_ADC16 1
#else
#define HPM_ADC_V2_HAS_ADC16 0
#endif

/** Set when both ADC12 and ADC16 are present; IP is selected from handle base address. */
#if (HPM_ADC_V2_HAS_ADC12 && HPM_ADC_V2_HAS_ADC16)
#define HPM_ADC_V2_HAS_DUAL_IP 1
#else
#define HPM_ADC_V2_HAS_DUAL_IP 0
#endif

/** Maps ADC12 @c diff_sel to @ref adc_v2_signal_mode_t; 0 when the SoC has no ADC12. */
#if HPM_ADC_V2_HAS_ADC12
#define HPM_ADC_V2_HAS_SIGNAL_MODE 1
#else
#define HPM_ADC_V2_HAS_SIGNAL_MODE 0
#endif

/** ADC16 differential-mode support; requires @c HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE. */
#if defined(HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE) && HPM_IP_FEATURE_ADC16_HAS_DIFF_MODE
#define HPM_ADC_V2_HAS_DIFF_MODE 1
#else
#define HPM_ADC_V2_HAS_DIFF_MODE 0
#endif

/** ADC16 channel oversampling loop; requires @c HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP. */
#if defined(HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP) && HPM_IP_FEATURE_ADC16_HAS_ADC_LOOP
#define HPM_ADC_V2_HAS_ADC_LOOP 1
#else
#define HPM_ADC_V2_HAS_ADC_LOOP 0
#endif

/** ADC16 sequence DMA 16-bit-only output; requires @c HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT. */
#if defined(HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT) && HPM_IP_FEATURE_ADC16_HAS_DMA_SEQ16BIT
#define HPM_ADC_V2_HAS_DMA_SEQ16BIT 1
#else
#define HPM_ADC_V2_HAS_DMA_SEQ16BIT 0
#endif

/** ADC16 INT_STS STOP_POS IRQ only; does not gate @ref hpm_adc_v2_set_seq_stop_pos or @c stop_en/@c stop_pos in @ref adc_v2_dma_config_t. */
#if defined(HPM_IP_FEATURE_ADC16_SEQ_STOP_POS_INT) && HPM_IP_FEATURE_ADC16_SEQ_STOP_POS_INT
#define HPM_ADC_V2_HAS_SEQ_STOP_POS_INT 1
#else
#define HPM_ADC_V2_HAS_SEQ_STOP_POS_INT 0
#endif

/** ADC16 motor-control data path (@ref hpm_adc_v2_enable_motor_mode); gate calls in app code. */
#if defined(HPM_IP_FEATURE_ADC16_HAS_MOT_EN) && HPM_IP_FEATURE_ADC16_HAS_MOT_EN
#define HPM_ADC_V2_HAS_MOTOR_MODE 1
#else
#define HPM_ADC_V2_HAS_MOTOR_MODE 0
#endif

/**
 * @name ADC v2 event capability (for application code)
 * @brief Compile-time checks before using optional @ref adc_v2_event_t bits.
 * @{
 * - Common events (@ref adc_v2_event_trig_complete through @ref adc_v2_event_dma_fifo_full):
 *   INT_STS masks shared by ADC12 and ADC16; safe on either handle IP.
 * - @ref adc_v2_event_seq_stop_pos: ADC16 IRQ bit only; use when @ref HPM_ADC_V2_HAS_SEQ_STOP_POS_INT is 1.
 *   @ref hpm_adc_v2_set_seq_stop_pos and sequence DMA @c stop_en/@c stop_pos work without this macro.
 *   When the macro is 0, @ref hpm_adc_v2_enable_interrupts drops @ref adc_v2_event_seq_stop_pos silently.
 * - @ref hpm_adc_v2_enable_motor_mode: present only when @ref HPM_ADC_V2_HAS_MOTOR_MODE is 1.
 * - Which events actually fire still depends on @ref adc_v2_conv_mode_t (preempt vs sequence).
 * @}
 */

/** @} */

/**
 * @brief Unified ADC HAL over ADC12 and ADC16.
 *
 * Replaces per-IP @c adc12_* / @c adc16_* branching in application code. IP type is detected from
 * @ref HPM_ADC_V2_HANDLE base address (@ref hpm_adc_v2_get_ip_type). Prefer this header over
 * deprecated @ref hpm_adc_interface.
 *
 * Typical preempt (motor FOC) flow: @ref hpm_adc_v2_get_default_config → @ref hpm_adc_v2_init →
 * @ref hpm_adc_v2_init_channel → @ref hpm_adc_v2_set_preempt_config → @ref hpm_adc_v2_init_pmt_dma →
 * @ref hpm_adc_v2_enable_interrupts. Example: @c samples/motor_ctrl/bldc_foc.
 *
 * @note @ref adc_v2_event_t values match INT_STS / @c adc12_event_* / @c adc16_event_* register masks.
 *       Use them with @ref hpm_adc_v2_get_status_flags, @ref hpm_adc_v2_enable_interrupts, and
 *       @ref hpm_adc_v2_clear_status_flags. ADC16 packed
 *       DMA word layout (@c adc16_seq_dma_data_t / @c adc16_pmt_dma_data_t) depends on
 *       @c ADC_SOC_IP_VERSION in @ref hpm_adc16_drv.h; use @ref hpm_adc_v2_parse_pmt_dma_word and
 *       @ref hpm_adc_v2_parse_seq_dma_word instead of fixed bit masks.
 *
 * @defgroup hpm_adc_v2_interface HPM ADC v2 driver APIs
 * @ingroup hpm_adc_interfaces
 * @{
 */

/** @brief ADC peripheral IP type detected from base address. */
typedef enum {
    adc_v2_ip_unknown = 0, /**< Unrecognized @c base; most APIs return @c status_invalid_argument. */
    adc_v2_ip_adc12,
    adc_v2_ip_adc16,
} adc_v2_ip_t;

/**
 * @brief ADC instance handle; @c base stores the peripheral base address.
 * @note Stateless identifier passed by value; construct locally with @ref HPM_ADC_V2_HANDLE.
 */
typedef struct {
    uint32_t base;
} adc_v2_handle_t;

/** @brief Create an ADC v2 handle from a peripheral base pointer or address. */
#define HPM_ADC_V2_HANDLE(adc_base) \
    ((adc_v2_handle_t){ .base = (uint32_t)(adc_base) })

/** @brief Semantic ADC resolution in bits (@ref adc_v2_resolution_bits_t). */
typedef enum {
    adc_v2_resolution_6bit = 6,   /**< Valid for ADC12 only. */
    adc_v2_resolution_8bit = 8,
    adc_v2_resolution_10bit = 10,
    adc_v2_resolution_12bit = 12,
    adc_v2_resolution_16bit = 16,   /**< Valid for ADC16 only. */
} adc_v2_resolution_bits_t;

/**
 * @brief Semantic ADC conversion mode (@c conv_mode in @ref adc_v2_config_t).
 * @note Enum values match underlying @c adc12_conversion_mode_t / @c adc16_conversion_mode_t.
 */
typedef enum {
    adc_v2_conv_mode_oneshot = 0,    /**< @ref hpm_adc_v2_get_oneshot_result; no preempt/seq events. */
    adc_v2_conv_mode_period,         /**< @ref hpm_adc_v2_get_prd_result and period config. */
    adc_v2_conv_mode_sequence,       /**< Sequence config/DMA; seq/DMA @ref adc_v2_event_t flags. */
    adc_v2_conv_mode_preemption,     /**< Preempt config/DMA; preempt @ref adc_v2_event_t flags; motor FOC. */
} adc_v2_conv_mode_t;

/** @brief Semantic ADC input signal mode (maps to ADC12 @c diff_sel). */
typedef enum {
    adc_v2_signal_mode_single_ended = 0,
    adc_v2_signal_mode_differential,
} adc_v2_signal_mode_t;

#if HPM_ADC_V2_HAS_ADC12
#include "hpm_adc12_drv.h"
#endif
#if HPM_ADC_V2_HAS_ADC16
#include "hpm_adc16_drv.h"
#endif

/**
 * @brief Unified ADC interrupt / status events (INT_STS register bit masks).
 * @note Values match @c adc12_event_* / @c adc16_event_* and follow SoC @c ADCxx_INT_STS_* macros.
 *       Combine with @c | and test with @c & against @ref hpm_adc_v2_get_status_flags.
 * @note @ref hpm_adc_v2_enable_interrupts and @ref hpm_adc_v2_clear_status_flags drop bits unsupported
 *       on the handle IP. @ref adc_v2_event_seq_stop_pos (IRQ only) requires @ref HPM_ADC_V2_HAS_SEQ_STOP_POS_INT;
 *       sequence stop position register/DMA config is independent of that macro.
 * @note Firing depends on conversion mode: preempt vs sequence/DMA event groups below.
 */
typedef enum {
#if HPM_ADC_V2_HAS_ADC12
    adc_v2_event_trig_complete       = (uint32_t) adc12_event_trig_complete,       /**< Preempt trigger complete. */
    adc_v2_event_trig_sw_conflict    = (uint32_t) adc12_event_trig_sw_conflict,    /**< Preempt SW trigger conflict. */
    adc_v2_event_trig_hw_conflict    = (uint32_t) adc12_event_trig_hw_conflict,    /**< Preempt HW trigger conflict. */
    adc_v2_event_read_conflict       = (uint32_t) adc12_event_read_conflict,       /**< Bus read conflict. */
    adc_v2_event_seq_sw_conflict     = (uint32_t) adc12_event_seq_sw_conflict,     /**< Sequence SW trigger conflict. */
    adc_v2_event_seq_hw_conflict     = (uint32_t) adc12_event_seq_hw_conflict,     /**< Sequence HW trigger conflict. */
    adc_v2_event_seq_dma_abort       = (uint32_t) adc12_event_seq_dma_abort,       /**< Sequence DMA abort. */
    adc_v2_event_seq_full_complete   = (uint32_t) adc12_event_seq_full_complete,   /**< Full sequence complete. */
    adc_v2_event_seq_single_complete = (uint32_t) adc12_event_seq_single_complete, /**< One sequence entry complete. */
    adc_v2_event_dma_fifo_full       = (uint32_t) adc12_event_dma_fifo_full,       /**< DMA FIFO full. */
#elif HPM_ADC_V2_HAS_ADC16
    adc_v2_event_trig_complete       = (uint32_t) adc16_event_trig_complete,
    adc_v2_event_trig_sw_conflict    = (uint32_t) adc16_event_trig_sw_conflict,
    adc_v2_event_trig_hw_conflict    = (uint32_t) adc16_event_trig_hw_conflict,
    adc_v2_event_read_conflict       = (uint32_t) adc16_event_read_conflict,
    adc_v2_event_seq_sw_conflict     = (uint32_t) adc16_event_seq_sw_conflict,
    adc_v2_event_seq_hw_conflict     = (uint32_t) adc16_event_seq_hw_conflict,
    adc_v2_event_seq_dma_abort       = (uint32_t) adc16_event_seq_dma_abort,
    adc_v2_event_seq_full_complete   = (uint32_t) adc16_event_seq_full_complete,
    adc_v2_event_seq_single_complete = (uint32_t) adc16_event_seq_single_complete,
    adc_v2_event_dma_fifo_full       = (uint32_t) adc16_event_dma_fifo_full,
#endif
#if HPM_ADC_V2_HAS_SEQ_STOP_POS_INT
    adc_v2_event_seq_stop_pos        = (uint32_t) adc16_event_seq_stop_pos,        /**< ADC16 INT_STS STOP_POS IRQ. */
#endif
} adc_v2_event_t;

/**
 * @brief Common ADC instance configuration (semantic fields).
 * @note Fields apply per handle IP from @ref hpm_adc_v2_get_ip_type. Unused fields
 *       are ignored. Invalid @c resolution_bits for the IP returns @c status_invalid_argument.
 */
typedef struct {
    uint8_t resolution_bits; /**< ADC12: 6/8/10/12; ADC16: 8/10/12/16 (@ref adc_v2_resolution_bits_t). */
    uint8_t conv_mode;       /**< @ref adc_v2_conv_mode_t; both IPs. */
    uint8_t clock_div;       /**< ADC clock divider; both IPs. */
    uint8_t signal_mode;     /**< ADC12 only (@ref adc_v2_signal_mode_t); ignored on ADC16. */
    bool wait_dis;           /**< Non-blocking bus read; both IPs. */
    bool sel_sync_ahb;       /**< Both IPs when supported; may be forced on some ADC16 SoCs. */
    bool port3_realtime;     /**< ADC16 only; ignored on ADC12. */
    bool dma_seq16bit;       /**< ADC16 only when @ref HPM_ADC_V2_HAS_DMA_SEQ16BIT; ignored otherwise. */
} adc_v2_config_t;

/**
 * @brief ADC channel configuration.
 * @note IP-specific fields are ignored when not applicable to the handle IP. See @c adc12_channel_config_t
 *       / @c adc16_channel_config_t in driver headers for field semantics.
 */
typedef struct {
    uint8_t ch;                   /**< Channel index. */
    uint8_t signal_mode;          /**< ADC12 only (@ref adc_v2_signal_mode_t); ignored on ADC16. */
    uint16_t thshdh;              /**< Watchdog upper threshold. */
    uint16_t thshdl;              /**< Watchdog lower threshold. */
    bool wdog_int_en;             /**< Enable watchdog interrupt for this channel. */
    uint8_t sample_cycle_shift;   /**< Sample-time shift (see driver header). */
    uint32_t sample_cycle;        /**< Sample cycles for this channel. */
    uint8_t adc_loop_exp;         /**< ADC16 only when @ref HPM_ADC_V2_HAS_ADC_LOOP; ignored otherwise. */
} adc_v2_channel_config_t;

/** @brief ADC channel threshold. */
typedef struct {
    uint8_t ch;
    uint16_t thshdh;
    uint16_t thshdl;
} adc_v2_channel_threshold_t;

/**
 * @brief ADC DMA configuration for sequence mode.
 * @note @c buff_len_in_2bytes is used on ADC16 when @ref HPM_ADC_V2_HAS_DMA_SEQ16BIT and
 *       instance @c dma_seq16bit is enabled; otherwise use @c buff_len_in_4bytes.
 */
typedef struct {
    uint32_t *start_addr;          /**< System-visible DMA buffer address; both IPs. */
    uint32_t buff_len_in_4bytes;   /**< Buffer length in 32-bit words; both IPs. */
    uint32_t buff_len_in_2bytes;   /**< ADC16 16-bit-only DMA length; ignored on ADC12. */
    uint32_t stop_pos;             /**< Sequence DMA stop index; both IPs; independent of @ref HPM_ADC_V2_HAS_SEQ_STOP_POS_INT. */
    bool stop_en;                  /**< Enable stop at @c stop_pos; both IPs. */
} adc_v2_dma_config_t;

/** @brief ADC period-mode configuration (maps to @c adc12_prd_config_t / @c adc16_prd_config_t). */
typedef struct {
    uint8_t ch;            /**< Channel for period conversion. */
    uint8_t prescale;      /**< Prescaler: 0 = 1×PRD, 1 = 2×PRD, 2 = 4×PRD. */
    uint8_t period_count;  /**< Reload value = actual period − 1. */
} adc_v2_prd_config_t;

/** @brief ADC sequence queue entry. */
typedef struct {
    bool seq_int_en;
    uint8_t ch;
} adc_v2_seq_queue_config_t;

/**
 * @brief ADC sequence-mode configuration.
 * @note @c queue[0..seq_len-1] defines channel order; @c restart_en/@c cont_en control wrap/continuous run.
 */
typedef struct {
    adc_v2_seq_queue_config_t queue[ADC_SOC_SEQ_MAX_LEN];
    bool restart_en;   /**< Restart queue after completion. */
    bool cont_en;      /**< Continuous sequence mode. */
    bool sw_trig_en;   /**< Allow @ref hpm_adc_v2_trigger_seq_by_sw. */
    bool hw_trig_en;   /**< Allow hardware trigger. */
    uint8_t seq_len;   /**< Active queue length (1..@c ADC_SOC_SEQ_MAX_LEN). */
} adc_v2_seq_config_t;

/**
 * @brief ADC preemption-mode trigger configuration.
 * @note @c inten[i]/@c adc_ch[i] pair trigger slot @c i; @c trig_ch selects active HW trigger source.
 */
typedef struct {
    bool inten[ADC_SOC_MAX_TRIG_CH_LEN];   /**< Per-slot preempt-complete interrupt enable. */
    uint8_t adc_ch[ADC_SOC_MAX_TRIG_CH_LEN]; /**< ADC channel for each trigger slot. */
    uint8_t trig_ch;   /**< Hardware trigger channel index. */
    uint8_t trig_len;  /**< Number of active trigger slots (see driver header). */
} adc_v2_pmt_config_t;

/** @brief Alias for preemption-mode configuration (motor samples). */
typedef adc_v2_pmt_config_t adc_v2_preempt_config_t;

/** @brief Cross-IP trigger-complete bit for motor preempt ISR (ADC12 and ADC16). */
#define HPM_ADC_V2_EVENT_TRIG_COMPLETE adc_v2_event_trig_complete

#if HPM_ADC_V2_HAS_DIFF_MODE
/** @brief ADC16 differential position mode (maps to @c adc16_diff_pos_mode_t). */
typedef enum {
    adc_v2_diff_pos_mode_differential = 0,
    adc_v2_diff_pos_mode_single_ended,
} adc_v2_diff_pos_mode_t;

/**
 * @brief ADC16 differential-mode configuration.
 * @note Present only when @ref HPM_ADC_V2_HAS_DIFF_MODE. ADC12 uses @c signal_mode in
 *       @ref adc_v2_config_t / @ref adc_v2_channel_config_t instead.
 */
typedef struct {
    uint8_t full_resolution;
    uint8_t position_mode; /**< @ref adc_v2_diff_pos_mode_t */
    bool master;
} adc_v2_diff_config_t;
#endif

/**
 * @brief Parsed DMA sample from a 32-bit DMA word.
 * @note Output is IP-agnostic. Packed-word decoding in the port layer follows @c adc12_*_dma_data_t or
 *       @c adc16_*_dma_data_t from the HPMSDK driver headers. On ADC16, @c adc_ch and @c trig_ch bit
 *       positions differ between @c ADC_SOC_IP_VERSION v1 and v2; use the parse APIs instead of manual shifts.
 *       When @c is_raw_16bit is true the source is ADC16 sequence DMA with @c dma_seq16bit enabled:
 *       @p raw carries one 16-bit conversion only and @c adc_ch / @c seq_num / @c trig_ch / @c cycle_bit
 *       are not present in the buffer entry.
 */
typedef struct {
    uint16_t result;
    uint8_t seq_num;
    uint8_t adc_ch;
    uint8_t trig_ch;
    bool cycle_bit;
    bool is_raw_16bit; /**< true for ADC16 @c dma_seq16bit sequence DMA halfword entries. */
} adc_v2_dma_sample_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Detect ADC IP type from handle base address.
 * @note Matches @c base against SoC @c HPM_ADCx_BASE macros; returns @ref adc_v2_ip_unknown
 *       for NULL/zero or non-ADC peripheral addresses. On dual-IP SoCs
 *       (@ref HPM_ADC_V2_HAS_DUAL_IP), @c HPM_ADC3_BASE maps to ADC16; other valid ADC
 *       bases map to ADC12.
 *
 * @param[in] handle ADC instance handle.
 * @return Detected IP type.
 */
adc_v2_ip_t hpm_adc_v2_get_ip_type(adc_v2_handle_t handle);

/**
 * @brief Fill @p config with default values for the handle IP.
 * @note IP-specific fields are filled with hardware defaults; unused fields are cleared.
 *       No-op when @p config is NULL.
 *
 * @param[in] handle ADC instance handle.
 * @param[out] config Configuration to initialize.
 */
void hpm_adc_v2_get_default_config(adc_v2_handle_t handle, adc_v2_config_t *config);

/**
 * @brief Fill channel configuration with defaults for the handle IP.
 * @note IP-specific fields are filled with hardware defaults; unused fields are cleared.
 *       No-op when @p config is NULL.
 *
 * @param[in] handle ADC instance handle.
 * @param[out] config Channel configuration to initialize.
 */
void hpm_adc_v2_get_channel_default_config(adc_v2_handle_t handle, adc_v2_channel_config_t *config);

/**
 * @brief Initialize an ADC instance.
 * @note IP-specific fields in @p config that do not apply to the handle IP are ignored.
 *       Invalid @c resolution_bits returns @c status_invalid_argument.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config Configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_init(adc_v2_handle_t handle, adc_v2_config_t *config);

/**
 * @brief De-initialize an ADC instance.
 *
 * @param[in] handle ADC instance handle.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_deinit(adc_v2_handle_t handle);

/**
 * @brief Initialize an ADC channel.
 * @note IP-specific fields in @p config that do not apply to the handle IP are ignored.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config Channel configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_init_channel(adc_v2_handle_t handle, adc_v2_channel_config_t *config);

/**
 * @brief Get channel threshold values.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] ch Channel number.
 * @param[out] config Threshold output.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_get_channel_threshold(adc_v2_handle_t handle, uint8_t ch, adc_v2_channel_threshold_t *config);

/**
 * @brief Configure period mode.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config Period configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_set_period_config(adc_v2_handle_t handle, adc_v2_prd_config_t *config);

/**
 * @brief Configure sequence mode.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config Sequence configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_set_sequence_config(adc_v2_handle_t handle, adc_v2_seq_config_t *config);

/**
 * @brief Configure preemption mode.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config Preemption configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_set_preempt_config(adc_v2_handle_t handle, adc_v2_pmt_config_t *config);

/**
 * @brief Set sequence DMA stop position offset (SEQ_DMA_CFG STOP_POS).
 * @note Available on ADC12 and ADC16 regardless of @ref HPM_ADC_V2_HAS_SEQ_STOP_POS_INT.
 *       That macro gates only @ref adc_v2_event_seq_stop_pos IRQ notification, not this register write.
 *       Same field can also be set via @c stop_pos in @ref hpm_adc_v2_init_seq_dma.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] stop_pos Stop position offset.
 */
void hpm_adc_v2_set_seq_stop_pos(adc_v2_handle_t handle, uint16_t stop_pos);

/**
 * @brief Configure preemption-mode DMA start address.
 * @note @p addr must be a system-visible address (e.g. from @c core_local_mem_to_sys_address on multi-core SoCs).
 *
 * @param[in] handle ADC instance handle.
 * @param[in] addr DMA buffer start address.
 */
void hpm_adc_v2_init_pmt_dma(adc_v2_handle_t handle, uint32_t addr);

/**
 * @brief Configure sequence-mode DMA.
 * @note @p config->start_addr must be system-visible (same as @ref hpm_adc_v2_init_pmt_dma).
 *       On ADC16 with @c dma_seq16bit enabled, set @c buff_len_in_2bytes in @p config;
 *       on ADC12 only @c buff_len_in_4bytes is used.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] config DMA configuration pointer.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_init_seq_dma(adc_v2_handle_t handle, adc_v2_dma_config_t *config);

/**
 * @brief Return whether sequence-mode DMA outputs raw 16-bit halfwords.
 * @note Reflects the live @c DMA_SEQ16BIT setting on ADC16 when @ref HPM_ADC_V2_HAS_DMA_SEQ16BIT;
 *       always false on ADC12. Use before indexing a sequence DMA buffer as @c uint16_t vs @c uint32_t.
 *
 * @param[in] handle ADC instance handle.
 * @return true when sequence DMA entries are raw 16-bit results only.
 */
bool hpm_adc_v2_is_seq_dma_16bit_enabled(adc_v2_handle_t handle);

/**
 * @brief Enable non-blocking bus read (WAIT_DIS).
 *
 * @param[in] handle ADC instance handle.
 */
void hpm_adc_v2_set_nonblocking_read(adc_v2_handle_t handle);

/**
 * @brief Enable blocking bus read (clear WAIT_DIS).
 *
 * @param[in] handle ADC instance handle.
 */
void hpm_adc_v2_set_blocking_read(adc_v2_handle_t handle);

/**
 * @brief Return whether non-blocking read is enabled.
 *
 * @param[in] handle ADC instance handle.
 * @return true if non-blocking read is active.
 */
bool hpm_adc_v2_is_nonblocking_mode(adc_v2_handle_t handle);

/**
 * @brief Get INT_STS status / interrupt event flags.
 * @note Return value uses @ref adc_v2_event_t (INT_STS bit masks). In ISR, test with
 *       @c (status & @ref HPM_ADC_V2_EVENT_TRIG_COMPLETE) or other @ref adc_v2_event_t constants.
 *
 * @param[in] handle ADC instance handle.
 * @return Event mask (@ref adc_v2_event_t) for bits supported on the handle IP.
 */
uint32_t hpm_adc_v2_get_status_flags(adc_v2_handle_t handle);

/**
 * @brief Filter @p event_mask to bits supported on @p handle IP.
 * @deprecated Prefer @ref adc_v2_event_t constants directly; values already match INT_STS layout.
 *       Equivalent to @c event_mask & supported_mask for the handle IP.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] event_mask Event mask (@ref adc_v2_event_t).
 * @return @p event_mask with unsupported bits cleared for the handle IP.
 */
uint32_t hpm_adc_v2_get_hw_event_mask(adc_v2_handle_t handle, uint32_t event_mask);

/**
 * @brief Clear status flags using @ref adc_v2_event_t mask.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] event_mask Event mask to clear.
 */
void hpm_adc_v2_clear_status_flags(adc_v2_handle_t handle, uint32_t event_mask);

/**
 * @brief Enable interrupts using @ref adc_v2_event_t mask.
 * @note Unsupported bits for the handle IP are dropped (no error). Gate only
 *       @ref adc_v2_event_seq_stop_pos (IRQ) with @ref HPM_ADC_V2_HAS_SEQ_STOP_POS_INT; stop position
 *       configuration via @ref hpm_adc_v2_set_seq_stop_pos does not require that macro.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] event_mask Event mask.
 */
void hpm_adc_v2_enable_interrupts(adc_v2_handle_t handle, uint32_t event_mask);

/**
 * @brief Disable interrupts using @ref adc_v2_event_t mask.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] event_mask Event mask.
 */
void hpm_adc_v2_disable_interrupts(adc_v2_handle_t handle, uint32_t event_mask);

/**
 * @brief Get conversion-valid status for a channel.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] ch Channel number.
 * @return true if the current conversion result is valid.
 */
bool hpm_adc_v2_get_conv_valid_status(adc_v2_handle_t handle, uint8_t ch);

/**
 * @brief Get oneshot-mode conversion result.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] ch Channel number.
 * @param[out] result Conversion result.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_get_oneshot_result(adc_v2_handle_t handle, uint8_t ch, uint16_t *result);

/**
 * @brief Get period-mode conversion result.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] ch Channel number.
 * @param[out] result Conversion result.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_get_prd_result(adc_v2_handle_t handle, uint8_t ch, uint16_t *result);

/**
 * @brief Software-trigger sequence conversion.
 *
 * @param[in] handle ADC instance handle.
 * @return status_success, status_fail, or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_trigger_seq_by_sw(adc_v2_handle_t handle);

/**
 * @brief Software-trigger preemption conversion on a trigger channel.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] trig_ch Trigger channel index.
 * @return status_success, status_fail, or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_trigger_pmt_by_sw(adc_v2_handle_t handle, uint8_t trig_ch);

/**
 * @brief Enable a preemption queue (ADC16 only).
 * @note Returns @c status_invalid_argument on ADC12.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] trig_ch Trigger channel index.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_enable_pmt_queue(adc_v2_handle_t handle, uint8_t trig_ch);

/**
 * @brief Disable a preemption queue (ADC16 only).
 * @note Returns @c status_invalid_argument on ADC12.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] trig_ch Trigger channel index.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_disable_pmt_queue(adc_v2_handle_t handle, uint8_t trig_ch);

#if HPM_ADC_V2_HAS_DIFF_MODE
/**
 * @brief Fill differential-mode defaults for an ADC16 handle.
 * @note No-op when @p config is NULL or handle IP is not ADC16.
 *
 * @param[in] handle ADC16 instance handle.
 * @param[out] config Differential configuration to initialize.
 */
void hpm_adc_v2_get_default_diff_config(adc_v2_handle_t handle, adc_v2_diff_config_t *config);

/**
 * @brief Enable ADC16 differential mode.
 * @note Returns @c status_invalid_argument on ADC12 or when @p config is NULL.
 *
 * @param[in] handle ADC16 instance handle.
 * @param[in] config Differential configuration.
 * @return status_success or status_invalid_argument.
 */
hpm_stat_t hpm_adc_v2_enable_diff_mode(adc_v2_handle_t handle, adc_v2_diff_config_t *config);

/**
 * @brief Disable ADC16 differential mode.
 * @note No-op when handle IP is not ADC16.
 *
 * @param[in] handle ADC16 instance handle.
 */
void hpm_adc_v2_disable_diff_mode(adc_v2_handle_t handle);
#endif

#if HPM_ADC_V2_HAS_MOTOR_MODE
/**
 * @brief Enable motor-control mode (ADC16 with @c HPM_IP_FEATURE_ADC16_HAS_MOT_EN).
 * @note Call after preempt DMA setup (@ref hpm_adc_v2_init_pmt_dma) in motor FOC samples.
 *       Gate application calls with @ref HPM_ADC_V2_HAS_MOTOR_MODE.
 *
 * @param[in] handle ADC16 instance handle.
 */
void hpm_adc_v2_enable_motor_mode(adc_v2_handle_t handle);
#endif

/** @brief Alias for @ref hpm_adc_v2_init_channel. */
static inline hpm_stat_t hpm_adc_v2_channel_init(adc_v2_handle_t handle, adc_v2_channel_config_t *config)
{
    return hpm_adc_v2_init_channel(handle, config);
}

/** @brief Alias for @ref hpm_adc_v2_enable_pmt_queue. */
static inline hpm_stat_t hpm_adc_v2_enable_preempt_queue(adc_v2_handle_t handle, uint8_t trig_ch)
{
    return hpm_adc_v2_enable_pmt_queue(handle, trig_ch);
}

/** @brief Alias for @ref hpm_adc_v2_init_pmt_dma. */
static inline void hpm_adc_v2_init_preempt_dma(adc_v2_handle_t handle, uint32_t addr)
{
    hpm_adc_v2_init_pmt_dma(handle, addr);
}

/**
 * @brief Parse a preemption-mode DMA buffer word into semantic fields.
 * @note Word layout differs between ADC12 and ADC16. On ADC16, @c adc16_pmt_dma_data_t has two variants
 *       gated by @c ADC_SOC_IP_VERSION in @ref hpm_adc16_drv.h; the port layer uses the variant built
 *       for the target SoC. Do not decode @p raw with fixed bit masks in application code.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] raw Raw 32-bit DMA word.
 * @param[out] sample Parsed sample fields.
 */
void hpm_adc_v2_parse_pmt_dma_word(adc_v2_handle_t handle, uint32_t raw, adc_v2_dma_sample_t *sample);

/**
 * @brief Parse a sequence-mode DMA buffer word into semantic fields.
 * @note Word layout differs between ADC12 and ADC16. On ADC16, @c adc16_seq_dma_data_t has two variants
 *       gated by @c ADC_SOC_IP_VERSION in @ref hpm_adc16_drv.h (notably @c adc_ch field position);
 *       the port layer uses the variant built for the target SoC. When @ref hpm_adc_v2_is_seq_dma_16bit_enabled
 *       is true, pass each buffer halfword zero-extended in @p raw; @p sample->is_raw_16bit is set and
 *       metadata fields are cleared. Otherwise @p raw is one packed 32-bit @c adc12_seq_dma_data_t or
 *       @c adc16_seq_dma_data_t word. Do not decode @p raw with fixed bit masks in application code.
 *
 * @param[in] handle ADC instance handle.
 * @param[in] raw Raw DMA buffer entry (32-bit packed word or zero-extended 16-bit halfword).
 * @param[out] sample Parsed sample fields.
 */
void hpm_adc_v2_parse_seq_dma_word(adc_v2_handle_t handle, uint32_t raw, adc_v2_dma_sample_t *sample);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
