# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Plus-POSIX/include)
sdk_inc(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Plus-POSIX/include/portable)
sdk_inc(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Plus-POSIX/include/portable/empty_portable)
sdk_inc(${CMAKE_CURRENT_SOURCE_DIR}/include)
sdk_inc(${CMAKE_CURRENT_SOURCE_DIR}/include/private)
sdk_src(FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c)
sdk_src(FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c)
sdk_src(FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c)
sdk_src(FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c)
sdk_src(FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c)