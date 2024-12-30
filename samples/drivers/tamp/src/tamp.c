/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_tamp_drv.h"

static void init_tamper_config(void);

int main(void)
{
    board_init();
    init_tamper_pins();

    printf("Tamper example\n");

    init_tamper_config();

    while (1) {
        ;
    }

    return 0;
}

SDK_DECLARE_EXT_ISR_M(IRQn_BVIO, tamper_isr)
void tamper_isr(void)
{
    if (tamp_check_ch_flag(HPM_TAMP, BOARD_TAMP_ACTIVE_CH) || tamp_check_ch_flag(HPM_TAMP, BOARD_TAMP_ACTIVE_CH + 1)) {
        tamp_clear_ch_flag(HPM_TAMP, BOARD_TAMP_ACTIVE_CH);
        tamp_clear_ch_flag(HPM_TAMP, BOARD_TAMP_ACTIVE_CH + 1);
        printf("Tamper active mode trigged!\n");
    }

#ifndef BOARD_TAMP_NO_LEVEL_PINS
    if (tamp_check_ch_flag(HPM_TAMP, BOARD_TAMP_LOW_LEVEL_CH)) {
        tamp_clear_ch_flag(HPM_TAMP, BOARD_TAMP_LOW_LEVEL_CH);
        printf("Tamper passive mode trigged!\n");
    }
#endif
}

/* static function definition */
static void init_tamper_config(void)
{
    tamper_ch_config_t config;

    tamp_get_default_ch_config(HPM_TAMP, &config);
    config.enable = true;
    config.active_mode = true;
    config.expect_high_level = false;
    config.filter_bypass = false;
    config.filter_len = filter_len_8_cycles;
    config.speed = spd_32_times_per_sec;
    config.auto_recover = true;
    config.poly = 0x80200003;
    config.lfsr = 0x5AA5;
    tamp_init_ch_config(HPM_TAMP, BOARD_TAMP_ACTIVE_CH, &config);
    tamp_enable_ch_irq(HPM_TAMP, BOARD_TAMP_ACTIVE_CH, true);
    tamp_enable_ch_irq(HPM_TAMP, BOARD_TAMP_ACTIVE_CH + 1, true);

#ifndef BOARD_TAMP_NO_LEVEL_PINS
    tamp_get_default_ch_config(HPM_TAMP, &config);
    config.enable = true;
    config.active_mode = false;
    config.expect_high_level = false;
    config.filter_bypass = false;
    config.filter_len = filter_len_128_cycles;
    config.speed = spd_1_time_per_sec;
    config.auto_recover = false;
    config.poly = 0;
    config.lfsr = 0;
    tamp_init_ch_config(HPM_TAMP, BOARD_TAMP_LOW_LEVEL_CH, &config);
    tamp_enable_ch_irq(HPM_TAMP, BOARD_TAMP_LOW_LEVEL_CH, true);
#endif

    intc_m_enable_irq_with_priority(IRQn_BVIO, 1);
}
