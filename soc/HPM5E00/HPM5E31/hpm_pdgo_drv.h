/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PDGO_DRV_H
#define HPM_PDGO_DRV_H

#include "hpm_common.h"
#include "hpm_pdgo_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DGO_GPR_WORD_COUNT                (8U)         /*!< DGO GPR register count */
#define DGO_WAKEUP_COUNTER_TICKS_PER_SEC  (32768UL)    /*!< DGO Wakeup Counter frequency */
#define DGO_TURNOFF_COUNTER_TICKS_PER_SEC (24000000UL) /*!< DGO Turn-off counter frequency */
#define DGO_WAKEUP_TICK_IN_US             (1000000UL / DGO_WAKEUP_COUNTER_TICKS_PER_SEC)
#define DGO_TURNOFF_TICKS_PER_US          (DGO_TURNOFF_COUNTER_TICKS_PER_SEC / 1000000UL)

/**
 * @brief wakeup source
 */
typedef enum {
    dgo_wakeup_wkup_pin = PDGO_WKUP_EN_WKUP_MASK,
    dgo_wakeup_software = PDGO_WKUP_EN_CYC_MASK,
    dgo_wakeup_wuio_pins = PDGO_WKUP_EN_WUIO_MASK,
    dgo_wakeup_pcap0 = PDGO_WKUP_EN_PCAP0_MASK,
    dgo_wakeup_pcnt0 = PDGO_WKUP_EN_PCNT0_MASK,
} dgo_wakeup_bit_mask_t; /*!< dgo_wakeup_bit_mask_t */

/**
 * @brief irq0 source
 */
typedef enum {
    dgo_irq0_wkup_pin = PDGO_IRQ0_EN_WKUP_MASK,
    dgo_irq0_software = PDGO_IRQ0_EN_CYC_MASK,
    dgo_irq0_wuio0_pin = PDGO_IRQ0_EN_WUIO0_MASK,
    dgo_irq0_wuio1_pin = PDGO_IRQ0_EN_WUIO1_MASK,
    dgo_irq0_wuio2_pin = PDGO_IRQ0_EN_WUIO2_MASK,
    dgo_irq0_wuio3_pin = PDGO_IRQ0_EN_WUIO3_MASK,
} dgo_irq0_bit_mask_t; /*!< dgo_irq0_bit_mask_t */

/**
 * @brief irq0 source
 */
typedef enum {
    dgo_irq1_pcap0_period = PDGO_IRQ1_EN_PCAP0_PERIOD_MASK,
    dgo_irq1_pcap0_dump = PDGO_IRQ1_EN_PCAP0_DUMP_MASK,
    dgo_irq1_pcap0_match = PDGO_IRQ1_EN_PCAP0_MATCH_MASK,
    dgo_irq1_pcnt0_update = PDGO_IRQ1_EN_PCNT0_UPDATE_MASK,
    dgo_irq1_pcnt0_z_match = PDGO_IRQ1_EN_PCNT0_Z_MASK,
    dgo_irq1_pcnt0_h_match = PDGO_IRQ1_EN_PCNT0_HOME_MASK,
    dgo_irq1_pcnt0_cmp0_match = PDGO_IRQ1_EN_PCNT0_MATCH0_MASK,
    dgo_irq1_pcnt0_tacho = PDGO_IRQ1_EN_PCNT0_TACHO_MASK,
    dgo_irq1_pcnt0_dec_err = PDGO_IRQ1_EN_PCNT0_DECERR_MASK,
    dgo_irq1_pcnt0_seq_err = PDGO_IRQ1_EN_PCNT0_SEQERR_MASK,
} dgo_irq1_bit_mask_t; /*!< dgo_irq1_bit_mask_t */

/**
 * @brief wuio pin number
 */
typedef enum {
    dgo_wuio_pin_0 = 0,
    dgo_wuio_pin_1 = 1,
    dgo_wuio_pin_2 = 2,
    dgo_wuio_pin_3 = 3,
} dgo_wuio_pin_num_t;

/**
 * @brief pulse capture number
 */
