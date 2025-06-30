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
#else
#define TEST_PLL      pllctlv2_pll1
#define TEST_PLL_CLK  pllctlv2_clk0
#define TEST_PLL_NAME clk_pll1clk0
#endif
#else
#define TEST_PLL      BOARD_APP_PLLCTLV2_TEST_PLL
#define TEST_PLL_CLK  BOARD_APP_PLLCTLV2_TEST_PLL_CLK
#define TEST_PLL_NAME BOARD_APP_PLLCTLV2_TEST_PLL_NAME
#endif

#define FREQ_1MHZ (1000000UL)

int main(void)
{
    board_init();

    /* switch debug console source clock to OSC 24MHz to avoid potential impacts during PLL reconfiguration */
    clock_set_source_divider(BOARD_CONSOLE_UART_CLK_NAME, clk_src_osc24m, 1);

    /* Re-Initialize the Console due to clock transition happened during preset switching process */
    board_init_console();

    clock_enable(TEST_PLL_NAME); /* Force On TESTPLL clock */

    printf("PLLCTLV2 basic example\n");
    printf("Now configure PLL at integer mode\n");

    /*
     * Set TESTPLL CLK Post divider to 1, so that the TEST PLL CLK frequency is equal to TEST PLL frequency
     */
    pllctlv2_set_postdiv(TEST_PLLCTL, TEST_PLL, TEST_PLL_CLK, pllctlv2_div_1p0);
    for (uint32_t i = 400 * FREQ_1MHZ; i <= 1000 * FREQ_1MHZ; i += FREQ_1MHZ) {
        printf("Update PLL%d to %dHz\n", TEST_PLL, i);
        if (status_success != pllctlv2_init_pll_with_freq(TEST_PLLCTL, TEST_PLL, i)) {
            printf("Failed to init int pll%d\n", TEST_PLL);
            continue;
            while (1) {
            }
        } else {
            printf("Actual PLL frequency is %dHz\n", clock_get_frequency(TEST_PLL_NAME));
        }
    }

    printf("\nPLLCTLV2 basic example finished\n");

    while (1) {
    }
    return 0;
}

