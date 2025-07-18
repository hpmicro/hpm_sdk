/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pllctl_drv.h"

#define PLLCTL_INT_PLL_MAX_FBDIV (2400U)
#define PLLCTL_INT_PLL_MIN_FBDIV (16U)

#define PLLCTL_FRAC_PLL_MAX_FBDIV (240U)
#define PLLCTL_FRAC_PLL_MIN_FBDIV (20U)

#define PLLCTL_PLL_MAX_REFDIV (63U)
#define PLLCTL_PLL_MIN_REFDIV (1U)

#define PLLCTL_PLL_MAX_POSTDIV1 (7U)
#define PLLCTL_PLL_MIN_POSTDIV1 (1U)

#define PLLCTL_FRAC_PLL_MIN_REF (10000000U)
#define PLLCTL_INT_PLL_MIN_REF (1000000U)


hpm_stat_t pllctl_set_pll_work_mode(PLLCTL_Type *ptr, uint8_t pll, bool int_mode)
{
    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }
    if (int_mode) {
        if (!(ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK)) {
            /* it was at frac mode, then it needs to be power down */
            pllctl_pll_powerdown(ptr, pll);
            ptr->PLL[pll].CFG0 |= PLLCTL_PLL_CFG0_DSMPD_MASK;
            pllctl_pll_poweron(ptr, pll);
        }
    } else {
        if (ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK) {
            /* pll has to be powered down to configure frac mode */
            pllctl_pll_powerdown(ptr, pll);
            ptr->PLL[pll].CFG0 &= ~PLLCTL_PLL_CFG0_DSMPD_MASK;
            pllctl_pll_poweron(ptr, pll);
        }
    }

    return status_success;
}

hpm_stat_t pllctl_set_refdiv(PLLCTL_Type *ptr, uint8_t pll, uint8_t div)
{
    uint32_t min_ref;

    if ((ptr == NULL)
        || (pll > (PLLCTL_SOC_PLL_MAX_COUNT - 1))
        || (div == 0U)
        || (div > (PLLCTL_PLL_CFG0_REFDIV_MASK >> PLLCTL_PLL_CFG0_REFDIV_SHIFT))) {
        return status_invalid_argument;
    }

    if (ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK) {
        min_ref = PLLCTL_INT_PLL_MIN_REF;
    } else {
        min_ref = PLLCTL_FRAC_PLL_MIN_REF;
    }

    if ((PLLCTL_SOC_PLL_REFCLK_FREQ / div) < min_ref) {
        return status_pllctl_out_of_range;
    }

    if (PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0) != div) {
        /* if div is different, it needs to be power down */
        pllctl_pll_powerdown(ptr, pll);
        ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0 & ~PLLCTL_PLL_CFG0_REFDIV_MASK)
            | PLLCTL_PLL_CFG0_REFDIV_SET(div);
        pllctl_pll_poweron(ptr, pll);

        while (pllctl_pll_is_enabled(ptr, pll) && !pllctl_pll_is_locked(ptr, pll)) {
            NOP();
        }
    }
    return status_success;
}

hpm_stat_t pllctl_init_int_pll_with_freq(PLLCTL_Type *ptr, uint8_t pll,
                                    uint32_t freq_in_hz)
{
    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }
    uint32_t freq, fbdiv, refdiv, postdiv;
    if ((freq_in_hz < PLLCTL_PLL_VCO_FREQ_MIN)
            || (freq_in_hz > PLLCTL_PLL_VCO_FREQ_MAX)) {
        return status_invalid_argument;
    }

    freq = freq_in_hz;
    refdiv = PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0);
    postdiv = PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0);
    fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
    if (fbdiv > PLLCTL_INT_PLL_MAX_FBDIV) {
        /* current refdiv can't be used for the given frequency */
        refdiv--;
        do {
            fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
            if (fbdiv > PLLCTL_INT_PLL_MAX_FBDIV) {
                refdiv--;
            } else {
                break;
            }
        } while (refdiv > PLLCTL_PLL_MIN_REFDIV);
    } else if (fbdiv < PLLCTL_INT_PLL_MIN_FBDIV) {
        /* current refdiv can't be used for the given frequency */
        refdiv++;
        do {
            fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
            if (fbdiv < PLLCTL_INT_PLL_MIN_FBDIV) {
                refdiv++;
            } else {
                break;
            }
        } while (refdiv < PLLCTL_PLL_MAX_REFDIV);
    }

    if ((refdiv > PLLCTL_PLL_MAX_REFDIV)
            || (refdiv < PLLCTL_PLL_MIN_REFDIV)
            || (fbdiv > PLLCTL_INT_PLL_MAX_FBDIV)
            || (fbdiv < PLLCTL_INT_PLL_MIN_FBDIV)
            || (((PLLCTL_SOC_PLL_REFCLK_FREQ / refdiv) < PLLCTL_INT_PLL_MIN_REF))) {
        return status_pllctl_out_of_range;
    }

    if (!(ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK)) {
        /* it was at frac mode, then it needs to be power down */
        pllctl_pll_powerdown(ptr, pll);
        ptr->PLL[pll].CFG0 |= PLLCTL_PLL_CFG0_DSMPD_MASK;
    }

    if (PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0) != refdiv) {
        /* if refdiv is different, it needs to be power down */
        pllctl_pll_powerdown(ptr, pll);
        ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0 & ~PLLCTL_PLL_CFG0_REFDIV_MASK)
            | PLLCTL_PLL_CFG0_REFDIV_SET(refdiv);
    }

    ptr->PLL[pll].CFG2 = (ptr->PLL[pll].CFG2 & ~(PLLCTL_PLL_CFG2_FBDIV_INT_MASK)) | PLLCTL_PLL_CFG2_FBDIV_INT_SET(fbdiv);

    pllctl_pll_poweron(ptr, pll);

    while (pllctl_pll_is_enabled(ptr, pll) && !pllctl_pll_is_locked(ptr, pll)) {
        NOP();
    }
    return status_success;
}

