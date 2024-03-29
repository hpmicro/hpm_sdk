/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "includes.h"
#include "task.h"

/*  HPM example includes. */
#include "board.h"
#include "hpm_gpio_drv.h"
#include <stdio.h>

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    board_init();
    board_init_gpio_pins();

#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, BOARD_LED_ON_LEVEL);
#endif
    OSInit(&err);
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
    OSStart(&err);
    for (;;) {
        ;
    }
    return 0;
}