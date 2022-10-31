# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(erpc_c/config)
sdk_inc(erpc_c/infra)
sdk_inc(erpc_c/port)
sdk_inc(erpc_c/setup)
sdk_inc(erpc_c/transports)

sdk_src(erpc_c/infra/erpc_basic_codec.cpp)
sdk_src(erpc_c/infra/erpc_crc16.cpp)
sdk_src(erpc_c/infra/erpc_message_buffer.cpp)
sdk_src(erpc_c/infra/erpc_message_loggers.cpp)
sdk_src(erpc_c/infra/erpc_pre_post_action.cpp)
sdk_src_ifdef(CONFIG_ERPC_CLIENT erpc_c/infra/erpc_client_manager.cpp)
sdk_src_ifdef(CONFIG_ERPC_SERVER erpc_c/infra/erpc_server.cpp)
sdk_src_ifdef(CONFIG_ERPC_SERVER erpc_c/infra/erpc_simple_server.cpp)

sdk_src(erpc_c/port/erpc_port_stdlib.cpp)

sdk_src(erpc_c/setup/erpc_setup_mbf_rpmsg.cpp)
sdk_src_ifdef(CONFIG_ERPC_CLIENT erpc_c/setup/erpc_client_setup.cpp)
sdk_src_ifdef(CONFIG_ERPC_CLIENT erpc_c/setup/erpc_setup_rpmsg_lite_master.cpp)
sdk_src_ifdef(CONFIG_ERPC_SERVER erpc_c/setup/erpc_server_setup.cpp)
sdk_src_ifdef(CONFIG_ERPC_SERVER erpc_c/setup/erpc_setup_rpmsg_lite_remote.cpp)

sdk_src(erpc_c/transports/erpc_rpmsg_lite_transport.cpp)