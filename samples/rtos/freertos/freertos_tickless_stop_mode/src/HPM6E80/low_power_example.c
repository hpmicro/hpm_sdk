/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"

/* In order to further reduce power consumption, turn off unnecessary peripherals here,
 * keep the required peripherals running, and turn on the wake-up interrupt of the
 * corresponding peripherals. */
ATTR_RAMFUNC void vPreSleepUserCustomProcessing(unsigned long uxExpectedIdleTime)
{
    (void)(uxExpectedIdleTime);

    /* Turn off the LED to indicate that we are in low power */
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, board_get_led_gpio_off_level());
}

ATTR_RAMFUNC void vPostSleepUserCustomProcessing(unsigned long uxExpectedIdleTime)
{
    (void)(uxExpectedIdleTime);
    /* Turn on the LED to indicate that we are out of low power */
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, board_get_led_gpio_off_level() ? 0 : 1);
}
