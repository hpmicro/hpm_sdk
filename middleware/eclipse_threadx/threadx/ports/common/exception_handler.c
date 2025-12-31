/*
 * Copyright (c) 2025 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_csr_drv.h"
#include <stdio.h>

#include "tx_api.h"
#include "tx_thread.h"
__attribute__((weak)) void threadx_exception_handler(void)
{
    uint32_t cause = read_csr(CSR_MCAUSE);
    uint32_t mepc = read_csr(CSR_MEPC);
    uint32_t mtval = read_csr(CSR_MTVAL);
    printf("###########################\r\nException occur!\r\n");
    printf("mcause: 0x%lx \r\nmepc: 0x%lx \r\nmtval: 0x%lx \r\n", cause, mepc, mtval);
    
    if (_tx_thread_system_state <= 1U) {
        TX_THREAD *current_thread;
        TX_THREAD_GET_CURRENT(current_thread);
        printf("Current Thread Name:  %s \n", current_thread->tx_thread_name);
        printf("Current Thread Stack: 0x%x \n", current_thread->tx_thread_stack_start);
        printf("Current Thread Stack Limit: 0x%x \n", (uint32_t)current_thread->tx_thread_stack_start + current_thread->tx_thread_stack_size);
        printf("------------------------------------------------------\r\nhex dump of Current Thread TCB\n");
        uint32_t *p = (uint32_t *)current_thread;
        for (uint32_t i = 0; i < sizeof(TX_THREAD); i++) {
            printf("%03d:0x%08x  ", i, p[i]);
            if (i % 4 == 3)
                printf("\r\n");
        }
    } else {
        printf("Trap may happen in ISR. ISR nested %d times\r\n", _tx_thread_system_state - 1);
    }
    while(1);
}