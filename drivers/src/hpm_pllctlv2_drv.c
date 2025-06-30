/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pllctlv2_drv.h"


#define PLLCTLV2_PLL_MFN_FACTOR (10U)                       /*!< PLLCTLV2 PLL MFN Factor */
#define PLLCTLV2_PLL_MFD_DEFAULT (240UL * 1000000UL)        /*!< PLLCTLV2 PLL Default MFD value */

#define PLLCTLV2_PLL_MFI_MIN (16U)
#define PLLCTLV2_PLL_MFI_MAX (42U)
#define PLLCTLV2_PLL_XTAL_FREQ (24000000UL)

#define PLLCTLV2_PLL_FREQ_MIN (PLLCTLV2_PLL_MFI_MIN * PLLCTLV2_PLL_XTAL_FREQ)
#define PLLCTLV2_PLL_FREQ_MAX ((PLLCTLV2_PLL_MFI_MAX + 1U) * PLLCTLV2_PLL_XTAL_FREQ)

#ifndef FREQ_1MHz
#define FREQ_1MHz (1000000UL)
#endif

hpm_stat_t pllctlv2_set_pll_with_mfi_mfn(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t mfi, uint32_t mfn)
{
    hpm_stat_t status;
    if ((ptr == NULL) || (mfi < PLLCTLV2_PLL_MFI_MIN) || (mfi > PLLCTLV2_PLL_MFI_MAX) ||
        (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        status = status_invalid_argument;
    } else {
        if (PLLCTLV2_PLL_MFI_MFI_GET(ptr->PLL[pll].MFI) == mfi) {
            ptr->PLL[pll].MFI = mfi - 1U;
        }
        ptr->PLL[pll].MFI = mfi;
        ptr->PLL[pll].MFN = mfn;

        while (!pllctlv2_pll_is_stable(ptr, pll)) {
            NOP();
        }

        status = status_success;
    }
    return status;
}

hpm_stat_t pllctlv2_init_pll_with_freq(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t freq_in_hz)
{
    hpm_stat_t status;
    if ((ptr == NULL) || (freq_in_hz < PLLCTLV2_PLL_FREQ_MIN) || (freq_in_hz > PLLCTLV2_PLL_FREQ_MAX) ||
        (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        status = status_invalid_argument;
    } else {
        uint32_t mfn = freq_in_hz % PLLCTLV2_PLL_XTAL_FREQ;
        uint32_t mfi = freq_in_hz / PLLCTLV2_PLL_XTAL_FREQ;

        if (PLLCTLV2_PLL_MFI_MFI_GET(ptr->PLL[pll].MFI) == mfi) {
            ptr->PLL[pll].MFI = mfi - 1U;
        }

        ptr->PLL[pll].MFI = mfi;
        /*
         * NOTE: Default MFD value is 240M
         */
        ptr->PLL[pll].MFN = mfn * PLLCTLV2_PLL_MFN_FACTOR;

        while (!pllctlv2_pll_is_stable(ptr, pll)) {
            NOP();
        }

        status = status_success;
    }
    return status;
}

void pllctlv2_enable_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint32_t step, uint32_t stop)
{
    /*
     * NOTE: The spread spectrum related registers cannot be configured under below conditions:
     *       1. PLL is enabled
     *       2. spread spectrum is enabled
     */
    if ((ptr != NULL) && (pll < PLLCTL_SOC_PLL_MAX_COUNT)) {

        ptr->PLL[pll].CONFIG &= ~PLLCTLV2_PLL_CONFIG_SPREAD_MASK;

        ptr->PLL[pll].SS_STEP = step;
        ptr->PLL[pll].SS_STOP = stop;

        ptr->PLL[pll].CONFIG |= PLLCTLV2_PLL_CONFIG_SPREAD_MASK;
    }
}

void pllctlv2_set_postdiv(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk, pllctlv2_div_t div_value)
{
    if ((ptr != NULL) && (pll < PLLCTL_SOC_PLL_MAX_COUNT)) {
        ptr->PLL[pll].DIV[clk] =
            (ptr->PLL[pll].DIV[clk] & ~PLLCTLV2_PLL_DIV_DIV_MASK) | PLLCTLV2_PLL_DIV_DIV_SET(div_value);

        while (!pllctlv2_pll_clk_is_stable(ptr, pll, clk)) {
            NOP();
        }
    }
}

uint32_t pllctlv2_get_pll_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll)
{
    uint32_t freq = 0;
    if ((ptr != NULL) && (pll < PLLCTL_SOC_PLL_MAX_COUNT)) {
        uint32_t mfi = PLLCTLV2_PLL_MFI_MFI_GET(ptr->PLL[pll].MFI);
        uint32_t mfn = PLLCTLV2_PLL_MFN_MFN_GET(ptr->PLL[pll].MFN);
        uint32_t mfd = PLLCTLV2_PLL_MFD_MFD_GET(ptr->PLL[pll].MFD);
        /* Trade-off for avoiding the float computing.
         * Ensure both `mfd` and `PLLCTLV2_PLL_XTAL_FREQ` are n * `FREQ_1MHz`, n is a positive integer
         */
        assert((mfd / FREQ_1MHz) * FREQ_1MHz == mfd);
        assert((PLLCTLV2_PLL_XTAL_FREQ / FREQ_1MHz) * FREQ_1MHz == PLLCTLV2_PLL_XTAL_FREQ);

        uint32_t scaled_num;
        uint32_t scaled_denom;
        uint32_t shifted_mfn;
        uint32_t max_mfn = 0xFFFFFFFF / (PLLCTLV2_PLL_XTAL_FREQ / FREQ_1MHz);
        if (mfn < max_mfn) {
            scaled_num =  (PLLCTLV2_PLL_XTAL_FREQ / FREQ_1MHz) * mfn;
            scaled_denom = mfd / FREQ_1MHz;
            freq = PLLCTLV2_PLL_XTAL_FREQ * mfi + scaled_num / scaled_denom;
        } else {
            shifted_mfn = mfn;
            while (shifted_mfn > max_mfn) {
                shifted_mfn >>= 1;
            }
            scaled_denom = mfd / FREQ_1MHz;
            freq = PLLCTLV2_PLL_XTAL_FREQ * mfi + ((PLLCTLV2_PLL_XTAL_FREQ / FREQ_1MHz) * shifted_mfn) / scaled_denom +  ((PLLCTLV2_PLL_XTAL_FREQ / FREQ_1MHz) * (mfn - shifted_mfn)) / scaled_denom;
        }
    }
    return freq;
}

