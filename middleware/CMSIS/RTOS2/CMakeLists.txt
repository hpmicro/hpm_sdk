# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(Include)
add_subdirectory_ifdef(CONFIG_FREERTOS FreeRTOS)
add_subdirectory_ifdef(CONFIG_RTTHREAD_NANO RTThread)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX ThreadX)
add_subdirectory_ifdef(CONFIG_UCOS_III MicriumOS)
