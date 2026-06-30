/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "user_tasks.h"
#include "start_task.h"
#include "board.h"
#include "hpm_gpio_drv.h"
#include <SEGGER_SYSVIEW.h>

#if !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)

#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif

#define APP_BOARD_GPTMR               BOARD_GPTMR_PWM
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_PWM_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_PWM_IRQ
#define APP_BOARD_GPTMR_CMPINIT       (0)
#define APP_BOARD_RELOAD_MS           (2000)
#define APP_BOARD_CMP_MS              (APP_BOARD_RELOAD_MS / 2)

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (100000UL) /* 100KHz */
#endif

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
    OS_ERR err;
    static uint32_t count;

    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0));
        count++;
        SEGGER_SYSVIEW_PrintfHostEx("GPTMR interrupt count: %d", SEGGER_SYSVIEW_LOG, count);
        OSSemPost(&gptmr_sem, OS_OPT_POST_1, &err);
    }
}

OS_TCB GptmrTaskTCB;
CPU_STK GPTMR_TASK_STK[GPTMR_STK_SIZE];

void gptmr_task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;

    /*
     * System Description (optional)
     * Upload interruption description for host computer viewing
     * Format: "I#<irq_num>=<irq_name>"
     */
    char desc[32];
    sprintf(desc, "I#%d=GPTMR_IRQ(%d)", APP_BOARD_GPTMR_IRQ, APP_BOARD_GPTMR_IRQ);
    SEGGER_SYSVIEW_SendSysDesc(desc);

    /* Initialize GPTMR */
    output_compare_config();

    while (1) {
        OSSemPend(&gptmr_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
        SEGGER_SYSVIEW_PrintfHostEx("gptmr_task", SEGGER_SYSVIEW_LOG);
    }
}

#endif /* !defined(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR) */

OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1) {
#ifdef BOARD_LED_GPIO_CTRL
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
        SEGGER_SYSVIEW_PrintfHostEx("Toggle LED", SEGGER_SYSVIEW_LOG);
#else
        SEGGER_SYSVIEW_PrintfHostEx("Toggle flag", SEGGER_SYSVIEW_LOG);
#endif

        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

OS_TCB PrintTaskTCB;
CPU_STK PRINT_TASK_STK[PRINT_STK_SIZE];

void print_task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1) {
#ifdef BOARD_LED_GPIO_CTRL
        bool led_on = gpio_get_pin_output_status(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN) == board_get_led_gpio_off_level() ? false : true;
        SEGGER_SYSVIEW_PrintfHostEx(led_on ? "LED is on" : "LED is off", SEGGER_SYSVIEW_LOG);
#else
        SEGGER_SYSVIEW_PrintfHostEx("Print task running", SEGGER_SYSVIEW_LOG);
#endif
        OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}