# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


if(DEFINED CONFIG_ECLIPSE_THREADX_GUIX_DEMO)
    sdk_inc(./${CONFIG_ECLIPSE_THREADX_GUIX_DEMO}/inc)
    file(GLOB_RECURSE GUIX_DEMO_SRC ${CONFIG_ECLIPSE_THREADX_GUIX_DEMO}/*.c)
    sdk_src(${GUIX_DEMO_SRC})
endif()
