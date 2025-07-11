/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_GPTMR_DRV_H
#define HPM_GPTMR_DRV_H
#include "hpm_common.h"
#include "hpm_gptmr_regs.h"
#include "hpm_soc_feature.h"

/**
 * @brief GPTMR driver APIs
 * @defgroup gptmr_interface GPTMR driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief GPTMR channel IRQ mask
 */
#define GPTMR_CH_CMP_IRQ_MASK(ch, cmp) (1 << (ch * 4 + 2 + cmp))
#define GPTMR_CH_CAP_IRQ_MASK(ch) (1 << (ch * 4 + 1))
#define GPTMR_CH_RLD_IRQ_MASK(ch) (1 << (ch * 4))

/**
 * @brief GPTMR channel status
 */
#define GPTMR_CH_CMP_STAT_MASK(ch, cmp) (1 << (ch * 4 + 2 + cmp))
#define GPTMR_CH_CAP_STAT_MASK(ch) (1 << (ch * 4 + 1))
#define GPTMR_CH_RLD_STAT_MASK(ch) (1 << (ch * 4))

/**
 * @brief GPTMR channel swsynct mask
 */
#define GPTMR_CH_GCR_SWSYNCT_MASK(ch) (1 << ch)

/**
 * @brief GPTMR one channel support output comparator count
 */
#define GPTMR_CH_CMP_COUNT (2U)

/**
 * @brief GPTMR synci valid edge
 */
typedef enum gptmr_synci_edge {
    gptmr_synci_edge_none = 0,
    gptmr_synci_edge_falling = GPTMR_CHANNEL_CR_SYNCIFEN_MASK,
    gptmr_synci_edge_rising = GPTMR_CHANNEL_CR_SYNCIREN_MASK,
    gptmr_synci_edge_both = gptmr_synci_edge_falling | gptmr_synci_edge_rising,
} gptmr_synci_edge_t;

/**
 * @brief GPTMR work mode
 */
typedef enum gptmr_work_mode {
    gptmr_work_mode_no_capture = 0,
    gptmr_work_mode_capture_at_rising_edge = 1,
    gptmr_work_mode_capture_at_falling_edge = 2,
    gptmr_work_mode_capture_at_both_edge = 3,
    gptmr_work_mode_measure_width = 4,
} gptmr_work_mode_t;

/**
 * @brief GPTMR DMA request event
 */
typedef enum gptmr_dma_request_event {
    gptmr_dma_request_on_cmp0 = 0,
    gptmr_dma_request_on_cmp1 = 1,
    gptmr_dma_request_on_input_signal_toggle = 2,
    gptmr_dma_request_on_reload = 3,
    gptmr_dma_request_disabled = 0xFF,
} gptmr_dma_request_event_t;

/**
 * @brief GPTMR counter type
 */
typedef enum gptmr_counter_type {
    gptmr_counter_type_rising_edge,
    gptmr_counter_type_falling_edge,
    gptmr_counter_type_measured_period,
    gptmr_counter_type_measured_duty_cycle,
    gptmr_counter_type_normal,
} gptmr_counter_type_t;

/**
 * @brief GPTMR counter mode
 */

#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
typedef enum gptmr_counter_mode {
    gptmr_counter_mode_internal = 0,
    gptmr_counter_mode_external,
} gptmr_counter_mode_t;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
typedef enum gptmr_channel_monitor_type {
    monitor_signal_period = 0,
    monitor_signal_high_level_time,
} gptmr_channel_monitor_type_t;

typedef struct gptmr_channel_monitor_config {
    gptmr_channel_monitor_type_t monitor_type;
    uint32_t max_value;   /**< The unit is the gptmr clock source period */
    uint32_t min_value;   /**< The unit is the gptmr clock source period */
} gptmr_channel_monitor_config_t;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_QEI_MODE) && (HPM_IP_FEATURE_GPTMR_QEI_MODE == 1)
typedef enum gptmr_qei_ch_group {
    gptmr_qei_ch_group_01 = 0,
    gptmr_qei_ch_group_23 = 2,
} gptmr_qei_ch_group_t;

