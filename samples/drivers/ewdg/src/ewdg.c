/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_ewdg_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_csr_drv.h"

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

#define EWDG_CNT_CLK_FREQ 32768UL

/***********************************************************************************
 *
 *  Prototypes
 *
 **********************************************************************************/
void show_menu(void);

/* Test EWDG service operation */
void ewdg_service_test(void);

/* Test EWDG window mode */
void ewdg_window_test(void);

/* Test EWDG reset */
void ewdg_reset_test(void);

/* Test EWDG interrupt */
void ewdg_interrupt_test(void);


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
    clock_add_to_group(clock_watchdog0, 0);
    printf("ewdg test\n");

    if ((HPM_PPOR->RESET_FLAG & RESET_SOURCE_WDG0) != 0U) {
        printf("The last reset was triggered by EWDG0 reset\n");
    }
    if ((HPM_PPOR->RESET_FLAG & RESET_SOURCE_WDG1) != 0U) {
        printf("The last reset was triggered by EWDG1 reset\n");
    }

    show_menu();

    while (1) {
        char opt = getchar();
        putchar(opt);
        putchar('\n');
        switch (opt) {
        case '1':
            ewdg_service_test();
            break;
        case '2':
            ewdg_reset_test();
            break;
        case '3':
            ewdg_window_test();
            break;
#if defined(HPM_IP_FEATURE_EWDG_SOC_SUPPORT_TIMEOUT_INTERRUPT) && (HPM_IP_FEATURE_EWDG_SOC_SUPPORT_TIMEOUT_INTERRUPT == 1)
        case '4':
            ewdg_interrupt_test();
            break;
#endif
        default:
            show_menu();
            break;
        }
    }

    return 0;
}

void show_menu(void)
{
#if defined(HPM_IP_FEATURE_EWDG_SOC_SUPPORT_TIMEOUT_INTERRUPT) && (HPM_IP_FEATURE_EWDG_SOC_SUPPORT_TIMEOUT_INTERRUPT == 1)
    static const char menu_info[] = "\n"
                                    "****************************************************************\n"
                                    "*                                                              *\n"
                                    "*  WDG Example Menu                                            *\n"
                                    "*  1. Test EWDG service                                        *\n"
                                    "*  2. Test EWDG reset                                          *\n"
                                    "*  3. Test EWDG Window Mode                                    *\n"
                                    "*  4. Test EWDG Timeout interrupt                              *\n"
                                    "****************************************************************\n";
#else
    static const char menu_info[] = "\n"
                                    "****************************************************************\n"
                                    "*                                                              *\n"
                                    "*  WDG Example Menu                                            *\n"
                                    "*  1. Test EWDG service                                        *\n"
                                    "*  2. Test EWDG reset                                          *\n"
                                    "*  3. Test EWDG Window Mode                                    *\n"
                                    "****************************************************************\n";
#endif
    printf("%s", menu_info);
}

/**
 * This function initialize the EWDG Reset timeout to 101ms, and it services the EWDG before EWDG timeout
 */
void ewdg_service_test(void)
{
    uint32_t ewdg_src_clk_freq = EWDG_CNT_CLK_FREQ;
    ewdg_config_t config;
    hpm_stat_t status;

    /**
     * Case 1: Use the high-level timeout setting (provide the timeout in terms of microseconds)
     */
    printf("Case 1: Use the high-level timeout setting\n");
    ewdg_get_default_config(HPM_EWDG0, &config);
    /* Enable EWDG */
    config.enable_watchdog = true;
    config.ctrl_config.use_lowlevel_timeout = false;
    /* Enable EWDG Timeout Reset */
    config.int_rst_config.enable_timeout_reset = true;
    /* Set EWDG Count clock source to OSC32 */
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /* Set the EWDG reset timeout to 101ms */
    config.cnt_src_freq = ewdg_src_clk_freq;
    config.ctrl_config.timeout_reset_us = 101UL * 1000UL;

    /* Initialize the EWDG */
    status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
    }

    board_delay_ms(100UL);
    /* Service EWDG */
    ewdg_refresh(HPM_EWDG0);

    /* Disable EWDG */
    ewdg_disable(HPM_EWDG0);

    uint32_t actual_timeout_us = ewdg_convert_timeout_ticks_to_timeout_us(HPM_EWDG0, ewdg_src_clk_freq,
                                                                          ewdg_get_timeout_reset_ticks(HPM_EWDG0));
    printf("Actual EWDG reset timeout is %dus\n", actual_timeout_us);


    /**
     * Case 2: Use the low-level timeout setting (provide the timeout in terms of counter ticks)
     */
    printf("Case 2: Use the low-level timeout setting\n");
    ewdg_get_default_config(HPM_EWDG0, &config);
    /* Enable EWDG */
    config.enable_watchdog = true;
    config.ctrl_config.use_lowlevel_timeout = true;
    /* Enable EWDG Timeout Reset */
    config.int_rst_config.enable_timeout_reset = true;
    /* Set EWDG Count clock source to OSC32 */
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /* Set the EWDG reset timeout to 101ms */
    config.cnt_src_freq = ewdg_src_clk_freq;

    uint32_t timeout_reset_us = 101U * 1000UL;

    uint64_t timeout_reset_ticks = ewdg_convert_timeout_us_to_timeout_ticks(ewdg_src_clk_freq, timeout_reset_us);
    uint32_t timeout_ticks_max;
