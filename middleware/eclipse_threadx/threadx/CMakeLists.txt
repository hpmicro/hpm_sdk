# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
# Define CONFIG_CUSTOM_RTOS_IRQ_STACK if there is need to change rtos's irq stack
if(DEFINED CONFIG_CUSTOM_RTOS_IRQ_STACK)
    sdk_compile_definitions(-D__threadx_irq_stack_top=${CONFIG_CUSTOM_RTOS_IRQ_STACK})
else()
    sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
endif()
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DTX_INCLUDE_USER_DEFINE_FILE=1)

if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()
message(STATUS "THREADX_ARCH: ${THREADX_ARCH}")
message(STATUS "THREADX_TOOLCHAIN: ${THREADX_TOOLCHAIN}")

# If CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER is defined, the mtimer is used to generate systic
# Otherwise use the gptimer to generate systic
if(NOT DEFINED CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER)
    sdk_compile_definitions("-DportasmHAS_MTIME=0")
else()
    sdk_compile_definitions("-DportasmHAS_MTIME=1")
endif()
# Define our target library and an alias for consumers

# Pick up the port specific variables and apply them

sdk_ses_inc(ports/riscv32/gnu/inc)
sdk_gcc_inc(ports/riscv32/gnu/inc)
sdk_iar_inc(ports/riscv32/iar/inc)

sdk_ses_src(ports/riscv32/gnu/src/tx_thread_context_restore.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_thread_context_save.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_thread_interrupt_control.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_thread_schedule.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_thread_stack_build.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_thread_system_return.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_timer_interrupt.S)
sdk_ses_src(ports/riscv32/gnu/src/tx_initialize_low_level.c)
sdk_ses_src(ports/riscv32/gnu/src/tx_disable_isr.S)
sdk_ses_src(ports/riscv32/gnu/src/portASM.S)

sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_context_restore.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_context_save.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_interrupt_control.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_schedule.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_stack_build.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_thread_system_return.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_timer_interrupt.S)
sdk_gcc_src(ports/riscv32/gnu/src/tx_initialize_low_level.c)
sdk_gcc_src(ports/riscv32/gnu/src/tx_disable_isr.S)
sdk_gcc_src(ports/riscv32/gnu/src/portASM.S)

sdk_iar_src(ports/riscv32/iar/src/tx_thread_context_restore.S)
sdk_iar_src(ports/riscv32/iar/src/tx_thread_context_save.S)
sdk_iar_src(ports/riscv32/iar/src/tx_thread_interrupt_control.S)
sdk_iar_src(ports/riscv32/iar/src/tx_thread_schedule.S)
sdk_iar_src(ports/riscv32/iar/src/tx_thread_stack_build.S)
sdk_iar_src(ports/riscv32/iar/src/tx_thread_system_return.S)
sdk_iar_src(ports/riscv32/iar/src/tx_timer_interrupt.S)
sdk_iar_src(ports/riscv32/iar/src/tx_initialize_low_level.S)
sdk_iar_src(ports/riscv32/iar/src/tx_disable_isr.S)
sdk_iar_src(ports/riscv32/iar/src/portASM.S)

add_subdirectory(ports/common)
# Pick up the common stuff
add_subdirectory(common)

sdk_compile_definitions_ifdef(CONFIG_AZURE_RTOS_TRACE "-DTX_ENABLE_EVENT_TRACE=1")

sdk_compile_definitions_ifdef(CONFIG_AZURE_RTOS_PROFILE "-DTX_EXECUTION_PROFILE_ENABLE=1")
add_subdirectory_ifdef(CONFIG_AZURE_RTOS_PROFILE utility/execution_profile_kit)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_LOWPOWER utility/low_power)

# Define the FreeRTOS adaptation layer
add_library(freertos-threadx EXCLUDE_FROM_ALL)
target_include_directories(freertos-threadx
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS
)
target_sources(freertos-threadx
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS/tx_freertos.c
)
target_link_libraries(freertos-threadx PUBLIC threadx)

# # If the user provided an override, copy it to the custom directory
# if (NOT TX_USER_FILE)
#     message(STATUS "Using default tx_user.h file")
#     set(TX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/inc/tx_user_sample.h)
# else()
#     message(STATUS "Using custom tx_user.h file from ${TX_USER_FILE}")
# endif()

# # set include file path to place configure file
# set(generated_file_path "${__build_dir}/generated")
# set(CUSTOM_INC_DIR ${generated_file_path}/include)
# message(STATUS "copy tx_user.h to " ${CUSTOM_INC_DIR})
# configure_file(${TX_USER_FILE} ${CUSTOM_INC_DIR}/tx_user.h COPYONLY)

# sdk_inc(${CUSTOM_INC_DIR})

# Enable a build target that produces a ZIP file of all sources
# set(CPACK_SOURCE_GENERATOR "ZIP")
# set(CPACK_SOURCE_IGNORE_FILES
#   \\.git/
#   \\.github/
#   _build/
#   \\.git
#   \\.gitattributes
#   \\.gitignore
#   ".*~$"
# )
# set(CPACK_VERBATIM_VARIABLES YES)
# include(CPack)
