/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
#include "hpm_mchtmr_drv.h"
#endif

#define GPIO_TOGGLE_COUNT 5

#if defined(DEBOUNCE_THRESHOLD_IN_MS)
#define MCHTMR_CLK_NAME (clock_mchtmr0)
static volatile uint64_t gpio_isr_rel_time; /* mark the real time from mchtimer */
static volatile uint64_t gpio_isr_pre_time; /* mark the last time marked on gpio_isr */
static volatile uint32_t level_value; /* mark the level */
static volatile uint32_t pre_level_value; /* mark the level before enter the isr */
static uint32_t debounce_threshold; /* debounce threshold */
#endif

SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)
void isr_gpio(void)
{
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
    /*  To eliminate debounce,here do 3 doftware judgments:
    *  1.mark the time and pre_time, if difference less than 150ms(experimental value), judge as jitter;
    *  2.read the level, if is 1, then judge as jitter;
    *  3.mark the pre_level,if pre_level is 0, then judge as jitter.
    */
    gpio_isr_rel_time = mchtmr_get_count(HPM_MCHTMR);
    level_value = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    if ((gpio_isr_rel_time - gpio_isr_pre_time > debounce_threshold) && (level_value != pre_level_value)) {
        gpio_isr_pre_time = gpio_isr_rel_time;
#endif
#ifdef BOARD_LED_GPIO_CTRL
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                            BOARD_LED_GPIO_PIN);
        printf("toggle led pin output\n");
#else
#if defined(GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT) && (GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT == 1)
        if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == false) {
            printf("user key pressed\n");
        } else {
            printf("user key released\n");
        }
#else
        printf("user key pressed\n");
#endif
#endif
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
    }
#endif
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                        BOARD_APP_GPIO_PIN);
}

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
#if defined(GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT) && (GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT == 1)
    trigger = gpio_interrupt_trigger_edge_both;
#else
    trigger = gpio_interrupt_trigger_edge_falling;
#endif
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN, trigger);
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX,
                           BOARD_APP_GPIO_PIN);
    intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 1);
    while (1) {
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
        pre_level_value = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#else
        __asm("wfi");
#endif
    }
}

#ifdef BOARD_LED_GPIO_CTRL
void test_gpio_toggle_output(void)
{
    printf("toggling led %u times in total\n", GPIO_TOGGLE_COUNT);
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                           BOARD_LED_GPIO_PIN);
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                        BOARD_LED_GPIO_PIN, board_get_led_gpio_off_level());

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

#if defined(DEBOUNCE_THRESHOLD_IN_MS)
    uint32_t mchtmr_freq;
    mchtmr_freq = clock_get_frequency(MCHTMR_CLK_NAME);
    debounce_threshold = DEBOUNCE_THRESHOLD_IN_MS * mchtmr_freq / 1000;
#endif

#ifdef BOARD_LED_GPIO_CTRL
    test_gpio_toggle_output();
#endif
    test_gpio_input_interrupt();

    while (1);
    return 0;
}
