/*
 * Copyright (c) 2022-2025 HPMicro
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
 * @brief Checks the stability status of the external crystal oscillator
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @return true if the external crystal oscillator is stable and ready for use
 */
static inline bool pllctlv2_xtal_is_stable(PLLCTLV2_Type *ptr)
{
    uint32_t status = ptr->XTAL;
    return (IS_HPM_BITMASK_CLR(status, PLLCTLV2_XTAL_ENABLE_MASK)
         || (IS_HPM_BITMASK_CLR(status, PLLCTLV2_XTAL_BUSY_MASK) && IS_HPM_BITMASK_SET(status, PLLCTLV2_XTAL_RESPONSE_MASK)));
}

/**
 * @brief Checks if the external crystal oscillator is enabled
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @return true if the external crystal oscillator is enabled, false otherwise
 */
static inline bool pllctlv2_xtal_is_enabled(PLLCTLV2_Type *ptr)
{
    return IS_HPM_BITMASK_SET(ptr->XTAL, PLLCTLV2_XTAL_ENABLE_MASK);
}

/**
 * @brief Configures the ramp-up time for the external crystal oscillator
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] rc24m_cycles Number of RC24M clock cycles for the ramp-up period
 * @note The ramp-up time affects how quickly the crystal oscillator reaches stable operation
 */
static inline void pllctlv2_xtal_set_rampup_time(PLLCTLV2_Type *ptr, uint32_t rc24m_cycles)
{
    ptr->XTAL = (ptr->XTAL & ~PLLCTLV2_XTAL_RAMP_TIME_MASK) | PLLCTLV2_XTAL_RAMP_TIME_SET(rc24m_cycles);
}

/**
 * @brief Checks if the specified PLL has achieved stable operation
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to check (pllctlv2_pll0 through pllctlv2_pll6)
 * @return true if the PLL is stable and locked, false otherwise
 */
static inline bool pllctlv2_pll_is_stable(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    uint32_t status = ptr->PLL[pll].MFI;
    return (IS_HPM_BITMASK_CLR(status, PLLCTLV2_PLL_MFI_ENABLE_MASK)
         || (IS_HPM_BITMASK_CLR(status, PLLCTLV2_PLL_MFI_BUSY_MASK) && IS_HPM_BITMASK_SET(status, PLLCTLV2_PLL_MFI_RESPONSE_MASK)));
}

/**
 * @brief Checks if the specified PLL is enabled
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to check (pllctlv2_pll0 through pllctlv2_pll6)
 * @return true if the PLL is enabled, false otherwise
 */
static inline bool pllctlv2_pll_is_enabled(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    return IS_HPM_BITMASK_SET(ptr->PLL[pll].MFI, PLLCTLV2_PLL_MFI_ENABLE_MASK);
}

/**
 * @brief Checks if the specified PLL CLK has achieved stable operation
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to check (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] clk Post-divider output index (pllctlv2_clk0 through pllctlv2_clk3)
 * @return true if the PLL CLK is stable and locked, false otherwise
 */
static inline bool pllctlv2_pll_clk_is_stable(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk)
{
    uint32_t status = ptr->PLL[pll].DIV[clk];
    return (IS_HPM_BITMASK_CLR(status, PLLCTLV2_PLL_DIV_ENABLE_MASK)
         || (IS_HPM_BITMASK_CLR(status, PLLCTLV2_PLL_DIV_BUSY_MASK) && IS_HPM_BITMASK_SET(status, PLLCTLV2_PLL_DIV_RESPONSE_MASK)));
}

/**
 * @brief Selects the reference clock source for the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] src Reference clock source selection:
 *                 - 0: External 24MHz crystal (XTAL24M)
 *                 - 1: Internal 24MHz RC oscillator (IRC24M)
 */
static inline void pllctlv2_select_reference_clock(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint8_t src)
{
    ptr->PLL[pll].CONFIG = (ptr->PLL[pll].CONFIG & ~PLLCTLV2_PLL_CONFIG_REFSEL_MASK) | PLLCTLV2_PLL_CONFIG_REFSEL_SET(src);
}

