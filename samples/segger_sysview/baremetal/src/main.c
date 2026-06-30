/*
 * Copyright (c) 2025-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"

#define PERIOD_IN_MS (1000)
#ifndef MCHTMR_CLK_NAME
#define MCHTMR_CLK_NAME (clock_mchtmr0)
#endif

#define APP_BOARD_GPTMR               BOARD_GPTMR_PWM
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_PWM_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_PWM_IRQ
#define APP_BOARD_GPTMR_CMPINIT       (0)         /*it's mean compare output start state, now is low*/
#define APP_BOARD_RELOAD_MS           (2000)
#define APP_BOARD_CMP_MS              (APP_BOARD_RELOAD_MS / 2)

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (100000UL) /* 100KHz */
#endif

#define MARK_ID_HPM_DELAY 100


SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)
void isr_mchtmr(void)
{
    static volatile int count;
    static uint32_t freq;

    if (freq == 0) {
        freq = clock_get_frequency(MCHTMR_CLK_NAME);
    }
    mchtmr_delay(HPM_MCHTMR, (uint64_t) PERIOD_IN_MS * freq / 1000);
    count++;
    SEGGER_SYSVIEW_PrintfHostEx("mchtmr interrupt count: %d", SEGGER_SYSVIEW_LOG, count);
}

static void output_compare_config(void)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq;

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);

#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    /* Target timer clock = 100KHz.
     * prescaler = source_clock / target = gptmr_freq / 100000
     * E.g. 100MHz / 1000 = 100KHz, each tick = 10us.
     */
    config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
    config.reload = ((uint64_t)APP_BOARD_RELOAD_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
    config.cmp[0] = APP_GPTMR_TARGET_FREQ / 1000 * APP_BOARD_CMP_MS;
#else
    config.reload = gptmr_freq / 1000 * APP_BOARD_RELOAD_MS;
    config.cmp[0] = gptmr_freq / 1000 * APP_BOARD_CMP_MS;
#endif
    config.cmp[1] = config.reload;
    config.cmp_initial_polarity_high = APP_BOARD_GPTMR_CMPINIT;

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_CMP_IRQ_MASK(APP_BOARD_GPTMR_CH, 0));
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    static uint32_t count;

    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0));
        count++;
        SEGGER_SYSVIEW_PrintfHostEx("GPTMR interrupt count: %d", SEGGER_SYSVIEW_LOG, count);
    }
}

int main(void)
{
    board_init();

    /*
     * RTT Init (optional)
     */
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_printf(0, "RTT: SEGGER SYSVIEW example");

    /*
     * Use RTT as STDIO (need CONFIG_NDEBUG_CONSOLE=1)
     */
    printf("SEGGER SYSVIEW example\n");

    /*
     * SYSVIEW Init
     */
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();
    SEGGER_SYSVIEW_Print("SYSVIEW: SEGGER SYSVIEW example");

    /*
     * System Description (optional)
     * Upload interruption description for host computer viewing
     * Format: "I#<irq_num>=<irq_name>"
     */
    char desc[32];
    sprintf(desc, "I#%d=GPTMR_IRQ(%d)", APP_BOARD_GPTMR_IRQ, APP_BOARD_GPTMR_IRQ);
    SEGGER_SYSVIEW_SendSysDesc(desc);

    /*
     * Mark Test
     */
    SEGGER_SYSVIEW_NameMarker(MARK_ID_HPM_DELAY, "HPM Delay Mark");
    SEGGER_SYSVIEW_MarkStart(MARK_ID_HPM_DELAY);
    board_delay_ms(10);
    SEGGER_SYSVIEW_Mark(MARK_ID_HPM_DELAY);
    board_delay_ms(5);
    SEGGER_SYSVIEW_MarkStop(MARK_ID_HPM_DELAY);

    /*
     * Interrupt Test
     */
    output_compare_config();
    enable_mchtmr_irq();
    mchtmr_delay(HPM_MCHTMR, 1);

    while (1) {
    }
    return 0;
}
