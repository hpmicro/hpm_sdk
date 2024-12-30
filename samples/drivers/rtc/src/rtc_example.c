/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_rtc_drv.h"
#include "hpm_ppor_drv.h"
#include "hpm_bpor_drv.h"

#ifndef TEST_RTC
#define TEST_RTC HPM_RTC
#endif


void show_menu(void);

void rtc_show_time(void);

void rtc_set_time(void);

void rtc_set_alarm(void);

void rtc_set_retention(void);

void rtc_set_interrupt(void);

void rtc_isr(void);

static volatile bool rtc_interrupt_occurred;

int main(void)
{
    board_init();

    show_menu();

    uint32_t reset_flags = ppor_reset_get_flags(HPM_PPOR);
    if (reset_flags != 0) {
        time_t current_time = rtc_get_time(TEST_RTC);
        printf("After reset, the RTC time is %s\n", ctime(&current_time));
        ppor_reset_clear_flags(HPM_PPOR, reset_flags);
    }

    while (1) {
        char option = getchar();
        putchar(option);
        putchar('\n');
        switch (option) {
        case '1':
            rtc_show_time();
            break;
        case '2':
            rtc_set_alarm();
            break;
        case '3':
            rtc_set_retention();
            break;
        case '4':
            rtc_set_time();
            break;
        case '5':
            rtc_set_interrupt();
            break;
        default:
            show_menu();
            break;
        }
    }

    return 0;
}

void rtc_show_time(void)
{
    time_t current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
}

void rtc_set_time(void)
{
    char month[20];
    int month_of_year;
    int year;
    int date_of_month;
    int hour;
    int minute;
    int second;
    sscanf(__DATE__, "%s %d %d", month, &date_of_month, &year);
    sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);
    switch (month[0]) {
    case 'J':
        /* Jan, Jun, July */
        month_of_year = (month[1] == 'a') ? 1 : ((month[2] == 'n') ? 6 : 7);
        break;
    case 'F':
        /* Feb */
        month_of_year = 2;
        break;
    case 'M':
        /* Mar, May */
        month_of_year = (month[2] == 'r') ? 3 : 5;
        break;
    case 'A':
        /* Apr, Aug */
        month_of_year = (month[1] == 'p') ? 4 : 8;
        break;
    case 'S':
        /* Sept */
        month_of_year = 9;
        break;
    case 'O':
        /* Oct */
        month_of_year = 10;
        break;
    case 'N':
        /* Nov */
        month_of_year = 11;
        break;
    case 'D':
        /* Dec */
        month_of_year = 12;
        break;
    default:
        /* Supress compiling warning */
        month_of_year = 1;
        break;
    }

    /* Configure Time */
    struct tm cfg_date_time;
    cfg_date_time.tm_year = year - 1900;        /* Year: (start from 1900) */
    cfg_date_time.tm_mon = month_of_year - 1;   /* Month:(start from 0) */
    cfg_date_time.tm_mday = date_of_month;      /* Day in Month */
    cfg_date_time.tm_hour = hour;               /* Hour */
    cfg_date_time.tm_min = minute;              /* Minute */
    cfg_date_time.tm_sec = second;              /* Second */
    time_t cfg_time = mktime(&cfg_date_time); /* Convert date time to tick */
    rtc_config_time(TEST_RTC, cfg_time);

    printf("Set RTC time to %s\n", ctime(&cfg_time));
}

void rtc_set_alarm(void)
{
    time_t current_time;
    rtc_alarm_config_t alarm_cfg;
    uint32_t elapsed_seconds;
    uint64_t start_ticks;
    bool timeout = false;
    /* Case 1, One-time RTC alarm */
    printf("Case 1: One-time RTC alarm, generate alarm in 2 seconds\n");
    alarm_cfg.index = 0;
    alarm_cfg.period = ALARM_PERIOD_ONE_SEC * 2;
    alarm_cfg.type = RTC_ALARM_TYPE_ONE_SHOT;
    rtc_config_alarm(TEST_RTC, &alarm_cfg);
    start_ticks = hpm_csr_get_core_mcycle();
    timeout = false;
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    while (!rtc_is_alarm_flag_asserted(TEST_RTC, 0)) {
        elapsed_seconds = (hpm_csr_get_core_mcycle() - start_ticks) / clock_get_frequency(clock_cpu0);
        if (elapsed_seconds > 3) {
            timeout = true;
            break;
        }
    }
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    printf("One-time RTC alarm test %s\n", timeout ? "FAILED" : "PASSED");
    rtc_clear_alarm_flag(TEST_RTC, 0);


    /* Case 2, Periodic RTC alarm */
    printf("Case 2: Periodical RTC alarm, generate alarm per second\n");
    alarm_cfg.index = 0;
    alarm_cfg.period = ALARM_PERIOD_ONE_SEC;
    alarm_cfg.type = RTC_ALARM_TYPE_PERIODIC;
    rtc_config_alarm(TEST_RTC, &alarm_cfg);
    rtc_enable_alarm_interrupt(TEST_RTC, 0, true);
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    uint32_t rtc_alarm_cnt = 0;
    while (rtc_alarm_cnt < 5) {
        start_ticks = hpm_csr_get_core_mcycle();
        while (!rtc_is_alarm_flag_asserted(TEST_RTC, 0)) {
            elapsed_seconds = (hpm_csr_get_core_mcycle() - start_ticks) / clock_get_frequency(clock_cpu0);
            if (elapsed_seconds > 3) {
                timeout = true;
                break;
            }
        }
        if (timeout) {
            break;
        }
        printf("Periodical RTC alarm test round #%d\n", rtc_alarm_cnt);
        current_time = rtc_get_time(TEST_RTC);
        printf("Current time is %s\n", ctime(&current_time));
        rtc_clear_alarm_flag(TEST_RTC, 0);
        ++rtc_alarm_cnt;
    }
    rtc_enable_alarm_interrupt(TEST_RTC, 0, false);
    printf("Periodical RTC alarm test %s\n", timeout ? "FAILED" : "PASSED");

    /* Case 3, Absolute-time based RTC alarm */
    printf("Case 3: Absolute-time based RTC alarm, generate alarm in next 5 seconds\n");
    current_time = rtc_get_time(TEST_RTC);
    alarm_cfg.index = 0;
    alarm_cfg.period = (uint32_t) (current_time) + ALARM_PERIOD_ONE_SEC * 5;
    alarm_cfg.type = RTC_ALARM_TYPE_ABSOLUTE_TIME_ONE_SHOT;
    rtc_config_alarm(TEST_RTC, &alarm_cfg);
    start_ticks = hpm_csr_get_core_mcycle();
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    while (!rtc_is_alarm_flag_asserted(TEST_RTC, 0)) {
        elapsed_seconds = (hpm_csr_get_core_mcycle() - start_ticks) / clock_get_frequency(clock_cpu0);
        if (elapsed_seconds > 6) {
            timeout = true;
            break;
        }
    }
    printf("Absolute-time based RTC alarm test %s\n", timeout ? "FAILED" : "PASSED");
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));

    /* Restore RTC alarm settings */
    rtc_clear_alarm_flag(TEST_RTC, 0);
    rtc_enable_alarm_interrupt(TEST_RTC, 0, false);
}

