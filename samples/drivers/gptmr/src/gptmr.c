/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_debug_console.h"


#define GPTMR BOARD_GPTMR
#define GPTMR_CH  BOARD_GPTMR_CHANNEL
#define GPTMR_IRQ BOARD_GPTMR_IRQ
#define GPTMR_PWM BOARD_GPTMR_PWM
#define GPTMR_PWM_CH BOARD_GPTMR_PWM_CHANNEL
#define GPTMR_CMP  0
#define GPTMR_CLK_NAME BOARD_GPTMR_CLK_NAME

#define GPTMR_PWM_DUTY_CYCLE_FP_MAX ((1U << 24) - 1)

volatile bool time_is_up = false;
volatile bool capture_is_done = false;
volatile uint32_t count;
gptmr_counter_type_t target_counter;

void isr_gptmr(void)
{
    volatile uint32_t s = GPTMR->SR;
    GPTMR->SR = s;
    if (s & GPTMR_CH_CMP_STAT_MASK(GPTMR_CH, GPTMR_CMP)) {
        time_is_up = true;
        count = gptmr_channel_get_counter(GPTMR, GPTMR_CH, gptmr_counter_type_normal);
    }
    if (s & GPTMR_CH_CAP_STAT_MASK(GPTMR_CH)) {
        capture_is_done = true;
        count = gptmr_channel_get_counter(GPTMR, GPTMR_CH, target_counter);
    }
}
SDK_DECLARE_EXT_ISR_M(GPTMR_IRQ, isr_gptmr)

static void test_compare(void)
{
    gptmr_channel_config_t config;
    gptmr_channel_get_default_config(GPTMR, &config);
    config.cmp[0] = 0x1000;

    gptmr_enable_irq(GPTMR, GPTMR_CH_CMP_IRQ_MASK(GPTMR_CH, GPTMR_CMP));
    gptmr_channel_config(GPTMR, GPTMR_CH, &config, false);
    gptmr_channel_reset_count(GPTMR, GPTMR_CH);
    gptmr_start_counter(GPTMR, GPTMR_CH);
    intc_m_enable_irq_with_priority(GPTMR_IRQ, 1);

    while(!time_is_up) {
        __asm("nop");
    }
    time_is_up = false;
    intc_m_disable_irq(GPTMR_IRQ);
    printf("time is up: 0x%x\n", count);
}

static void test_capture(void)
{
    gptmr_channel_config_t config;
    gptmr_channel_get_default_config(GPTMR, &config);
    config.mode = gptmr_work_mode_capture_at_rising_edge;
    target_counter = gptmr_counter_type_rising_edge;

    gptmr_enable_irq(GPTMR, GPTMR_CH_CAP_IRQ_MASK(GPTMR_CH));
    gptmr_channel_config(GPTMR, GPTMR_CH, &config, false);
    gptmr_channel_reset_count(GPTMR, GPTMR_CH);
    gptmr_start_counter(GPTMR, GPTMR_CH);
    intc_m_enable_irq_with_priority(GPTMR_IRQ, 1);

    while(!capture_is_done) {
        __asm("nop");
    }
    capture_is_done = false;
    intc_m_disable_irq(GPTMR_IRQ);
    printf("captured rising edge: 0x%x\n", count);
}

static void test_measure(void)
{
    uint32_t period, duty;
    gptmr_channel_config_t config;

    uint32_t gptmr_freq = clock_get_frequency(BOARD_GPTMR_CLK_NAME);
    gptmr_channel_get_default_config(GPTMR, &config);
    config.mode = gptmr_work_mode_measure_width;
    gptmr_channel_config(GPTMR, GPTMR_CH, &config, false);
    gptmr_channel_reset_count(GPTMR, GPTMR_CH);
    gptmr_start_counter(GPTMR, GPTMR_CH);

    /* please make sure deplay time is more than one pwm cycle */
    board_delay_ms(100);
    period = gptmr_channel_get_counter(GPTMR, GPTMR_CH, gptmr_counter_type_measured_period);
    duty = gptmr_channel_get_counter(GPTMR, GPTMR_CH, gptmr_counter_type_measured_duty_cycle);
    printf("measured frequency: %f\n", (float) gptmr_freq / period);
    printf("measured duty cycle: %.2f%%\n", ((float) duty / period) * 100);

}

static void test_generate_pwm_waveform_edge_aligned(void)
{
    uint32_t duty_step, duty, cmp;
    uint32_t reload = 0x10000;
    bool increase_duty_cycle;

    printf("test generate waveform edge aligned pwm\n");

    gptmr_channel_config_t config;
    gptmr_channel_get_default_config(GPTMR_PWM, &config);

    config.reload = reload;
    config.cmp[0] = reload - 1;
    config.cmp_initial_polarity_high = false;
    gptmr_channel_config(GPTMR_PWM, GPTMR_PWM_CH, &config, false);
    gptmr_channel_reset_count(GPTMR_PWM, GPTMR_PWM_CH);
    gptmr_start_counter(GPTMR_PWM, GPTMR_PWM_CH);

    duty_step = GPTMR_PWM_DUTY_CYCLE_FP_MAX / 100;
    duty = GPTMR_PWM_DUTY_CYCLE_FP_MAX / 100;
    increase_duty_cycle = true;
    for (uint32_t i = 0; ; i++) {
        if (increase_duty_cycle) {
            if ((duty + duty_step) > GPTMR_PWM_DUTY_CYCLE_FP_MAX) {
                increase_duty_cycle = false;
                continue;
            }
            duty += duty_step;
        } else {
            if (duty < duty_step) {
                increase_duty_cycle = true;
                continue;
            }
            duty -= duty_step;
        }

        /* in case overflow */
        cmp = (uint64_t) (reload) * duty / GPTMR_PWM_DUTY_CYCLE_FP_MAX;
        gptmr_update_cmp(GPTMR_PWM, GPTMR_PWM_CH, 0, cmp);
        board_delay_ms(100);
    }
}

int main(void)
{
    board_init();

    init_gptmr_pins(GPTMR);
    init_gptmr_pins(GPTMR_PWM);
    printf("timer testing\n");
    test_compare();
    test_capture();
    test_measure();
    test_generate_pwm_waveform_edge_aligned();
    while(1);
    return 0;
}
