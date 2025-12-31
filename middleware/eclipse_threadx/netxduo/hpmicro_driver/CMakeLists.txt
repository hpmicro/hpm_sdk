# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if (!CONFIG_ECLIPSE_THREADX_NETXDUO_DISABLE_DEFERED_PROCESS)
    sdk_compile_definitions(-DNX_DRIVER_ENABLE_DEFERRED=1)
endif()

sdk_compile_definitions(-DNX_ENABLE_INTERFACE_CAPABILITY=1)

sdk_inc(.)

if(APP_USE_ENET_PORT_COUNT EQUAL 1)
    sdk_src(hpmicro_netx_driver.c)
else()
    sdk_src(hpmicro_netx_driver_dual_port.c)
endif()

sdk_src(hpmicro_enet_adapter.c)