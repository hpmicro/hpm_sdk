/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-21     WangShun     the first version
 * 2024-08-30     WangShun     add addr2line function
 * 2024-09-19     WangShun     improvement of function formal parameter
 * 2024-10-30     HPMicro      add baremetal rvbacktrace function
 * 2024-11-18     HPMicro      fix finsh call rvbacktrace_all
 */

#include "../include/rvbacktrace.h"
#include "rvcoredump.h"
#include <string.h>

extern unsigned int rvstack_frame[STACK_FRAME_LEN]; // stack frame
extern unsigned int rvstack_frame_len; // stack frame len
extern struct rv_coredump_regs core_regs;

#if defined(RV_BACKTRACE_USE_FP_RTTHREAD)
static rt_uint32_t _rt_susrstack;
static rt_uint32_t _rt_eusrstack;
static rt_thread_t _backtrace_thread;
static rt_thread_t _backtrace_threadn;
static rt_object_t * thread_object_table = RT_NULL;

static void walk_stackframe(int (*print_func)(const char *fmt, ...))
{
    rt_uint32_t num = 0;
    _backtrace_thread = rt_thread_self(); //    get current thread
    _rt_susrstack = (rt_uint32_t)(uintptr_t)_backtrace_thread->stack_addr; // stack start address
    _rt_eusrstack = (rt_uint32_t)(uintptr_t)(_backtrace_thread->stack_addr + _backtrace_thread->stack_size); // stack end address

    unsigned long sp, fp, ra, pc; // stack pointer, frame pointer, return address, program counter
    struct stackframe *frame;

    register const unsigned long current_sp __asm__("sp"); //   get current stack pointer
    sp = current_sp;
    fp = (unsigned long)__builtin_frame_address(0); //  get current frame pointer
    print_func("Current Thread Name:  %s \n", _backtrace_thread->name);
    while (1)
    {
        frame = (struct stackframe *)(fp - BACKTRACE_LEN); //   get frame pointer

        if ((rt_uint32_t *)frame > (rt_uint32_t *)(uintptr_t)_rt_eusrstack)
        {
            rvstack_frame_len = num;
            return;
        }

        sp = fp;  // get stack pointer
        fp = frame->s_fp; // get frame pointer
        ra = frame->s_ra; // get return address
        pc = frame->s_ra - 4; // get program counter

        //  print stack interval, return address, program counter
        print_func("[%d]Stack interval :[0x%lx - 0x%lx]  ra 0x%lx\n", num, sp, fp, ra);
        rvstack_frame[num] = pc; // save stack frame address
        num++;
    }
}

#if defined(BACKTRACE_ALL)
static void walk_stackframe_all(int (*print_func)(const char *fmt, ...))
{
    rt_uint32_t num = 0, i = 0;
    int thread_object_len = 0;
    unsigned long sp, fp, ra, pc; // stack pointer, frame pointer, return address, program counter
    struct stackframe *frame;

    thread_object_len = rt_object_get_length(RT_Object_Class_Thread);
    if (thread_object_len == RT_NULL)
    {
        return;
    }

    thread_object_table = (rt_object_t *) rt_malloc((sizeof(rt_object_t) * thread_object_len));
    RT_ASSERT(thread_object_table != RT_NULL);

    rt_object_get_pointers(RT_Object_Class_Thread, (rt_object_t *) thread_object_table, thread_object_len);

    for (i = 0; (int)i < thread_object_len; i++)
    {
        _backtrace_threadn = (rt_thread_t) thread_object_table[i];

        if (_backtrace_threadn == (rt_thread_t) rt_thread_self())
        {
            continue;
        }

        _rt_susrstack = (rt_uint32_t) (uintptr_t) _backtrace_threadn->stack_addr; // stack start address
        _rt_eusrstack = (rt_uint32_t) (uintptr_t) (_backtrace_threadn->stack_addr + _backtrace_threadn->stack_size); // stack end address

        print_func("------------------------------Thread: %s backtrace------------------------------\r\n",
                _backtrace_threadn->name);
        print_func("[%d]Thread Name:  %s \n", i, _backtrace_threadn->name);
        sp = (unsigned long) _backtrace_threadn->sp;
        fp = ((rt_ubase_t *) (_backtrace_threadn->sp))[BACKTRACE_FP_POS]; // get current frame pointer
        while (1)
        {
            frame = (struct stackframe *) (fp - BACKTRACE_LEN); //   get frame pointer

            if ((rt_uint32_t *) frame > (rt_uint32_t *) (uintptr_t) _rt_eusrstack)
            {
                rvstack_frame_len = num;
                rvbacktrace_addr2line((uint32_t *) &rvstack_frame[0], print_func);
                num = 0;
                break;
            }

            sp = fp;  // get stack pointer
            fp = frame->s_fp; // get frame pointer
            ra = frame->s_ra; // get return address
            pc = frame->s_ra - 4; // get program counter

            //  print stack interval, return address, program counter
            print_func("[%d]Stack interval :[0x%lx - 0x%lx]  ra 0x%lx \n", num, sp, fp, ra);
            rvstack_frame[num] = pc; // save stack frame address
            num++;
        }
    }
    print_func("Thread Total Num: %d\n", thread_object_len);
}