typedef enum gptmr_qei_type {
    gptmr_qei_ud_mode = 0,
    gptmr_qei_ab_mode,
    gptmr_qei_pd_mode,
} gptmr_qei_type_t;

typedef struct gptmr_qei_config {
    gptmr_qei_type_t type;
    gptmr_qei_ch_group_t ch_group;
    uint32_t phmax;
} gptmr_qei_config_t;

#endif

#if defined(HPM_IP_FEATURE_GPTMR_BURST_MODE) && (HPM_IP_FEATURE_GPTMR_BURST_MODE == 1)
typedef enum gptmr_burst_counter_mode {
    gptmr_burst_counter_restart = 0,
    gptmr_burst_counter_continue,
} gptmr_burst_counter_mode_t;

#endif

/**
 * @brief GPTMR channel config
 */
typedef struct gptmr_channel_config {
    gptmr_work_mode_t mode;
    gptmr_dma_request_event_t dma_request_event;
    gptmr_synci_edge_t synci_edge;
    uint32_t cmp[GPTMR_CH_CMP_COUNT];
    uint32_t reload;
    bool cmp_initial_polarity_high;
    bool enable_cmp_output;
    bool enable_sync_follow_previous_channel;
    bool enable_software_sync;
    bool debug_mode;
#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR == 1)
    bool enable_monitor;
    gptmr_channel_monitor_config_t monitor_config;
#endif
#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE == 1)
    gptmr_counter_mode_t counter_mode;
#endif
#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE == 1)
    bool enable_opmode;
#endif
} gptmr_channel_config_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief gptmr channel enable
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] enable
 *  @arg true: enable
 *  @arg false: disable
 */
static inline void gptmr_channel_enable(GPTMR_Type *ptr, uint8_t ch_index, bool enable)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
         & ~(GPTMR_CHANNEL_CR_CNTRST_MASK | GPTMR_CHANNEL_CR_CMPEN_MASK))
        | GPTMR_CHANNEL_CR_CMPEN_SET(enable);
}

/**
 * @brief gptmr channel reset counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_reset_count(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_CNTRST_MASK;
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_CNTRST_MASK;
}

/**
 * @brief gptmr channel update counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] value updated vaue
 */
static inline void gptmr_channel_update_count(GPTMR_Type *ptr,
                                             uint8_t ch_index,
                                             uint32_t value)
{
    if ((value > 0) && (value != 0xFFFFFFFFu)) {
        value--;
    }
    ptr->CHANNEL[ch_index].CNTUPTVAL = GPTMR_CHANNEL_CNTUPTVAL_CNTUPTVAL_SET(value);
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_CNTUPT_MASK;
}

/**
 * @brief gptmr channel slect synci valid edge
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] edge gptmr_synci_edge_t
 */
static inline void gptmr_channel_select_synci_valid_edge(GPTMR_Type *ptr,
                                                        uint8_t ch_index,
                                                        gptmr_synci_edge_t edge)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
            & ~(GPTMR_CHANNEL_CR_SYNCIFEN_MASK
              | GPTMR_CHANNEL_CR_SYNCIREN_MASK)) | edge;
}

/**
 * @brief gptmr channel enable dma request
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] enable
 *  @arg true: enable
 *  @arg false: disable
 */
static inline void gptmr_channel_enable_dma_request(GPTMR_Type *ptr,
                                                   uint8_t ch_index,
                                                   bool enable)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
            & ~(GPTMR_CHANNEL_CR_DMAEN_MASK)) | GPTMR_CHANNEL_CR_DMAEN_SET(enable);
}

/**
 * @brief gptmr channel get counter value
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] capture gptmr_counter_type_t
 */
