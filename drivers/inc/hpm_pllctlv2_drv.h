/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PLLCTLV2_DRV_H
#define HPM_PLLCTLV2_DRV_H


#include "hpm_common.h"
#include "hpm_soc_feature.h"
#include "hpm_pllctlv2_regs.h"

#define PLLCTLV2_CLK_SRC_XTAL24M (0U)
#define PLLCTLV2_CLK_SRC_IRC24M (1U)

typedef enum {
    pllctlv2_pll0,
    pllctlv2_pll1,
    pllctlv2_pll2,
    pllctlv2_pll3,
    pllctlv2_pll4,
    pllctlv2_pll5,
    pllctlv2_pll6,
} pllctlv2_pll_t;

typedef enum {
    pllctlv2_clk0,
    pllctlv2_clk1,
    pllctlv2_clk2,
    pllctlv2_clk3,
} pllctlv2_clk_t;

typedef enum {
    pllctlv2_div_1p0,    /* div 1.0 */
    pllctlv2_div_1p2,    /* div 1.2 */
    pllctlv2_div_1p4,    /* div 1.4 */
    pllctlv2_div_1p6,    /* div 1.6 */
    pllctlv2_div_1p8,    /* div 1.8 */
    pllctlv2_div_2p0,    /* div 2.0 */
    pllctlv2_div_2p2,    /* div 2.2 */
    pllctlv2_div_2p4,    /* div 2.4 */
    pllctlv2_div_2p6,    /* div 2.6 */
    pllctlv2_div_2p8,    /* div 2.8 */
    pllctlv2_div_3p0,    /* div 3.0 */
    pllctlv2_div_3p2,    /* div 3.2 */
    pllctlv2_div_3p4,    /* div 3.4 */
    pllctlv2_div_3p6,    /* div 3.6 */
    pllctlv2_div_3p8,    /* div 3.8 */
    pllctlv2_div_4p0,    /* div 4.0 */
    pllctlv2_div_4p2,    /* div 4.2 */
    pllctlv2_div_4p4,    /* div 4.4 */
    pllctlv2_div_4p6,    /* div 4.6 */
    pllctlv2_div_4p8,    /* div 4.8 */
    pllctlv2_div_5p0,    /* div 5.0 */
    pllctlv2_div_5p2,    /* div 5.2 */
    pllctlv2_div_5p4,    /* div 5.4 */
    pllctlv2_div_5p6,    /* div 5.6 */
    pllctlv2_div_5p8,    /* div 5.8 */
    pllctlv2_div_6p0,    /* div 6.0 */
    pllctlv2_div_6p2,    /* div 6.2 */
    pllctlv2_div_6p4,    /* div 6.4 */
    pllctlv2_div_6p6,    /* div 6.6 */
    pllctlv2_div_6p8,    /* div 6.8 */
    pllctlv2_div_7p0,    /* div 7.0 */
    pllctlv2_div_7p2,    /* div 7.2 */
    pllctlv2_div_7p4,    /* div 7.4 */
    pllctlv2_div_7p6,    /* div 7.6 */
    pllctlv2_div_7p8,    /* div 7.8 */
    pllctlv2_div_8p0,    /* div 8.0 */
    pllctlv2_div_8p2,    /* div 8.2 */
    pllctlv2_div_8p4,    /* div 8.4 */
    pllctlv2_div_8p6,    /* div 8.6 */
    pllctlv2_div_8p8,    /* div 8.8 */
    pllctlv2_div_9p0,    /* div 9.0 */
    pllctlv2_div_9p2,    /* div 9.2 */
    pllctlv2_div_9p4,    /* div 9.4 */
    pllctlv2_div_9p6,    /* div 9.6 */
    pllctlv2_div_9p8,    /* div 9.8 */
    pllctlv2_div_10p0,   /* div 10.0 */
    pllctlv2_div_10p2,   /* div 10.2 */
    pllctlv2_div_10p4,   /* div 10.4 */
    pllctlv2_div_10p6,   /* div 10.6 */
    pllctlv2_div_10p8,   /* div 10.8 */
    pllctlv2_div_11p0,   /* div 11.0 */
    pllctlv2_div_11p2,   /* div 11.2 */
    pllctlv2_div_11p4,   /* div 11.4 */
    pllctlv2_div_11p6,   /* div 11.6 */
    pllctlv2_div_11p8,   /* div 11.8 */
    pllctlv2_div_12p0,   /* div 12.0 */
    pllctlv2_div_12p2,   /* div 12.2 */
    pllctlv2_div_12p4,   /* div 12.4 */
    pllctlv2_div_12p6,   /* div 12.6 */
    pllctlv2_div_12p8,   /* div 12.8 */
    pllctlv2_div_13p0,   /* div 13.0 */
    pllctlv2_div_13p2,   /* div 13.2 */
    pllctlv2_div_13p4,   /* div 13.4 */
    pllctlv2_div_13p6,   /* div 13.6 */
 } pllctlv2_div_t;

/**
 * @brief PLLCTLV2 driver APIs
 * @defgroup pllctlv2_interface PLLCTLV2 driver APIs
 * @{
 */

/**
 * @brief Check if external crystal is stable
 * @param [in] ptr PLLCTLV2 base address
 * @return true if external crystal is stable
 */
static inline bool pllctlv2_xtal_is_stable(PLLCTLV2_Type *ptr)
{
    return IS_HPM_BITMASK_SET(ptr->XTAL, PLLCTLV2_XTAL_RESPONSE_MASK);
}

