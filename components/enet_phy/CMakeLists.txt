# Copyright (c) 2022-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCONFIG_ENET_PHY=1)

sdk_inc(.)
sdk_src(hpm_enet_phy_port.c)
sdk_src(hpm_enet_phy_adaptive.c)
sdk_src(hpm_enet_phy_adaptive_util.c)
sdk_src(hpm_enet_phy_adaptive_glue.c)

if($ENV{CONFIG_ENET_PHY_RTL8211})
    add_subdirectory(rtl8211)
    message(STATUS "ENet PHY Enabled: RTL8211")
endif()

if($ENV{CONFIG_ENET_PHY_RTL8201})
    add_subdirectory(rtl8201)
    message(STATUS "ENet PHY Enabled: RTL8201")
endif()

if($ENV{CONFIG_ENET_PHY_DP83867})
    add_subdirectory(dp83867)
    message(STATUS "ENet PHY Enabled: DP83867")
endif()

if($ENV{CONFIG_ENET_PHY_DP83848})
    add_subdirectory(dp83848)
    message(STATUS "ENet PHY Enabled: DP83848")
endif()

if($ENV{CONFIG_ENET_PHY_LAN8720})
    add_subdirectory(lan8720)
    message(STATUS "ENet PHY Enabled: LAN8720")
endif()

if($ENV{CONFIG_ENET_PHY_JL1111})
    add_subdirectory(jl1111)
    message(STATUS "ENet PHY Enabled: JL1111")
endif()

