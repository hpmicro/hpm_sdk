/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_GPTMRV2_DRV_H
#define HPM_GPTMRV2_DRV_H
#include "hpm_common.h"
#include "hpm_gptmrv2_regs.h"
#include "hpm_soc_feature.h"

#define GPTMR_Type GPTMRV2_Type

/**
 * @brief GPTMR driver APIs
 * @defgroup gptmr_interface GPTMR driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief GPTMR status
 */
enum {
    status_gptmr_cmp_overflow = MAKE_STATUS(status_group_gptmr, 1),
    status_gptmr_reload_overflow = MAKE_STATUS(status_group_gptmr, 2),
};

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
    gptmr_synci_edge_falling = GPTMRV2_CHANNEL_CR_SYNCIFEN_MASK,
    gptmr_synci_edge_rising = GPTMRV2_CHANNEL_CR_SYNCIREN_MASK,
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

typedef enum gptmr_counter_mode {
    gptmr_counter_mode_internal = 0,
    gptmr_counter_mode_external,
} gptmr_counter_mode_t;

typedef enum gptmr_channel_monitor_type {
    monitor_signal_period = 0,
    monitor_signal_high_level_time,
} gptmr_channel_monitor_type_t;

typedef struct gptmr_channel_monitor_config {
    gptmr_channel_monitor_type_t monitor_type;
    uint32_t max_value;   /**< The unit is the gptmr clock source period */
    uint32_t min_value;   /**< The unit is the gptmr clock source period */
} gptmr_channel_monitor_config_t;

typedef enum gptmr_burst_counter_mode {
    gptmr_burst_counter_restart = 0,
    gptmr_burst_counter_continue,
} gptmr_burst_counter_mode_t;

/**
 * @brief GPTMR channel config
 */
typedef struct gptmr_channel_cfg {
    gptmr_work_mode_t mode;                             /**< Work mode: capture at rising/falling/both edges, measure width, or no capture */
    gptmr_dma_request_event_t dma_request_event;        /**< DMA request event source: cmp0, cmp1, input toggle, reload, or disabled */
    gptmr_synci_edge_t synci_edge;                      /**< SYNCI valid edge: none, falling, rising, or both */
    uint32_t cmp[GPTMR_CH_CMP_COUNT];                   /**< Comparator values (hardware register = cmp - 1 for non-zero/0xFFFFFF) */
    uint32_t reload;                                    /**< Reload value (hardware register = reload - 1 for non-zero/0xFFFFFF) */
    bool cmp_initial_polarity_high;                     /**< Initial output polarity: true = high, false = low */
    bool enable_cmp_output;                             /**< Enable comparator output to pin */
    bool enable_sync_follow_previous_channel;           /**< Enable sync follow previous channel */
    bool enable_software_sync;                          /**< Enable software sync trigger */
    bool debug_mode;                                    /**< Enable debug mode (timer stops on core halt) */
    bool enable_monitor;                                /**< Enable input signal monitor */
    gptmr_channel_monitor_config_t monitor_config;      /**< Monitor configuration: monitor type, max/min thresholds */
    gptmr_counter_mode_t counter_mode;                  /**< Counter mode: internal or external clock source */
    bool enable_opmode;                                 /**< Enable one-shot mode (timer stops at reload) */
    bool enable_shadow;                                 /**< Enable shadow register (reload/cmp updated on reload event) */
    uint16_t prescaler;                                 /**< Clock prescaler: 1 = no division, N = divide-by-N */
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
static inline void gptmr_channel_enable(GPTMRV2_Type *ptr, uint8_t ch_index, bool enable)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
         & ~(GPTMRV2_CHANNEL_CR_CNTRST_MASK | GPTMRV2_CHANNEL_CR_CMPEN_MASK))
        | GPTMRV2_CHANNEL_CR_CMPEN_SET(enable);
}

/**
 * @brief gptmr channel reset counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_reset_count(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_CNTRST_MASK;
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CNTRST_MASK;
}

/**
 * @brief gptmr channel update counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] value updated vaue
 */
