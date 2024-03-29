# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_AGILE_MODBUS 1)
set(CONFIG_AGILE_MODBUS_RTU 1)
set(CONFIG_MODBUS_SERIAL 1)
set(CONFIG_AGILE_MODBUS_SLAVE_UTIL 1)


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(modbus_rtu_slave)

sdk_inc(../../common)
sdk_inc(../../common/timer)
sdk_app_src(../../common/timer/timer.c)

if (CONFIG_MODBUS_SERIAL)
sdk_inc(../../common/serial)
sdk_app_src(../../common/serial/serial.c)
endif()

sdk_app_src(src/main.c)
sdk_app_src(src/hold_registers.c)
generate_ide_projects()