hpm_stat_t pllctl_init_frac_pll_with_freq(PLLCTL_Type *ptr, uint8_t pll,
                                    uint32_t freq_in_hz)
{
    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }
    uint32_t frac, refdiv, fbdiv, freq, postdiv;
    double div;
    if ((freq_in_hz < PLLCTL_PLL_VCO_FREQ_MIN)
            || (freq_in_hz > PLLCTL_PLL_VCO_FREQ_MAX)) {
        return status_invalid_argument;
    }

    freq = freq_in_hz;
    refdiv = PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0);
    postdiv = PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0);
    fbdiv = (freq / postdiv) / (PLLCTL_SOC_PLL_REFCLK_FREQ / refdiv);

    if (fbdiv > PLLCTL_FRAC_PLL_MAX_FBDIV) {
        /* current refdiv can't be used for the given frequency */
        refdiv--;
        do {
            fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
            if (fbdiv > PLLCTL_FRAC_PLL_MAX_FBDIV) {
                refdiv--;
            } else {
                break;
            }
        } while (refdiv > PLLCTL_PLL_MIN_REFDIV);
    } else if (fbdiv < PLLCTL_FRAC_PLL_MIN_FBDIV) {
        /* current refdiv can't be used for the given frequency */
        refdiv++;
        do {
            fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
            if (fbdiv < PLLCTL_FRAC_PLL_MIN_FBDIV) {
                refdiv++;
            } else {
                break;
            }
        } while (refdiv < PLLCTL_PLL_MAX_REFDIV);
    }

    if ((refdiv > PLLCTL_PLL_MAX_REFDIV)
            || (refdiv < PLLCTL_PLL_MIN_REFDIV)
            || (fbdiv > PLLCTL_FRAC_PLL_MAX_FBDIV)
            || (fbdiv < PLLCTL_FRAC_PLL_MIN_FBDIV)
            || (((PLLCTL_SOC_PLL_REFCLK_FREQ / refdiv) < PLLCTL_FRAC_PLL_MIN_REF))) {
        return status_pllctl_out_of_range;
    }

    div = (double) freq / PLLCTL_SOC_PLL_REFCLK_FREQ * (refdiv * postdiv);
    fbdiv = freq / (PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv));
    frac = (uint32_t)((div - fbdiv) * (1 << 24));

    /*
     * pll has to be powered down to configure frac mode
     */
    pllctl_pll_powerdown(ptr, pll);

    ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0
            & ~(PLLCTL_PLL_CFG0_REFDIV_MASK | PLLCTL_PLL_CFG0_DSMPD_MASK))
        | PLLCTL_PLL_CFG0_REFDIV_SET(refdiv);

    pllctl_pll_ss_disable(ptr, pll);
    ptr->PLL[pll].FREQ = (ptr->PLL[pll].FREQ
            & ~(PLLCTL_PLL_FREQ_FRAC_MASK | PLLCTL_PLL_FREQ_FBDIV_FRAC_MASK))
        | PLLCTL_PLL_FREQ_FBDIV_FRAC_SET(fbdiv) | PLLCTL_PLL_FREQ_FRAC_SET(frac);

    pllctl_pll_poweron(ptr, pll);

    while (pllctl_pll_is_enabled(ptr, pll) && !pllctl_pll_is_locked(ptr, pll)) {
        NOP();
    }
    return status_success;
}