static inline void gptmr_channel_update_count(GPTMRV2_Type *ptr,
                                             uint8_t ch_index,
                                             uint32_t value)
{
    if ((value > 0) && (value != 0xFFFFFFFFu)) {
        value--;
    }
    ptr->CHANNEL[ch_index].CNTUPTVAL = GPTMRV2_CHANNEL_CNTUPTVAL_CNTUPTVAL_SET(value);
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_CNTUPT_MASK;
}

/**
 * @brief gptmr channel select synci valid edge
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] edge gptmr_synci_edge_t
 */
static inline void gptmr_channel_select_synci_valid_edge(GPTMRV2_Type *ptr,
                                                        uint8_t ch_index,
                                                        gptmr_synci_edge_t edge)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
            & ~(GPTMRV2_CHANNEL_CR_SYNCIFEN_MASK
              | GPTMRV2_CHANNEL_CR_SYNCIREN_MASK)) | edge;
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
static inline void gptmr_channel_enable_dma_request(GPTMRV2_Type *ptr,
                                                   uint8_t ch_index,
                                                   bool enable)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR
            & ~(GPTMRV2_CHANNEL_CR_DMAEN_MASK)) | GPTMRV2_CHANNEL_CR_DMAEN_SET(enable);
}

/**
 * @brief gptmr channel get counter value
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] capture gptmr_counter_type_t
 */
static inline uint32_t gptmr_channel_get_counter(GPTMRV2_Type *ptr,
                                                     uint8_t ch_index,
                                                     gptmr_counter_type_t capture)
{
    uint32_t value;
    switch (capture) {
    case gptmr_counter_type_rising_edge:
        value = (ptr->CHANNEL[ch_index].CAPPOS & GPTMRV2_CHANNEL_CAPPOS_CAPPOS_MASK) >> GPTMRV2_CHANNEL_CAPPOS_CAPPOS_SHIFT;
        break;
    case gptmr_counter_type_falling_edge:
        value = (ptr->CHANNEL[ch_index].CAPNEG & GPTMRV2_CHANNEL_CAPNEG_CAPNEG_MASK) >> GPTMRV2_CHANNEL_CAPNEG_CAPNEG_SHIFT;
        break;
    case gptmr_counter_type_measured_period:
        value = (ptr->CHANNEL[ch_index].CAPPRD & GPTMRV2_CHANNEL_CAPPRD_CAPPRD_MASK) >> GPTMRV2_CHANNEL_CAPPRD_CAPPRD_SHIFT;
        break;
    case gptmr_counter_type_measured_duty_cycle:
        value = (ptr->CHANNEL[ch_index].CAPDTY & GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_MASK) >> GPTMRV2_CHANNEL_CAPDTY_MEAS_HIGH_SHIFT;
        break;
    default:
        value = (ptr->CHANNEL[ch_index].CNT & GPTMRV2_CHANNEL_CNT_COUNTER_MASK) >> GPTMRV2_CHANNEL_CNT_COUNTER_SHIFT;
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
static inline void gptmr_trigger_channel_software_sync(GPTMRV2_Type *ptr, uint32_t ch_index_mask)
{
    ptr->GCR = (ptr->GCR & ~GPTMRV2_GCR_SWSYNCT_MASK) | GPTMRV2_GCR_SWSYNCT_SET(ch_index_mask);
}

/**
 * @brief gptmr enable irq
 *
 * @param [in] ptr GPTMR base address
 * @param [in] irq_mask irq mask
 */
static inline void gptmr_enable_irq(GPTMRV2_Type *ptr, uint32_t irq_mask)
{
    ptr->IRQEN |= irq_mask;
}

/**
 * @brief gptmr disable irq
 *
 * @param [in] ptr GPTMR base address
 * @param [in] irq_mask irq mask
 */
static inline void gptmr_disable_irq(GPTMRV2_Type *ptr, uint32_t irq_mask)
{
    ptr->IRQEN &= ~irq_mask;
}

/**
 * @brief gptmr check status
 *
 * @param [in] ptr GPTMR base address
 * @param [in] mask channel flag mask
 */
static inline bool gptmr_check_status(GPTMRV2_Type *ptr, uint32_t mask)
{
    return (ptr->SR & mask) == mask;
}

/**
 * @brief gptmr clear status
 *
 * @param [in] ptr GPTMR base address
 * @param [in] mask channel flag mask
 */
static inline void gptmr_clear_status(GPTMRV2_Type *ptr, uint32_t mask)
{
    ptr->SR = mask;
}

/**
 * @brief gptmr get status
 *
 * @param [in] ptr GPTMR base address
 * @retval SR register value
 */
static inline uint32_t gptmr_get_status(GPTMRV2_Type *ptr)
{
    return ptr->SR;
}


/**
 * @brief gptmr channel stop counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_stop_counter(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CEN_MASK;
}

/**
 * @brief gptmr channel enable compare output
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_enable_cmp_output(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_CMPEN_MASK;
}

/**
 * @brief gptmr channel disable compare output
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_disable_cmp_output(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CMPEN_MASK;
}

/**
 * @brief gptmr channel set capmode
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] mode enum gptmr_work_mode_capture_at_rising_edge or gptmr_work_mode_capture_at_falling_edge and so on
 */
static inline void gptmr_channel_set_capmode(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_work_mode_t mode)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMRV2_CHANNEL_CR_CAPMODE_MASK) | GPTMRV2_CHANNEL_CR_CAPMODE_SET(mode);
}