#if defined(EWDG_SOC_OVERTIME_REG_WIDTH) && (EWDG_SOC_OVERTIME_REG_WIDTH == 16)
    timeout_ticks_max = 0xFFFFUL;
#else
    timeout_ticks_max = 0xFFFFFFFFUL;
#endif
    uint32_t cnt_clk_div_by_pwr_2 = 0;
    while (timeout_reset_ticks > timeout_ticks_max) {
        ++cnt_clk_div_by_pwr_2;
        timeout_reset_ticks >>= 1;
    }
    if (cnt_clk_div_by_pwr_2 > EWDG_SOC_CLK_DIV_VAL_MAX) {
        printf("EWDG reset timeout interval is out of range\n");
        return;
    }
    config.ctrl_config.timeout_reset_val = (uint32_t) (timeout_reset_ticks & 0xFFFFFFFFUL);
    config.ctrl_config.clock_div_by_power_of_2 = cnt_clk_div_by_pwr_2;

    /* Initialize the EWDG */
    status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
    }

    board_delay_ms(100UL);
    /* Service EWDG */
    ewdg_refresh(HPM_EWDG0);

    /* Disable EWDG */
    ewdg_disable(HPM_EWDG0);

    actual_timeout_us = ewdg_convert_timeout_ticks_to_timeout_us(HPM_EWDG0, ewdg_src_clk_freq,
                                                                 ewdg_get_timeout_reset_ticks(HPM_EWDG0));
    printf("Actual EWDG reset timeout is %dus\n", actual_timeout_us);


    /**
     * Case 3: Use Configuration Protection, Refresh Period, Refresh lock, etc
     */
    printf("Case 3: Use complicated EWDG protection and refresh unlock method\n");
    ewdg_get_default_config(HPM_EWDG0, &config);

    config.enable_watchdog = true;/* Enable EWDG */
    config.ctrl_config.use_lowlevel_timeout = false;
    config.int_rst_config.enable_timeout_reset = true; /* Enable EWDG Timeout Reset */
    config.ctrl_config.enable_config_lock = true; /* Lock Configuration registers */
    config.ctrl_config.enable_refresh_lock = true; /* Enable Refresh lock */
    config.ctrl_config.enable_refresh_period = true; /* Enable Refresh period protection */
    config.ctrl_config.refresh_unlock_method = ewdg_refresh_unlock_method_fixed_key;
    config.ctrl_config.keep_running_in_debug_mode = false;
    /* Set EWDG Count clock source to OSC32 */
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /* Set the EWDG reset timeout to 101ms */
    config.cnt_src_freq = ewdg_src_clk_freq;
    config.ctrl_config.timeout_reset_us = 101UL * 1000UL;

    /* Initialize the EWDG */
    status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
    }

    uint32_t service_cnt = 0;
    while (service_cnt++ < 10) {
        board_delay_ms(90UL);
        /* Service EWDG */
        ewdg_refresh(HPM_EWDG0);
    }
    /* Disable EWDG */
    ewdg_disable(HPM_EWDG0);


    printf("EWDG Serving test PASSED!\n");
}

/**
 * This case set the EWDG Reset Timeout interval to 1 second
 * After around 1 second, the SoC will be reset
 */
void ewdg_reset_test(void)
{
    ewdg_config_t config;
    ewdg_get_default_config(HPM_EWDG0, &config);

    config.enable_watchdog = true;
    config.int_rst_config.enable_timeout_reset = true;
    config.ctrl_config.use_lowlevel_timeout = false;
    uint32_t ewdg_src_clk_freq = EWDG_CNT_CLK_FREQ;
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /* Set the EWDG reset timeout to 1 second */
    config.cnt_src_freq = ewdg_src_clk_freq;
    config.ctrl_config.timeout_reset_us = 1000UL * 1000;

    /* Initialize the WDG */
    hpm_stat_t status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
    }

    printf("Waiting for WDG reset...\n");
}

