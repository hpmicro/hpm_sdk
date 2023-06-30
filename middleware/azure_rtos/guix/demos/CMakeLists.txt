# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


if(DEFINED CONFIG_AZURE_RTOS_GUIX_DEMO)
    sdk_inc(./${CONFIG_AZURE_RTOS_GUIX_DEMO}/inc)
    file(GLOB_RECURSE GUIX_DEMO_SRC ${CONFIG_AZURE_RTOS_GUIX_DEMO}/*.c)
    sdk_src(${GUIX_DEMO_SRC})
endif()
