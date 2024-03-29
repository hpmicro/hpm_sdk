# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
if(NOT DEFINED UX_INCLUDE_USER_DEFINE_FILE)
    sdk_compile_definitions(-DUX_INCLUDE_USER_DEFINE_FILE=1)
endif()
if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

# Pick up the port specific stuff first
if(DEFINED USBX_CUSTOM_PORT)
    add_subdirectory(${USBX_CUSTOM_PORT} usbx_port)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN})
endif()

add_subdirectory(ports/hpmicro)

# Then the common files
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common)

# If the user provided an override, copy it to the custom directory
if (NOT UX_USER_FILE)
    message(STATUS "Using default ux_user.h file")
    set(UX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/core/inc/ux_user_sample.h)
else()
    message(STATUS "Using custom ux_user.h file from ${UX_USER_FILE}")
endif()

# set include file path to place configure file
set(generated_file_path "${__build_dir}/generated")
set(CUSTOM_INC_DIR ${generated_file_path}/include)
message(STATUS "copy ux_user.h to " ${CUSTOM_INC_DIR})
configure_file(${UX_USER_FILE} ${CUSTOM_INC_DIR}/ux_user.h COPYONLY)

sdk_inc(${CUSTOM_INC_DIR})
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_USBX_DEVICE hpmicro_device_controllers)
sdk_compile_definitions(-DUX_DEVICE_BIDIRECTIONAL_ENDPOINT_SUPPORT=1)