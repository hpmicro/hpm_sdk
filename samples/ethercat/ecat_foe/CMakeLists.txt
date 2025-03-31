# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_EEPROM_EMULATION 1) # using eeprom emulation compenent

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ecat_foe)

sdk_inc(SSC/Src)
sdk_src(SSC/Src/bootmode.c)
sdk_src(SSC/Src/coeappl.c)
sdk_src(SSC/Src/ecatappl.c)
sdk_src(SSC/Src/ecatcoe.c)
sdk_src(SSC/Src/ecatfoe.c)
sdk_src(SSC/Src/ecatslv.c)
sdk_src(SSC/Src/mailbox.c)
sdk_src(SSC/Src/objdef.c)
sdk_src(SSC/Src/sdoserv.c)
sdk_src(SSC/Src/foeappl.c)

sdk_inc(../port)
sdk_src(../port/hpm_ecat_hw.c)
sdk_src(../port/hpm_ecat_phy.c)
if(CONFIG_EEPROM_EMULATION)
sdk_src(../port/hpm_ecat_e2p_emulation.c)
endif()
sdk_src(../port/hpm_ecat_foe.c)

sdk_inc(.)
sdk_src(ssc_application.c)
sdk_src(ecat_foe_support.c)
sdk_app_src(ecat_foe.c)
generate_ide_projects()
