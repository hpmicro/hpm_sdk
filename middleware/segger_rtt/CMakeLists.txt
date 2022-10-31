# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(Config)
sdk_inc(RTT)

sdk_src(RTT/SEGGER_RTT.c)
sdk_src(RTT/SEGGER_RTT_printf.c)
sdk_ses_src(Syscalls/SEGGER_RTT_Syscalls_SES.c)
sdk_gcc_src(Syscalls/SEGGER_RTT_Syscalls_GCC.c)
