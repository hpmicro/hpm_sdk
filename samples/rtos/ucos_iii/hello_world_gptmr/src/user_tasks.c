/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "user_tasks.h"
#include "board.h"
#include "hpm_gpio_drv.h"

OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

void led0_task(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    while (1) {
#ifdef BOARD_LED_GPIO_CTRL
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
        printf("Toggle the led pin level.\n");
#else
        printf("Toggle the flag.\n");
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
        printf("LED is %s.\n", led_on ? "on" : "off");
#else
        printf("flag is %s.\n", led_on ? "set" : "not set");
#endif
        OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}