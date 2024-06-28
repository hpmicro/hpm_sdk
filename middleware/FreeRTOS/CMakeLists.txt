# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions_ifdef(CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE "-DCONFIG_FREERTOS_TICKLESS_USE_STOP_MODE=1")
sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)

add_subdirectory(Source)
