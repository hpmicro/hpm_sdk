/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"

#ifndef BOARD_LED_OFF_LEVEL
#define BOARD_LED_OFF_LEVEL 0
#endif

#define GPIO_TOGGLE_COUNT 5

void isr_gpio(void)
{
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                        BOARD_APP_GPIO_PIN);
#ifdef BOARD_LED_GPIO_CTRL
    gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                          BOARD_LED_GPIO_PIN);
    printf("toggle led pin output\n");
#else
    printf("user key pressed\n");
#endif

}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)

void test_gpio_input_interrupt(void)
{
    gpio_interrupt_trigger_t trigger;

    printf("input interrupt\n");
#ifdef BOARD_LED_GPIO_CTRL
    printf("user led will be switched on off based on user switch\n");
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                           BOARD_LED_GPIO_PIN);
#endif
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN);

    trigger = gpio_interrupt_trigger_edge_falling;

    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN, trigger);
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN);
    intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 1);
    while(1) {
        __asm("wfi");
    }
}

#ifdef BOARD_LED_GPIO_CTRL
void test_gpio_toggle_output(void)
{
    printf("toggling led %u times in total\n", GPIO_TOGGLE_COUNT);
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                           BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                        BOARD_LED_GPIO_PIN, BOARD_LED_OFF_LEVEL);

    for (uint32_t i = 0; i < GPIO_TOGGLE_COUNT; i++) {
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                            BOARD_LED_GPIO_PIN);
        board_delay_ms(500);
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                            BOARD_LED_GPIO_PIN);
        board_delay_ms(500);
        printf("toggling led %u/%u times\n", i + 1, GPIO_TOGGLE_COUNT);
    }
}
#endif

int main(void)
{
    board_init();
    board_init_gpio_pins();
    printf("gpio example\n");

#ifdef BOARD_LED_GPIO_CTRL
    test_gpio_toggle_output();
#endif
    test_gpio_input_interrupt();

    while(1);
    return 0;
}
