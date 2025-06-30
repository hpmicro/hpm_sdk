# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)

#set(CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER 1) to use mchtmr as system's tick source
#set(CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(threadx_hello)


sdk_inc(src)
sdk_app_src(src/demo_threadx.c)
generate_ide_projects()
