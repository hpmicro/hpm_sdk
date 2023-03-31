# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)

add_subdirectory(Source)
