/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_pllctl_drv.h"
#include "hpm_sysctl_drv.h"

#define TEST_PLLCTL HPM_PLLCTL
#define TEST_PLL 0
#define TEST_PLL_CLK 0

#define _PLL_MONITOR_TARGET(pll, div) monitor_target_clk ## div ##_pll ##pll
#define TEST_PLL_MONITOR_TARGET(pll, div) _PLL_MONITOR_TARGET(pll, div)

#define _PLL_SRC_CLK_NAME(pll, div) clk_src_pll ##pll ##_clk ##div
#define TEST_PLL_SRC_CLK_NAME(pll, div) _PLL_SRC_CLK_NAME(pll, div)

#define TEST_HELPER_CLK_NAME clock_display

int main(void)
{
    board_init();
    sysctl_clock_set_preset(HPM_SYSCTL, sysctl_preset_0);
    board_init_console();

    printf("PLLCTL example\n");
    printf("Now configure PLL at integer mode\n");
    for (uint32_t i = 380000000; i < 840000000; i += 20000000) {
        printf("Update PLL%d to %dHz @integer mode\n", TEST_PLL, i);
        if (status_success != pllctl_init_int_pll_with_freq(TEST_PLLCTL, TEST_PLL, i)) {
            printf("Failed to init int pll%d\n", TEST_PLL);
            continue;
            while(1);
        }
        clock_set_source_divider(TEST_HELPER_CLK_NAME, TEST_PLL_SRC_CLK_NAME(TEST_PLL, TEST_PLL_CLK), 1U);
        printf("Actual f = %dHz\n", clock_get_frequency(TEST_HELPER_CLK_NAME));
    }

    clock_set_source_divider(TEST_HELPER_CLK_NAME, clk_src_osc24m, 1U);

    printf("\n\nNow configure PLL at fraction mode\n");
    for (uint32_t i = 380000000; i < 840000000; i += 20000000) {
        if (status_success != pllctl_init_frac_pll_with_freq(TEST_PLLCTL, TEST_PLL, i)) {
            printf("Failed to update PLL%d to %dHz @fraction mode\n", TEST_PLL, i);
            continue;
            while(1);
        }
        printf("Update PLL%d to %dHz @fraction mode\n", TEST_PLL, i);
     
        clock_set_source_divider(TEST_HELPER_CLK_NAME, TEST_PLL_SRC_CLK_NAME(TEST_PLL, TEST_PLL_CLK), 1U);
        printf("Actual f = %dHz\n", clock_get_frequency(TEST_HELPER_CLK_NAME));
    }

    clock_set_source_divider(TEST_HELPER_CLK_NAME, clk_src_osc24m, 1U);
    if (status_success != pllctl_pll_ss_enable(TEST_PLLCTL, TEST_PLL, 0x1F, 4, false)) {
        printf("Failed to enable ss\n");
        while(1);
    }
    printf("PLL%d ss has been enabled\n", TEST_PLL);

    printf("\nPLLCTL example finished\n");

    while(1);
    return 0;
}