/**
 * @brief gptmr channel get capmode
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_work_mode_t enum gptmr_work_mode_capture_at_rising_edge or gptmr_work_mode_capture_at_falling_edge
 */
static inline gptmr_work_mode_t gptmr_channel_get_capmode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (gptmr_work_mode_t)GPTMRV2_CHANNEL_CR_CAPMODE_GET(ptr->CHANNEL[ch_index].CR);
}

/**
 * @brief gptmr channel update comparator
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] cmp_index comparator index
 * @param [in] cmp comparator value
 */
static inline void gptmr_update_cmp(GPTMRV2_Type *ptr, uint8_t ch_index, uint8_t cmp_index, uint32_t cmp)
{
    if ((cmp > 0) && (cmp != 0xFFFFFFFFu)) {
        cmp--;
    }
    ptr->CHANNEL[ch_index].CMP[cmp_index] = GPTMRV2_CHANNEL_CMP_CMP_SET(cmp);
}

/**
 * @brief gptmr channel get reload
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval RLD register value
 */
static inline uint32_t gptmr_channel_get_reload(GPTMRV2_Type *ptr, uint8_t ch_index)
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
static inline void gptmr_channel_config_update_reload(GPTMRV2_Type *ptr, uint8_t ch_index, uint32_t reload)
{
    if ((reload > 0) && (reload != 0xFFFFFFFFu)) {
        reload--;
    }
    ptr->CHANNEL[ch_index].RLD = GPTMRV2_CHANNEL_RLD_RLD_SET(reload);
}

/**
 * @brief gptmr channel get dma request event
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_dma_request_event_t gptmr_dma_request_on_cmp0 or gptmr_dma_request_on_reload
 */
static inline gptmr_dma_request_event_t gptmr_channel_get_dma_request_event(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (gptmr_dma_request_event_t)GPTMRV2_CHANNEL_CR_DMASEL_GET(ptr->CHANNEL[ch_index].CR);
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
hpm_stat_t gptmr_channel_config(GPTMRV2_Type *ptr,
                         uint8_t ch_index,
                         gptmr_channel_config_t *config,
                         bool enable);

/**
 * @brief gptmr channel get default config
 *
 * @param [in] ptr GPTMR base address
 * @param [out] config gptmr_channel_config_t
 */
void gptmr_channel_get_default_config(GPTMRV2_Type *ptr, gptmr_channel_config_t *config);


/**
 * @brief gptmr set counter mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] mode gptmr_counter_mode_t, gptmr_counter_mode_external for gptmr enable external counting mode
 */
static inline void gptmr_channel_set_counter_mode(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_counter_mode_t mode)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMRV2_CHANNEL_CR_CNT_MODE_MASK) | GPTMRV2_CHANNEL_CR_CNT_MODE_SET(mode);
}

