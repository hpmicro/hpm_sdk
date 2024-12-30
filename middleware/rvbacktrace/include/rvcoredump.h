#ifndef RVCOREDUMP_H
#define RVCOREDUMP_H

#include <stdio.h>
#include "rvbacktrace.h"

struct rv_coredump_regs {
    unsigned long t6;
    unsigned long t5;
    unsigned long t4;
    unsigned long t3;
    unsigned long a7;
    unsigned long a6;
    unsigned long a5;
    unsigned long a4;
    unsigned long a3;
    unsigned long a2;
    unsigned long a1;
    unsigned long a0;
#ifdef BACKTRACE_USE_FP
    unsigned long s0;
#endif
#if defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
    unsigned long s1;
    unsigned long s0;
#endif
    unsigned long t2;
    unsigned long t1;
    unsigned long t0;
    unsigned long ra;
};

#endif /* RVCOREDUMP_H */