typedef enum {
    dgo_pcap_0 = 0,
} dgo_pcap_num_t;

/**
 * @brief pulse counter number
 */
typedef enum {
    dgo_pcnt_0 = 0,
} dgo_pcnt_num_t;

/**
 * @brief filter mode
 */
typedef enum {
    dgo_wuio_irq_disable = 0,
    dgo_wuio_irq_level_mode = 1,
    dgo_wuio_irq_rise_edge_mode = 2,
    dgo_wuio_irq_both_edge_mode = 3,
} dgo_wuio_irq_mode_t;

/**
 * @brief filter mode
 */
typedef enum {
    dgo_filter_mode_rapid_change = 0,
    dgo_filter_mode_delay = 1,
    dgo_filter_mode_stable_high = 2,
    dgo_filter_mode_stable_low = 3,
    dgo_filter_mode_bypass = 4,
} dgo_filter_mode_t;

/**
 * @brief pulse capture match mode
 */
typedef enum {
    dgo_pcap_match_disable = 0,
    dgo_pcap_match_mode_cnt_ge_cmp = 2,
    dgo_pcap_match_mode_cnt_le_cmp,
    dgo_pcap_match_mode_highdump_ge_cmp,
    dgo_pcap_match_mode_highdump_le_cmp,
    dgo_pcap_match_mode_lowdump_ge_cmp,
    dgo_pcap_match_mode_lowdump_le_cmp,
    dgo_pcap_match_mode_highdump_add_lowdump_ge_cmp,
    dgo_pcap_match_mode_highdump_add_lowdump_le_cmp,
    dgo_pcap_match_mode_abs_highdump_minus_lowdump_ge_cmp,
    dgo_pcap_match_mode_abs_highdump_minus_lowdump_le_cmp,
    dgo_pcap_match_mode_highdump_minus_lowdump_ge_cmp,
    dgo_pcap_match_mode_lowdump_minus_highdump_le_cmp,
    dgo_pcap_match_mode_lowdump_minus_highdump_ge_cmp,
    dgo_pcap_match_mode_highdump_minus_lowdump_le_cmp,
} dgo_pcap_match_mode_t;

/**
 * @brief pulse capture dump mode
 */
typedef enum {
    dgo_pcap_dump_disable = 0,
    dgo_pcap_dump_wuio_rise_edge,
    dgo_pcap_dump_wuio_fall_edge,
    dgo_pcap_dump_cnt_finish,
} dgo_pcap_dump_mode_t;

/**
 * @brief pulse capture counter direction
 */
typedef enum {
    dgo_pcap_cnt_up = 0,
    dgo_pcap_cnt_down,
} dgo_pcap_cnt_dir_t;

/**
 * @brief pulse capture counter mode
 */
typedef enum {
    dgo_pcap_cnt_oneshot = 0,
    dgo_pcap_cnt_period,
} dgo_pcap_cnt_mode_t;

/**
 * @brief pulse capture counter event
 */
typedef enum {
    dgo_pcap_cnt_internal_clk = 0,
    dgo_pcap_cnt_wuio_rise_edge,
    dgo_pcap_cnt_wuio_fall_edge,
    dgo_pcap_cnt_wuio_both_edge,
} dgo_pcap_cnt_event_t;

/**
 * @brief pulse counter decode mode
 */
typedef enum {
    dgo_pcnt_ab_4x = 0,
    dgo_pcnt_ab_2x,
    dgo_pcnt_ab_1x,
    dgo_pcnt_pd_1x = 4,
    dgo_pcnt_pd_f_1x,
    dgo_pcnt_pd_2x,
    dgo_pcnt_pd_f_2x,
    dgo_pcnt_ud_1x,
    dgo_pcnt_ud_f_1x,
    dgo_pcnt_ud_2x,
    dgo_pcnt_ud_f_2x,
    dgo_pcnt_ud_uvw_6x,
    dgo_pcnt_ud_uvw_6y,
} dgo_pcnt_decode_mode_t;

/**
 * @brief pulse counter z phase mode
 */
