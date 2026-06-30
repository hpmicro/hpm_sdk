# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_compile_definitions(-DCONFIG_SEGGER_SYSVIEW_UCOSIII)
sdk_src(SEGGER_SYSVIEW_uCOSIII.c)