static void rvbacktrace_all(void)
{
    BACKTRACE_PRINTF("\r\n---- RV_Backtrace Call Frame Start: ----\r\n");
    walk_stackframe_all(BACKTRACE_PRINTF);
    BACKTRACE_PRINTF("---- RV_Backtrace Call Frame End:----\r\n");
    BACKTRACE_PRINTF("\r\n");
}

MSH_CMD_EXPORT_ALIAS(rvbacktrace_all, rv_backtrace_all, backtrace all threads);
#endif /* BACKTRACE_ALL */

//  backtrace function
void rvbacktrace_fno(int (*print_func)(const char *fmt, ...))
{
    print_func("\r\n---- RV_Backtrace Call Frame Start: ----\r\n");
    print_func("###Please consider the value of ra as accurate and the value of sp as only for reference###\n");
    print_func("------------------------------Thread: %s backtrace------------------------------\r\n", ((rt_thread_t)rt_thread_self())->name);
    walk_stackframe(print_func);
    rvbacktrace_addr2line((uint32_t *)&rvstack_frame[0], print_func); // addr2line function
    print_func("---- RV_Backtrace Call Frame End:----\r\n");
    print_func("\r\n");
}
#endif /* BACKTRACE_USE_FP */


#if defined(RV_BACKTRACE_USE_FP_BAREMETAL)
static uintptr_t estack;

__attribute__((always_inline)) static inline void *rvbacktrace_get_fp(void)
{
    void *fp;
    __asm volatile("mv %0, s0\n" : "=r"(fp));
    return fp;
}

__attribute__((always_inline)) static inline void *rvbacktrace_get_sp(void)
{
    void *sp;
    __asm__ volatile("mv %0, sp\n" : "=r"(sp));
    return sp;
}

static uint32_t rvbacktrace_judge_stackframe(long *bottom, uintptr_t *estack)
{
    long *b = bottom;
    long *rs0;
    uint32_t pos[3] = {BACKTRACE_FP_P0S_GCC_ISR, BACKTRACE_FP_POS_NORMAL, BACKTRACE_FP_POS_TRAP};

    for (uint32_t i = 0; i < sizeof(pos) / sizeof(pos[0]); i++) {
        rs0 = (long *) *(b - pos[i]);
        if (rs0 > (long *) *estack)
            continue;
        if (rs0 <= b)
            continue;
        if (((char *)rs0 - (char *)b) % 16 == 0) {
            return pos[i];
        }
    }
    printf("Not found suitable s0 value, s0[%x]\n", rs0);
    return 0;
}

static void rvbacktrace_stack(long **pFP, long **pSP, int (*print_func)(const char *fmt, ...))
{
    long *FP = *pFP;
    long *SP = *pSP;
    uint32_t num = 0;
    uint32_t index;
    uint32_t offset = sizeof(struct rv_coredump_regs) / sizeof(unsigned long);
    unsigned long ra;
    struct rv_coredump_regs *cd;

    rvbacktrace_get_stack(&estack);

    while (1) {
        if ((uintptr_t)FP >= (uintptr_t)estack) {
            rvstack_frame_len = num;
            return;
        }

        index = rvbacktrace_judge_stackframe(FP, &estack);
        if (index == BACKTRACE_FP_P0S_GCC_ISR) {
            ra = (unsigned long) *SP;
        } else {
            ra = (unsigned long) *(FP - 1);
        }

        if (index == BACKTRACE_FP_POS_TRAP) {
            cd = (struct rv_coredump_regs *)(FP - offset);
            memcpy(&core_regs, cd, sizeof(struct rv_coredump_regs));
        }

        print_func("[%d]Stack interval :[0x%lx - 0x%lx] ra 0x%lx\n", num, SP, FP, ra);
        rvstack_frame[num] = ra;
        num++;

        SP = FP;
        FP = (long *) *(FP - index);
    }
}

void rvbacktrace_fno_bm(int (*print_func)(const char *fmt, ...))
{
    long *FP;
    long *SP;

    FP = rvbacktrace_get_fp();
    SP = rvbacktrace_get_sp();
    print_func("\r\n---- RV_Backtrace Call Frame Start: ----\r\n");
    rvbacktrace_stack(&FP, &SP, print_func);
    rvbacktrace_addr2line((uint32_t *)&rvstack_frame[0], print_func);
    print_func("---- RV_Backtrace Call Frame End:----\r\n");
}
#endif