typedef enum {
    dgo_pcnt_z_disable = 0,
    dgo_pcnt_z_edge_mode,
    dgo_pcnt_z_level_mode_with_ab,
    dgo_pcnt_z_level_mode,
} dgo_pcnt_z_mode_t;

/**
 * @brief pulse counter h phase mode
 */
typedef enum {
    dgo_pcnt_h_disable = 0,
    dgo_pcnt_h_edge_mode,
    dgo_pcnt_h_level_mode_with_ab,
    dgo_pcnt_h_level_mode,
} dgo_pcnt_h_mode_t;

/**
 * @brief pulse counter dump mode
 */
typedef enum {
    dgo_pcnt_dump_disable = 0,
    dgo_pcnt_dump_h_edge_mode,
    dgo_pcnt_dump_h_level_mode_with_ab,
    dgo_pcnt_dump_h_level_mode,
} dgo_pcnt_dump_mode_t;

/**
 * @brief pulse counter tacho mode
 */
typedef enum {
    dgo_pcnt_tacho_disable = 0,
    dgo_pcnt_tacho_period_mode,
    dgo_pcnt_tacho_travel_mode,
} dgo_pcnt_tacho_mode_t;

/**
 * @brief pulse counter compare mode
 */
typedef enum {
    dgo_pcnt_cmp_disable = 0,
    dgo_pcnt_cmp_range_match_mode,
    dgo_pcnt_cmp_match_mode0,
    dgo_pcnt_cmp_match_mode1,
    dgo_pcnt_cmp_match_forward_mode0,
    dgo_pcnt_cmp_match_forward_mode1,
    dgo_pcnt_cmp_match_reverse_mode0,
    dgo_pcnt_cmp_match_reverse_mode1,
} dgo_pcnt_cmp_mode_t;

/**
 * @brief pulse counter range match mode compare source
 */
typedef enum {
    dgo_pcnt_cmp_src_line_phase = 0,
    dgo_pcnt_cmp_src_revolution_line_phase,
    dgo_pcnt_cmp_src_tacho_line,
    dgo_pcnt_cmp_src_tacho_time,
} dgo_pcnt_range_cmp_src_t;

/**
 * @brief wakeup io pins filter config structure
 */
typedef struct {
    bool enable;
    bool output_invert;
    bool output_init_value;
    uint16_t filter_length;
    dgo_filter_mode_t filter_mode;
    dgo_wuio_irq_mode_t irq_mode;
} dgo_wuio_filter_cfg_t;

/**
 * @brief pulse capture config structure
 */
typedef struct {
    bool enable;
    dgo_wuio_pin_num_t wuio_select;
    dgo_pcap_match_mode_t match_mode;
    dgo_pcap_dump_mode_t dump_mode;
    dgo_pcap_cnt_dir_t cnt_dir;
    dgo_pcap_cnt_mode_t cnt_mode;
    dgo_pcap_cnt_event_t cnt_event;
    uint16_t cmp_value;
    uint16_t period_value;
} dgo_pcap_cfg_t;

typedef union {
    uint8_t state;
    struct {
        uint8_t a_level : 1;
        uint8_t b_level : 1;
        uint8_t z_edge : 1;
        uint8_t h_level : 1;
        uint8_t   : 4;
    } level_mode;
    struct {
        uint8_t rise_forward : 1;
        uint8_t fall_reverse : 1;
        uint8_t fall_forward : 1;
        uint8_t rise_reverse : 1;
        uint8_t              : 4;
    } edge_mode;
} dgo_pcnt_sig_state_t;

typedef struct {
    uint8_t phase;
    uint32_t line;
    uint32_t revolution;
} dgo_pcnt_counter_t;

/**
 * @brief pulse counter config structure
 */
