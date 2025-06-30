/*
 * Copyright (c) 2025 HPMicro
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

#ifndef MODULATION_FREQ
#define MODULATION_FREQ (30000)
#endif

#ifndef SS_RANGE
#define SS_RANGE (5)
#endif

#ifndef PLLCTL_SS_TYPE
#define PLLCTL_SS_TYPE pllctl_ss_downspread
#endif

#define TEST_PLL_FREQ (960000000UL)
#define TEST_CLK_REF_DIV (12)

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

    printf("PLLCTL spread spectrum example\n\n");

    clock_set_source_divider(TEST_HELPER_CLK_NAME, clk_src_osc24m, 1U);

    pllctl_init_frac_pll_with_freq(TEST_PLLCTL, TEST_PLL, TEST_PLL_FREQ);
    clock_set_source_divider(BOARD_APP_CLK_REF_CLK_NAME, TEST_PLL_SRC_CLK_NAME(TEST_PLL, TEST_PLL_CLK), TEST_CLK_REF_DIV);
    clock_add_to_group(BOARD_APP_CLK_REF_CLK_NAME, BOARD_RUNNING_CORE);

    if (status_success != pllctl_pll_setup_spread_spectrum(TEST_PLLCTL, TEST_PLL, SS_RANGE, MODULATION_FREQ, PLLCTL_SS_TYPE)) {
        printf("Failed to enable ss\n");
        while (1) {
        }
    }

    printf("PLL%dCLK%d @ %dMhz ss has been enabled (%s) with range 0.%d%% @ %dHz, which can be observed on \"%s\" pin by the oscilloscope\n",
                TEST_PLL, TEST_PLL_CLK, TEST_PLL_FREQ / TEST_CLK_REF_DIV,
                PLLCTL_SS_TYPE == pllctl_ss_downspread ? "downspread" : "centerspread",
                SS_RANGE, MODULATION_FREQ, BOARD_APP_CLK_REF_PIN_NAME);

    printf("\nPLLCTL example finished\n");

    while (1) {
    }
    return 0;
}
