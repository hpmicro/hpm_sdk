# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(inc)
sdk_src(src/agile_modbus.c)

if(CONFIG_AGILE_MODBUS_RTU)
    sdk_src(src/agile_modbus_rtu.c)
    sdk_compile_definitions(-DAGILE_MODBUS_USING_RTU)
endif()

if(CONFIG_AGILE_MODBUS_TCP)
    sdk_src(src/agile_modbus_tcp.c)
    sdk_compile_definitions(-DUAGILE_MODBUS_USING_TCPP)
endif()

if(CONFIG_AGILE_MODBUS_SLAVE_UTIL)
    sdk_inc(util)
    sdk_src(util/agile_modbus_slave_util.c)
endif()