typedef struct {
    bool enable;
    bool dir_invert_en;
    uint8_t phase_bias;
    dgo_wuio_pin_num_t a_sel;
    dgo_wuio_pin_num_t b_sel;
    dgo_wuio_pin_num_t z_sel;
    dgo_wuio_pin_num_t h_sel;
    dgo_pcnt_decode_mode_t decode_mode;
    uint32_t resolution;
    dgo_pcnt_z_mode_t z_mode;
    dgo_pcnt_sig_state_t z_state;
    dgo_pcnt_h_mode_t h_mode;
    dgo_pcnt_sig_state_t h_state;
    dgo_pcnt_dump_mode_t dump_mode;
    dgo_pcnt_sig_state_t dump_state;
    dgo_pcnt_tacho_mode_t tacho_mode;
    uint16_t tacho_len;
    dgo_pcnt_cmp_mode_t cmp0_mode;
    dgo_pcnt_range_cmp_src_t range_cmp_src;
    uint32_t cmp0_value;
    uint32_t cmp1_value;
} dgo_pcnt_cfg_t;

/**
 *
 * @brief PDGO driver APIs
 * @defgroup pdgo_interface DGO driver APIs
 * @ingroup pdgo_interfaces
 * @{
 *
 */

/**
 * @brief Set DGO turn-off counter
 * @param [in] ptr DGO base address
 * @param [in] counter Turn-off counter value. Clock source is 24M
 */
static inline void pdgo_set_turnoff_counter(PDGO_Type *ptr, uint32_t counter)
{
    ptr->DGO_TURNOFF = counter;
}

/**
 * @brief Enable Software Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_software_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN |= PDGO_WKUP_EN_CYC_MASK;
}

/**
 * @brief Disable Software Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_software_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN &= ~PDGO_WKUP_EN_CYC_MASK;
}

/**
 * @brief Set DGO wakeup counter
 * @param [in] ptr DGO base address
 * @param [in] counter Wakeup counter value. Clock source is 32K.
 */
static inline void pdgo_set_wakeup_counter(PDGO_Type *ptr, uint32_t counter)
{
    ptr->WKUP_CYC = 0;
    ptr->WAKEUP = counter;
}

/**
 * @brief Get DGO wakeup counter value
 * @param [in] ptr DGO base address
 *
 * @return DGO wakeup counter value
 */
static inline uint32_t pdgo_get_wakeup_counter(PDGO_Type *ptr)
{
    return ptr->WAKEUP;
}

/**
 * @brief Set DGO cycle wakeup counter
 * @param [in] ptr DGO base address
 * @param [in] counter Wakeup counter value. Clock source is 32K.
 */
static inline void pdgo_set_cycle_wakeup_counter(PDGO_Type *ptr, uint32_t counter)
{
    ptr->WKUP_CYC = counter;
}

/**
 * @brief Enable wkup pin Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_wkup_pin_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN |= PDGO_WKUP_EN_WKUP_MASK;
}

/**
 * @brief Disable wkup pin Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_wkup_pin_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN &= ~PDGO_WKUP_EN_WKUP_MASK;
}

/**
 * @brief Enable wuio pins Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_wuio_pins_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN |= PDGO_WKUP_EN_WUIO_MASK;
}

/**
 * @brief Disable wuio pins Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_wuio_pins_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN &= ~PDGO_WKUP_EN_WUIO_MASK;
}

/**
 * @brief Enable pulse capture0 Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_pulse_capture0_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN |= PDGO_WKUP_EN_PCAP0_MASK;
}

/**
 * @brief Disable pulse capture0 Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_pulse_capture0_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN &= ~PDGO_WKUP_EN_PCAP0_MASK;
}

/**
 * @brief Enable pulse counter0 Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_pulse_counter0_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN |= PDGO_WKUP_EN_PCNT0_MASK;
}

/**
 * @brief Disable pulse counter0 Wake-up feature on DGO
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_pulse_counter0_wakeup(PDGO_Type *ptr)
{
    ptr->WKUP_EN &= ~PDGO_WKUP_EN_PCNT0_MASK;
}

/**
 * @brief Enable Wake-up by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_wakeup_bit_mask_t
 */
static inline void pdgo_enable_wakeup_by_bit_mask(PDGO_Type *ptr, uint32_t mask)
{
    ptr->WKUP_EN |= mask;
}

/**
 * @brief Disable Wake-up by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_wakeup_bit_mask_t
 */
