# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_EEPROM_EMULATION 1) # using eeprom emulation compenent
set(CONFIG_CIA402_USING_ACTUAL_MOTOR 1) # using actual motor

if(CONFIG_CIA402_USING_ACTUAL_MOTOR)
set(CONFIG_MOTORCTRL_V2 1)
set(CONFIG_HPM_ADC 1)

set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ecat_cia402)

if(CONFIG_CIA402_USING_ACTUAL_MOTOR)
sdk_compile_definitions(-DCONFIG_MCL_HAS_EXTRA_CONFIG="mcl_app_config.h")
sdk_compile_definitions(-DCONFIG_CIA402_USING_ACTUAL_MOTOR=1)

sdk_inc(motor)
sdk_src(motor/bldc_foc.c)
endif()

sdk_inc(SSC/Src)
sdk_src(SSC/Src/cia402appl.c)
sdk_src(SSC/Src/coeappl.c)
sdk_src(SSC/Src/ecatappl.c)
sdk_src(SSC/Src/ecatcoe.c)
sdk_src(SSC/Src/ecatslv.c)
sdk_src(SSC/Src/mailbox.c)
sdk_src(SSC/Src/objdef.c)
sdk_src(SSC/Src/sdoserv.c)

sdk_inc(../port)
sdk_src(../port/hpm_ecat_hw.c)
sdk_src(../port/hpm_ecat_phy.c)
if(CONFIG_EEPROM_EMULATION)
sdk_src(../port/hpm_ecat_e2p_emulation.c)
endif()

sdk_inc(.)
sdk_app_src(ecat_cia402.c)

generate_ide_projects()
