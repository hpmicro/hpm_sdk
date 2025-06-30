/*
 * Copyright (c) 2021,2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
#include "hpm_pwmv2_drv.h"
#else
#include "hpm_pwm_drv.h"
#endif

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

/**
 * @brief Structure to hold LED PWM configuration
 */
typedef struct {
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
    PWMV2_Type *pwm;
    uint8_t pwm_shadow;
#else
    PWM_Type *pwm;
#endif
    uint32_t reload;
    uint32_t step;
    bool pwm_cmp_initial_zero;
    uint8_t pwm_irq;
    uint8_t pwm_cmp;
    uint8_t pwm_ch;
} led_pwm_t;

/**
 * @brief Enumeration for LED color indices
 */
typedef enum {
    red = BOARD_RGB_RED,
    green = BOARD_RGB_GREEN,
    blue = BOARD_RGB_BLUE
} led_index_t;

/** Global variables */
led_pwm_t leds[3];                      /**< Array of LED PWM configurations */
volatile bool do_update;                /**< Flag to indicate PWM update needed */
volatile led_index_t current;           /**< Current LED being controlled */

SDK_DECLARE_EXT_ISR_M(RED_PWM_IRQ, pwm_isr)
#if (GREEN_PWM_IRQ != RED_PWM_IRQ)
SDK_DECLARE_EXT_ISR_M(GREEN_PWM_IRQ, pwm_isr)
#endif
#if (BLUE_PWM_IRQ != RED_PWM_IRQ) && (GREEN_PWM_IRQ != BLUE_PWM_IRQ)
SDK_DECLARE_EXT_ISR_M(BLUE_PWM_IRQ, pwm_isr)
#endif
void pwm_isr(void)
{
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
    pwm_counter_t pwm_counter = leds[current].pwm_ch / 2;
    uint32_t reload_status = pwmv2_get_reload_irq_status(leds[current].pwm);
    /* the used counter is reload  */
    if (reload_status & (1 << pwm_counter)) {
        do_update = true;
        pwmv2_clear_reload_irq_status(leds[current].pwm, (1 << pwm_counter)); /* clear counter reload status */
    }
#else
    pwm_clear_status(leds[current].pwm, PWM_IRQ_HALF_RELOAD);
    do_update = true;
#endif
}

#ifdef HPMSOC_HAS_HPMSDK_PWMV2
/**
 * @brief Configure PWM for LED control (PWMv2 version)
 * @param ptr PWM peripheral instance
 * @param pwm_channel PWM channel number
 * @param reload PWM reload value
 * @param shadow Shadow register index
 * @param cmp_initial_zero Initial compare value is zero
 * @param off_level_high Output is high when PWM is off
 */
