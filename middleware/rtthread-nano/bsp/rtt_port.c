/*
 * Copyright (c) 2021 - 2024 HPMicro
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
void rt_console_config(void);
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

#ifdef RT_USING_CONSOLE
    rt_console_config();
#endif

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
            while (status_success != uart_send_byte(BOARD_RT_CONSOLE_BASE, '\r')) {
            }
        }
        uart_send_byte(BOARD_RT_CONSOLE_BASE, *str++);
    }
}

#ifdef RT_USING_FINSH
struct rt_finsh_rb {
    uint32_t wr;
    uint32_t rd;
    uint8_t buffer[FINSH_RX_BUF_SIZE];
};

static struct rt_finsh_rb finsh_rx_rb;
static struct rt_semaphore finsh_rx_sem;

void rt_finsh_rx_write(struct rt_finsh_rb *rb, char *data)
{
    rb->buffer[rb->wr] = *data;
    rb->wr = (rb->wr + 1) % FINSH_RX_BUF_SIZE;

    if (rb->wr == rb->rd) {
        rb->rd = (rb->rd + 1) % FINSH_RX_BUF_SIZE;
    }
}

int rt_finsh_rx_read(struct rt_finsh_rb *rb, char *data)
{
    if (rb->wr == rb->rd)
        return -1;

    *data = rb->buffer[rb->rd];
    rb->rd = (rb->rd + 1) % FINSH_RX_BUF_SIZE;
    return 0;
}

char rt_hw_console_getchar(void)
{
    char ch = -1;

    while (rt_finsh_rx_read(&finsh_rx_rb, &ch) != 0) {
        rt_sem_take(&finsh_rx_sem, RT_WAITING_FOREVER);
    }

    return ch;
}

SDK_DECLARE_EXT_ISR_M(BOARD_RT_CONSOLE_IRQ, rt_finsh_uart_isr);
void rt_finsh_uart_isr(void)
{
    char ch = -1;
    uint8_t irq_id = uart_get_irq_id(BOARD_RT_CONSOLE_BASE);

    rt_interrupt_enter();
    if (irq_id == uart_intr_id_rx_data_avail) {
        uart_receive_byte(BOARD_RT_CONSOLE_BASE, (uint8_t *)&ch);
        rt_finsh_rx_write(&finsh_rx_rb, &ch);
        rt_sem_release(&finsh_rx_sem);
    }
    rt_interrupt_leave();
}
#endif /* RT_USING_FINSH */

#ifdef RT_USING_CONSOLE
void rt_console_config(void)
{
    hpm_stat_t stat;

    uart_config_t config = {0};

    init_uart_pins((UART_Type *) BOARD_RT_CONSOLE_BASE);
    clock_add_to_group(BOARD_RT_CONSOLE_CLK_NAME, 0);
    uart_default_config(BOARD_RT_CONSOLE_BASE, &config);
    config.baudrate = RT_CONSOLE_BAUDRATE;
    config.fifo_enable = false;
    config.src_freq_in_hz = clock_get_frequency(BOARD_RT_CONSOLE_CLK_NAME);

    stat = uart_init(BOARD_RT_CONSOLE_BASE, &config);
    if (stat != status_success) {
        rt_kprintf("failed to initialize uart\n");
        while (1) {
        }
    }

#ifdef RT_USING_FINSH
    rt_sem_init(&finsh_rx_sem, "finsh_rx", 0, RT_IPC_FLAG_FIFO);
    uart_enable_irq(BOARD_RT_CONSOLE_BASE, uart_intr_rx_data_avail_or_timeout);
    uart_disable_irq(BOARD_RT_CONSOLE_BASE, uart_intr_tx_slot_avail);
    intc_m_enable_irq_with_priority(BOARD_RT_CONSOLE_IRQ, 1);
#endif
}
#endif/* RT_USING_CONSOLE */

SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)
void isr_mchtmr(void)
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

/**
 * @brief halt cpu when exception occur
 * 
 * @param cause mcause
 * @param epc mepc
 * @return long 
 */
long exception_handler(long cause, long epc) 
{
    (void)cause;
    
    while (1) {

    };
    return epc;
}