static inline void pdgo_disable_wakeup_by_bit_mask(PDGO_Type *ptr, uint32_t mask)
{
    ptr->WKUP_EN &= ~mask;
}

/**
 * @brief Get Wake-up status
 * @param [in] ptr DGO base address
 *
 * @return DGO wakeup status, @ref dgo_wakeup_bit_mask_t
 */
static inline uint32_t pdgo_get_wakeup_status(PDGO_Type *ptr)
{
    return ptr->WKUP_STS;
}

/**
 * @brief Clear Wake-up status
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_wakeup_bit_mask_t
 */
static inline void pdgo_clear_wakeup_status(PDGO_Type *ptr, uint32_t mask)
{
    ptr->WKUP_STS = mask;
}

/**
 * @brief Disable DGO all interrupts
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_all_irq(PDGO_Type *ptr)
{
    ptr->IRQ0_EN = 0;
    ptr->IRQ1_EN = 0;
}

/**
 * @brief Enable DGO interrupt0 by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq0_bit_mask_t
 */
static inline void pdgo_enable_irq0_by_bit_mask(PDGO_Type *ptr, dgo_irq0_bit_mask_t mask)
{
    ptr->IRQ0_EN |= mask;
}

/**
 * @brief Disable DGO interrupt0 by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq0_bit_mask_t
 */
static inline void pdgo_disable_irq0_by_bit_mask(PDGO_Type *ptr, dgo_irq0_bit_mask_t mask)
{
    ptr->IRQ0_EN &= ~mask;
}

/**
 * @brief Get DGO interrupt0 enable status
 * @param [in] ptr DGO base address
 */
static inline uint32_t pdgo_get_irq0_en(PDGO_Type *ptr)
{
    return ptr->IRQ0_EN;
}

/**
 * @brief Get irq0 status
 * @param [in] ptr DGO base address
 *
 * @return DGO irq0 status, @ref dgo_irq0_bit_mask_t
 */
static inline uint32_t pdgo_get_irq0_status(PDGO_Type *ptr)
{
    return ptr->IRQ0_STS;
}

/**
 * @brief Clear irq0 status
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq0_bit_mask_t
 */
static inline void pdgo_clear_irq0_status(PDGO_Type *ptr, uint32_t mask)
{
    ptr->IRQ0_STS = mask;
}

/**
 * @brief Enable DGO interrupt1 by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq0_bit_mask_t
 */
static inline void pdgo_enable_irq1_by_bit_mask(PDGO_Type *ptr, dgo_irq1_bit_mask_t mask)
{
    ptr->IRQ1_EN |= mask;
}

/**
 * @brief Disable DGO interrupt1 by bit mask
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq1_bit_mask_t
 */
static inline void pdgo_disable_irq1_by_bit_mask(PDGO_Type *ptr, dgo_irq1_bit_mask_t mask)
{
    ptr->IRQ1_EN &= ~mask;
}

/**
 * @brief Get DGO interrupt1 enable status
 * @param [in] ptr DGO base address
 */
static inline uint32_t pdgo_get_irq1_en(PDGO_Type *ptr)
{
    return ptr->IRQ1_EN;
}

/**
 * @brief Get irq1 status
 * @param [in] ptr DGO base address
 *
 * @return DGO irq1 status, @ref dgo_irq1_bit_mask_t
 */
static inline uint32_t pdgo_get_irq1_status(PDGO_Type *ptr)
{
    return ptr->IRQ1_STS;
}

/**
 * @brief Clear irq1 status
 * @param [in] ptr DGO base address
 * @param [in] mask @ref dgo_irq1_bit_mask_t
 */
static inline void pdgo_clear_irq1_status(PDGO_Type *ptr, uint32_t mask)
{
    ptr->IRQ1_STS = mask;
}

/**
 * @brief Enable DGO register retention mode
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_retention_mode(PDGO_Type *ptr)
{
    ptr->DGO_CTRL |= PDGO_DGO_CTRL_RET_MASK;
}

/**
 * @brief Check whether the DGO retention mode is enabled or not
 * @param [in] ptr DGO base address
 *
 * @retval true Retention mode is enabled
 * @retval false Retention mode is disabled
 */