void config_pwm(PWMV2_Type *ptr, pwm_channel_t pwm_channel, uint32_t reload, uint8_t shadow, bool cmp_initial_zero, bool off_level_high)
{
    /* according to pwm_channel to use pwm_counter and pwm_cmp, use two cmp to generate pwm signal */
    pwm_counter_t pwm_counter = pwm_channel / 2;
    uint8_t pwm_cmp_1 = pwm_channel * 2;
    uint8_t pwm_cmp_2 = pwm_channel * 2 + 1;

    /* pwm shadow for reload/cmp1/cpm2 value */
    uint8_t reload_shdow = shadow;     /* shadow register index for reload function */
    uint8_t cmp1_shadow = shadow + 1;  /* shadow register index for cmp 1 function */
    uint8_t cmp2_shadow = shadow + 2;  /* shadow register index for cmp 2 function */

    uint32_t cmp1_initial_val = cmp_initial_zero ? 0 : reload;

    /* disable shadow lock function, shadow will always access */
    pwmv2_disable_shadow_lock_feature(ptr);

    /* disable and reset pwm counter */
    pwmv2_disable_counter(ptr, pwm_counter);
    pwmv2_reset_counter(ptr, pwm_counter);

    /* set shdow register value */
    pwmv2_set_shadow_val(ptr, reload_shdow, reload, 0, false); /* for reload */
    pwmv2_set_shadow_val(ptr, cmp1_shadow, cmp1_initial_val, 0, false); /* for cmp_1 */
    pwmv2_set_shadow_val(ptr, cmp2_shadow, reload, 0, false); /* for cmp_2 */

    /* config reload value from shadow, update valid when issue shadow lock */
    pwmv2_counter_select_data_offset_from_shadow_value(ptr, pwm_counter, shadow);
    pwmv2_set_reload_update_time(ptr, pwm_counter, pwm_reload_update_on_shlk);

    /* config two cmp value from shadow, update valid when issue shadow lock */
    pwmv2_select_cmp_source(ptr, pwm_cmp_1, cmp_value_from_shadow_val, cmp1_shadow);
    pwmv2_cmp_update_trig_time(ptr, pwm_cmp_1, pwm_shadow_register_update_on_shlk);
    pwmv2_select_cmp_source(ptr, pwm_cmp_2, cmp_value_from_shadow_val, cmp2_shadow);
    pwmv2_cmp_update_trig_time(ptr, pwm_cmp_2, pwm_shadow_register_update_on_shlk);

    /* trigger reload value and cmp value update from sahdow */
    pwmv2_issue_shadow_register_lock_event(ptr);

    /* disable burst mode */
    pwmv2_counter_burst_disable(ptr, pwm_counter);

    /* config use two cmp to generate pwm signal */
    pwmv2_disable_four_cmp(ptr, HPM_NUM_TO_EVEN_FLOOR(pwm_channel));

    /* when cmp_1 is 0, cmp_2 is reload value, the pwm generate high, if off_level_hig is false, need invert pwm out */
    if (cmp_initial_zero && (!off_level_high)) {
        pwmv2_enable_output_invert(ptr, pwm_channel);
    }

    /* config cmp_1 update when reload event, change cmp_1 to change duty in update_rgb_led() */
    pwmv2_cmp_update_trig_time(ptr, pwm_cmp_1, pwm_shadow_register_update_on_reload);
}

/**
 * @brief Update RGB LED brightness with breathing effect
 * @details Implements smooth brightness transition using PWM duty cycle control
 */
void update_rgb_led(void)
{
    bool increase_step = true;
    uint32_t step = leds[current].step;
    uint32_t reload = leds[current].reload;
    PWMV2_Type *pwm = leds[current].pwm;
    pwm_channel_t pwm_channel = leds[current].pwm_ch;
    uint32_t duty = step;

    /* according to pwm_channel to use pwm_counter and pwm_cmp */
    pwm_counter_t pwm_counter = pwm_channel / 2; /* the relationship of pwm_channel and pwm_counter is fixed in hardware */
    uint8_t pwm_cmp_1_shadow = leds[current].pwm_shadow + 1; /* this relationship of cmp and shadow is configurated in config_pwm() */

    pwmv2_enable_reload_irq(pwm, pwm_counter);     /* enable reload irq, */
    pwmv2_channel_enable_output(pwm, pwm_channel); /* enable channel output */
    pwmv2_enable_counter(pwm, pwm_counter);        /* enable counter */
    pwmv2_start_pwm_output(pwm, pwm_counter);

    /* The duty increases first and then decreases, exits the loop when it decreases to 0 */
    while (!(!increase_step && (duty == 0))) {
        if (increase_step && (duty + step > reload)) {
            increase_step = false;
        }

        if (increase_step) {
            duty += step;
        } else {
            duty -= step;
        }

        while (!do_update) {
        }
        pwmv2_set_shadow_val(pwm, pwm_cmp_1_shadow, duty, 0, false); /* set cmp_1 shadow value */
        do_update = false;
    }
    pwmv2_channel_disable_output(pwm, pwm_channel); /* disable channel output */
    pwmv2_disable_counter(pwm, pwm_counter);        /* disable counter */
    pwmv2_reset_counter(pwm, pwm_counter);          /* reset counter */
    pwmv2_disable_reload_irq(pwm, pwm_counter);     /* disable reload irq */
}

#else

/**
 * @brief Configure PWM for LED control (PWM version)
 * @param ptr PWM peripheral instance
 * @param pin PWM output pin
 * @param cmp_index Compare channel index
 * @param reload PWM reload value
 * @param cmp_initial_zero Initial compare value is zero
 * @param hw_event_cmp Hardware event compare channel
 * @param off_level_high Output is high when PWM is off
 */
