/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_acmp_drv.h"

#if defined(ACMP_SOC_BANDGAP) && ACMP_SOC_BANDGAP
#include "hpm_acmp_soc_drv.h"
#endif

#define TEST_ACMP BOARD_ACMP
#define TEST_ACMP_IRQ BOARD_ACMP_IRQ
#define TEST_ACMP_CHANNEL BOARD_ACMP_CHANNEL
#define TEST_ACMP_PLUS_INPUT BOARD_ACMP_PLUS_INPUT /* use internal DAC */
#define TEST_ACMP_MINUS_INPUT BOARD_ACMP_MINUS_INPUT /* align with used pin */

volatile bool acmp_output_toggle;

void isr_acmp(void)
{
    acmp_output_toggle = true;
    acmp_channel_enable_irq(TEST_ACMP, TEST_ACMP_CHANNEL, ACMP_EVENT_RISING_EDGE, false);
    intc_m_disable_irq(TEST_ACMP_IRQ);
}
SDK_DECLARE_EXT_ISR_M(TEST_ACMP_IRQ, isr_acmp)

int main(void)
{
    acmp_channel_config_t acmp_channel_configure;

    board_init();
#if defined(ACMP_SOC_BANDGAP) && ACMP_SOC_BANDGAP
    acmp_enable_bandgap();
#endif
    init_acmp_pins();

    acmp_channel_get_default_config(TEST_ACMP, &acmp_channel_configure);
    acmp_channel_configure.plus_input = TEST_ACMP_PLUS_INPUT;
    acmp_channel_configure.minus_input = TEST_ACMP_MINUS_INPUT;
    acmp_channel_configure.enable_dac = true;
    acmp_channel_configure.enable_cmp_output = true;
    acmp_channel_configure.enable_clock_sync = true;
    acmp_channel_configure.hyst_level = ACMP_HYST_LEVEL_3;
    acmp_channel_config(TEST_ACMP, TEST_ACMP_CHANNEL, &acmp_channel_configure, true);

    acmp_channel_enable_irq(TEST_ACMP, TEST_ACMP_CHANNEL, ACMP_EVENT_RISING_EDGE, true);
    intc_m_enable_irq_with_priority(TEST_ACMP_IRQ, 1);

    printf("acmp example\n");

    while (1) {
        for (uint32_t dac_value = 0; dac_value <= 0xff; dac_value++)
        {
            acmp_channel_config_dac(TEST_ACMP, TEST_ACMP_CHANNEL, dac_value);
            board_delay_ms(15U);
            if (acmp_output_toggle) {
                printf("acmp out toggled, the dac set value is 0x%x\n", dac_value);
                acmp_output_toggle = false;
                break;
            }
        }

        board_delay_ms(1000U);
    }

    return 0;
}