static inline bool pdgo_is_retention_mode_enabled(PDGO_Type *ptr)
{
    return ((ptr->DGO_CTRL & PDGO_DGO_CTRL_RET_MASK) != 0U);
}

/**
 * @brief Disable DGO register retention mode
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_retention_mode(PDGO_Type *ptr)
{
    ptr->DGO_CTRL &= ~PDGO_DGO_CTRL_RET_MASK;
}

/**
 * @brief Enable pull-up resistor for Reset Pin
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_pullup_resistor_for_reset_pin(PDGO_Type *ptr)
{
    ptr->PIN_CTRL &= ~PDGO_PIN_CTRL_RSTN_MASK;
}

/**
 * @brief Disable pull-up resistor for Reset Pin
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_pullup_resistor_for_reset_pin(PDGO_Type *ptr)
{
    ptr->PIN_CTRL |= PDGO_PIN_CTRL_RSTN_MASK;
}

/**
 * Enable pull-down resistor for Wakeup pin
 * @param [in] ptr DGO base address
 */
static inline void pdgo_enable_pulldown_resistor_for_wakeup_pin(PDGO_Type *ptr)
{
    ptr->PIN_CTRL &= ~PDGO_PIN_CTRL_WKUP_MASK;
}

/**
 * Disable pull-down resistor for Wakeup pin
 * @param [in] ptr DGO base address
 */
static inline void pdgo_disable_pulldown_resistor_for_wakeup_pin(PDGO_Type *ptr)
{
    ptr->PIN_CTRL |= PDGO_PIN_CTRL_WKUP_MASK;
}

/**
 * Enable pull-down resistor for Wakeup IO pin
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_wuio_pin_num_t
 */
static inline void pdgo_enable_pulldown_resistor_for_wuio_pin(PDGO_Type *ptr, dgo_wuio_pin_num_t num)
{
    ptr->PIN_CTRL &= ~(PDGO_PIN_CTRL_WUIO0_MASK << num);
}

/**
 * Disable pull-down resistor for Wakeup IO pin
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_wuio_pin_num_t
 */
static inline void pdgo_disable_pulldown_resistor_for_wuio_pin(PDGO_Type *ptr, dgo_wuio_pin_num_t num)
{
    ptr->PIN_CTRL |= (PDGO_PIN_CTRL_WUIO0_MASK << num);
}

/**
 * @brief Write data to DGO GPR register
 * @param [in] ptr DGO base address
 * @param [in] index GPR register index
 * @param [in] content Data to be written to DGO GPR register
 */
static inline void pdgo_write_gpr(PDGO_Type *ptr, uint32_t index, uint32_t content)
{
    if (index < DGO_GPR_WORD_COUNT) {
        ptr->GPR[index] = content;
    }
}

/**
 * @brief Read data from DGO GPR register
 * @param [in] ptr DGO base address
 * @param [in] index GPR register index
 *
 * @return DGO GPR register value
 */
static inline uint32_t pdgo_read_gpr(PDGO_Type *ptr, uint32_t index)
{
    uint32_t reg_val = 0;
    if (index < DGO_GPR_WORD_COUNT) {
        reg_val = ptr->GPR[index];
    }
    return reg_val;
}

/**
 * @brief Convert the microsecond to DGO Wake-up counter value
 * @param [in] us microsecond to be converted
 *
 * @return Converted DGO Wake-up counter value
 */
static inline uint32_t pdgo_get_wakeup_counter_from_us(uint32_t us)
{
    return (us + DGO_WAKEUP_TICK_IN_US - 1U) / DGO_WAKEUP_TICK_IN_US;
}

/**
 * @brief Convert the DGO Wake-up counter to microseconds
 * @param [in] counter  DGO counter
 *
 * @return Converted microseconds
 */
static inline uint32_t pdgo_get_us_from_wakeup_counter(uint32_t counter)
{
    return (counter * DGO_WAKEUP_TICK_IN_US);
}

