/*
 * Copyright (c) 2021-2023,2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_wdg_drv.h"

/***********************************************************************************
 *
 *  Definitions
 *
 **********************************************************************************/
#define RESET_SOURCE_WDG0 (1UL << 16)
#define RESET_SOURCE_WDG1 (1UL << 17)
#define RESET_SOURCE_WDG2 (1UL << 18)
#define RESET_SOURCE_WDG3 (1UL << 19)
#define RESET_SOURCE_WDG4 (1UL << 20)

/* NOTE: The actual reset interval = WDG_INTERRUPT_INTERVAL_US + WDG_RESET_INTERVAL_US */
#define WDG_INTERRUPT_INTERVAL_US (1000UL * 1000UL) /* WDG interrupt interval: 1s  */
#define WDG_RESET_INTERVAL_US     (1000UL)          /* WDOG reset interval:1ms */

/** Test Watchdog info */
#define TEST_WDG_BASE HPM_WDG0_BASE

#if TEST_WDG_BASE == HPM_WDG0_BASE
#define TEST_WDG_CLK clock_watchdog0
#define TEST_WDG_IRQ IRQn_WDG0
#elif TEST_WDG_BASE == HPM_WDG1_BASE
#define TEST_WDG_CLK clock_watchdog1
#define TEST_WDG_IRQ IRQn_WDG1
#else
#if defined(HPM_WDG2_BASE)
#if TEST_WDG_BASE == HPM_WDG2_BASE
#define TEST_WDG_CLK clock_watchdog2
#define TEST_WDG_IRQ IRQn_WDG2
#endif
#endif
#ifdef HPM_WDG3_BASE
#if TEST_WDG_BASE == HPM_WDG3_BASE
#define TEST_WDG_CLK clock_watchdog3
#define TEST_WDG_IRQ IRQn_WDG3
#endif
#endif
#ifdef HPM_PWDG_BASE
#if TEST_WDG_BASE == HPM_PWDG_BASE
#define TEST_WDG_CLK clock_pwdg
#define TEST_WDG_IRQ IRQn_PWDG
#endif
#endif

#endif

#if defined(TEST_WDG_BASE)
#define TEST_WDG ((WDG_Type *)TEST_WDG_BASE)
#else
#error The TEST_WDG must be specified explictilty
#endif

/***********************************************************************************
 *
 *  Prototypes
 *
 **********************************************************************************/
void show_menu(void);
void wdg_interrupt_test(void);
void wdg_reset_test(void);

void show_valid_interrupt_intervals(void);
void show_valid_reset_intervals(void);

/***********************************************************************************
 *
 *  Variables
 *
 **********************************************************************************/
static volatile bool has_interrupt;

/***********************************************************************************
 *
 *  Codes
 *
 **********************************************************************************/
int main(void)
{
    board_init();
    clock_add_to_group(TEST_WDG_CLK, BOARD_RUNNING_CORE);
    printf("wdg test\n");

    if ((HPM_PPOR->RESET_FLAG & RESET_SOURCE_WDG0) != 0U) {
        printf("The last reset was triggered by WDG0 reset\n");
    }

    show_menu();

    while (1) {
        char opt = getchar();
        putchar(opt);
        putchar('\n');
        switch (opt) {
        case '1':
            wdg_interrupt_test();
            break;
        case '2':
            wdg_reset_test();
            break;
        case '3':
            show_valid_interrupt_intervals();
            break;
        case '4':
            show_valid_reset_intervals();
            break;
        default:
            show_menu();
            break;
        }
    }

    return 0;
}

void show_menu(void)
{
    static const char menu_info[] = "\n"
                                    "****************************************************************\n"
                                    "*                                                              *\n"
                                    "*  WDG Example Menu                                            *\n"
                                    "*  1. Test WDG interrupt                                       *\n"
                                    "*  2. Test WDG reset                                           *\n"
                                    "*  3. Show valid Interrupt values                              *\n"
                                    "*  4. Show valid Reset values                                  *\n"
                                    "*                                                              *\n"
                                    "*  NOTE:                                                       *\n"
                                    "*    WDG total reset interval =                                *\n"
                                    "*    interrupt interval + reset interval                       *\n"
                                    "****************************************************************\n";

    printf("%s", menu_info);
}

