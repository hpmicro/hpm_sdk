/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "led_task.h"
#include "board.h"

OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg)
{
    OS_ERR err;
    uint32_t temp = 0;
    uint8_t i = 0;
    CPU_SR_ALLOC();
    (void)p_arg;
    while (1) {
        CPU_CRITICAL_ENTER();
        printf("led0 task run %d times\r\n", temp++);
        CPU_CRITICAL_EXIT();
        for (i = 0; i < 5; i++)
            printf("LED : 12345 \r\n");
        board_led_toggle();
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}