/**
 * @brief Convert the microsecond to DGO Turn-off counter value
 * @param [in] us microsecond to be converted
 *
 * @return Converted DGO Turn-off counter value
 */
static inline uint32_t pdgo_get_turnoff_counter_from_us(uint32_t us)
{
    return (us * DGO_TURNOFF_TICKS_PER_US);
}

/**
 * @brief Convert the DGO Turn-off counter to microseconds
 * @param [in] counter  DGO Turn-off counter
 *
 * @return Converted microseconds
 */
static inline uint32_t pdgo_get_us_from_turnoff_counter(uint32_t counter)
{
    return (counter + DGO_TURNOFF_TICKS_PER_US - 1U) / DGO_TURNOFF_TICKS_PER_US;
}

/**
 * @brief Set WUIO pin enable
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_wuio_pin_num_t
 * @param [in] enable true to enable, false to disable
 */
static inline void pdgo_set_wuio_pin_enable(PDGO_Type *ptr, dgo_wuio_pin_num_t num, bool enable)
{
    ptr->IOFILTER[num] = (ptr->IOFILTER[num] & ~PDGO_IOFILTER_EN_MASK) | PDGO_IOFILTER_EN_SET(enable);
}

/**
 * @brief Set PCAP enable
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcap_num_t
 * @param [in] enable true to enable, false to disable
 */
static inline void pdgo_set_pcap_enable(PDGO_Type *ptr, dgo_pcap_num_t num, bool enable)
{
    ptr->PCAP[num].CTRL = (ptr->PCAP[num].CTRL & ~PDGO_PCAP_CTRL_EN_MASK) | PDGO_PCAP_CTRL_EN_SET(enable);
}

/**
 * @brief Set PCAP counter value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcap_num_t
 * @param [in] value counter value
 */
static inline void pdgo_set_pcap_counter_value(PDGO_Type *ptr, dgo_pcap_num_t num, uint16_t value)
{
    ptr->PCAP[num].COUNTER = value;
    while (PDGO_PCAP_COUNTER_COUNTER_GET(ptr->PCAP[num].COUNTER) != value) {
        ;
    }
}

/**
 * @brief Get PCAP counter value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcap_num_t
 *
 * @return counter value
 */
static inline uint16_t pdgo_get_pcap_counter_value(PDGO_Type *ptr, dgo_pcap_num_t num)
{
    return PDGO_PCAP_COUNTER_COUNTER_GET(ptr->PCAP[num].COUNTER);
}

/**
 * @brief Set PCNT enable
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] enable true to enable, false to disable
 */
static inline void pdgo_set_pcnt_enable(PDGO_Type *ptr, dgo_pcnt_num_t num, bool enable)
{
    ptr->PCNT[num].CNT_CFG = (ptr->PCNT[num].CNT_CFG & ~PDGO_PCNT_CNT_CFG_EN_MASK) | PDGO_PCNT_CNT_CFG_EN_SET(enable);
}

/**
 * @brief Set PCNT cmp mode
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] mode cmp mode @ref dgo_pcnt_cmp_mode_t
 */
static inline void pdgo_set_pcnt_cmp_mode(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_cmp_mode_t mode)
{
    ptr->PCNT[num].HOMING_CFG = (ptr->PCNT[num].HOMING_CFG & ~PDGO_PCNT_HOMING_CFG_CMP0_MODE_MASK) | PDGO_PCNT_HOMING_CFG_CMP0_MODE_SET(mode);
}

/**
 * @brief Set PCNT cmp0 value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] value cmp0 value
 */
static inline void pdgo_set_pcnt_cmp0_value(PDGO_Type *ptr, dgo_pcnt_num_t num, uint32_t value)
{
    ptr->PCNT[num].CMP0 = value;
    while (ptr->PCNT[num].CMP0 != value) {
        ;
    }
}

/**
 * @brief Set PCNT cmp1 value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] value cmp1 value
 */
static inline void pdgo_set_pcnt_cmp1_value(PDGO_Type *ptr, dgo_pcnt_num_t num, uint32_t value)
{
    ptr->PCNT[num].CMP1 = value;
    while (ptr->PCNT[num].CMP1 != value) {
        ;
    }
}

