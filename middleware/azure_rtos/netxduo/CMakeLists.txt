# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED NX_INCLUDE_USER_DEFINE_FILE)
    sdk_compile_definitions(-DNX_INCLUDE_USER_DEFINE_FILE=1)
endif()
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

if(${NXD_ENABLE_AZURE_IOT})
    message(STATUS "NXD_ENABLE_AZURE_IOT - defined")
endif()

# A place for generated/copied include files (no need to change)
set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

# Pick up the port specific stuff first
if(DEFINED NETXDUO_CUSTOM_PORT)
    add_subdirectory(${NETXDUO_CUSTOM_PORT} netxduo_port)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN})
endif()

# Then the common files
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common)

# Check wether use default TCP ack N packets
if (NOT DEFINED NETX_TCP_ACK_EVERY_N_PACKETS)
    sdk_compile_definitions(-DNX_TCP_ACK_EVERY_N_PACKETS=2)
else()
    sdk_compile_definitions("-DNX_TCP_ACK_EVERY_N_PACKETS=${NETX_TCP_ACK_EVERY_N_PACKETS}")   
endif()
# Pick up the apps directory containing the protocol and app-layer components

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/addons)

# Network security and crypto components
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/crypto_libraries)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/nx_secure)

# Utility components
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utility)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/hpmicro_driver)

# If the user provided an override, copy it to the custom directory
if (NOT NX_USER_FILE)
    message(STATUS "Using default nx_user.h file")
    set(NX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/inc/nx_user_sample.h)
else()
    message(STATUS "Using custom nx_user.h file from ${NX_USER_FILE}")
endif()

# set include file path to place configure file
set(generated_file_path "${__build_dir}/generated")
set(CUSTOM_INC_DIR ${generated_file_path}/include)
message(STATUS "copy nx_user.h to " ${CUSTOM_INC_DIR})
configure_file(${NX_USER_FILE} ${CUSTOM_INC_DIR}/nx_user.h COPYONLY)
sdk_inc(${CUSTOM_INC_DIR})