/**
 * @brief gptmr channel get external trigger input counting mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_counter_mode_external for external counting mode, gptmr_counter_mode_internal for internal counting mode
 */
static inline gptmr_counter_mode_t gptmr_channel_get_counter_mode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMRV2_CHANNEL_CR_CNT_MODE_MASK) ==
            GPTMRV2_CHANNEL_CR_CNT_MODE_MASK) ?
            gptmr_counter_mode_external : gptmr_counter_mode_internal;
}

/**
 * @brief gptmr channel enable opmode, it's one-shot mode, timer will stopped at reload point.
 *
 * @note  reload irq will be always set at one-shot mode at end
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_enable_opmode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_OPMODE_MASK;
}

/**
 * @brief gptmr channel disable opmode, it's round mode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_opmode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_OPMODE_MASK;
}

/**
 * @brief gptmr channel get opmode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true for opmode, false for normal mode
 */
static inline bool gptmr_channel_is_opmode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMRV2_CHANNEL_CR_OPMODE_MASK) == GPTMRV2_CHANNEL_CR_OPMODE_MASK) ? true : false;
}

/**
 * @brief gptmr channel enable monitor, set to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_enable_monitor(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_MONITOR_EN_MASK;
}

/**
 * @brief gptmr channel disable monitor
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_monitor(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_MONITOR_EN_MASK;
}

/**
 * @brief gptmr channel set to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] type gptmr_channel_monitor_type_t
 */
static inline void gptmr_channel_set_monitor_type(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_channel_monitor_type_t type)
{
    ptr->CHANNEL[ch_index].CR = (ptr->CHANNEL[ch_index].CR & ~GPTMRV2_CHANNEL_CR_MONITOR_SEL_MASK) | GPTMRV2_CHANNEL_CR_MONITOR_SEL_SET(type);
}

/**
 * @brief gptmr channel get to monitor input signal period or high level time.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval gptmr_channel_monitor_type_t monitor_signal_high_level_time or monitor_signal_period
 */
static inline gptmr_channel_monitor_type_t gptmr_channel_get_monitor_type(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (gptmr_channel_monitor_type_t)GPTMRV2_CHANNEL_CR_MONITOR_SEL_GET(ptr->CHANNEL[ch_index].CR);
}
/**
 * @brief gptmr channel get default monitor config
 *
 * @param [in] ptr GPTMR base address
 * @param [out] config gptmr_channel_monitor_config_t
 */
void gptmr_channel_get_default_monitor_config(GPTMRV2_Type *ptr, gptmr_channel_monitor_config_t *config);

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

hpm_stat_t gptmr_channel_monitor_config(GPTMRV2_Type *ptr, uint8_t ch_index,
                                        gptmr_channel_monitor_config_t *config,
                                        bool enable);

/**
 * @brief gptmr channel enable burst mode, it's burst mode, Timer stops after reaching reload value burst_cfg times.

 *
 * @note  reload irq will be always set at one-shot mode at end
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline hpm_stat_t gptmr_channel_enable_burst_mode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    /* enable burst mode, should disable opmode first */
    if (gptmr_channel_is_opmode(ptr, ch_index) == true) {
        return status_invalid_argument;
    }
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_BURST_MODE_MASK;
    return status_success;
}

/**
 * @brief gptmr channel disable opmode, it's circular mode.the timer restarts from 0 after reaching the reload value.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_disable_burst_mode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_BURST_MODE_MASK;
}

/**
 * @brief gptmr channel get burstmode.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true for burstmode, false for circular mode
 */
static inline bool gptmr_channel_is_burst_mode(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return ((ptr->CHANNEL[ch_index].CR & GPTMRV2_CHANNEL_CR_BURST_MODE_MASK) == GPTMRV2_CHANNEL_CR_BURST_MODE_MASK) ? true : false;
}

/**
 * @brief gptmr channel set target burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] burst_count configure burst count: number of times timer reaches reload value.
 */
static inline void gptmr_channel_set_target_burst_count(GPTMRV2_Type *ptr, uint8_t ch_index, uint16_t burst_count)
{
    ptr->CHANNEL[ch_index].BURST_CFG = (ptr->CHANNEL[ch_index].BURST_CFG & ~GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_MASK) | GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_SET(burst_count);
}

