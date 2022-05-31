/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PCFG_DRV_H
#define HPM_PCFG_DRV_H

#include "hpm_common.h"
#include "hpm_pcfg_regs.h"

#define PCFG_DCDC_MODE_TURN_OFF   (0U)
#define PCFG_DCDC_MODE_PERFORMACE (1U)
#define PCFG_DCDC_MODE_GENERIC    (2U)
#define PCFG_DCDC_MODE_AUTOMATIC  (3U)
#define PCFG_DCDC_MODE_EXPERT     (4U)

#define PCFG_CLOCK_GATE_MODE_ALWAYS_ON          (0x11U)
#define PCFG_CLOCK_GATE_MODE_ALWAYS_OFF         (0x10U)
#define PCFG_CLOCK_GATE_MODE_ALWAYS_FOLLOW_FLOW (0x01U)

typedef enum {
    pcfg_module_fuse = 0,
    pcfg_module_sram,
    pcfg_module_vad,
    pcfg_module_gpio,
    pcfg_module_ioc,
    pcfg_module_timer,
    pcfg_module_wdog,
    pcfg_module_uart,
    pcfg_module_debug,
} pcfg_module_t;

typedef enum {
    pcfg_irc24m_reference_32k = 0,
    pcfg_irc24m_reference_24m_xtal = 1
} pcfg_irc24m_reference_t;

enum {
    status_pcfg_ldo_out_of_range = MAKE_STATUS(status_group_pcfg, 1),
};

typedef struct {
    uint32_t freq_in_hz;
    pcfg_irc24m_reference_t reference;
    bool return_to_default_on_xtal_loss;
    bool free_run;
} pcfg_irc24m_config_t;


#define PCFG_CLOCK_GATE_CONTROL_MASK(module, mode) \
    ((uint32_t) (mode) << ((module) << 1))

#define PCFG_DEBUG_STOP_SOURCE_ENABLE_CORE0 (PCFG_DEBUG_STOP_CPU0_MASK)
#define PCFG_DEBUG_STOP_SOURCE_DISABLE_CORE0 (0)
#define PCFG_DEBUG_STOP_SOURCE_ENABLE_CORE1 (PCFG_DEBUG_STOP_CPU1_MASK)
#define PCFG_DEBUG_STOP_SOURCE_DISABLE_CORE1 (0)

