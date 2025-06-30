# Copyright (c) 2023-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(ports)
add_subdirectory(uC-CPU)
add_subdirectory(uC-LIB)
add_subdirectory(uC-OS3)
sdk_compile_definitions(-DCONFIG_UCOS_III=1)
# Define CONFIG_CUSTOM_RTOS_IRQ_STACK if there is need to change rtos's irq stack
if(DEFINED CONFIG_CUSTOM_RTOS_IRQ_STACK)
    sdk_compile_definitions(-D__ucos_iii_irq_stack_top=${CONFIG_CUSTOM_RTOS_IRQ_STACK})
else()
    sdk_compile_definitions(-D__ucos_iii_irq_stack_top=_stack)
endif()

if (DEFINED CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR)
    sdk_compile_definitions(-DCONFIG_UCOS_III_TIMER_RESOURCE_NOT_MCHTMR=1)
endif()

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)