void ewdg_window_test(void)
{
    ewdg_config_t config;
    ewdg_get_default_config(HPM_EWDG0, &config);

    /* Enable EWDG */
    config.enable_watchdog = true;
    /* Enable EWDG Timeout Reset */
    config.int_rst_config.enable_timeout_reset = true;
    config.int_rst_config.enable_refresh_violation_reset = true;
    config.int_rst_config.enable_refresh_unlock_fail_reset = true;
    config.ctrl_config.use_lowlevel_timeout = false;
    /* Set EWDG Count clock source to OSC32 */
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /** The EWDG window works in the following manner:
     *  - start = window_lower_limit
     *  - end = window_lower_limit + window_upper_limit
     *
     *  In the example, the allowed EWDG service window is [1/2, 1] EWDG timeout interval
     *
     */
    config.ctrl_config.enable_window_mode = true;
    config.ctrl_config.window_lower_limit = ewdg_window_lower_timeout_period_8_div_16;
    config.ctrl_config.window_upper_limit = ewdg_window_upper_timeout_period_8_div_16;

    /* Set the EWDG reset timeout to 101ms */
    uint32_t ewdg_src_clk_freq = EWDG_CNT_CLK_FREQ;

    config.cnt_src_freq = ewdg_src_clk_freq;
    /* EWDG reset timeout interval = 100us */
    config.ctrl_config.timeout_reset_us = 101UL * 1000UL;

    /* Initialize the WDG */
    hpm_stat_t status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
    }

    uint32_t service_cnt = 0;
    while (service_cnt++ < 10) {
        board_delay_ms(100UL);
        /* Service EWDG */
        ewdg_refresh(HPM_EWDG0);
    }

    printf("EWDG Window test in expected window PASSED!\n");

    printf("Test servicing EWDG in invalid window, EWDG will trigger reset\n");
    ewdg_refresh(HPM_EWDG0);
}

void ewdg_interrupt_test(void)
{
    uint32_t ewdg_src_clk_freq = EWDG_CNT_CLK_FREQ;
    ewdg_config_t config;
    hpm_stat_t status;
    ewdg_get_default_config(HPM_EWDG0, &config);
    /* Enable EWDG */
    config.enable_watchdog = true;
    config.ctrl_config.use_lowlevel_timeout = false;
    /* Enable EWDG Timeout interrupt */
    config.int_rst_config.enable_timeout_interrupt = true;
    /* Enable EWDG Timeout Reset */
    config.int_rst_config.enable_timeout_reset = true;
    /* Set EWDG Count clock source to OSC32 */
    config.ctrl_config.cnt_clk_sel = ewdg_cnt_clk_src_ext_osc_clk;

    /* Set the EWDG timeout interrupt interval to 100ms
     * Set the EWDG timeout reset interval to 101ms
     */
    config.cnt_src_freq = ewdg_src_clk_freq;
    config.ctrl_config.timeout_reset_us = 101UL * 1000UL;
    config.ctrl_config.timeout_interrupt_us = 100UL * 1000UL;

    /* Initialize the EWDG */
    status = ewdg_init(HPM_EWDG0, &config);
    if (status != status_success) {
        printf(" EWDG initialization failed, error_code=%d\n", status);
        return;
    }

#if defined(EWDG_TIMEOUT_INTERRUPT_REQUIRE_EDGE_TRIGGER) && (EWDG_TIMEOUT_INTERRUPT_REQUIRE_EDGE_TRIGGER == 1)
    __plic_enable_irq_edge_trigger(HPM_PLIC_BASE, IRQn_EWDG0);
#endif
    intc_m_enable_irq_with_priority(IRQn_EWDG0, 1);

    while (!has_interrupt) {
    }
    /* Service EWDG */
    ewdg_refresh(HPM_EWDG0);

    /* Disable EWDG */
    ewdg_disable(HPM_EWDG0);

    printf("EWDG timeout interrupt test passed\n");
}

SDK_DECLARE_EXT_ISR_M(IRQn_EWDG0, ewdg_isr)
void ewdg_isr(void)
{
    uint32_t ewdg_stat = ewdg_get_status_flags(HPM_EWDG0);
    has_interrupt = true;
    if ((ewdg_stat & EWDG_EVENT_TIMEOUT_INTERRUPT) != 0) {
        ewdg_refresh(HPM_EWDG0);
    }
    ewdg_clear_status_flags(HPM_EWDG0, ewdg_stat);
}
