/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_pllctlv2_drv.h"
#include "hpm_sysctl_drv.h"

#define TEST_PLLCTL HPM_PLLCTLV2

#ifndef BOARD_APP_PLLCTLV2_TEST_PLL
#if defined(PLLCTL_SOC_PLL_MAX_COUNT) && (PLLCTL_SOC_PLL_MAX_COUNT > 2)
#define TEST_PLL      pllctlv2_pll2
#define TEST_PLL_CLK  pllctlv2_clk0
#define TEST_PLL_NAME clk_pll2clk0
#define TEST_CLK_REF_SRC clk_src_pll2_clk0
#else
#define TEST_PLL      pllctlv2_pll1
#define TEST_PLL_CLK  pllctlv2_clk0
#define TEST_PLL_NAME clk_pll1clk0
#define TEST_CLK_REF_SRC clk_src_pll1_clk0
#endif
#else
#define TEST_PLL      BOARD_APP_PLLCTLV2_TEST_PLL
#define TEST_PLL_CLK  BOARD_APP_PLLCTLV2_TEST_PLL_CLK
#define TEST_PLL_NAME BOARD_APP_PLLCTLV2_TEST_PLL_NAME
#define TEST_CLK_REF_SRC BOARD_APP_CLK_REF_SRC_NAME
#endif

#ifndef MOD_FREQ
/* modulation frequency for spread spectrum */
#define MOD_FREQ (30000UL)
#endif

#ifndef SS_RANGE
/* range of spread spectrum, 0.x% */
#define SS_RANGE (5)
#endif

#define FREQ_1MHz (1000000UL)
#define TEST_PLL_FREQ (960000000UL)
#define TEST_PLL_DIV (12)

void test_pll_spread_spectrum(void)
{
    pllctlv2_init_pll_with_freq(TEST_PLLCTL, TEST_PLL, TEST_PLL_FREQ);
    pllctlv2_set_postdiv(TEST_PLLCTL, TEST_PLL, TEST_PLL_CLK, pllctlv2_div_1p0);
    pllctlv2_setup_spread_spectrum(TEST_PLLCTL, TEST_PLL, SS_RANGE, MOD_FREQ);
    printf("PLL%dCLK%d @ %dMhz spread spectrum has been enabled with range 0.%d%% @ %dHz, which can be observed on \"%s\" pin by the oscilloscope\n",
                TEST_PLL, TEST_PLL_CLK, TEST_PLL_FREQ / TEST_PLL_DIV,
                SS_RANGE, MOD_FREQ, BOARD_APP_CLK_REF_PIN_NAME);
}

int main(void)
{
    clock_set_source_divider(BOARD_CONSOLE_UART_CLK_NAME, clk_src_osc24m, 1);
    board_init();
    board_init_clk_ref_pin();

    clock_add_to_group(BOARD_APP_CLK_REF_CLK_NAME, BOARD_RUNNING_CORE);
    clock_set_source_divider(BOARD_APP_CLK_REF_CLK_NAME, TEST_CLK_REF_SRC, TEST_PLL_DIV);

    clock_enable(TEST_PLL_NAME); /* Force On TESTPLL clock */

    printf("PLLCTLV2 spread spectrum example\n");

    test_pll_spread_spectrum();

    printf("\nPLLCTLV2 spread spectrum example finished\n");

    while (1) {
    }
    return 0;
}