static inline uint32_t gptmr_channel_get_counter(GPTMR_Type *ptr,
                                                     uint8_t ch_index,
                                                     gptmr_counter_type_t capture)
{
    uint32_t value;
    switch (capture) {
    case gptmr_counter_type_rising_edge:
        value = (ptr->CHANNEL[ch_index].CAPPOS & GPTMR_CHANNEL_CAPPOS_CAPPOS_MASK) >> GPTMR_CHANNEL_CAPPOS_CAPPOS_SHIFT;
        break;
    case gptmr_counter_type_falling_edge:
        value = (ptr->CHANNEL[ch_index].CAPNEG & GPTMR_CHANNEL_CAPNEG_CAPNEG_MASK) >> GPTMR_CHANNEL_CAPNEG_CAPNEG_SHIFT;
        break;
    case gptmr_counter_type_measured_period:
        value = (ptr->CHANNEL[ch_index].CAPPRD & GPTMR_CHANNEL_CAPPRD_CAPPRD_MASK) >> GPTMR_CHANNEL_CAPPRD_CAPPRD_SHIFT;
        break;
    case gptmr_counter_type_measured_duty_cycle:
        value = (ptr->CHANNEL[ch_index].CAPDTY & GPTMR_CHANNEL_CAPDTY_MEAS_HIGH_MASK) >> GPTMR_CHANNEL_CAPDTY_MEAS_HIGH_SHIFT;
        break;
    default:
        value = (ptr->CHANNEL[ch_index].CNT & GPTMR_CHANNEL_CNT_COUNTER_MASK) >> GPTMR_CHANNEL_CNT_COUNTER_SHIFT;
        break;
    }
    return value;
}

/**
 * @brief gptmr trigger channel software sync
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index_mask channel index mask
 */
static inline void gptmr_trigger_channel_software_sync(GPTMR_Type *ptr, uint32_t ch_index_mask)
{
#if defined(HPM_IP_FEATURE_GPTMR_QEI_MODE) && (HPM_IP_FEATURE_GPTMR_QEI_MODE == 1)
    ptr->GCR = (ptr->GCR & ~GPTMR_GCR_SWSYNCT_MASK) | GPTMR_GCR_SWSYNCT_SET(ch_index_mask);
#else
    ptr->GCR = ch_index_mask;
#endif
}

/**
 * @brief gptmr enable irq
 *
 * @param [in] ptr GPTMR base address
 * @param [in] irq_mask irq mask
 */
static inline void gptmr_enable_irq(GPTMR_Type *ptr, uint32_t irq_mask)
{
    ptr->IRQEN |= irq_mask;
}

/**
 * @brief gptmr disable irq
 *
 * @param [in] ptr GPTMR base address
 * @param [in] irq_mask irq mask
 */
static inline void gptmr_disable_irq(GPTMR_Type *ptr, uint32_t irq_mask)
{
    ptr->IRQEN &= ~irq_mask;
}

/**
 * @brief gptmr check status
 *
 * @param [in] ptr GPTMR base address
 * @param [in] mask channel flag mask
 */
static inline bool gptmr_check_status(GPTMR_Type *ptr, uint32_t mask)
{
    return (ptr->SR & mask) == mask;
}

/**
 * @brief gptmr clear status
 *
 * @param [in] ptr GPTMR base address
 * @param [in] mask channel flag mask
 */
static inline void gptmr_clear_status(GPTMR_Type *ptr, uint32_t mask)
{
    ptr->SR = mask;
}

/**
 * @brief gptmr get status
 *
 * @param [in] ptr GPTMR base address
 * @retval SR register value
 */
static inline uint32_t gptmr_get_status(GPTMR_Type *ptr)
{
    return ptr->SR;
}


/**
 * @brief gptmr channel stop counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_stop_counter(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_CEN_MASK;
}

/**
 * @brief gptmr channel enable compare output
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_enable_cmp_output(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_CMPEN_MASK;
}

/**
 * @brief gptmr channel disable compare output
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_disable_cmp_output(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_CMPEN_MASK;
}

/**
 * @brief gptmr channel set capmode
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] mode enum gptmr_work_mode_capture_at_rising_edge or gptmr_work_mode_capture_at_falling_edge and so on
 */
