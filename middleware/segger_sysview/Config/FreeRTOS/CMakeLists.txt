# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_compile_definitions(-DCONFIG_SEGGER_SYSVIEW_FREERTOS)
sdk_src(SEGGER_SYSVIEW_FreeRTOS.c)