/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* uC/OS-III kernel includes. */
#include "includes.h"
#include "task.h"

/* HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include <SEGGER_RTT.h>
#include <SEGGER_SYSVIEW.h>

/*
 * Note: To enable SystemView tracing for uC/OS-III, the following
 * configuration must be set in os_cfg.h:
 *
 * #define OS_CFG_TRACE_EN            1u
 * #define OS_CFG_TRACE_API_ENTER_EN  1u
 * #define OS_CFG_TRACE_API_EXIT_EN   1u
 *
 * These are already configured in ucos_cfg/os_cfg.h for this example.
 */

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();

    board_init();

    /*
     * RTT Init (optional)
     */
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_printf(0, "RTT: SEGGER SYSVIEW uC/OS-III example\r\n");

    /*
     * Use RTT as STDIO (need CONFIG_NDEBUG_CONSOLE=1)
     */
    printf("SEGGER SYSVIEW uC/OS-III example\r\n");

    /*
     * SYSVIEW Init
     * uC/OS-III will automatically call OS_TRACE_INIT() internally
     * when OS_CFG_TRACE_EN is enabled.
     */
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();
    SEGGER_SYSVIEW_Print("SYSVIEW: SEGGER SYSVIEW uC/OS-III example\r\n");

    /* Initialize LED GPIO */
    board_init_gpio_pins();
#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, !board_get_led_gpio_off_level());
#endif

    OSInit(&err);
    if (err != OS_ERR_NONE) {
        SEGGER_SYSVIEW_PrintfHostEx("OSInit failed!", SEGGER_SYSVIEW_ERROR);
        for (;;) {
            ;
        }
    }

    CPU_CRITICAL_ENTER();
    OSTaskCreate((OS_TCB *)&StartTaskTCB,
        (CPU_CHAR *)"start task",
        (OS_TASK_PTR)start_task,
        (void *)0,
        (OS_PRIO)START_TASK_PRIO,
        (CPU_STK *)&START_TASK_STK[0],
        (CPU_STK_SIZE)START_STK_SIZE / 10,
        (CPU_STK_SIZE)START_STK_SIZE,
        (OS_MSG_QTY)0,
        (OS_TICK)0,
        (void *)0,
        (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
        (OS_ERR *)&err);
    CPU_CRITICAL_EXIT();

    if (err != OS_ERR_NONE) {
        SEGGER_SYSVIEW_PrintfHostEx("Start task creation failed!", SEGGER_SYSVIEW_ERROR);
        for (;;) {
            ;
        }
    }

    OSStart(&err);
    for (;;) {
        ;
    }
    return 0;
}