void rtc_set_retention(void)
{
    static const char func_desc[] = "This example enable the retention mode of Battery domain and reset the SoC\n"
                                    "After reset, the RTC time will not be reset\n"
                                    "--------------------------------------------------------------------------\n"
                                    "NOTE: Please ensure the RTC battery is still working for this test\n";
    printf("%s\n", func_desc);
    bpor_enable_reg_value_retention(HPM_BPOR);
    ppor_sw_reset(HPM_PPOR, 1000);
}

void rtc_set_interrupt(void)
{
    uint64_t start_ticks;
    rtc_alarm_config_t alarm_cfg;
    bool timeout = false;

    /* Clear all pending interrupt flags first */
    rtc_clear_alarm_flag(TEST_RTC, 0);
    rtc_clear_alarm_flag(TEST_RTC, 1);

    printf("RTC alarm interrupt test, generate alarm in 2 seconds\n");
    alarm_cfg.index = 1;
    alarm_cfg.period = ALARM_PERIOD_ONE_SEC * 2;
    alarm_cfg.type = RTC_ALARM_TYPE_ONE_SHOT;
    rtc_config_alarm(TEST_RTC, &alarm_cfg);
    rtc_enable_alarm_interrupt(TEST_RTC, 1, true);
    intc_m_enable_irq_with_priority(IRQn_RTC, 1);
    start_ticks = hpm_csr_get_core_mcycle();
    time_t current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    while (!rtc_interrupt_occurred) {
        uint64_t elapsed_seconds = (hpm_csr_get_core_mcycle() - start_ticks) / clock_get_frequency(clock_cpu0);
        if (elapsed_seconds > 3) {
            timeout = true;
            break;
        }
    }
    current_time = rtc_get_time(TEST_RTC);
    printf("Current time is %s\n", ctime(&current_time));
    printf("RTC Alarm interrupt test %s\n", timeout ? "FAILED" : "PASSED");
    rtc_clear_alarm_flag(TEST_RTC, 1);
    intc_m_disable_irq(IRQn_RTC);
    rtc_interrupt_occurred = false;
}

SDK_DECLARE_EXT_ISR_M(IRQn_RTC, rtc_isr)
void rtc_isr(void)
{
    uint32_t rtc_alarm_flag = rtc_get_alarm_flags(TEST_RTC);
    if ((rtc_alarm_flag & RTC_ALARM_FLAG_ALARM0_MASK) != 0)
    {
        rtc_interrupt_occurred = true;
    }
    if ((rtc_alarm_flag & RTC_ALARM_FLAG_ALARM1_MASK) != 0) {
        rtc_interrupt_occurred = true;
    }
    rtc_clear_alarm_flags(TEST_RTC, RTC_ALARM_FLAG_ALARM0_MASK | RTC_ALARM_FLAG_ALARM1_MASK);
}

void show_menu(void)
{
    static const char menu_str[] = "\n"
                                   "---------------------------------------------------------------------------\n"
                                   "*                                                                         *\n"
                                   "*                                   RTC Demo                              *\n"
                                   "*                                                                         *\n"
                                   "*       1. Show RTC time                                                  *\n"
                                   "*       2. Demonstrate 3 types of RTC alarm                               *\n"
                                   "*       3. Demonstrate the retention mode of Battery Domain               *\n"
                                   "*       4. Set RTC Time                                                   *\n"
                                   "*       5. Demonstrate RTC interrupt                                      *\n"
                                   "*                                                                         *\n"
                                   "---------------------------------------------------------------------------\n";
    printf("%s\n", menu_str);
}
