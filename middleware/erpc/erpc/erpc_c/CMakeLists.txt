# Copyright (c) 2022-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(infra)
sdk_inc(port)
sdk_inc(setup)
sdk_inc(transports)

sdk_src(infra/erpc_basic_codec.cpp)
sdk_src(infra/erpc_crc16.cpp)
sdk_src(infra/erpc_message_buffer.cpp)
sdk_src(infra/erpc_message_loggers.cpp)
sdk_src(infra/erpc_pre_post_action.cpp)

sdk_src(setup/erpc_setup_mbf_rpmsg.cpp)

if(CONFIG_ERPC_CLIENT)
    sdk_src(infra/erpc_client_manager.cpp)
    if(CONFIG_ERPC_ARBITRATOR)
        sdk_src(infra/erpc_arbitrated_client_manager.cpp)
        sdk_src(setup/erpc_arbitrated_client_setup.cpp)
    else()
        sdk_src(setup/erpc_client_setup.cpp)
    endif()
endif()

if(CONFIG_ERPC_SERVER)
    sdk_src(infra/erpc_server.cpp)
    sdk_src(infra/erpc_simple_server.cpp)
    sdk_src(setup/erpc_server_setup.cpp)
endif()

if(CONFIG_ERPC_RPMSG_MASTER)
    if(CONFIG_FREERTOS)
        sdk_src(setup/erpc_setup_rpmsg_lite_rtos_master.cpp)
    else()
        sdk_src(setup/erpc_setup_rpmsg_lite_master.cpp)
    endif()
else()
    if(CONFIG_FREERTOS)
        sdk_src(setup/erpc_setup_rpmsg_lite_rtos_remote.cpp)
    else()
        sdk_src(setup/erpc_setup_rpmsg_lite_remote.cpp)
    endif()
endif()

if(CONFIG_FREERTOS)
    sdk_src(port/erpc_port_freertos.cpp)
    sdk_src(port/erpc_setup_extensions_freertos.cpp)
    sdk_src(port/erpc_threading_freertos.cpp)

    sdk_src(transports/erpc_rpmsg_lite_rtos_transport.cpp)
else()
    sdk_src(port/erpc_port_stdlib.cpp)

    sdk_src(transports/erpc_rpmsg_lite_transport.cpp)
endif()

if(CONFIG_ERPC_ARBITRATOR)
    sdk_src(infra/erpc_transport_arbitrator.cpp)
endif()