uint32_t pllctl_get_pll_freq_in_hz(PLLCTL_Type *ptr, uint8_t pll)
{
    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }
    uint32_t fbdiv, frac, refdiv, postdiv, refclk, freq;
    if (ptr->PLL[pll].CFG1 & PLLCTL_PLL_CFG1_PLLPD_SW_MASK) {
        /* pll is powered down */
        return 0;
    }

    refdiv = PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0);
    postdiv = PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0);
    refclk = PLLCTL_SOC_PLL_REFCLK_FREQ / (refdiv * postdiv);

    if (ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK) {
        /* pll int mode */
        fbdiv = PLLCTL_PLL_CFG2_FBDIV_INT_GET(ptr->PLL[pll].CFG2);
        freq = refclk * fbdiv;
    } else {
        /* pll frac mode */
        fbdiv = PLLCTL_PLL_FREQ_FBDIV_FRAC_GET(ptr->PLL[pll].FREQ);
        frac = PLLCTL_PLL_FREQ_FRAC_GET(ptr->PLL[pll].FREQ);
        freq = (uint32_t)((refclk * (fbdiv + ((double) frac / (1 << 24)))) + 0.5);
    }
    return freq;
}

hpm_stat_t pllctl_pll_ss_enable(PLLCTL_Type *ptr, uint8_t pll,
                                                uint8_t spread, uint8_t div,
                                                bool down_spread)
{
    if ((pll > (PLLCTL_SOC_PLL_MAX_COUNT - 1))
            || (spread > (PLLCTL_PLL_CFG0_SS_SPREAD_MASK >> PLLCTL_PLL_CFG0_SS_SPREAD_SHIFT))
            || (div > (PLLCTL_PLL_CFG0_SS_DIVVAL_MASK >> PLLCTL_PLL_CFG0_SS_DIVVAL_SHIFT))) {
        return status_invalid_argument;
    }

    ptr->PLL[pll].CFG0 |= PLLCTL_PLL_CFG0_SS_DISABLE_SSCG_MASK | PLLCTL_PLL_CFG0_SS_RSTPTR_MASK | PLLCTL_PLL_CFG0_SS_RESET_MASK;

    if (!(ptr->PLL[pll].CFG1 & PLLCTL_PLL_CFG1_PLLPD_SW_MASK)) {
        pllctl_pll_powerdown(ptr, pll);
    }

    ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0
        & ~(PLLCTL_PLL_CFG0_SS_SPREAD_MASK | PLLCTL_PLL_CFG0_SS_DIVVAL_MASK | PLLCTL_PLL_CFG0_SS_DOWNSPREAD_MASK))
        | PLLCTL_PLL_CFG0_SS_SPREAD_SET(spread)
        | PLLCTL_PLL_CFG0_SS_DIVVAL_SET(div)
        | PLLCTL_PLL_CFG0_SS_DOWNSPREAD_SET(down_spread);

    ptr->PLL[pll].CFG0 &= ~(PLLCTL_PLL_CFG0_SS_DISABLE_SSCG_MASK | PLLCTL_PLL_CFG0_SS_RESET_MASK);
    pllctl_pll_poweron(ptr, pll);
    while (pllctl_pll_is_enabled(ptr, pll) && !pllctl_pll_is_locked(ptr, pll)) {
        NOP();
    }
    ptr->PLL[pll].CFG0 &= ~PLLCTL_PLL_CFG0_SS_RSTPTR_MASK;
    return status_success;
}

hpm_stat_t pllctl_pll_setup_spread_spectrum(PLLCTL_Type *ptr, uint8_t pll,
                                                uint8_t ss_range, uint32_t modulation_freq,
                                                pllctl_ss_type ss_type)
{
    if ((pll > (PLLCTL_SOC_PLL_MAX_COUNT - 1))
            || (ss_range > (PLLCTL_PLL_CFG0_SS_SPREAD_MASK >> PLLCTL_PLL_CFG0_SS_SPREAD_SHIFT))) {
        return status_invalid_argument;
    }
    uint32_t ss_div = PLLCTL_SOC_PLL_REFCLK_FREQ / modulation_freq / 128 / PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0);

    if (ss_div > (PLLCTL_PLL_CFG0_SS_DIVVAL_MASK >> PLLCTL_PLL_CFG0_SS_DIVVAL_SHIFT)) {
        return status_invalid_argument;
    }

    return pllctl_pll_ss_enable(ptr, pll, ss_range, ss_div, ss_type);
}
