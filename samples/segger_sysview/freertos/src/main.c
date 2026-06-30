/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include <SEGGER_RTT.h>
#include <SEGGER_SYSVIEW.h>

#if !defined(portTIMER_SOURCE_GPTMR) || (portTIMER_SOURCE_GPTMR == 0)

#define task3_PRIORITY    (configMAX_PRIORITIES - 3U)
static SemaphoreHandle_t xSemaphore;

#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
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
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    static uint32_t count;

    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CMP_STAT_MASK(APP_BOARD_GPTMR_CH, 0));
        count++;
        SEGGER_SYSVIEW_PrintfHostEx("GPTMR interrupt count: %d", SEGGER_SYSVIEW_LOG, count);
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
    }

    /*
     * Note:
     *    The Yield is required for SYSVIEW recording the ISR exit to scheduler event.
     */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void task3(void *pvParameters)
{
    (void)pvParameters;
    xSemaphore = xSemaphoreCreateBinary();
    output_compare_config();
    for (;;) {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        SEGGER_SYSVIEW_PrintfHostEx("task3", SEGGER_SYSVIEW_LOG);
    }
}
#endif

#define task1_PRIORITY    (configMAX_PRIORITIES - 5U)
#define task2_PRIORITY    (configMAX_PRIORITIES - 4U)


volatile bool led_on;

static void task1(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        SEGGER_SYSVIEW_PrintfHostEx("task1", SEGGER_SYSVIEW_LOG);
        vTaskDelay(100);
    }
}

static void task2(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
#ifdef BOARD_LED_GPIO_CTRL
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
#endif
        SEGGER_SYSVIEW_PrintfHostEx("Toggle the led pin level.", SEGGER_SYSVIEW_LOG);
        led_on = !led_on;
        vTaskDelay(500);
    }
}

/*
 * Note: Adding follow code in FreeRTOSConfig.h is necessary for SYSVIEW to record task start/end events
 *
 * #ifdef CONFIG_SEGGER_SYSVIEW
 * #include <SEGGER_SYSVIEW_FreeRTOS.h>
 * #endif
 *
 * You can refer to samples/segger_sysview/freertos/src/FreeRTOSConfig.h
 */
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

#if !defined(portTIMER_SOURCE_GPTMR) || (portTIMER_SOURCE_GPTMR == 0)
    /*
     * System Description (optional)
     * Upload interruption description for host computer viewing
     * Format: "I#<irq_num>=<irq_name>"
     */
    char desc[32];
    sprintf(desc, "I#%d=GPTMR_IRQ(%d)", APP_BOARD_GPTMR_IRQ, APP_BOARD_GPTMR_IRQ);
    SEGGER_SYSVIEW_SendSysDesc(desc);

    if (xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE + 256U, NULL, task3_PRIORITY, NULL) != pdPASS) {
        SEGGER_SYSVIEW_PrintfHostEx("Task3 creation failed!.", SEGGER_SYSVIEW_ERROR);
        for (;;) {
            ;
        }
    }
#endif

    board_init_gpio_pins();
#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, !board_get_led_gpio_off_level());
#endif
    led_on = true;

    if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 256U, NULL, task1_PRIORITY, NULL) != pdPASS) {
        SEGGER_SYSVIEW_PrintfHostEx("Task1 creation failed!.", SEGGER_SYSVIEW_ERROR);
        for (;;) {
            ;
        }
    }
    if (xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE + 256U, NULL, task2_PRIORITY, NULL) != pdPASS) {
        SEGGER_SYSVIEW_PrintfHostEx("Task2 creation failed!.", SEGGER_SYSVIEW_ERROR);
        for (;;) {
            ;
        }
    }
    vTaskStartScheduler();
    for (;;) {
        ;
    }
    return 0;
}