void show_valid_interrupt_intervals(void)
{
    printf("Valid WDOG interrupt interval list:\n------------------------\n");
    for (interrupt_interval_t i = 0; i < interrupt_interval_out_of_range; i++) {
        uint64_t timeout_us = wdg_convert_interrupt_interval_to_us(WDG_EXT_CLK_FREQ, i);
        if (timeout_us < 1000UL) {
            printf("%.2fus\n", (float)timeout_us);
        } else if (timeout_us < 1000000UL) {
            printf("%.2fms\n", (float)(1.0 * timeout_us / 1000));
        } else {
            printf("%.2fs\n", (float)(1.0 * timeout_us / 1000000));
        }
    }
}

void show_valid_reset_intervals(void)
{
    printf("Valid WDOG reset interval list:\n------------------------\n");
    for (reset_interval_t i = 0; i < reset_interval_out_of_range; i++) {
        uint64_t timeout_us = wdg_convert_reset_interval_to_us(WDG_EXT_CLK_FREQ, i);
        if (timeout_us < 1000UL) {
            printf("%.2fus\n", (float)timeout_us);
        } else if (timeout_us < 1000000UL) {
            printf("%.2fms\n", (float)(1.0 * timeout_us / 1000));
        } else {
            printf("%.2fs\n", (float)(1.0 * timeout_us / 1000000));
        }
    }
}

SDK_DECLARE_EXT_ISR_M(TEST_WDG_IRQ, wdg_isr)
void wdg_isr(void)
{
    has_interrupt = true;
    wdg_clear_status(TEST_WDG, WDG_ST_INTEXPIRED_MASK);
}

void wdg_interrupt_test(void)
{
    /**
     * NOTE: The application should determine the following configuration based on the real need
     *       WDG reset time = WDG interrupt interval + WDG reset interval
     */
    wdg_control_t wdg_ctrl = {
        .reset_enable = false,
        .interrupt_enable = true,
        .wdg_enable = true,
        .clksrc = wdg_clksrc_extclk,
    };
    wdg_ctrl.reset_interval = wdg_convert_reset_interval_from_us(WDG_EXT_CLK_FREQ, WDG_RESET_INTERVAL_US);
    wdg_ctrl.interrupt_interval = wdg_convert_interrupt_interval_from_us(WDG_EXT_CLK_FREQ, WDG_INTERRUPT_INTERVAL_US);

    /* Initialize the WDG */
    wdg_init(TEST_WDG, &wdg_ctrl);
    intc_m_enable_irq_with_priority(TEST_WDG_IRQ, 1);

    uint32_t interrupt_interval_in_us = wdg_get_interrupt_interval_in_us(TEST_WDG, WDG_EXT_CLK_FREQ);
    printf("Actual WDG interrupt interval:%dms\n", interrupt_interval_in_us / 1000U);

    printf("Waiting for WDG interrupt...\n");
    while (!has_interrupt) {
    }

    has_interrupt = false;
    printf("WDG interrupt happened!\n");
    /* Service WDG */
    wdg_restart(TEST_WDG);
    /* Disable WDG */
    wdg_disable(TEST_WDG);
}

void wdg_reset_test(void)
{
    /**
     * NOTE: The application should determine the following configuration based on the real need
     *       WDG reset time = WDG interrupt interval + WDG reset interval
     */
    wdg_control_t wdg_ctrl = {
        .reset_enable = true,
        .interrupt_enable = false,
        .wdg_enable = true,
        .clksrc = wdg_clksrc_extclk,
    };
    wdg_ctrl.reset_interval = wdg_convert_reset_interval_from_us(WDG_EXT_CLK_FREQ, WDG_RESET_INTERVAL_US);
    wdg_ctrl.interrupt_interval = wdg_convert_interrupt_interval_from_us(WDG_EXT_CLK_FREQ, WDG_INTERRUPT_INTERVAL_US);

    /* Initialize the WDG */
    wdg_init(TEST_WDG, &wdg_ctrl);

    uint32_t reset_interval_in_us = wdg_get_total_reset_interval_in_us(TEST_WDG, WDG_EXT_CLK_FREQ);
    printf("Actual WDG reset interval:%dms\n", reset_interval_in_us / 1000U);

    printf("Waiting for WDG reset...\n");
}