/**
 * @brief gptmr channel get target burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval uint16_t configure burst count: number of times timer reaches reload value.
 */
static inline uint16_t gptmr_channel_get_target_burst_count(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (uint16_t)GPTMRV2_CHANNEL_BURST_CFG_BURST_CFG_GET(ptr->CHANNEL[ch_index].BURST_CFG);
}

/**
 * @brief gptmr channel get current burst count.
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval uint32_t current burst count
 */

static inline uint32_t gptmr_channel_get_current_burst_count(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return GPTMRV2_CHANNEL_BURST_COUNT_BURST_COUNT_GET(ptr->CHANNEL[ch_index].BURST_COUNT);

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
hpm_stat_t gptmr_channel_burst_mode_start_counter(GPTMRV2_Type *ptr, uint8_t ch_index, gptmr_burst_counter_mode_t mode);

/**
 * @brief gptmr channel burst mode stop counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_channel_burst_mode_stop_counter(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CEN_MASK;
}

/**
 * @brief gptmr channel start counter
 *
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 */
static inline void gptmr_start_counter(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    /* if support opmode, should clear CEN and set CEN */
    if (gptmr_channel_is_opmode(ptr, ch_index) == true) {
        ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CEN_MASK;
    }
    ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_CEN_MASK;
}

/**
 * @brief gptmr channel set comparator initial output polarity
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] high  true: initial output is high level
 */
static inline void gptmr_channel_set_cmp_initial_polarity(GPTMRV2_Type *ptr, uint8_t ch_index, bool high)
{
    if (high) {
        ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_CMPINIT_MASK;
    } else {
        ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_CMPINIT_MASK;
    }
}

/**
 * @brief gptmr channel get comparator initial output polarity
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true: initial output is high level
 */
static inline bool gptmr_channel_get_cmp_initial_polarity(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (ptr->CHANNEL[ch_index].CR & GPTMRV2_CHANNEL_CR_CMPINIT_MASK) ? true : false;
}

/**
 * @brief gptmr channel set shadow enable
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] enable true: shadow enable, false: shadow disable
 */
static inline void gptmr_channel_set_shadow_en(GPTMRV2_Type *ptr, uint8_t ch_index, bool enable)
{
    if (enable) {
        ptr->CHANNEL[ch_index].CR |= GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK;
    } else {
        ptr->CHANNEL[ch_index].CR &= ~GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK;
    }
}

/**
 * @brief gptmr channel get shadow enable
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval bool true: shadow enable, false: shadow disable
 */
static inline bool gptmr_channel_get_shadow_en(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (ptr->CHANNEL[ch_index].CR & GPTMRV2_CHANNEL_CR_SHADOW_EN_MASK) ? true : false;
}

/**
 * @brief gptmr channel set prescaler
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @param [in] prescaler prescaler value
 * @retval hpm_stat_t status_invalid_argument or status_success
 * @note prescaler: 1 = no division (timer clock), 2 = divide-by-2, N = divide-by-(N)
 */
static inline hpm_stat_t gptmr_channel_set_prescaler(GPTMRV2_Type *ptr, uint8_t ch_index, uint16_t prescaler)
{
    if (prescaler == 0) {
        return status_invalid_argument;
    }
    ptr->CHANNEL[ch_index].BURST_CFG = (ptr->CHANNEL[ch_index].BURST_CFG & ~GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_MASK) | GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_SET(prescaler);
    return status_success;
}

/**
 * @brief gptmr channel get prescaler
 * @param [in] ptr GPTMR base address
 * @param [in] ch_index channel index
 * @retval uint16_t prescaler value
 * @note prescaler: 1 = no division (timer clock), 2 = divide-by-2, N = divide-by-(N)
 */
static inline uint16_t gptmr_channel_get_prescaler(GPTMRV2_Type *ptr, uint8_t ch_index)
{
    return (GPTMRV2_CHANNEL_BURST_CFG_PRE_DIV_GET(ptr->CHANNEL[ch_index].BURST_CFG) - 1);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* HPM_GPTMRV2_DRV_H */