static inline void gptmr_channel_set_capmode(GPTMR_Type *ptr, uint8_t ch_index, gptmr_work_mode_t mode)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMR_CHANNEL_CR_CAPMODE_MASK) | GPTMR_CHANNEL_CR_CAPMODE_SET(mode);
}

/**
 * @brief gptmr channel get capmode
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_work_mode_t enum gptmr_work_mode_capture_at_rising_edge or gptmr_work_mode_capture_at_falling_edge
 */
static inline gptmr_work_mode_t gptmr_channel_get_capmode(GPTMR_Type *ptr, uint8_t ch_index)
{
    return (gptmr_work_mode_t)GPTMR_CHANNEL_CR_CAPMODE_GET(ptr->CHANNEL[ch_index].CR);
}

/**
 * @brief gptmr channel update comparator
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] cmp_index comparator index
 * @param [in] cmp comparator value
 */
static inline void gptmr_update_cmp(GPTMR_Type *ptr, uint8_t ch_index, uint8_t cmp_index, uint32_t cmp)
{
    if ((cmp > 0) && (cmp != 0xFFFFFFFFu)) {
        cmp--;
    }
    ptr->CHANNEL[ch_index].CMP[cmp_index] = GPTMR_CHANNEL_CMP_CMP_SET(cmp);
}

/**
 * @brief gptmr channel get reload
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval RLD register value
 */
static inline uint32_t gptmr_channel_get_reload(GPTMR_Type *ptr, uint8_t ch_index)
{
    return ptr->CHANNEL[ch_index].RLD;
}

/**
 * @brief gptmr channel update reload
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] reload reload value
 */
static inline void gptmr_channel_config_update_reload(GPTMR_Type *ptr, uint8_t ch_index, uint32_t reload)
{
    if ((reload > 0) && (reload != 0xFFFFFFFFu)) {
        reload--;
    }
    ptr->CHANNEL[ch_index].RLD = GPTMR_CHANNEL_RLD_RLD_SET(reload);
}

/**
 * @brief gptmr channel get dma request event
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_dma_request_event_t gptmr_dma_request_on_cmp0 or gptmr_dma_request_on_reload
 */
static inline gptmr_dma_request_event_t gptmr_channel_get_dma_request_event(GPTMR_Type *ptr, uint8_t ch_index)
{
    return (gptmr_dma_request_event_t)GPTMR_CHANNEL_CR_DMASEL_GET(ptr->CHANNEL[ch_index].CR);
}

/**
 * @brief gptmr channel config
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] config gptmr_channel_config_t
 * @param [in] enable
 *  @arg true: enable
 *  @arg false: disable
 *
 * @retval hpm_stat_t status_invalid_argument or status_success
 */
hpm_stat_t gptmr_channel_config(GPTMR_Type *ptr,
                         uint8_t ch_index,
                         gptmr_channel_config_t *config,
                         bool enable);

/**
 * @brief gptmr channel get default config
 *
 * @param [in] ptr GPTMR base address
 * @param [out] config gptmr_channel_config_t
 */
void gptmr_channel_get_default_config(GPTMR_Type *ptr, gptmr_channel_config_t *config);


#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
/**
 * @brief gptmr set counter mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] mode gptmr_counter_mode_t, gptmr_counter_mode_external for gptmr enable external counting mode
 */
static inline void gptmr_channel_set_counter_mode(GPTMR_Type *ptr, uint8_t ch_index, gptmr_counter_mode_t mode)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMR_CHANNEL_CR_CNT_MODE_MASK) | GPTMR_CHANNEL_CR_CNT_MODE_SET(mode);
}

