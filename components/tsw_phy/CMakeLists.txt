# Copyright (c) 2024,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_compile_definitions(-DCONFIG_TSW_PHY=1)
sdk_src(hpm_tsw_phy_port.c)
sdk_src(hpm_tsw_phy_adaptive.c)
sdk_src(hpm_tsw_phy_adaptive_util.c)
sdk_src(hpm_tsw_phy_adaptive_glue.c)

if($ENV{CONFIG_TSW_PHY_RTL8211})
    add_subdirectory(rtl8211)
    message(STATUS "TSW PHY Enabled: RTL8211")
endif()

if($ENV{CONFIG_TSW_PHY_JL1111})
    add_subdirectory(jl1111)
    message(STATUS "TSW PHY Enabled: JL1111")
endif()

