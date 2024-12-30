#ifndef RV_BACKTRANCE_H
#define RV_BACKTRANCE_H

#include <stdio.h>
#include <stdint.h>
#include "backtrace_config.h"

#if (RV_BACKTRACE_ENV == RV_BACKTRACE_RTTHREAD)
#include "rtthread.h"
#define BACKTRACE_PRINTF rt_kprintf
#ifdef BACKTRACE_USE_FP
#define RV_BACKTRACE_USE_FP_RTTHREAD
#endif
#elif (RV_BACKTRACE_ENV == RV_BACKTRACE_BAREMETAL)
#define BACKTRACE_PRINTF printf
#ifdef BACKTRACE_USE_FP
#define RV_BACKTRACE_USE_FP_BAREMETAL
#endif
#elif
#define BACKTRACE_PRINTF printf
#endif

/* Backtrace All Threads */
#if defined(BACKTRACE_USE_FP) && defined(BACKTRACE_ALL)
#define BACKTRACE_ALL_THREAD
#define BACKTRACE_FP_POS (8)
#endif /* BACKTRACE_USE_FP && defined BACKTRACE_ALL */

/* User Parameter */
#define STACK_FRAME_LEN (10)
#define STACK_BUFFER_LEN (100)

#if __riscv_xlen == 32
#define BACKTRACE_LEN 8
#endif

#if __riscv_xlen == 64
#define BACKTRACE_LEN 16
#endif

#ifdef BACKTRACE_USE_FP
#define RV_BACKTRACE_USE_FP
#endif /* BACKTRACE_USE_FP */

#define BACKTRACE_TRAP_DEPTH       (3)
#define BACKTRACE_FP_P0S_GCC_ISR   (1)
#define BACKTRACE_FP_POS_NORMAL    (2)
#define BACKTRACE_FP_POS_TRAP      (5)

#if defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
    extern const uint32_t CSTACK$$Limit[];
    #define _stack CSTACK$$Limit
#else 
    extern char *_stack;
#endif

struct stackframe
{
    unsigned long s_fp; // frame pointer
    unsigned long s_ra; // return address
};

static inline void rvbacktrace_get_stack(uintptr_t *estack)
{
   *estack = (uintptr_t)&_stack;
}

void rvbacktrace(void); // backtrace function for usr
void rvbacktrace_trap(void *casue, void *epc); // backtrace function used in trap
void rvbacktrace_fno(int (*print_func)(const char *fmt, ...));
void rvbacktrace_fno_bm(int (*print_func)(const char *fmt, ...));
int rvbacktrace_fomit(int (*print_func)(const char *fmt, ...));
int rvbacktrace_fomit_trap(int (*print_func)(const char *fmt, ...));
void rvbacktrace_addr2line(uint32_t *frame, int (*print_func)(const char *fmt, ...));

#endif /* RV_BACKTRANCE_H */
