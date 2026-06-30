/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "hpm_clock_drv.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ

#define APP_TICK_MS                   (1000)

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (100000UL) /* 100KHz */
#endif

static void opmode_config(void);

static volatile bool cmp_flag;

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0));
        cmp_flag = true;
    }
}

int main(void)
{
    cmp_flag = false;
    board_init();
    printf("gptmr opmode test\n");
    opmode_config();
    while (1) {
        if (cmp_flag == true) {
            cmp_flag =  false;
            printf("gptmr opmode test done\n");
        }
    }
}

static void opmode_config(void)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq;

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
    config.reload = ((uint64_t)APP_TICK_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
#else
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
#endif
    config.cmp[0] = config.reload - 1;
    config.enable_opmode = true;
    if (gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false) != status_success) {
        printf("config gptmr channel failed\n");
        return;
    }
    if (gptmr_channel_is_opmode(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH) == true) {
        printf("now gptmr is opmode one-shot mode\n");
    } else {
        printf("now gptmr is round mode\n");
    }
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    /* reload irq will be always set at one-shot mode at end, so it is recommended to use cmp irq */
    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CMP_IRQ_MASK(APP_BOARD_GPTMR_CH, 0));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