/**
 * @brief gptmr channel get external trigger input counting mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_counter_mode_external for external counting mode, gptmr_counter_mode_internal for internal counting mode
 */
static inline gptmr_counter_mode_t gptmr_channel_get_counter_mode(GPTMR_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMR_CHANNEL_CR_CNT_MODE_MASK) ==
            GPTMR_CHANNEL_CR_CNT_MODE_MASK) ?
            gptmr_counter_mode_external : gptmr_counter_mode_internal;
}

#endif

#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
/**
 * @brief gptmr channel enable opmode, it's one-shot mode, timer will stopped at reload point.
 *
 * @note  reload irq will be always set at one-shot mode at end
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_enable_opmode(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_OPMODE_MASK;
}

/**
 * @brief gptmr channel disable opmode, it's round mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_opmode(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_OPMODE_MASK;
}

/**
 * @brief gptmr channel get opmode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true for opmode, false for normal mode
 */
static inline bool gptmr_channel_is_opmode(GPTMR_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMR_CHANNEL_CR_OPMODE_MASK) == GPTMR_CHANNEL_CR_OPMODE_MASK) ? true : false;
}
#endif

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
/**
 * @brief gptmr channel enable monitor, set to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_enable_monitor(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_MONITOR_EN_MASK;
}

/**
 * @brief gptmr channel disable monitor
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_monitor(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_MONITOR_EN_MASK;
}

/**
 * @brief gptmr channel set to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] type gptmr_channel_monitor_type_t
 */
static inline void gptmr_channel_set_monitor_type(GPTMR_Type *ptr, uint8_t ch_index, gptmr_channel_monitor_type_t type)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMR_CHANNEL_CR_MONITOR_SEL_MASK) | GPTMR_CHANNEL_CR_MONITOR_SEL_SET(type);
}

/**
 * @brief gptmr channel get to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_channel_monitor_type_t monitor_signal_high_level_time or monitor_signal_period
 */
static inline gptmr_channel_monitor_type_t gptmr_channel_get_monitor_type(GPTMR_Type *ptr, uint8_t ch_index)
{
    return (gptmr_channel_monitor_type_t)GPTMR_CHANNEL_CR_MONITOR_SEL_GET(ptr->CHANNEL[ch_index].CR);
}
/**
 * @brief gptmr channel get default monitor config
 *
 * @param [in] ptr GPTMR base address
 * @param [out] config gptmr_channel_monitor_config_t
 */
void gptmr_channel_get_default_monitor_config(GPTMR_Type *ptr, gptmr_channel_monitor_config_t *config);

/**
 * @brief gptmr channel monitor config
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] config gptmr_channel_monitor_config_t
 * @param [in] enable
 *  @arg true: enable monitor and reset reload count
 *  @arg false: disable monitor
 *
 * @retval hpm_stat_t status_invalid_argument or status_success
 */

hpm_stat_t gptmr_channel_monitor_config(GPTMR_Type *ptr, uint8_t ch_index,
                                        gptmr_channel_monitor_config_t *config,
                                        bool enable);

#endif

#if defined(HPM_IP_FEATURE_GPTMR_BURST_MODE) && (HPM_IP_FEATURE_GPTMR_BURST_MODE == 1)
/**
 * @brief gptmr channel enable burst mode, it's burst mode, Timer stops after reaching reload value burst_cfg times.

 *
 * @note  reload irq will be always set at one-shot mode at end
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline hpm_stat_t gptmr_channel_enable_burst_mode(GPTMR_Type *ptr, uint8_t ch_index)
{
    /* enable burst mode, should disable opmode first */
    if (gptmr_channel_is_opmode(ptr, ch_index) == true) {
        return status_invalid_argument;
    }
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_BURST_MODE_MASK;
    return status_success;
}

/**
 * @brief gptmr channel disable opmode, it's circular mode.the timer restarts from 0 after reaching the reload value.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_burst_mode(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_BURST_MODE_MASK;
}

/**
 * @brief gptmr channel get burstmode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true for burstmode, false for circular mode
 */