uint32_t pllctlv2_get_pll_postdiv_freq_in_hz(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, pllctlv2_clk_t clk)
{
    uint32_t postdiv_freq = 0;
    if ((ptr != NULL) && (pll < PLLCTL_SOC_PLL_MAX_COUNT)) {
        uint32_t postdiv = PLLCTLV2_PLL_DIV_DIV_GET(ptr->PLL[pll].DIV[clk]);
        uint32_t pll_freq = pllctlv2_get_pll_freq_in_hz(ptr, pll);
        postdiv_freq = (uint32_t) (pll_freq / (100 + postdiv * 100 / 5U) * 100);
    }

    return postdiv_freq;
}

void pllctlv2_setup_spread_spectrum(PLLCTLV2_Type *ptr, pllctlv2_pll_t pll, uint8_t spread_range, uint32_t modulation_freq)
{
    uint32_t vco_freq = pllctlv2_get_pll_freq_in_hz(ptr, pll);
    uint32_t mfd = PLLCTLV2_PLL_MFD_MFD_GET(HPM_PLLCTLV2->PLL[pll].MFD);
    uint32_t delta_freq = (vco_freq / PLLCTLV2_PLL_XTAL_FREQ) * spread_range * (mfd / 1000);
    uint32_t step = (delta_freq * 2) / (PLLCTLV2_PLL_XTAL_FREQ / modulation_freq);

    pllctlv2_enable_spread_spectrum(ptr, pll, step, delta_freq);
}