void config_pwm(PWM_Type * ptr, uint8_t pin, uint8_t cmp_index, uint32_t reload, bool cmp_initial_zero, uint8_t hw_event_cmp, bool off_level_high)
{
    pwm_cmp_config_t cmp_config = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(ptr);
    pwm_get_default_pwm_config(ptr, &pwm_config);
    pwm_get_default_cmp_config(ptr, &cmp_config);

    pwm_config.enable_output = false;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = !(cmp_initial_zero && off_level_high);

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
        printf("failed to setup waveform\n");
        while(1);
    }

    /*
     * config hw event
     */
    cmp_config.cmp = reload;
    cmp_config.update_trigger = pwm_shadow_register_update_on_modify;
    pwm_load_cmp_shadow_on_match(ptr, hw_event_cmp, &cmp_config);
}

/**
 * @brief Turn on RGB LED
 * @param i LED index (red, green, or blue)
 */
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

/**
 * @brief Turn off RGB LED
 * @param i LED index (red, green, or blue)
 */
void turn_off_rgb_led(led_index_t i)
{
    board_disable_output_rgb_led(i);
}
/**
 * @brief Update the RGB LED brightness in a smooth transition.
 *
 * This function updates the brightness of the currently selected RGB LED by
 * gradually increasing and then decreasing the duty cycle of the PWM signal.
 * The duty cycle is updated in steps, and the function exits when the duty
 * cycle decreases back to 0.
 *
 * The function enables the PWM interrupt for half reload, turns on the RGB LED,
 * and then enters a loop where it updates the duty cycle. The duty cycle is
 * increased until it reaches the maximum reload value, and then it is decreased
 * back to 0. The function waits for the `do_update` flag to be set before
 * updating the duty cycle.
 *
 * Once the duty cycle reaches 0, the function turns off the RGB LED and disables
 * the PWM interrupt.
 */
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
    /* The duty increases first and then decreases, exits the loop when it decreases to 0 */
    while (!(!increase_step && (duty == 0))) {
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
#endif

int main(void){
    uint32_t freq;
    board_init();
    board_init_rgb_pwm_pins();
    printf("rgb led example\n");

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

#ifndef HPMSOC_HAS_HPMSDK_PWMV2
    /*
     * Try to find an available comparator for hardware events that is not already used by the RED, GREEN, or BLUE PWM comparators.
     * It iterates through all possible comparator indices and selects the first available one.
     * If no available comparator is found, it prints an error message and enters an infinite loop.
     */
    uint32_t hw_event_cmp;
    hw_event_cmp = PWM_SOC_CMP_MAX_COUNT;
    for (uint8_t i = 0; i < PWM_SOC_CMP_MAX_COUNT; i++) {
        if ((i == RED_PWM_CMP) || (i == GREEN_PWM_CMP) || (i == BLUE_PWM_CMP)) {
            continue;
        }
        hw_event_cmp = i;
        break;
    }
    if (hw_event_cmp == PWM_SOC_CMP_MAX_COUNT) {
        printf("Failed to find a comparator for hardware event\n");
        while (1) {
        };
    }
#endif

    for (uint8_t i = 0; i < ARRAY_SIZE(leds); i++) {
        leds[i].step = leds[i].reload / PWM_DUTY_STEP_COUNT;
#ifdef HPMSOC_HAS_HPMSDK_PWMV2
        leds[i].pwm_shadow = i * 3; /* per pwm use 3 shadow */
        config_pwm(leds[i].pwm, leds[i].pwm_ch, leds[i].reload, leds[i].pwm_shadow, leds[i].pwm_cmp_initial_zero, board_get_led_pwm_off_level());
#else
        config_pwm(leds[i].pwm, leds[i].pwm_ch, leds[i].pwm_cmp, leds[i].reload, leds[i].pwm_cmp_initial_zero, hw_event_cmp, board_get_led_pwm_off_level());
        pwm_start_counter(leds[i].pwm);
#endif
        intc_m_enable_irq_with_priority(leds[i].pwm_irq, 1);
    }

    current = red;
    while(1) {
        update_rgb_led();
        current = (current + 1) % (blue + 1);
    }
    return 0;
}
