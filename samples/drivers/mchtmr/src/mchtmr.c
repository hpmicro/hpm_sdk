/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_gpio_drv.h"

#define PERIOD_IN_MS (3000)
#ifndef MCHTMR_CLK_NAME
#define MCHTMR_CLK_NAME (clock_mchtmr0)
#endif

uint32_t freq = 0;

void delay_ms(uint32_t ms)
{
    mchtmr_delay(HPM_MCHTMR, (uint64_t) ms * freq / 1000);
}

SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)
void isr_mchtmr(void)
{
#ifdef BOARD_LED_GPIO_CTRL
    gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                        BOARD_LED_GPIO_PIN);
#endif
    printf("mchtmr interrupt!\n");
    delay_ms(PERIOD_IN_MS);
}


int main(void)
{
    board_init();
    /* make sure mchtmr will not be gated on "wfi" */
    board_ungate_mchtmr_at_lp_mode();
    board_init_gpio_pins();
    freq = clock_get_frequency(MCHTMR_CLK_NAME);

#ifdef BOARD_LED_GPIO_CTRL
    gpio_set_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX,
                           BOARD_LED_GPIO_PIN);
#endif
    printf("mchtmr example\n");
    enable_mchtmr_irq();
    delay_ms(PERIOD_IN_MS);

#ifdef BOARD_LED_GPIO_CTRL
    gpio_write_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN, 1);
    printf("LED shall be flashing every %ds\n", PERIOD_IN_MS / 1000);
#else
    printf("mchtmr interrupt will occur every %ds\n", PERIOD_IN_MS / 1000);
#endif

    while (1) {
        __asm("wfi\n");
    }
    return 0;
}