/**
 * @brief Enables and configures the spread spectrum modulation for the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] step Modulation step size for spread spectrum
 * @param [in] stop Maximum modulation point for spread spectrum
 * @note Spread spectrum modulation helps reduce electromagnetic interference (EMI)
 */
void pllctlv2_enable_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t step, uint32_t stop);

/**
 * @brief Disables the spread spectrum modulation for the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 */
static inline void pllctlv2_disable_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    ptr->PLL[pll].CONFIG &= ~PLLCTLV2_PLL_CONFIG_SPREAD_MASK;
}

/**
 * @brief Sets the lock time for the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] xtal_cycles Number of external crystal clock cycles to wait for PLL lock
 * @note The lock time determines how long the PLL waits to achieve stable operation
 */
static inline void pllctlv2_set_pll_lock_time(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t xtal_cycles)
{
    ptr->PLL[pll].LOCKTIME = PLLCTLV2_PLL_LOCKTIME_LOCKTIME_SET(xtal_cycles);
}

/**
 * @brief Sets the step time for frequency changes in the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] xtal_cycles Number of external crystal clock cycles between frequency steps
 * @note Step time affects how quickly the PLL can change its output frequency
 */
static inline void pllctlv2_set_pll_step_time(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t xtal_cycles)
{
    ptr->PLL[pll].STEPTIME = PLLCTLV2_PLL_STEPTIME_STEPTIME_SET(xtal_cycles);
}

/**
 * @brief Configures the post-divider for a specific PLL output clock
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] clk Post-divider output index (pllctlv2_clk0 through pllctlv2_clk3)
 * @param [in] div_value Divider value selection (pllctlv2_div_1p0 through pllctlv2_div_13p6)
 * @note The actual divider ratio is calculated as 1 + div_value/5
 */
void pllctlv2_set_postdiv(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk, pllctlv2_div_t div_value);

/**
 * @brief Configures the PLL frequency using direct MFI and MFN values
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] mfi Integer multiplication factor
 * @param [in] mfn Fractional multiplication factor numerator
 * @return status_success if configuration is valid and applied successfully
 * @return status_invalid_argument if any parameter is out of valid range
 * @note PLL frequency = REF_CLOCK * (mfi + 1.0 * mfn / mfd)
 */
hpm_stat_t pllctlv2_set_pll_with_mfi_mfn(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t mfi, uint32_t mfn);

/**
 * @brief Initializes the PLL to generate a specific output frequency
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] freq_in_hz Desired PLL output frequency in Hertz
 * @return status_success if PLL is configured successfully
 * @return status_invalid_argument if requested frequency is not achievable
 * @note The PLL must be enabled before calling this function
 */
hpm_stat_t pllctlv2_init_pll_with_freq(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t freq_in_hz);

/**
 * @brief Retrieves the current output frequency of the specified PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to query (pllctlv2_pll0 through pllctlv2_pll6)
 * @return Current PLL output frequency in Hertz
 */
uint32_t pllctlv2_get_pll_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll);

/**
 * @brief Retrieves the frequency of a specific PLL post-divider output
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to query (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] clk Post-divider output index (pllctlv2_clk0 through pllctlv2_clk3)
 * @return Frequency of the selected post-divider output in Hertz
 */
uint32_t pllctlv2_get_pll_postdiv_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk);

/**
 * @brief Configures spread spectrum modulation parameters for a PLL
 * @param [in] ptr Base address of the PLLCTLV2 peripheral
 * @param [in] pll Index of the PLL to configure (pllctlv2_pll0 through pllctlv2_pll6)
 * @param [in] spread_range Spread spectrum range as a percentage (0.x%)
 * @param [in] modulation_freq Desired modulation frequency in Hertz
 * @note This function automatically calculates appropriate step and stop values
 *       based on the provided spread range and modulation frequency
 */
void pllctlv2_setup_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint8_t spread_range, uint32_t modulation_freq);

/**
 * @}
 */
#endif /* HPM_PLLCTLV2_DRV_H */