/**
 * @brief Check if external crystal is enabled
 * @param [in] ptr PLLCTLV2 base address
 * @return true if external crystal is enabled
 */
static inline bool pllctlv2_xtal_is_enabled(PLLCTLV2_Type *ptr)
{
    return IS_HPM_BITMASK_SET(ptr->XTAL, PLLCTLV2_XTAL_ENABLE_MASK);
}

/**
 * @brief Set external crystal ramp-up time
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] rc24m_cycles Cycles of RC24M clock
 */
static inline void pllctlv2_xtal_set_rampup_time(PLLCTLV2_Type *ptr, uint32_t rc24m_cycles)
{
    ptr->XTAL = (ptr->XTAL & ~PLLCTLV2_XTAL_RAMP_TIME_MASK) | PLLCTLV2_XTAL_RAMP_TIME_SET(rc24m_cycles);
}

/**
 * @brief Check is PLL is stable
 * @param [in] ptr PLLCTLv2 base address
 * @param [in] pll PLL index
 * @return true if PLL is stable
 */
static inline bool pllctlv2_pll_is_stable(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    return IS_HPM_BITMASK_SET(ptr->PLL[pll].MFI, PLLCTLV2_PLL_MFI_RESPONSE_MASK);
}

/**
 * @brief Check if PLL is enabled
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 * @return true if PLL is enabled
 */
static inline bool pllctlv2_pll_is_enabled(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    return IS_HPM_BITMASK_SET(ptr->PLL[pll].MFI, PLLCTLV2_PLL_MFI_ENABLE_MASK);
}

/**
 * @brief Select the PLL reference clock
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 * @param [in] src PLL reference lcock source
 * @arg 0 - XTAL24M
 * @arg 1 - IRC24M
 */
static inline void pllctlv2_select_reference_clock(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint8_t src)
{
    ptr->PLL[pll].CONFIG = (ptr->PLL[pll].CONFIG & ~PLLCTLV2_PLL_CONFIG_REFSEL_MASK) | PLLCTLV2_PLL_CONFIG_REFSEL_SET(src);
}

/**
 * @brief Enable PLL Spread Spectrum feature
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 * @param [in] step Step of spread spectrum modulator
 * @param [in] stop Stop point of spread spectrum modulator
 */
void pllctlv2_enable_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t step, uint32_t stop);

/**
 * @brief Disable PLL Spread spectrum
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 */
static inline void pllctlv2_disable_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    ptr->PLL[pll].CONFIG &= ~PLLCTLV2_PLL_CONFIG_SPREAD_MASK;
}

/**
 * @brief Set PLL lock time
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 * @param [in] xtal_cycles external Crystal cycles
 */
static inline void pllctlv2_set_pll_lock_time(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t xtal_cycles)
{
    ptr->PLL[pll].LOCKTIME = PLLCTLV2_PLL_LOCKTIME_LOCKTIME_SET(xtal_cycles);
}

/**
 * @brief Set PLL step time
 * @param [in] ptr PLLCTLV2 base address
 * @param [in] pll PLL index
 * @param [in] xtal_cycles external Crystal cycles
 */
static inline void pllctlv2_set_pll_step_time(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t xtal_cycles)
{
    ptr->PLL[pll].STEPTIME = PLLCTLV2_PLL_STEPTIME_STEPTIME_SET(xtal_cycles);
}

/**
 * @brief Set PLL Post divider
 * @param [in] ptr PLLCTLV2 base
 * @param [in] pll PLL index
 * @param [in] clk Divider index
 * @param [in] div_value divider value, divider factor is 1 + div_value / 5
 */
void pllctlv2_set_postdiv(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll,  pllctlv2_clk_t clk, pllctlv2_div_t div_value);

/**
 * @brief Set the PLL via the low-level MFI, MFD and MFN
 *        PLL frequency = REF CLOCK * (mfi + 1.0 * mfn / mfd)
 * @param [in] ptr  PLLCTLV2 base
 * @param [in] pll  PLL index
 * @param [in] mfi  MFI value
 * @param [in] mfn  MFN value
 * @retval status_invalid_argument some parameters are invalid
 * @retval status_success operation is successful
 */
hpm_stat_t pllctlv2_set_pll_with_mfi_mfn(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll,  uint32_t mfi, uint32_t mfn);

/**
 * @brief Initialize PLL to specified frequency
 *        Note: the specified PLL clock needs to be enabled before being configured
 * @param [in] ptr PLLCTLV2 base
 * @param [in] pll PLL index
 * @param [in] freq_in_hz expected PLL frequency
 * @retval status_invalid_argument some parameters are invalid
 * @retval status_success operation is successful
 */
hpm_stat_t pllctlv2_init_pll_with_freq(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t freq_in_hz);

/**
 * @brief Get the specified PLl clock frequency
 * @param [in] ptr PLLCTLV2 base
 * @param [in] pll PLL index
 * @return PLL frequency in Hz
 */
uint32_t pllctlv2_get_pll_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll);

/**
 * @brief Get the selected PLL post divider frequency
 * @param [in] ptr PLLCTLV2 base
 * @param [in] pll PLL index
 * @param [in] clk Post divider index
 * @return PLL frequency in Hz
 */
uint32_t pllctlv2_get_pll_postdiv_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk);


/**
 * @}
 */
#endif /* HPM_PLLCTLV2_DRV_H */
