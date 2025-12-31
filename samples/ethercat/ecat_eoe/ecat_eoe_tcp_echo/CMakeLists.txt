# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

# EtherCAT EEPROM
set(CONFIG_EEPROM_EMULATION 1) # using eeprom emulation compenent

# LwIP
set(CONFIG_LWIP 1)
set(APP_USE_ENET_PORT_COUNT 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# LwIP definition
sdk_compile_definitions(-DUSE_LWIPOPTS_APP_H=1)

project(ecat_eoe_tcp_echo)

# ECAT SSC code
add_subdirectory(../SSC ${CMAKE_BINARY_DIR}/SSC_build)

sdk_inc(../../port)
sdk_src(../../port/hpm_ecat_hw.c)
sdk_src(../../port/hpm_ecat_phy.c)
if(CONFIG_EEPROM_EMULATION)
sdk_src(../../port/hpm_ecat_e2p_emulation.c)
endif()

# LwIP port code
sdk_inc(../../port/lwip/baremetal/single)
sdk_inc(../../port/lwip/baremetal/single/arch)
sdk_app_src(../../port/lwip/baremetal/single/arch/sys_arch.c)
sdk_app_src(../../port/lwip/baremetal/single/eoe_ethernetif.c)

# Application code
sdk_inc(inc)
sdk_app_src(src/main.c)
sdk_app_src(src/tcp_echo.c)
sdk_app_src(src/ecat_application.c)
generate_ide_projects()
