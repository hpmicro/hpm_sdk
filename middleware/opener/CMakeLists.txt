# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

add_subdirectory(cip)
add_subdirectory(enet_encap)
add_subdirectory(ports)
add_subdirectory(utils)

sdk_compile_definitions(-DHPMICRO)
sdk_compile_definitions(-DRESTRICT=__restrict)

if (DEFINED PC_OPENER_ETHERNET_BUFFER_SIZE)
    sdk_compile_definitions(-DPC_OPENER_ETHERNET_BUFFER_SIZE=${PC_OPENER_ETHERNET_BUFFER_SIZE})
else()
    sdk_compile_definitions(-DPC_OPENER_ETHERNET_BUFFER_SIZE=512)
endif()