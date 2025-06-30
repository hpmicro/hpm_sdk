/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"

#define APP_BOARD_PWM                 BOARD_GPTMR_PWM
#define APP_BOARD_PWM_CH              BOARD_GPTMR_PWM_CHANNEL
#define APP_BOARD_SYNC_PWM            BOARD_GPTMR_PWM_SYNC
#define APP_BOARD_SYNC_PWM_CH         BOARD_GPTMR_PWM_SYNC_CHANNEL

#define APP_PWM_FREQ                  (100000U)
#define APP_PWM_DUTY                  (50U)

typedef struct {
    uint32_t gptmr_frequency;
    uint32_t pwm_frequency;
    uint8_t  pwm_duty;
    uint8_t  cn_index;
    bool     cmp_init_high;
} gptmr_cfg_t;

static void pwm_config(GPTMR_Type *ptr, gptmr_cfg_t *cfg);

uint32_t gptmr_freq;

int main(void)
{
    gptmr_cfg_t cfg;
    board_init();
    printf("gptmr counter_sync testing\n");
    init_gptmr_pins(APP_BOARD_PWM);
    init_gptmr_pins(APP_BOARD_SYNC_PWM);

    cfg.gptmr_frequency = board_init_gptmr_clock(APP_BOARD_PWM);
    cfg.pwm_frequency   = APP_PWM_FREQ;
    cfg.pwm_duty        = APP_PWM_DUTY;
    cfg.cmp_init_high   = false;
    cfg.cn_index        = APP_BOARD_PWM_CH;
    pwm_config(APP_BOARD_PWM, &cfg);

    cfg.gptmr_frequency = board_init_gptmr_clock(APP_BOARD_SYNC_PWM);
    cfg.cn_index        = APP_BOARD_SYNC_PWM_CH;
    pwm_config(APP_BOARD_SYNC_PWM, &cfg);

    /* frist: start counter*/
    gptmr_start_counter(APP_BOARD_PWM, APP_BOARD_PWM_CH);
    gptmr_start_counter(APP_BOARD_SYNC_PWM, APP_BOARD_SYNC_PWM_CH);

    /* second: enable soft sync channel*/
    gptmr_trigger_channel_software_sync(APP_BOARD_PWM, GPTMR_CH_GCR_SWSYNCT_MASK(APP_BOARD_PWM_CH) | GPTMR_CH_GCR_SWSYNCT_MASK(APP_BOARD_SYNC_PWM_CH));

    /* third: enable compare output, in order to no wave out in power-on, delay best */
    board_delay_us(100);
    gptmr_enable_cmp_output(APP_BOARD_PWM, APP_BOARD_PWM_CH);
    gptmr_enable_cmp_output(APP_BOARD_SYNC_PWM, APP_BOARD_SYNC_PWM_CH);
    while (1) {
        __asm("nop");
    }
    return 0;
}

static void pwm_config(GPTMR_Type *ptr, gptmr_cfg_t *cfg)
{
    gptmr_channel_config_t config;
    uint32_t reload;

    gptmr_channel_get_default_config(ptr, &config);
    reload = cfg->gptmr_frequency / cfg->pwm_frequency;
    config.reload = reload;
    config.cmp[0] = (reload * cfg->pwm_duty) / 100;
    config.cmp[1] = reload;
    config.cmp_initial_polarity_high = cfg->cmp_init_high;
    config.enable_software_sync = true;
    config.enable_cmp_output = false;
    gptmr_stop_counter(ptr, cfg->cn_index);
    gptmr_channel_config(ptr, cfg->cn_index, &config, false);
    gptmr_channel_reset_count(ptr, cfg->cn_index);
}
