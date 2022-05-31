/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_rtc_drv.h"

#ifndef TEST_RTC
#define TEST_RTC HPM_RTC
#endif

int main(void)
{
    board_init();

    printf("rtc test\n");

    // Configure RTC time
    struct tm cfg_date_time;
    cfg_date_time.tm_year = 121; // Year: 2021 (started from 1900)
    cfg_date_time.tm_mon = 6;    // Month: July (Started from 0)
    cfg_date_time.tm_mday = 12;  // Day in Month: 12
    cfg_date_time.tm_hour = 16;  // Hour: 16
    cfg_date_time.tm_min = 45;   // Minute: 45
    cfg_date_time.tm_sec = 41;   // Second: 41
    time_t cfg_time = mktime(&cfg_date_time); // Convert date time to tick
    rtc_config_time(TEST_RTC, cfg_time);

    // Configure RTC alarm
    rtc_alarm_config_t alarm_cfg;
    alarm_cfg.index = 0;
    alarm_cfg.period = ALARM_PERIOD_ONE_SEC;
    alarm_cfg.type = RTC_ALARM_TYPE_PERIODIC;
    rtc_config_alarm(TEST_RTC, &alarm_cfg);


    while(1) {
        if (rtc_is_alarm_flag_asserted(TEST_RTC, 0)) {
            time_t current_time = rtc_get_time(TEST_RTC);
            printf("Current time is %s\n", ctime(&current_time));
            rtc_clear_alarm_flag(TEST_RTC, 0);
        }
    }

    return 0;
}
