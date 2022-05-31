/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#include "freemaster.h"
#include "freemaster_example.h"

static void init_freemaster_uart(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    clock_set_source_divider(BOARD_FREEMASTER_UART_CLK_NAME, clk_src_osc24m, 1U);
    board_init_uart(BOARD_FREEMASTER_UART_BASE);
    uart_default_config(BOARD_FREEMASTER_UART_BASE, &config);
    config.fifo_enable = true;
    config.src_freq_in_hz = clock_get_frequency(BOARD_FREEMASTER_UART_CLK_NAME);

    stat = uart_init(BOARD_FREEMASTER_UART_BASE, &config);
    if (stat != status_success) {
        /* uart failed to be initialized */
        printf("failed to initialize uart\n");
        while(1);
    }

#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
    /* Enable UART interrupts. */
    uart_enable_irq(BOARD_FREEMASTER_UART_BASE, uart_intr_rx_data_avail_or_timeout, true);
    intc_m_enable_irq_with_priority(BOARD_FREEMASTER_UART_IRQ, 1);
#endif

}

#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
/*
 *  Application interrupt handler of communication peripheral used in interrupt modes
 * of FreeMASTER communication.
 */

void uart_isr(void)
{
    FMSTR_Isr();
}

SDK_DECLARE_EXT_ISR_M(BOARD_FREEMASTER_UART_IRQ, uart_isr)
#endif

int main(void)
{
    board_init();

    init_freemaster_uart();

    /* This example uses shared code from FreeMASTER generic example application */
    FMSTR_Example_Init();

    while(1)
    {
        /* FreeMASTER example increments several variables periodically, use the FreeMASTER PC Host tool to visualize the variables */
        FMSTR_Example_Poll();
    }
}

