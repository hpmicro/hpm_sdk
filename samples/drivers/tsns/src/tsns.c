/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_tsns_drv.h"
#include "hpm_otp_drv.h"

#ifndef TEST_TSNS
#define TEST_TSNS HPM_TSNS
#endif

#ifndef VERBOSE
#define VERBOSE 0
#endif

#define SHOW_TEMP_PERIOD 1000

#if VERBOSE
#define DEBUG_INFO printf
#else
#define DEBUG_INFO(x, ...)
#endif


#define OTP_SHADOW_TSNS (21U)
#define OTP_SHADOW_TSNS_BASE_MASK ((1 << 12) - 1)
#define OTP_SHADOW_TSNS_BASE_EN_MASK (1 << 24)
#define OTP_TSNS_25C_BASE (0x956U)
#define OTP_TSNS_RATIO (0x870 << 12)

void calibrate_tsns(void)
{
    volatile float temp;
    uint32_t i = 0;
    i = otp_read_from_shadow(OTP_SHADOW_TSNS) & OTP_SHADOW_TSNS_BASE_MASK;

    if (i) {
        /* assume calibrated already */
        return;
    }

    i = OTP_TSNS_25C_BASE;
    printf("before calibration: temp %.1f, read from shadow 0x%x\n", tsns_get_current_temp(TEST_TSNS), i);
    printf("calibration starts\n");
    while (1) {
        printf(".");
        temp = tsns_get_current_temp(TEST_TSNS);
        if (temp > 25.1) {
            DEBUG_INFO("%.1f too high: ", temp);
            i -= 1;
        } else if (temp < 24.9) {
            DEBUG_INFO("%.1f too low: ", temp);
            i += 1;
        } else {
            break;
        }
        if (status_success != otp_write_shadow_register(OTP_SHADOW_TSNS, OTP_SHADOW_TSNS_BASE_EN_MASK
                                                                       | OTP_TSNS_RATIO
                                                                       | (i & OTP_SHADOW_TSNS_BASE_MASK))) {
            while (1) {
                ;
            }
        }
        DEBUG_INFO("set new offset: 0x%X\n", i);
        board_delay_ms(1000);
    }
    printf("\ncalibration done\n");
    printf("after calibration: temp %.1f, read from shadow 0x%x\n", tsns_get_current_temp(TEST_TSNS), otp_read_from_shadow(OTP_SHADOW_TSNS));
}

void show_temperature(void)
{
    printf("current temperature %.1f (0x%x)\n", tsns_get_current_temp(TEST_TSNS), tsns_get_current_temp_in_raw(TEST_TSNS));
}

int main(void)
{
    board_init();

#if !defined(__ICCRISCV__) || (defined(_DLIB_FILE_DESCRIPTOR) && (_DLIB_FILE_DESCRIPTOR == 1))
    /* disable printf buffering for stdout */
    setbuf(stdout, NULL);
#endif

    clock_add_to_group(clock_tsns, 0);
    tsns_enable_continuous_mode(TEST_TSNS);
    tsns_enable(TEST_TSNS);

    calibrate_tsns();

    while (1) {
        board_delay_ms(SHOW_TEMP_PERIOD);
        show_temperature();
    }
    return 0;
}
