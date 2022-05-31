/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_pwm_drv.h"

#ifdef BOARD_RED_PWM
#define RED_PWM_IRQ BOARD_RED_PWM_IRQ
#define RED_PWM BOARD_RED_PWM
#define RED_PWM_CMP BOARD_RED_PWM_CMP
#define RED_PWM_CMP_INITIAL_ZERO BOARD_RED_PWM_CMP_INITIAL_ZERO
#define RED_PWM_OUT BOARD_RED_PWM_OUT
#define RED_PWM_CLOCK_NAME BOARD_RED_PWM_CLOCK_NAME
#else
#error BOARD_RED_PWM needs to be defined
#endif

#ifdef BOARD_GREEN_PWM
#define GREEN_PWM_IRQ BOARD_GREEN_PWM_IRQ
#define GREEN_PWM BOARD_GREEN_PWM
#define GREEN_PWM_CMP BOARD_GREEN_PWM_CMP
#define GREEN_PWM_CMP_INITIAL_ZERO BOARD_GREEN_PWM_CMP_INITIAL_ZERO
#define GREEN_PWM_OUT BOARD_GREEN_PWM_OUT

#define GREEN_PWM_CLOCK_NAME BOARD_BLUE_PWM_CLOCK_NAME
#else
#error BOARD_GREEN_PWM needs to be defined
#endif

#ifdef BOARD_BLUE_PWM
#define BLUE_PWM_IRQ BOARD_BLUE_PWM_IRQ
#define BLUE_PWM BOARD_BLUE_PWM
#define BLUE_PWM_CMP BOARD_BLUE_PWM_CMP
#define BLUE_PWM_CMP_INITIAL_ZERO BOARD_BLUE_PWM_CMP_INITIAL_ZERO
#define BLUE_PWM_OUT BOARD_BLUE_PWM_OUT
#define BLUE_PWM_CLOCK_NAME BOARD_BLUE_PWM_CLOCK_NAME
#else
#error BOARD_BLUE_PWM needs to be defined
#endif

#define PWM_PERIOD_IN_MS (1U)
#define PWM_DUTY_STEP_COUNT (1000U)

typedef struct {
    PWM_Type *pwm;
    uint32_t reload;
    uint32_t step;
    bool pwm_cmp_initial_zero;
    uint8_t pwm_irq;
    uint8_t pwm_cmp;
    uint8_t pwm_ch;
} led_pwm_t;

typedef enum {
    red = BOARD_RGB_RED,
    green = BOARD_RGB_GREEN,
    blue = BOARD_RGB_BLUE
} led_index_t;

led_pwm_t leds[3];
uint32_t reload;
volatile bool do_update;
volatile led_index_t current;

void pwm_isr(void)
{
    pwm_clear_status(leds[current].pwm, PWM_IRQ_HALF_RELOAD);
    do_update = true;
}

SDK_DECLARE_EXT_ISR_M(RED_PWM_IRQ, pwm_isr)
#if (GREEN_PWM_IRQ != RED_PWM_IRQ)
SDK_DECLARE_EXT_ISR_M(GREEN_PWM_IRQ, pwm_isr)
#endif
#if (BLUE_PWM_IRQ != RED_PWM_IRQ) && (GREEN_PWM_IRQ != BLUE_PWM_IRQ)
SDK_DECLARE_EXT_ISR_M(BLUE_PWM_IRQ, pwm_isr)
#endif

void config_pwm(PWM_Type *ptr, uint8_t pin, uint8_t cmp_index, uint32_t reload, bool cmp_initial_zero, uint8_t hw_event_cmp)
{
    pwm_cmp_config_t cmp_config = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(ptr);
    pwm_get_default_pwm_config(ptr, &pwm_config);
    pwm_get_default_cmp_config(ptr, &cmp_config);

    pwm_config.enable_output = false;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;

    /*
     * reload and start counter
     */
    pwm_set_reload(ptr, 0, reload);
    pwm_set_start_count(ptr, 0, 0);

    cmp_config.mode = pwm_cmp_mode_output_compare;

    cmp_config.cmp = cmp_initial_zero ? 0 : reload + 1;
    cmp_config.update_trigger = pwm_shadow_register_update_on_modify;

    /* config initial compare value which should take affect immediately */
    pwm_config_cmp(ptr, cmp_index, &cmp_config);

    /* update trigger type so that compare value will be updated on hardware event (RELOAD) */
    cmp_config.update_trigger = pwm_shadow_register_update_on_hw_event;
    /*
     * config pwm as output driven by cmp
     */
    if (status_success != pwm_setup_waveform(ptr, pin, &pwm_config, cmp_index, &cmp_config, 1)) {
        while (1) {
        }
    }

    /*
     * config hw event
     */
    cmp_config.cmp = reload - 1;
    cmp_config.update_trigger = pwm_shadow_register_update_on_hw_event;
    pwm_load_cmp_shadow_on_match(ptr, hw_event_cmp, &cmp_config);
}

