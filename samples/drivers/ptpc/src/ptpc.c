/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_debug_console.h"
#include "hpm_ptpc_drv.h"

#ifndef PTPC
#define PTPC HPM_PTPC
#define PTPC_IRQ IRQn_PTPC
#define PTPC_INDEX PTPC_PTPC_0
#define PTPC_EVENT_COMPARE_MASK PTPC_EVENT_COMPARE0_MASK
#define PTPC_EVENT_PPS_MASK PTPC_EVENT_PPS0_MASK
#endif

uint32_t freq;
volatile uint32_t data_ready;

SDK_DECLARE_EXT_ISR_M(PTPC_IRQ, isr_ptpc)
void isr_ptpc(void)
{
    data_ready = ptpc_get_irq_status(PTPC);
    ptpc_clear_irq_status(PTPC, PTPC_EVENT_COMPARE_MASK | PTPC_EVENT_PPS_MASK);
}

void timer_compare(void)
{
    uint32_t sec, ns;
    ptpc_config_t config = {0};

    ptpc_get_default_config(PTPC, &config);
    config.coarse_increment = false;
    config.src_frequency = freq;
    config.capture_keep = false;
    config.ns_rollover_mode = ptpc_ns_counter_rollover_digital;
    ptpc_init(PTPC, PTPC_INDEX, &config);

    ptpc_irq_enable(PTPC, PTPC_EVENT_COMPARE_MASK);
    intc_m_enable_irq_with_priority(PTPC_IRQ, 1);

    /*
     * config target timestamp
     */
    sec = 3;
    ns = 80000;
    printf("Config target timestamp: %d s, %d ns\n", sec, ns);
    ptpc_config_compare(PTPC, PTPC_INDEX, sec, ns);
    ptpc_init_timer(PTPC, PTPC_INDEX);

    while (!(data_ready & PTPC_EVENT_COMPARE_MASK)) {
        ;
    }
    data_ready &= ~PTPC_EVENT_COMPARE_MASK;
    printf("Got event at %d s, %d ns\n", ptpc_get_timestamp_second(PTPC, PTPC_INDEX), ptpc_get_timestamp_ns(PTPC, PTPC_INDEX));
    ptpc_irq_disable(PTPC, PTPC_EVENT_COMPARE_MASK);
    intc_m_disable_irq(PTPC_IRQ);
    printf("%s done\n", __func__);
}

void pps_interrupt(void)
{
    uint8_t p;
    ptpc_config_t config = {0};
    board_init_led_pins();

    ptpc_get_default_config(PTPC, &config);
    config.coarse_increment = false;
    config.src_frequency = freq;
    config.ns_rollover_mode = ptpc_ns_counter_rollover_digital;
    ptpc_init(PTPC, PTPC_INDEX, &config);

    ptpc_irq_enable(PTPC, PTPC_EVENT_PPS0_MASK);
    intc_m_enable_irq_with_priority(PTPC_IRQ, 1);

    /*
     * config pps frequency
     */
    p = 1;
    printf("Config pps IRQ frequency: %d Hz\n", 1 << p);
    if (status_success != ptpc_set_pps(PTPC, PTPC_INDEX, p)) {
        printf("Failed to set frequency to %d Hz\n", 1 << p);
        return;
    }
    ptpc_init_timer(PTPC, PTPC_INDEX);

    printf("LED will be toggled at this frequency\n");
    board_led_write(false);
    for (uint32_t i = 0; i < 100; i++) {
        while (!(data_ready & PTPC_EVENT_PPS0_MASK)) {
            ;
        }
        data_ready &= ~PTPC_EVENT_PPS0_MASK;
        board_led_toggle();
    }
    printf("%s done\n", __func__);
    ptpc_irq_disable(PTPC, PTPC_EVENT_PPS0_MASK);
    intc_m_disable_irq(PTPC_IRQ);
}

int main(void)
{
    board_init();
    clock_add_to_group(clock_ptpc, 0);
    freq = clock_get_frequency(clock_ptpc);
    printf("ptpc example\n");

    timer_compare();
    pps_interrupt();
    return 0;
}
