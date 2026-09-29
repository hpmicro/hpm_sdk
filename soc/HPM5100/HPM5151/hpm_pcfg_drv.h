/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PCFG_DRV_H
#define HPM_PCFG_DRV_H

#include "hpm_common.h"
#include "hpm_pcfg_regs.h"

/**
 *
 * @brief PCFG driver APIs
 * @defgroup pcfg_hpm5151_interface PCFG driver APIs for HPM5151
 * @ingroup io_interfaces
 * @{
 */
#define PCFG_CLOCK_GATE_MODE_ALWAYS_ON          (0x3UL)
#define PCFG_CLOCK_GATE_MODE_ALWAYS_OFF         (0x2UL)

#define PCFG_PERIPH_KEEP_CLOCK_ON(p) (PCFG_CLOCK_GATE_MODE_ALWAYS_ON << (p))
#define PCFG_PERIPH_KEEP_CLOCK_OFF(p) (PCFG_CLOCK_GATE_MODE_ALWAYS_OFF << (p))

/* @brief PCFG irc24m reference */
typedef enum {
    pcfg_irc24m_reference_32k = 0,
    pcfg_irc24m_reference_24m_xtal = 1
} pcfg_irc24m_reference_t;

/* @brief PCFG pmc domain peripherals */
typedef enum {
    pcfg_pmc_periph_gpio = 6,
    pcfg_pmc_periph_ioc = 8,
    pcfg_pmc_periph_timer = 10,
    pcfg_pmc_periph_wdog = 12,
    pcfg_pmc_periph_uart = 14,
} pcfg_pmc_periph_t;

/* @brief PCFG wakeup source */
typedef enum {
    pcfg_wakeup_src_soc = (1 << 0),
    pcfg_wakeup_src_puart = (1 << 7),
    pcfg_wakeup_src_ptimer = (1 << 8),
    pcfg_wakeup_src_pwdg = (1 << 9),
    pcfg_wakeup_src_pgpio = (1 << 10),
    pcfg_wakeup_src_wkup = (1 << 31),
} pcfg_wakeup_src_t;

/* @brief PCFG status */
enum {
    status_pcfg_ldo_out_of_range = MAKE_STATUS(status_group_pcfg, 1),
};

/* @brief PCFG irc24m config */
typedef struct {
    uint32_t freq_in_hz;
    pcfg_irc24m_reference_t reference;
    bool return_to_default_on_xtal_loss;
    bool free_run;
} pcfg_irc24m_config_t;


#define PCFG_CLOCK_GATE_CONTROL_MASK(module, mode) \
    ((uint32_t) (mode) << ((module) << 1))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief check if bandgap is trimmed or not
 *
 * @param[in] ptr base address
 *
 * @retval true if bandgap is trimmed
 */
static inline bool pcfg_bandgap_is_trimmed(PCFG_Type *ptr)
{
    return ptr->BANDGAP & PCFG_BANDGAP_VBG_TRIMMED_MASK;
}

/**
 * @brief bandgap reload trim value
 *
 * @param[in] ptr base address
 */
static inline void pcfg_bandgap_reload_trim(PCFG_Type *ptr)
{
    ptr->BANDGAP &= ~PCFG_BANDGAP_VBG_TRIMMED_MASK;
}

/**
 * @brief turn off LDO2P5
 *
 * @param[in] ptr base address
 */
static inline void pcfg_ldo2p5_turn_off(PCFG_Type *ptr)
{
    ptr->LDO2P5 &= ~PCFG_LDO2P5_ENABLE_MASK;
}

/**
 * @brief turn on LDO 2.5V
 *
 * @param[in] ptr base address
 */
static inline void pcfg_ldo2p5_turn_on(PCFG_Type *ptr)
{
    ptr->LDO2P5 |= PCFG_LDO2P5_ENABLE_MASK;
}

/**
 * @brief check if LDO 2.5V is stable
 *
 * @param[in] ptr base address
 *
 * @retval true if LDO2P5 is stable
 */
static inline bool pcfg_ldo2p5_is_stable(PCFG_Type *ptr)
{
    return PCFG_LDO2P5_READY_GET(ptr->LDO2P5);
}

/**
 * @brief clear wakeup cause flag
 *
 * @param[in] ptr base address
 * @param[in] mask mask of flags to be cleared
 */
static inline void pcfg_clear_wakeup_cause(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_CAUSE = mask;
}

/**
 * @brief get wakeup cause
 *
 * @param[in] ptr base address
 *
 * @retval mask of wake cause
 */
static inline uint32_t pcfg_get_wakeup_cause(PCFG_Type *ptr)
{
    return ptr->WAKE_CAUSE;
}

/**
 * @brief enable wakeup source
 *
 * @param[in] ptr base address
 * @param[in] mask wakeup source mask
 */
static inline void pcfg_enable_wakeup_source(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_MASK &= ~mask;
}

/**
 * @brief disable wakeup source
 *
 * @param[in] ptr base address
 * @param[in] mask source to be disabled as wakeup source
 */
static inline void pcfg_disable_wakeup_source(PCFG_Type *ptr, uint32_t mask)
{
    ptr->WAKE_MASK |= mask;
}

/**
 * @brief set clock gate mode in vpmc domain
 *
 * @param[in] ptr base address
 * @param[in] mode clock gate mode mask
 */
static inline void pcfg_set_periph_clock_mode(PCFG_Type *ptr, uint32_t mode)
{
    ptr->SCG_CTRL = mode;
}

/**
 * @brief update clock gate mode in vpmc domain
 *
 * @param[in] ptr base address
 * @param[in] periph peripherals to be updated
 * @param[in] on true - always on, false - always off
 */
static inline void pcfg_update_periph_clock_mode(PCFG_Type *ptr, pcfg_pmc_periph_t periph, bool on)
{
    if (on) {
        ptr->SCG_CTRL = (ptr->SCG_CTRL & ~(0x03 << periph)) | PCFG_PERIPH_KEEP_CLOCK_ON(periph);
    } else {
        ptr->SCG_CTRL = (ptr->SCG_CTRL & ~(0x03 << periph)) | PCFG_PERIPH_KEEP_CLOCK_OFF(periph);
    }
}

/**
 * @brief check if irc24m is trimmed
 *
 * @param[in] ptr base address
 *
 * @retval true if it is trimmed
 */
static inline bool pcfg_irc24m_is_trimmed(PCFG_Type *ptr)
{
    return ptr->RC24M & PCFG_RC24M_RC_TRIMMED_MASK;
}

/**
 * @brief reload irc24m trim value
 *
 * @param[in] ptr base address
 */
static inline void pcfg_irc24m_reload_trim(PCFG_Type *ptr)
{
    ptr->RC24M &= ~PCFG_RC24M_RC_TRIMMED_MASK;
}

/**
 * @brief config irc24m track
 *
 * @param[in] ptr base address
 * @param[in] config config data
 */
void pcfg_irc24m_config_track(PCFG_Type *ptr, pcfg_irc24m_config_t *config);

/*
 * @brief set output voltage of LDO 2.5V in mV
 * @param[in] ptr base address
 * @param[in] mv target voltage
 * @retval status_success if successfully configured
 */
hpm_stat_t pcfg_ldo2p5_set_voltage(PCFG_Type *ptr, uint16_t mv);

/*
 * @brief set output voltage of LDO 1V in mV
 * @param[in] ptr base address
 * @param[in] mv target voltage
 * @retval status_success if successfully configured
 */
hpm_stat_t pcfg_ldo1p1_set_voltage(PCFG_Type *ptr, uint16_t mv);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* HPM_PCFG_DRV_H */
