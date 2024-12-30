/*
 * Copyright (c) 2024 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BACKTRANCE_CONFIG_H
#define BACKTRANCE_CONFIG_H

#define RV_BACKTRACE_NULL       0
#define RV_BACKTRACE_RTTHREAD   1
#define RV_BACKTRACE_BAREMETAL  2

/* User Configure */
#define BACKTRACE_USE_FP /* To enable this option, add the [-fno-omit-frame-pointer] option to ASM,C/C++. */
/* #define BACKTRACE_ALL    // Before enabling this option, enable the BACKTRACE_USE_FP / Outputs the stack of all threads */
/* #define BACKTRACE_FSTACK_PROTECT // To enable this option, add the [-fstack-protector-strong] option to ASM,C/C++, add [-Wl,--wrap,_exit] flag to link option. */
#define RV_BACKTRACE_ENV    RV_BACKTRACE_NULL

#if (RV_BACKTRACE_ENV == RV_BACKTRACE_NULL)
#undef RV_BACKTRACE_ENV
#define RV_BACKTRACE_ENV RV_BACKTRACE_BAREMETAL
#endif

#endif /* BACKTRANCE_CONFIG_H */
