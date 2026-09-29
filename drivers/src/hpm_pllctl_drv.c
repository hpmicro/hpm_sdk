/*
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pllctl_drv.h"

#define PLLCTL_INT_PLL_MAX_FBDIV (2400U)
#define PLLCTL_INT_PLL_MIN_FBDIV (16U)

#define PLLCTL_FRAC_PLL_MAX_FBDIV (240U)
#define PLLCTL_FRAC_PLL_MIN_FBDIV (20U)
#define PLLCTL_FRAC_PLL_SCALE (1ULL << 24)

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

hpm_stat_t pllctl_set_postdiv1(PLLCTL_Type *ptr, uint8_t pll, uint8_t div)
{
    if ((ptr == NULL)
        || (pll > (PLLCTL_SOC_PLL_MAX_COUNT - 1))
        || (div == 0U)
        || (div > (PLLCTL_PLL_CFG0_POSTDIV1_MASK >> PLLCTL_PLL_CFG0_POSTDIV1_SHIFT))) {
        return status_invalid_argument;
    }

    if (PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0) != div) {
        pllctl_pll_powerdown(ptr, pll);
        ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0 & ~PLLCTL_PLL_CFG0_POSTDIV1_MASK)
            | PLLCTL_PLL_CFG0_POSTDIV1_SET(div);
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
    uint32_t refdiv;
    uint32_t postdiv;
    uint32_t fbdiv;
    uint32_t best_refdiv = 0;
    uint32_t best_postdiv = 0;
    uint32_t best_fbdiv = 0;
    bool need_powerdown;
    uint64_t best_error_numerator = UINT64_MAX;
    uint64_t best_error_denominator = 1;

    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }

    if ((freq_in_hz < PLLCTL_PLL_VCO_FREQ_MIN)
            || (freq_in_hz > PLLCTL_PLL_VCO_FREQ_MAX)) {
        return status_invalid_argument;
    }

    /*
     * Search all legal integer PLL divider combinations. The PLL output is:
     * FOUT = FREF / REFDIV * FBDIV_INT / POSTDIV1.
     *
     * Compare errors as fractions to avoid losing precision when the PFD clock
     * is not an integer number of Hz. For an equal error, favor a higher PFD
     * clock (smaller REFDIV), then a smaller POSTDIV1.
     */
    for (refdiv = PLLCTL_PLL_MIN_REFDIV; refdiv <= PLLCTL_PLL_MAX_REFDIV; refdiv++) {
        uint64_t refclk;

        if ((PLLCTL_SOC_PLL_REFCLK_FREQ / refdiv) < PLLCTL_INT_PLL_MIN_REF) {
            break;
        }

        refclk = PLLCTL_SOC_PLL_REFCLK_FREQ;
        for (postdiv = PLLCTL_PLL_MIN_POSTDIV1; postdiv <= PLLCTL_PLL_MAX_POSTDIV1; postdiv++) {
            uint64_t divider_product = (uint64_t) refdiv * postdiv;
            uint64_t fbdiv_numerator = (uint64_t) freq_in_hz * divider_product;
            uint32_t candidate_fbdiv = (uint32_t) (fbdiv_numerator / refclk);
            uint32_t candidate_index;

            for (candidate_index = 0; candidate_index < 2; candidate_index++) {
                uint64_t error_numerator;
                uint64_t output_numerator;

                fbdiv = candidate_fbdiv + candidate_index;
                if ((fbdiv < PLLCTL_INT_PLL_MIN_FBDIV) || (fbdiv > PLLCTL_INT_PLL_MAX_FBDIV)) {
                    continue;
                }

                /* Fvco = Fref / REFDIV * FBDIV_INT, and must not exceed the VCO limit. */
                if (((uint64_t) refclk * fbdiv)
                    > ((uint64_t) PLLCTL_PLL_VCO_FREQ_MAX * refdiv)) {
                    break;
                }

                output_numerator = refclk * fbdiv;
                if (output_numerator >= fbdiv_numerator) {
                    error_numerator = output_numerator - fbdiv_numerator;
                } else {
                    error_numerator = fbdiv_numerator - output_numerator;
                }

                if ((best_refdiv == 0U)
                    || (error_numerator * best_error_denominator < best_error_numerator * divider_product)
                    || ((error_numerator * best_error_denominator == best_error_numerator * divider_product)
                        && ((refdiv < best_refdiv)
                            || ((refdiv == best_refdiv) && (postdiv < best_postdiv))))) {
                    best_refdiv = refdiv;
                    best_postdiv = postdiv;
                    best_fbdiv = fbdiv;
                    best_error_numerator = error_numerator;
                    best_error_denominator = divider_product;
                }
            }
        }
    }

    if (best_refdiv == 0U) {
        return status_pllctl_out_of_range;
    }

    /* DSMPD=1 selects integer mode. Mode, REFDIV, or POSTDIV1 changes require power-down. */
    need_powerdown = !(ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK)
        || (PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0) != best_refdiv)
        || (PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0) != best_postdiv);
    if (need_powerdown) {
        pllctl_pll_powerdown(ptr, pll);

        ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0
                & ~(PLLCTL_PLL_CFG0_DSMPD_MASK | PLLCTL_PLL_CFG0_REFDIV_MASK | PLLCTL_PLL_CFG0_POSTDIV1_MASK))
            | PLLCTL_PLL_CFG0_DSMPD_MASK | PLLCTL_PLL_CFG0_REFDIV_SET(best_refdiv) | PLLCTL_PLL_CFG0_POSTDIV1_SET(best_postdiv);
    }

    ptr->PLL[pll].CFG2 = (ptr->PLL[pll].CFG2 & ~(PLLCTL_PLL_CFG2_FBDIV_INT_MASK)) | PLLCTL_PLL_CFG2_FBDIV_INT_SET(best_fbdiv);

    if (need_powerdown) {
        pllctl_pll_poweron(ptr, pll);
    }

    while (pllctl_pll_is_enabled(ptr, pll) && !pllctl_pll_is_locked(ptr, pll)) {
        NOP();
    }
    return status_success;
}