void turn_on_rgb_led(led_index_t i)
{
    PWM_Type *pwm;
    uint8_t pwm_cmp;
    uint32_t initial;

    /* make sure cmp value is set to initial, before enabling output */
    pwm = leds[i].pwm;
    pwm_cmp = leds[i].pwm_cmp;
    initial = leds[i].pwm_cmp_initial_zero ? 0 : (leds[i].reload + 1);
    pwm_update_raw_cmp_edge_aligned(pwm, pwm_cmp, initial);

    board_enable_output_rgb_led(i);
}

void turn_off_rgb_led(led_index_t i)
{
    board_disable_output_rgb_led(i);
}

void update_rgb_led(void)
{
    uint32_t duty;
    bool increase_step = true;
    PWM_Type *pwm;
    uint32_t reload, pwm_cmp, step;

    step = leds[current].step;
    reload = leds[current].reload;
    pwm = leds[current].pwm;
    pwm_cmp = leds[current].pwm_cmp;
    duty = step;

    pwm_enable_irq(pwm, PWM_IRQ_HALF_RELOAD);
    turn_on_rgb_led(current);
    while (!(!increase_step && (duty < 2 * step))) {
        if (increase_step && (duty + step > reload)) {
            increase_step = false;
        }

        if (increase_step) {
            duty += step;
        } else {
            duty -= step;
        }

        while (!do_update) {
            ;
        }
        pwm_update_raw_cmp_edge_aligned(pwm, pwm_cmp, duty);
        do_update = false;
    }
    turn_off_rgb_led(current);
    pwm_disable_irq(pwm, PWM_IRQ_HALF_RELOAD);
}

int main(void)
{
    uint32_t freq;
    uint32_t hw_event_cmp;
    board_init_rgb_pwm_pins();

    freq = clock_get_frequency(RED_PWM_CLOCK_NAME);
    leds[red].reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;
    leds[red].pwm = RED_PWM;
    leds[red].pwm_ch = RED_PWM_OUT;
    leds[red].pwm_cmp = RED_PWM_CMP;
    leds[red].pwm_cmp_initial_zero = RED_PWM_CMP_INITIAL_ZERO;
    leds[red].pwm_irq = RED_PWM_IRQ;

    freq = clock_get_frequency(GREEN_PWM_CLOCK_NAME);
    leds[green].reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;
    leds[green].pwm = GREEN_PWM;
    leds[green].pwm_ch = GREEN_PWM_OUT;
    leds[green].pwm_cmp = GREEN_PWM_CMP;
    leds[green].pwm_cmp_initial_zero = GREEN_PWM_CMP_INITIAL_ZERO;
    leds[green].pwm_irq = GREEN_PWM_IRQ;

    freq = clock_get_frequency(BLUE_PWM_CLOCK_NAME);
    leds[blue].reload = freq / 1000 * PWM_PERIOD_IN_MS - 1;
    leds[blue].pwm = BLUE_PWM;
    leds[blue].pwm_ch = BLUE_PWM_OUT;
    leds[blue].pwm_cmp = BLUE_PWM_CMP;
    leds[blue].pwm_cmp_initial_zero = BLUE_PWM_CMP_INITIAL_ZERO;
    leds[blue].pwm_irq = BLUE_PWM_IRQ;

    hw_event_cmp = PWM_SOC_CMP_MAX_COUNT;
    for (uint8_t i = 0; i < PWM_SOC_CMP_MAX_COUNT; i++) {
        if ((i != RED_PWM_CMP) && (i != GREEN_PWM_CMP) && (i != BLUE_PWM_CMP)) {
            continue;
        }
        hw_event_cmp = i;
        break;
    }
    if (hw_event_cmp == PWM_SOC_CMP_MAX_COUNT) {
        while (1) {
        };
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        leds[i].step = leds[i].reload / PWM_DUTY_STEP_COUNT;
        config_pwm(leds[i].pwm, leds[i].pwm_ch, leds[i].pwm_cmp, leds[i].reload, leds[i].pwm_cmp_initial_zero, hw_event_cmp);
        pwm_start_counter(leds[i].pwm);
        intc_m_enable_irq_with_priority(leds[i].pwm_irq, 1);
    }

    current = red;
    while (1) {
        update_rgb_led();
        do_update = false;
        current = (current + 1) % (blue + 1);
    }
    return 0;
}
