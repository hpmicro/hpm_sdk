/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_ptpc_drv.h"
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
#include "hpm_mchtmr_drv.h"
#endif
#if defined(HPMSOC_HAS_HPMSDK_PTPC)
#include "hpm_ptpc_drv.h"
#elif defined(HPMSOC_HAS_HPMSDK_GPTMR)
#include "hpm_gptmr_drv.h"
#endif

#if defined(USE_S_MODE_IRQ) && USE_S_MODE_IRQ && (!defined(SOC_HAS_S_MODE) || !SOC_HAS_S_MODE)
#error SOC does not support supervisor mode IRQ
#endif

#if defined(USE_S_MODE_IRQ) && USE_S_MODE_IRQ && defined(SOC_HAS_S_MODE) && SOC_HAS_S_MODE
#define PLIC_TEST_MODE "Supervisor mode"
#define TEST_S_MODE 1
#else
#define PLIC_TEST_MODE "Machine mode"
#endif

#if !defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)
#define VECTOR_MODE "Vector"
#else
#define VECTOR_MODE "Non-Vector"
#endif

#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
#define PREEMPTIVE_MODE "Preemptive"
#else
#define PREEMPTIVE_MODE "Non-Preemptive"
#endif

#if defined(HPMSOC_HAS_HPMSDK_GPTMR)
#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

#define APP_TICK_MS                   (2000)
#endif

#define MAX_TOGGLE_IN_NESTED_IRQ 5

void test_plicsw_interrupt(void);
static volatile uint32_t toggled;

#if defined(DEBOUNCE_THRESHOLD_IN_MS)
#define MCHTMR_CLK_NAME (clock_mchtmr0)
static volatile uint64_t gpio_isr_rel_time; /* mark the real time from mchtimer */
static volatile uint64_t gpio_isr_pre_time; /* mark the last time marked on gpio_isr */
static volatile uint32_t level_value; /* mark the level */
static volatile uint32_t pre_level_value; /* mark the level before enter the isr */
static uint32_t debounce_threshold; /* debounce threshold */
#endif

#ifdef TEST_S_MODE
SDK_DECLARE_EXT_ISR_S(BOARD_APP_GPIO_IRQ, isr_gpio)
#else
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)
#endif
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
        if (toggled < MAX_TOGGLE_IN_NESTED_IRQ) {
            printf("gpio interrupt occurred in nested irq context\n");
        }
        printf("gpio interrupt start, toggled %d times\n", toggled + 1);
        toggled++;
    #ifdef BOARD_LED_GPIO_CTRL
        gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    #endif
        test_plicsw_interrupt();
        printf("gpio interrupt end\n");
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
    }
#endif
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

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

#if defined(HPMSOC_HAS_HPMSDK_PTPC)
#ifdef TEST_S_MODE
SDK_DECLARE_EXT_ISR_S(IRQn_PTPC, isr_ptpc)
#else
SDK_DECLARE_EXT_ISR_M(IRQn_PTPC, isr_ptpc)
#endif
void isr_ptpc(void)
{
    printf("ptpc interrupt start\n");
    printf("+ now next %d gpio interrupts will occur in nested irq context\n", MAX_TOGGLE_IN_NESTED_IRQ);
    toggled = 0;
    while (toggled < MAX_TOGGLE_IN_NESTED_IRQ) {
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
        pre_level_value = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#endif
    }
    ptpc_clear_irq_status(HPM_PTPC, PTPC_EVENT_COMPARE0_MASK);
    printf("ptpc interrupt end\n");
    printf("- now the following gpio interrupts will occur normal irq context\n");
}

void test_interrupt_nesting(void)
{
    uint32_t freq = clock_get_frequency(clock_ptpc);
    ptpc_config_t config = {0};

    clock_add_to_group(clock_ptpc, 0);
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
#elif defined(HPMSOC_HAS_HPMSDK_GPTMR)
SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    printf("gptmr interrupt start\n");
    printf("+ now next %d gpio interrupts will occur in nested irq context\n", MAX_TOGGLE_IN_NESTED_IRQ);
    toggled = 0;
    while (toggled < MAX_TOGGLE_IN_NESTED_IRQ) {
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
        pre_level_value = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#endif
    }
    gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
    gptmr_disable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    printf("gptmr interrupt end\n");
    printf("- now the following gpio interrupts will occur normal irq context\n");
}

void test_interrupt_nesting(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(APP_BOARD_GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));

    /* configure gpio interrupt */
    test_gpio_input_interrupt();

#ifdef TEST_S_MODE
    intc_s_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 2);
#else
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 2);
#endif
}
#endif

#ifdef TEST_S_MODE
SDK_DECLARE_SWI_ISR_S(isr_plicsw_s)
void isr_plicsw_s(void)
{
    printf("plicsw start\n");
    printf("plicsw end\n");
    intc_s_disable_swi();
}

void test_plicsw_interrupt(void)
{
    intc_s_enable_swi();
    intc_s_trigger_swi();
}

#else
SDK_DECLARE_SWI_ISR(isr_plicsw)
void isr_plicsw(void)
{
    printf("plicsw start\n");
    printf("plicsw end\n");
    intc_m_disable_swi();
}

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

#if defined(DEBOUNCE_THRESHOLD_IN_MS)
    uint32_t mchtmr_freq;
    mchtmr_freq = clock_get_frequency(MCHTMR_CLK_NAME);
    debounce_threshold = DEBOUNCE_THRESHOLD_IN_MS * mchtmr_freq / 1000;
#endif

    printf("%s (%s %s) interrupt test\n", PLIC_TEST_MODE, VECTOR_MODE, PREEMPTIVE_MODE);
    printf("Press button will trigger GPIO interrupt, in which software interrupt will be triggered as well.\n");

#ifdef TEST_S_MODE
    switch_to_s_mode(test_interrupt_nesting);
#else
    test_interrupt_nesting();
#endif

    while (1) {
#if defined(DEBOUNCE_THRESHOLD_IN_MS)
        pre_level_value = gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
#endif
    }
    return 0;
}
