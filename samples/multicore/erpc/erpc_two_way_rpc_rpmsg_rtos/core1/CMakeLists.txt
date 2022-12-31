# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE sec_core_img)
set(BUILD_FOR_SECONDARY_CORE 1)
set(IS_SEC_CORE_IMG true)
set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)
set(CONFIG_FREERTOS 1)
set(CONFIG_ERPC 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(erpc_two_way_rpc_rtos_core1)

set(CMAKE_CXX_STANDARD 11)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_B)
sdk_compile_definitions(-DBOARD_RUNNING_CORE=1)
sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_FREERTOS=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(src)
sdk_app_src(src/main_core1.c)

sdk_inc(../config)
sdk_inc(../generated)
sdk_src(../generated/erpc_two_way_rpc_Core1Interface_client.cpp)
sdk_src(../generated/erpc_two_way_rpc_Core0Interface_server.cpp)

# ERPC RTOS Server Middleware Files
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_basic_codec.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_client_manager.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_arbitrated_client_manager.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_crc16.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_message_buffer.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_message_loggers.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_pre_post_action.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_server.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_simple_server.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/infra/erpc_transport_arbitrator.cpp)

sdk_src(../../../../../middleware/erpc/erpc/erpc_c/port/erpc_port_freertos.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/port/erpc_setup_extensions_freertos.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/port/erpc_threading_freertos.cpp)

sdk_src(../../../../../middleware/erpc/erpc/erpc_c/setup/erpc_arbitrated_client_setup.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/setup/erpc_server_setup.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/setup/erpc_setup_mbf_rpmsg.cpp)
sdk_src(../../../../../middleware/erpc/erpc/erpc_c/setup/erpc_setup_rpmsg_lite_rtos_remote.cpp)

sdk_src(../../../../../middleware/erpc/erpc/erpc_c/transports/erpc_rpmsg_lite_rtos_transport.cpp)

sdk_ld_options("--std=c++11")
generate_ses_project()


