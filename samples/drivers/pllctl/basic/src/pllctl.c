/*
 * Copyright (c) 2021-2025 HPMicro
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
#include "hpm_romapi.h"

#define TEST_PLLCTL HPM_PLLCTL

#ifndef BOARD_APP_PLLCTL_TEST_PLL
#define TEST_PLL 0
#define TEST_PLL_CLK 0
#else
#define TEST_PLL BOARD_APP_PLLCTL_TEST_PLL
#define TEST_PLL_CLK BOARD_APP_PLLCTL_TEST_PLL_CLK
#endif

#define _PLL_MONITOR_TARGET(pll, div) monitor_target_clk ## div ##_pll ##pll
#define TEST_PLL_MONITOR_TARGET(pll, div) _PLL_MONITOR_TARGET(pll, div)

#define _PLL_SRC_CLK_NAME(pll, div) clk_src_pll ##pll ##_clk ##div
#define TEST_PLL_SRC_CLK_NAME(pll, div) _PLL_SRC_CLK_NAME(pll, div)

#define TEST_HELPER_CLK_NAME clock_display

/**
 * @brief Switches the system to operate using the preset clock configuration 0.
 *
 * This function adjusts the system clock to use the predefined preset configuration,
 * which can be useful for changing clock settings based on different operating modes.
 * It also handles specific actions required when Flash XIP (eXecute In Place) is enabled,
 * ensuring clock stability and resetting the XPI prefetch buffer to prevent any issues
 * with instruction fetching during the clock transition.
 */
ATTR_RAMFUNC void app_switch_to_preset0(void)
{
#if defined(FLASH_XIP)
    uint32_t clock_node = SYSCTL_CLOCK_CLK_TOP_XPI0;
    XPI_Type *base = BOARD_APP_XPI_NOR_XPI_BASE;
#if defined(HPM_XPI1)
    if (base == HPM_XPI1) {
        clock_node = SYSCTL_CLOCK_CLK_TOP_XPI1;
    }
#endif
    uint32_t clk_info = HPM_SYSCTL->CLOCK[clock_node];
#endif
    clock_enable(clk_osc0clk0);
    sysctl_clock_set_preset(HPM_SYSCTL, sysctl_preset_0);
#if defined(FLASH_XIP)
    HPM_SYSCTL->CLOCK[clock_node] = clk_info;
    /* Wait a while until the FLASH clock is stable */
    for (volatile uint32_t delay_cnt = 100000; delay_cnt > 0; delay_cnt--) {
        NOP();
    }
    ROM_API_TABLE_ROOT->xpi_driver_if->software_reset(base); /* Reset the XPI prefetch buffer */
    fencei(); /* Invalid I-cache in case the instruction prefetch happened in the period that XPI clock is unstable */
#endif
}

int main(void)
{
    board_init();
    board_init_clk_ref_pin();
    app_switch_to_preset0();
    board_init_console();

    printf("PLLCTL example\n");
    printf("Now configure PLL at integer mode\n");
    for (uint32_t i = 380000000; i < 840000000; i += 20000000) {
        printf("Update PLL%d to %dHz @integer mode\n", TEST_PLL, i);
        if (status_success != pllctl_init_int_pll_with_freq(TEST_PLLCTL, TEST_PLL, i)) {
            printf("Failed to init int pll%d\n", TEST_PLL);
            continue;
            while (1) {
            }
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

    printf("\nPLLCTL example finished\n");

    while (1) {
    }
    return 0;
}
