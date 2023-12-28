/*
 * Copyright (c) 2021 - 2023 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rtt_port.h"
#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_ppor_drv.h"
#include "rtconfig.h"
#include <rthw.h>
#include <rtthread.h>

void os_tick_config(void);

#ifdef RT_USING_HEAP
#ifndef RT_HEAP_SIZE
#define RT_HEAP_SIZE (16 * 1024)
#endif
static uint8_t rt_heap[RT_HEAP_SIZE];
#endif

__attribute__((weak)) void rt_hw_board_init(void)
{
    board_init();
    rtt_base_init();
}

void rtt_base_init(void)
{
    /* initialize memory system */
#ifdef RT_USING_HEAP
    rt_system_heap_init(rt_heap, rt_heap + RT_HEAP_SIZE);
#endif

    /* Configure the OS Tick */
    os_tick_config();

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

void os_tick_config(void)
{
    mchtmr_delay(HPM_MCHTMR, RT_MCHTMR_FREQ_IN_HZ / RT_TICK_PER_SECOND);

    enable_mchtmr_irq();
}

void rt_hw_console_output(const char *str)
{
    while (*str != '\0')
    {
        if (*str == '\n') {
            while (status_success != uart_send_byte(BOARD_APP_UART_BASE, '\r')) {
            }
        }
        uart_send_byte(BOARD_APP_UART_BASE, *str++);
    }
}

#ifdef RT_USING_FINSH
char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (uart_check_status(BOARD_APP_UART_BASE, uart_stat_data_ready)) {
        uart_receive_byte(BOARD_APP_UART_BASE, (uint8_t *)&ch);
    } else {
        rt_thread_mdelay(10);
    }

    return ch;
}

#endif

ATTR_PLACE_AT(".isr_vector") void mchtmr_isr(void)
{
    mchtmr_delay(HPM_MCHTMR, RT_MCHTMR_FREQ_IN_HZ / RT_TICK_PER_SECOND);

    rt_tick_increase();
}

void rt_hw_cpu_reset(void)
{
    ppor_reset_mask_set_source_enable(HPM_PPOR, ppor_reset_software);
    /* reset after 1us */
    ppor_sw_reset(HPM_PPOR, 24);
    while(1) {

    }
}

MSH_CMD_EXPORT_ALIAS(rt_hw_cpu_reset, reset, reset the board);
