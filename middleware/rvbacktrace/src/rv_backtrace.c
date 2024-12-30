/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-23     WangShun     the first version
 * 2024-08-30     WangShun     add addr2line function
 * 2024-10-30     HPMicro      add rvbacktrace_trap function
 * 2024-11-18     HPMicro      Unified print interface
 */

#include "../include/rvbacktrace.h"
#include "rvcoredump.h"

unsigned int rvstack_frame[STACK_FRAME_LEN]; // stack frame
unsigned int rvstack_frame_len; // stack frame len
struct rv_coredump_regs core_regs; // restore exception cpu registers

extern unsigned int rvstack_frame_len; // stack frame len

void riscv_coredump(void *cause, void *epc, int (*print_func)(const char *fmt, ...))
{
    struct rv_coredump_regs *cd = &core_regs;

    print_func("---- RV Core Dump:----\r\n");
    print_func("\t mcause: \t 0x%lx \r\n \
    \t mepc: \t\t 0x%lx \r\n", *(long *)cause, *(long*)epc);

    print_func("\t ra: \t\t 0x%lx \r\n", cd->ra);
    print_func("\t t0: \t\t 0x%lx \r\n", cd->t0);
    print_func("\t t1: \t\t 0x%lx \r\n", cd->t1);
    print_func("\t t2: \t\t 0x%lx \r\n", cd->t2);
#ifdef BACKTRACE_USE_FP
    print_func("\t s0: \t\t 0x%lx \r\n", cd->s0);
#endif
#if defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
    print_func("\t s0: \t\t 0x%lx \r\n", cd->s0);
    print_func("\t s1: \t\t 0x%lx \r\n", cd->s1);
#endif
    print_func("\t a0: \t\t 0x%lx \r\n", cd->a0);
    print_func("\t a1: \t\t 0x%lx \r\n", cd->a1);
    print_func("\t a2: \t\t 0x%lx \r\n", cd->a2);
    print_func("\t a3: \t\t 0x%lx \r\n", cd->a3);
    print_func("\t a4: \t\t 0x%lx \r\n", cd->a4);
    print_func("\t a5: \t\t 0x%lx \r\n", cd->a5);
    print_func("\t a6: \t\t 0x%lx \r\n", cd->a6);
    print_func("\t a7: \t\t 0x%lx \r\n", cd->a7);
    print_func("\t t3: \t\t 0x%lx \r\n", cd->t3);
    print_func("\t t4: \t\t 0x%lx \r\n", cd->t4);
    print_func("\t t5: \t\t 0x%lx \r\n", cd->t5);
    print_func("\t t6: \t\t 0x%lx \r\n", cd->t6);

    print_func("---- RV Core Dump End:----\r\n");
}

void rvbacktrace(void)
{
#ifdef RV_BACKTRACE_USE_FP_RTTHREAD
    rvbacktrace_fno(BACKTRACE_PRINTF);
#elif defined(RV_BACKTRACE_USE_FP_BAREMETAL)
    rvbacktrace_fno_bm(BACKTRACE_PRINTF);
#else
    rvbacktrace_fomit(BACKTRACE_PRINTF);
#endif /* RV_BACKTRACE_USE_FP */
}

void rvbacktrace_trap(void *casue, void *epc)
{
#ifdef RV_BACKTRACE_USE_FP_RTTHREAD
    BACKTRACE_PRINTF("Please use rvbacktrace() in RTThread\n");
#elif defined(RV_BACKTRACE_USE_FP_BAREMETAL)
    rvbacktrace_fno_bm(BACKTRACE_PRINTF);
#else
    rvbacktrace_fomit_trap(BACKTRACE_PRINTF);
#endif /* RV_BACKTRACE_USE_FP */
    riscv_coredump(casue, epc, BACKTRACE_PRINTF);
}

void rvbacktrace_addr2line(uint32_t *frame, int (*print_func)(const char *fmt, ...))
{
    char buffer[STACK_BUFFER_LEN];
    int offset = 0;

    for (uint32_t i = 0; i < rvstack_frame_len; i++)
    {
        offset += snprintf(buffer + offset, STACK_BUFFER_LEN - offset, "%lx ", frame[i]);
        if (offset >= STACK_BUFFER_LEN)
            break;
    }
    print_func("\naddr2line -e xxx.elf -a -f %s\n", buffer); 
}

#if defined (BACKTRACE_FSTACK_PROTECT)
__attribute__ ((noreturn)) void __wrap__exit(int status)
{
    extern void rvbacktrace(void);
    extern void __rt_libc_exit(int status);
    rvbacktrace();
    __rt_libc_exit(status);
    while (1);
}
#endif /* BACKTRACE_FSTACK_PROTECT */