hpm_stat_t pllctl_init_frac_pll_with_freq(PLLCTL_Type *ptr, uint8_t pll,
                                    uint32_t freq_in_hz)
{
    uint32_t refdiv;
    uint32_t postdiv;
    uint32_t best_refdiv = 0;
    uint32_t best_postdiv = 0;
    uint64_t multiplier;
    uint64_t best_multiplier = 0;
    uint64_t best_error_numerator = UINT64_MAX;
    uint64_t best_error_denominator = 1;
    bool need_powerdown;

    if ((ptr == NULL) || (pll >= PLLCTL_SOC_PLL_MAX_COUNT)) {
        return status_invalid_argument;
    }

    if ((freq_in_hz < PLLCTL_PLL_VCO_FREQ_MIN)
            || (freq_in_hz > PLLCTL_PLL_VCO_FREQ_MAX)) {
        return status_invalid_argument;
    }

    /*
     * Search all legal fractional PLL divider combinations. The PLL output is:
     * FOUT = FREF / REFDIV * (FBDIV_FRAC + FRAC / 2^24) / POSTDIV1.
     *
     * Round the complete fractional multiplier to the nearest 24-bit value and
     * compare each actual output as a fraction. This avoids floating-point
     * rounding and selects the legal REFDIV and POSTDIV1 with the least error.
     */
    for (refdiv = PLLCTL_PLL_MIN_REFDIV; refdiv <= PLLCTL_PLL_MAX_REFDIV; refdiv++) {
        uint64_t denominator;

        if ((PLLCTL_SOC_PLL_REFCLK_FREQ / refdiv) < PLLCTL_FRAC_PLL_MIN_REF) {
            break;
        }

        for (postdiv = PLLCTL_PLL_MIN_POSTDIV1; postdiv <= PLLCTL_PLL_MAX_POSTDIV1; postdiv++) {
            uint64_t target_numerator;
            uint64_t output_numerator;
            uint64_t error_numerator;

            denominator = (uint64_t) refdiv * postdiv * PLLCTL_FRAC_PLL_SCALE;
            target_numerator = (uint64_t) freq_in_hz * denominator;
            multiplier = (target_numerator + (PLLCTL_SOC_PLL_REFCLK_FREQ / 2U)) / PLLCTL_SOC_PLL_REFCLK_FREQ;

            if ((multiplier < ((uint64_t) PLLCTL_FRAC_PLL_MIN_FBDIV * PLLCTL_FRAC_PLL_SCALE))
                    || (multiplier > ((uint64_t) PLLCTL_FRAC_PLL_MAX_FBDIV * PLLCTL_FRAC_PLL_SCALE
                        + (PLLCTL_PLL_FREQ_FRAC_MASK >> PLLCTL_PLL_FREQ_FRAC_SHIFT)))) {
                continue;
            }

            /* Fvco = Fref / REFDIV * (FBDIV_FRAC + FRAC / 2^24). */
            if ((uint64_t) PLLCTL_SOC_PLL_REFCLK_FREQ * multiplier
                > (uint64_t) PLLCTL_PLL_VCO_FREQ_MAX * refdiv * PLLCTL_FRAC_PLL_SCALE) {
                continue;
            }

            output_numerator = (uint64_t) PLLCTL_SOC_PLL_REFCLK_FREQ * multiplier;
            if (output_numerator >= target_numerator) {
                error_numerator = output_numerator - target_numerator;
            } else {
                error_numerator = target_numerator - output_numerator;
            }

            if ((best_refdiv == 0U)
                    || (error_numerator * best_error_denominator < best_error_numerator * denominator)
                    || ((error_numerator * best_error_denominator == best_error_numerator * denominator)
                        && ((refdiv < best_refdiv)
                            || ((refdiv == best_refdiv) && (postdiv < best_postdiv))))) {
                best_refdiv = refdiv;
                best_postdiv = postdiv;
                best_multiplier = multiplier;
                best_error_numerator = error_numerator;
                best_error_denominator = denominator;
            }
        }
    }

    if (best_refdiv == 0U) {
        return status_pllctl_out_of_range;
    }

    /* Mode, REFDIV, or POSTDIV1 changes require power-down. */
    need_powerdown = (ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK)
        || (PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0) != best_refdiv)
        || (PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0) != best_postdiv);
    if (need_powerdown) {
        pllctl_pll_powerdown(ptr, pll);

        ptr->PLL[pll].CFG0 = (ptr->PLL[pll].CFG0
                & ~(PLLCTL_PLL_CFG0_DSMPD_MASK | PLLCTL_PLL_CFG0_REFDIV_MASK | PLLCTL_PLL_CFG0_POSTDIV1_MASK))
            | PLLCTL_PLL_CFG0_REFDIV_SET(best_refdiv) | PLLCTL_PLL_CFG0_POSTDIV1_SET(best_postdiv);
    }

    ptr->PLL[pll].FREQ = (ptr->PLL[pll].FREQ
            & ~(PLLCTL_PLL_FREQ_FRAC_MASK | PLLCTL_PLL_FREQ_FBDIV_FRAC_MASK))
        | PLLCTL_PLL_FREQ_FBDIV_FRAC_SET(best_multiplier >> 24)
        | PLLCTL_PLL_FREQ_FRAC_SET(best_multiplier & (PLLCTL_PLL_FREQ_FRAC_MASK >> PLLCTL_PLL_FREQ_FRAC_SHIFT));

    if (need_powerdown) {
        pllctl_pll_poweron(ptr, pll);
    }

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
    uint32_t fbdiv, frac, refdiv, postdiv, freq;
    uint64_t multiplier;
    uint64_t divider;
    if (ptr->PLL[pll].CFG1 & PLLCTL_PLL_CFG1_PLLPD_SW_MASK) {
        /* pll is powered down */
        return 0;
    }

    refdiv = PLLCTL_PLL_CFG0_REFDIV_GET(ptr->PLL[pll].CFG0);
    postdiv = PLLCTL_PLL_CFG0_POSTDIV1_GET(ptr->PLL[pll].CFG0);
    divider = (uint64_t) refdiv * postdiv;

    if (ptr->PLL[pll].CFG0 & PLLCTL_PLL_CFG0_DSMPD_MASK) {
        /* pll int mode */
        fbdiv = PLLCTL_PLL_CFG2_FBDIV_INT_GET(ptr->PLL[pll].CFG2);
        freq = (uint32_t) (((uint64_t) PLLCTL_SOC_PLL_REFCLK_FREQ * fbdiv + (divider / 2U)) / divider);
    } else {
        /* pll frac mode */
        fbdiv = PLLCTL_PLL_FREQ_FBDIV_FRAC_GET(ptr->PLL[pll].FREQ);
        frac = PLLCTL_PLL_FREQ_FRAC_GET(ptr->PLL[pll].FREQ);
        multiplier = ((uint64_t) fbdiv << 24) + frac;
        divider *= PLLCTL_FRAC_PLL_SCALE;
        freq = (uint32_t) (((uint64_t) PLLCTL_SOC_PLL_REFCLK_FREQ * multiplier + (divider / 2U)) / divider);
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
