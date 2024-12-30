# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions_ifdef(CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE "-DCONFIG_FREERTOS_TICKLESS_USE_STOP_MODE=1")
sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)

# Define CONFIG_CUSTOM_RTOS_IRQ_STACK if there is need to change rtos's irq stack
if(DEFINED CONFIG_CUSTOM_RTOS_IRQ_STACK)
    sdk_compile_definitions(-D__freertos_irq_stack_top=${CONFIG_CUSTOM_RTOS_IRQ_STACK})
else()
    sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
endif()

sdk_compile_definitions(-DCONFIG_FREERTOS=1)

## Lwip related definitions
if (DEFINED CONFIG_LWIP)
    sdk_compile_definitions(-D__ENABLE_FREERTOS=1)
    sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=1)
endif()

add_subdirectory(Source)
