/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_ptpc_drv.h"

#if defined(USE_S_MODE_IRQ) && USE_S_MODE_IRQ && (!defined(SOC_HAS_S_MODE) || !SOC_HAS_S_MODE)
#error SOC does not support supervisor mode IRQ
#endif

#if defined(USE_S_MODE_IRQ) && USE_S_MODE_IRQ && defined(SOC_HAS_S_MODE) && SOC_HAS_S_MODE
#define PLIC_TEST_MODE "Supervisor mode"
#define TEST_S_MODE 1
#else
#define PLIC_TEST_MODE "Machine mode"
#endif

#ifndef USE_NONVECTOR_MODE
#define VECTOR_MODE "Vector"
#else
#define VECTOR_MODE "Non-Vector"
#endif

#ifndef DISABLE_IRQ_PREEMPTIVE
#define PREEMPTIVE_MODE "Preemptive"
#else
#define PREEMPTIVE_MODE "Non-Preemptive"
#endif

void test_plicsw_interrupt(void);

void isr_gpio(void)
{
    printf("gpio interrupt start\n");
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#ifdef BOARD_LED_GPIO_CTRL
    gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
#endif
    test_plicsw_interrupt();
    printf("gpio interrupt end\n");
}
#ifdef TEST_S_MODE
SDK_DECLARE_EXT_ISR_S(BOARD_APP_GPIO_IRQ, isr_gpio)
#else
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)
#endif

void test_gpio_input_interrupt(void)
{
    gpio_interrupt_trigger_t trigger;

#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
#endif
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    trigger = gpio_interrupt_trigger_edge_falling;
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN, trigger);
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#ifdef TEST_S_MODE
    intc_s_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 5);
#else
    intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 5);
#endif
}

void isr_ptpc(void)
{
    printf("ptpc interrupt start\n");
    board_delay_ms(50000); /* could press button here */
    ptpc_clear_irq_status(HPM_PTPC, PTPC_EVENT_COMPARE0_MASK);
    printf("ptpc interrupt end\n");
}
#ifdef TEST_S_MODE
SDK_DECLARE_EXT_ISR_S(IRQn_PTPC, isr_ptpc)
#else
SDK_DECLARE_EXT_ISR_M(IRQn_PTPC, isr_ptpc)
#endif

void test_interrupt_nesting(void)
{
    uint32_t freq = clock_get_frequency(clock_ptpc);
    ptpc_config_t config = {0};

    ptpc_get_default_config(HPM_PTPC, &config);
    config.coarse_increment = false;
    config.src_frequency = freq;
    config.capture_keep = false;
    config.ns_rollover_mode = ptpc_ns_counter_rollover_digital;
    ptpc_init(HPM_PTPC, PTPC_PTPC_0, &config);

    ptpc_config_compare(HPM_PTPC, PTPC_PTPC_0, 2, 0);
    ptpc_init_timer(HPM_PTPC, PTPC_PTPC_0);

    /* configure gpio interrupt */
    test_gpio_input_interrupt();

    ptpc_irq_enable(HPM_PTPC, PTPC_EVENT_COMPARE0_MASK);
#ifdef TEST_S_MODE
    intc_s_enable_irq_with_priority(IRQn_PTPC, 2);
#else
    intc_m_enable_irq_with_priority(IRQn_PTPC, 2);
#endif
}

void delay_ms(uint32_t ms)
{
    board_delay_ms(ms);
}

#ifdef TEST_S_MODE
void isr_plicsw_s(void)
{
    printf("plicsw start\n");
    printf("plicsw end\n");
    intc_s_disable_swi();
}
SDK_DECLARE_SWI_ISR_S(isr_plicsw_s)

void test_plicsw_interrupt(void)
{
    intc_s_enable_swi();
    intc_s_trigger_swi();
}

#else
void isr_plicsw(void)
{
    printf("plicsw start\n");
    printf("plicsw end\n");
    intc_m_disable_swi();
}
SDK_DECLARE_SWI_ISR(isr_plicsw)

void test_plicsw_interrupt(void)
{
    intc_m_enable_swi();
    intc_m_init_swi();
    intc_m_trigger_swi();
}
#endif


int main(void)
{
    board_init();
    board_init_gpio_pins();

    printf("%s (%s %s) interrupt test\n", PLIC_TEST_MODE, VECTOR_MODE, PREEMPTIVE_MODE);
    printf("Press button will trigger GPIO interrupt, in which software interrupt will be triggered as well.\n");

#ifdef TEST_S_MODE
    switch_to_s_mode(test_interrupt_nesting);
#else
    test_interrupt_nesting();
#endif

    while(1);
    return 0;
}