#ifdef __cplusplus
extern "C" {
#endif

static inline void pcfg_bandgap_disable_power_save_mode(PCFG_Type *ptr)
{
    ptr->BANDGAP &= ~PCFG_BANDGAP_POWER_SAVE_MASK;
}

static inline void pcfg_bandgap_enable_power_save_mode(PCFG_Type *ptr)
{
    ptr->BANDGAP |= PCFG_BANDGAP_POWER_SAVE_MASK;
}

static inline void pcfg_bandgap_disable_lowpower_mode(PCFG_Type *ptr)
{
    ptr->BANDGAP &= ~PCFG_BANDGAP_LOWPOWER_MODE_MASK;
}

static inline void pcfg_bandgap_enable_lowpower_mode(PCFG_Type *ptr)
{
    ptr->BANDGAP |= PCFG_BANDGAP_LOWPOWER_MODE_MASK;
}

static inline bool pcfg_bandgap_is_trimmed(PCFG_Type *ptr)
{
    return ptr->BANDGAP & PCFG_BANDGAP_VBG_TRIMMED_MASK;
}

static inline void pcfg_bandgap_reload_trim(PCFG_Type *ptr)
{
    ptr->BANDGAP &= ~PCFG_BANDGAP_VBG_TRIMMED_MASK;
}

static inline void pcfg_ldo1p1_turn_off(PCFG_Type *ptr)
{
    ptr->LDO1P1 &= ~PCFG_LDO1P1_ENABLE_MASK;
}

static inline void pcfg_ldo1p1_turn_on(PCFG_Type *ptr)
{
    ptr->LDO1P1 |= PCFG_LDO1P1_ENABLE_MASK;
}

/*
 * set output voltage of LDO 1V in mV
 */
hpm_stat_t pcfg_ldo1p1_set_voltage(PCFG_Type *ptr, uint16_t mv);

static inline void pcfg_ldo2p5_turn_off(PCFG_Type *ptr)
{
    ptr->LDO2P5 &= ~PCFG_LDO2P5_ENABLE_MASK;
}

static inline void pcfg_ldo2p5_turn_on(PCFG_Type *ptr)
{
    ptr->LDO2P5 |= PCFG_LDO2P5_ENABLE_MASK;
}

/*
 * set output voltage of LDO 2.5V in mV
 */
hpm_stat_t pcfg_ldo2p5_set_voltage(PCFG_Type *ptr, uint16_t mv);

/*
 * set DCDC voltage at standby mode
 */
hpm_stat_t pcfg_dcdc_set_lpmode_voltage(PCFG_Type *ptr, uint16_t mv);

static inline void pcfg_dcdc_enable_protection(PCFG_Type *ptr, uint32_t mask)
{
    ptr->DCDC_PROT |= mask;
}

static inline void pcfg_dcdc_disable_protection(PCFG_Type *ptr, uint32_t mask)
{
    ptr->DCDC_PROT &= ~mask;
}

static inline void pcfg_dcdc_disable_measure_current(PCFG_Type *ptr)
{
    ptr->DCDC_CURRENT &= ~PCFG_DCDC_CURRENT_ESTI_EN_MASK;
}

static inline void pcfg_dcdc_enable_measure_current(PCFG_Type *ptr)
{
    ptr->DCDC_CURRENT |= PCFG_DCDC_CURRENT_ESTI_EN_MASK;
}

static inline bool pcfg_dcdc_is_measure_current_valid(PCFG_Type *ptr)
{
    return ptr->DCDC_CURRENT & PCFG_DCDC_CURRENT_VALID_MASK;
}

/*
 * get current DCDC current level in mA
 */
uint16_t pcfg_dcdc_get_current_level(PCFG_Type *ptr);

/*
 * set DCDC start/resume time in number of 24MHz clock cycles
 */
static inline uint32_t pcfg_get_dcdc_start_time_in_cycle(PCFG_Type *ptr)
{
    return PCFG_DCDC_START_TIME_START_TIME_GET(ptr->DCDC_START_TIME);
}

static inline uint32_t pcfg_get_dcdc_resume_time_in_cycle(PCFG_Type *ptr)
{
    return PCFG_DCDC_RESUME_TIME_RESUME_TIME_GET(ptr->DCDC_RESUME_TIME);
}

static inline void pcfg_set_dcdc_start_time_in_cycle(PCFG_Type *ptr, uint32_t cycles)
{
    ptr->DCDC_START_TIME = PCFG_DCDC_START_TIME_START_TIME_SET(cycles);
}

static inline void pcfg_set_dcdc_resume_time_in_cycle(PCFG_Type *ptr, uint32_t cycles)
{
    ptr->DCDC_RESUME_TIME = PCFG_DCDC_RESUME_TIME_RESUME_TIME_SET(cycles);
}

static inline void pcfg_disable_power_trap(PCFG_Type *ptr)
{
    ptr->POWER_TRAP &= ~PCFG_POWER_TRAP_TRAP_MASK;
}

static inline void pcfg_enable_power_trap(PCFG_Type *ptr)
{
    ptr->POWER_TRAP |= PCFG_POWER_TRAP_TRAP_MASK;
}

static inline bool pcfg_is_power_trap_triggered(PCFG_Type *ptr)
{
    return ptr->POWER_TRAP & PCFG_POWER_TRAP_TRIGGERED_MASK;
}

static inline void pcfg_disable_dcdc_retention(PCFG_Type *ptr)
{
    ptr->POWER_TRAP &= ~PCFG_POWER_TRAP_RETENTION_MASK;
}

static inline void pcfg_enable_dcdc_retention(PCFG_Type *ptr)
{
    ptr->POWER_TRAP |= PCFG_POWER_TRAP_RETENTION_MASK;
}

static inline void pcfg_clear_wakeup_cause(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_CAUSE |= mask;
}

static inline uint32_t pcfg_get_wakeup_cause(PCFG_Type *ptr)
{
    return ptr->WAKE_CAUSE;
}

static inline void pcfg_enable_wakeup_source(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_MASK &= ~mask;
}

static inline void pcfg_disable_wakeup_source(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_MASK |= mask;
}

static inline void pcfg_set_clock_gate_in_pmic(PCFG_Type *ptr, uint32_t mode)
{
    ptr->SCG_CTRL = mode;
}

static inline void pcfg_config_debug_stop_source(PCFG_Type *ptr, uint8_t mask)
{
    ptr->DEBUG_STOP = mask;
}

static inline bool pcfg_irc24m_is_trimmed(PCFG_Type *ptr)
{
    return ptr->RC24M & PCFG_RC24M_RC_TRIMMED_MASK;
}

static inline void pcfg_irc24m_reload_trim(PCFG_Type *ptr)
{
    ptr->RC24M &= ~PCFG_RC24M_RC_TRIMMED_MASK;
}

void pcfg_irc24m_config_track(PCFG_Type *ptr, pcfg_irc24m_config_t *config);

#ifdef __cplusplus
}
#endif
#endif /* HPM_PMIC_PCFG_DRV_H */
