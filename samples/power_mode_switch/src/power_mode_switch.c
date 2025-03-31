/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_uart_drv.h"
#include "low_power.h"

char uart_get_char(void);

void show_menu(void);

volatile bool has_data;
volatile uint8_t byte_read;

int main(void)
{
    has_data = false;
    byte_read = 0;

    board_init();

    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);

    prepare_soc_low_power();
    show_menu();

    while (true) {

        char ch = uart_get_char();
        if (ch == 'w') {
            continue;
        }
        switch (ch) {
        case '1':
            enter_wait_mode();
            printf("Woke up from the wait mode\r\n");
            break;
        case '2':
            enter_stop_mode();
            printf("Woke up from the stop mode\r\n");
            break;
        case '3':
            enter_standby_mode();
            printf("Woke up from the standby mode\r\n");
            break;
        case '4':
            enter_shutdown_mode();
            break;
        default:
            printf("[!!] Please choose options listed above.\r\n");
            break;
        }
        show_menu();
    }
}

void show_menu(void)
{
    const char menu_str[] =
        "\r\nPower mode switch demo:\r\n"
        "  1 - Enter wait mode\r\n"
        "  2 - Enter stop mode\r\n"
        "  3 - Enter standby mode\r\n"
        "  4 - Enter shutdown mode\r\n\r\n";
    printf(menu_str);
}

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, console_uart_isr)
void console_uart_isr(void)
{
    if (uart_check_status(BOARD_CONSOLE_UART_BASE, uart_stat_data_ready)) {
        has_data = true;
        byte_read = uart_read_byte(BOARD_CONSOLE_UART_BASE);
    }
}

char uart_get_char(void)
{
    while (!has_data) {
    }
    has_data = false;
    return byte_read;
}