/**
 * @brief Set PCNT raw counter value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] value counter value
 */
static inline void pdgo_set_pcnt_raw_counter_value(PDGO_Type *ptr, dgo_pcnt_num_t num, uint32_t value)
{
    uint8_t bias = PDGO_PCNT_CNT_CFG_BIAS_GET(ptr->PCNT[num].CNT_CFG);

    ptr->PCNT[num].CNT = value;
    while (ptr->PCNT[num].CNT != (value + bias)) {
        ;
    }
}

/**
 * @brief Get PCNT raw counter value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 *
 * @return counter value
 */
static inline uint32_t pdgo_get_pcnt_raw_counter_value(PDGO_Type *ptr, dgo_pcnt_num_t num)
{
    return ptr->PCNT[num].CNT;
}

/**
 * @brief Enable PCNT dir override
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 */
static inline void pdgo_enable_pcnt_dir_override(PDGO_Type *ptr, dgo_pcnt_num_t num)
{
    ptr->PCNT[num].CNT_CFG |= PDGO_PCNT_CNT_CFG_OVRD_MASK;
}

/**
 * @brief Disable PCNT dir override
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 */
static inline void pdgo_disable_pcnt_dir_override(PDGO_Type *ptr, dgo_pcnt_num_t num)
{
    ptr->PCNT[num].CNT_CFG &= ~PDGO_PCNT_CNT_CFG_OVRD_MASK;
}

/**
 * @brief Set PCNT dir override value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] reverse true:reverse, false:forward
 */
static inline void pdgo_set_pcnt_dir_override_value(PDGO_Type *ptr, dgo_pcnt_num_t num, bool reverse)
{
    ptr->PCNT[num].DIRECTION = (ptr->PCNT[num].DIRECTION & ~PDGO_PCNT_DIRECTION_DIR_MASK) | PDGO_PCNT_DIRECTION_DIR_SET(reverse);
}

/**
 * @brief Config wakeup IO filter
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_wuio_pin_num_t
 * @param [in] cfg @ref dgo_wuio_filter_cfg_t
 */
void pdgo_config_wuio_filter(PDGO_Type *ptr, dgo_wuio_pin_num_t num, dgo_wuio_filter_cfg_t *cfg);

/**
 * @brief Config pulse capture
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcap_num_t
 * @param [in] cfg @ref dgo_pcap_cfg_t
 */
void pdgo_config_pcap(PDGO_Type *ptr, dgo_pcap_num_t num, dgo_pcap_cfg_t *cfg);

/**
 * @brief Get pulse capture default config
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [out] cfg @ref dgo_pcnt_cfg_t
 */
void pdgo_get_pcnt_defconfig(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_cfg_t *cfg);

/**
 * @brief Config pulse capture
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] cfg @ref dgo_pcnt_cfg_t
 */
void pdgo_config_pcnt(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_cfg_t *cfg);

/**
 * @brief Get PCNT counter
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [out] counter data. @ref dgo_pcnt_counter_t
 */
void pdgo_get_pcnt_counter(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_counter_t *counter);

/**
 * @brief Convert PCNT counter to raw counter value
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] counter data. @ref dgo_pcnt_counter_t
 *
 * @return raw counter value
 */
uint32_t pdgo_convert_pcnt_counter(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_counter_t *counter);

/**
 * @brief Config PCNT range compare
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 * @param [in] cmp0 compare0 value
 * @param [in] cmp1 compare1 value
 */
void pdgo_config_pcnt_range_compare(PDGO_Type *ptr, dgo_pcnt_num_t num, uint32_t cmp0, uint32_t  cmp1);

/**
 * @brief Calculate PCNT tacho, unit: cycles per line
 * @param [in] ptr DGO base address
 * @param [in] num @ref dgo_pcnt_num_t
 *
 * @return tacho calculated value, unit: cycles per line
 */
float pdgo_calc_pcnt_tacho_cycles_per_line(PDGO_Type *ptr, dgo_pcnt_num_t num);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* HPM_DGO_DRV_H */