static inline bool gptmr_channel_is_burst_mode(GPTMR_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMR_CHANNEL_CR_BURST_MODE_MASK) == GPTMR_CHANNEL_CR_BURST_MODE_MASK) ? true : false;
}

/**
 * @brief gptmr channel set target burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] burst_count configure burst count: number of times timer reaches reload value.
 */
static inline void gptmr_channel_set_target_burst_count(GPTMR_Type *ptr, uint8_t ch_index, uint16_t burst_count)
{
    ptr->CHANNEL[ch_index].BURST_CFG = (ptr->CHANNEL[ch_index].BURST_CFG & ~GPTMR_CHANNEL_BURST_CFG_BURST_CFG_MASK) | GPTMR_CHANNEL_BURST_CFG_BURST_CFG_SET(burst_count);
}

/**
 * @brief gptmr channel get target burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval uint16_t configure burst count: number of times timer reaches reload value.
 */
static inline uint16_t gptmr_channel_get_target_burst_count(GPTMR_Type *ptr, uint8_t ch_index)
{
    return (uint16_t)GPTMR_CHANNEL_BURST_CFG_BURST_CFG_GET(ptr->CHANNEL[ch_index].BURST_CFG);
}

/**
 * @brief gptmr channel get current burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval uint32_t current burst count
 */

static inline uint32_t gptmr_channel_get_current_burst_count(GPTMR_Type *ptr, uint8_t ch_index)
{
    return GPTMR_CHANNEL_BURST_COUNT_BURST_COUNT_GET(ptr->CHANNEL[ch_index].BURST_COUNT);

}

/**
 * @brief gptmr channel burst mode start counter
 * @note in gptmr_burst_counter_continue mode, restart uses current counter without reset if current burst count < set burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] mode gptmr_burst_counter_mode_t
 * @retval hpm_stat_t status_invalid_argument or status_success
 */
hpm_stat_t gptmr_channel_burst_mode_start_counter(GPTMR_Type *ptr, uint8_t ch_index, gptmr_burst_counter_mode_t mode);

/**
 * @brief gptmr channel burst mode stop counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_burst_mode_stop_counter(GPTMR_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_CEN_MASK;
}
#endif

/**
 * @brief gptmr channel start counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_start_counter(GPTMR_Type *ptr, uint8_t ch_index)
{
#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
    /* if support opmode, should clear CEN and set CEN */
     if (gptmr_channel_is_opmode(ptr, ch_index) == true) {
        ptr->CHANNEL[ch_index].CR &= ~GPTMR_CHANNEL_CR_CEN_MASK;
     }
#endif
    ptr->CHANNEL[ch_index].CR |= GPTMR_CHANNEL_CR_CEN_MASK;
}

#if defined(HPM_IP_FEATURE_GPTMR_QEI_MODE) && (HPM_IP_FEATURE_GPTMR_QEI_MODE == 1)

/**
 * @brief gptmr config qei.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] qei_config gptmr_qei_config_t
 */
void gptmr_config_qei(GPTMR_Type *ptr, gptmr_qei_config_t *qei_config);

/**
 * @brief gptmr set qei type.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_group gptmr_qei_ch_group_t
 * @param [in] type gptmr_qei_type_t
 */
void gptmr_set_qei_type(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group, gptmr_qei_type_t type);

/**
 * @brief gptmr get qei type.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_group gptmr_qei_ch_group_t
 * @retval gptmr_qei_type_t ud_mode or ab_mode or pd_mode
 */
gptmr_qei_type_t gptmr_get_qei_type(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group);

/**
 * @brief gptmr get qei phase count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_group gptmr_qei_ch_group_t
 * @retval uint32_t qei phase count
 */
uint32_t gptmr_get_qei_phcnt(GPTMR_Type *ptr, gptmr_qei_ch_group_t ch_group);

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* HPM_GPTMR_DRV_H */
